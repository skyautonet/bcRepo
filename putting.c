/*













*/

#include <stdio.h>
#include "budycaddy.h"


#define PI 3.141592653589  
#define g  9.806
#define RTA_MAX 20
#define RTA_SPLIT_MAX 10
#define Dextra 1.0
#define ROLL_MAX 20
#define TMAX 1000
#define DGM 0.1

rta_type** RTA;
bta_type*** bta;
mesh_frame** MG;
double Xb,Yb,Zb,Xh,Yh,Zh,Mug,nz,Gspeed;


void Rolling(int i,int j,double EKin, double Apsi ,bool* Sflag  )
{
    double Vin;
    double* V, *vx, *vy, *vz;
    double* x, *y, *z, *c, *dr;
    double fx,fy,fz;
    double Troll, Tstep, vel;
    double Xc,Yc, Tc;
    double mu ; 
    double nx,ny;
    double dhq=50.0;
    double DCup=0.108;
    double Mur=0.8 ; // recheck
    double Vlim=0.3;
    double drk,hpeak,dhs;
    int X1,Y1,X2,Y2;
    int k,ibk,jbk;
    int ks,kr,kq;
    bool Bstop=false;
    position_type d,a,cross;


    printf("%s [%d,%d] B(%f,%f,%f) H(%f,%f,%f), Mug:%f Nz:%f, start\n",__func__, i,j,Xb,Yb,Zb,Xh,Yh,Zh,Mug,nz  );

    drk=hpeak=0.0;
    ks=kr=kq=0;

    V=(double*)malloc(sizeof(double)*ROLL_MAX);
    vx=(double*)malloc(sizeof(double)*ROLL_MAX);
    vy=(double*)malloc(sizeof(double)*ROLL_MAX);
    vz=(double*)malloc(sizeof(double)*ROLL_MAX);

    x=(double*)malloc(sizeof(double)*TMAX);
    y=(double*)malloc(sizeof(double)*TMAX);
    z=(double*)malloc(sizeof(double)*TMAX); //???
    c=(double*)malloc(sizeof(double)*TMAX); //???
    dr=(double*)malloc(sizeof(double)*TMAX); //???

    Vin= sqrt (2.0 * RTA[i][j].EK);
    V[0]=Vin;
    vx[0]=Vin*cos(RTA[i][j].psi);
    vy[0]=Vin*sin(RTA[i][j].psi);
    vz[0]=Vin*(1-nz*nz);  // nz ??? 


    bta[i][j][0].x=x[0]=Xb;   
    bta[i][j][0].y=y[0]=Yb;   
    bta[i][j][0].z=z[0]=Zb;   
    //    bta[i][j][0].timestamp=0;   

    Troll=0.0;  // ??
    Tstep=0.01;  // ??


    ibk=(int)floor(Xb/DGM);
    jbk=(int)floor(Yb/DGM);

    for(k=0;k<TMAX-1;k++)
    {
	Troll = Troll + Tstep;
	vel = V[k];
	mu=Mug;

	if ( MG[jbk][ibk].terrain == 2 )
	    mu=Mur*Mug;            

	if ( vel < 0.6 ) 
	    mu=mu*(-0.864*pow(vel,3) + 1.14 *  pow(vel,2) + 0.533 * vel + 0.139 )/0.043 ;


	ibk=(int)floor(x[k]/DGM);
	jbk=(int)floor(y[k]/DGM);


	 printf("k:%d mu:%f ibk:%d,jbk:%d, x[k]:%f vel:%f\n",k,mu,ibk,jbk, x[k],vel);    

	nx=MG[jbk][ibk].nx1;          
	ny=MG[jbk][ibk].ny1;          
	nz=MG[jbk][ibk].nz1;          

	//      if ( x[k] + y[k] >= (ibk+1)*DGM + jbk*DGM )
	if ( x[k] + y[k] >= (ibk+1)*DGM + jbk*DGM ) 
	{
	    nx = MG[jbk][ibk].nx2;            
	    ny = MG[jbk][ibk].ny2;            
	    nz = MG[jbk][ibk].nz2;            
	}
	//        else
	//        {
	fx = (g*nx - mu*vx[k] / V[k] ) * nz;
	fy = (g*ny - mu*vy[k] / V[k] ) * nz;
	fz = (g*nz - mu*vz[k] / V[k] ) * nz;
	//        }

	vx[k+1]=vx[k]+fx*Tstep;
	vy[k+1]=vy[k]+fy*Tstep;
	vz[k+1]=vz[k]+fz*Tstep;
	//V[k+1]= sqrt ( pow(vx[k+1],2) + pow(vy[k+1],2) + pow(vz[k+1],2));
	V[k+1]= sqrt ( pow(vx[k+1],2) + pow(vy[k+1],2));
	bta[i][j][k+1].x=x[k+1]=x[k]+(vx[k]+vx[k+1])*Tstep/2.0 ;   
	bta[i][j][k+1].y=y[k+1]=y[k]+(vy[k]+vy[k+1])*Tstep/2.0 ;   
	bta[i][j][k+1].z=z[k+1]=z[k]+(vz[k]+vz[k+1])*Tstep/2.0 ;   
	//      bta[i][j][k].timestamp=k;

	printf("V[k]:%f, V[k+1]:%f,Vlim: %f\n", V[k], V[k+1], Vlim);
        printf("v[%d] vx:%f vy:%f vz:%f \n",k, vx[k],vy[k],vz[k]); 
        printf("v[%d] vx:%f vy:%f vz:%f \n",k+1, vx[k+1],vy[k+1],vz[k+1]); 
	if ( V[k+1] < Vlim )
	{
	    Bstop=true;
	    ks=k+1;
	    break;
	}

	d.x=x[k]; // XD
	d.y=y[k]; // YD
	a.x=x[k+1];
	a.y=y[k+1];

	if (a.x <=0.0 || a.y <=0.0)
	{
		printf(" Abnormal value!! XA:%f YA:%f\n", a.x, a.y);
		Bstop=false;
		break;    
        }

	cross=find_cross(d,a);
	Xc=cross.x;
	Yc=cross.y;

	if (Xc==a.x && Yc==a.y)
	    continue;

	printf("--------------------------------------------Crossed! Xc:%f Yc:%f\n",Xc,Yc);

	Tc=sqrt(pow(Xc-d.x,2) + (Yc-d.y))/vel;
	Troll=Troll-Tstep+Tc;
	vx[k+1]=vx[k]+fx*Tc;          
	vy[k+1]=vy[k]+fy*Tc;          
	vz[k+1]=vz[k]+fz*Tc;          
	V[k+1]=sqrt(pow(vx[k+1],2) + pow(vy[k+1],2) + pow(vz[k+1],2));
	bta[i][j][k+1].x=x[k]=Xc;
	bta[i][j][k+1].y=y[k]=Yc;
	bta[i][j][k+1].z=z[k]=(-nx*(Xc-x[k])-ny*(Yc-y[k]))/nz+z[k];

	ibk=(int)(floor((Xc+vx[k+1]*0.0001)/DGM));
	jbk=(int)(floor((Yc+vx[k+1]*0.0001)/DGM));
	X1=ibk*DGM;
	Y1=jbk*DGM;
	X2=(ibk+1)*DGM;
	Y2=jbk*DGM;

	if (Xc+Yc < X2+Y2) 
	{
		nx=MG[jbk][ibk].nx1; 
		ny=MG[jbk][ibk].ny1; 
		nz=MG[jbk][ibk].nz1; 
	}
	else if ( Xb+Yb >= X2+Y2 ) 
	{
		nx=MG[jbk][ibk].nx2; 
		ny=MG[jbk][ibk].ny2; 
		nz=MG[jbk][ibk].nz2; 
	}
//	else
//	{
	c[k+1]=sqrt ( pow( x[k+1] - Xh,2) + pow( y[k+1] - Yh,2 ));
	if ( c[k+1] < dhq ) 
	{
	    kq=k+1;
	    dhq=c[k+1];
	}
	else
	{
	    dr[k+1]=abs( (Yh-Yb)*x[k+1]+(Xh-Xb)*y[k+1] -(Yh-Yb)*Xb - (Xh-Xb)*Yb ) / sqrt( pow(Xh-Xb ,2) + pow( Yh-Yb,2));
	    if ( dr[k+1] > drk )
	    {
		kr=k+1;
		drk=dr[k+1];    
	    }
	    else 
		continue;
	}
//	}
    }


    if (Bstop== true) 
    {
	printf("The ball stops and the trajectory calculation is completed!\n");

	RTA[i][j].kq = kq;
	RTA[i][j].Xq = x[kq];//4
	RTA[i][j].Yq = y[kq];
	RTA[i][j].Zq = z[kq];
	RTA[i][j].ks = ks;
	RTA[i][j].Xs = x[ks];
	RTA[i][j].Ys = y[ks];
	RTA[i][j].Zs = z[ks];
	RTA[i][j].kr = kr;
	RTA[i][j].Xr = x[kr];
	RTA[i][j].Yr = y[kr];
	RTA[i][j].Zr = z[kr];

	dhs = sqrt ( pow ( x[ks] - Xh , 2) + pow ( y[ks] - Yh , 2 ) );

	RTA[i][j].dhq = dhq;
	RTA[i][j].dhs = dhs;
	RTA[i][j].drk = drk;

	if ( dhq <= DCup/2.0 )
	{
  	    printf("%d\n dhq:%f ",__LINE__, dhq);  
	    *Sflag = true;               
	}
	else
	{
	    *Sflag = false;
	    printf("No putting area! %f \n",dhq);
	}

    }

//    return Sflag;


}


//int putting_process(double Gspeed, double Xb,double Yb,double Zb,double Xh,double Yh,double Zh,mesh_frame** MG )
int putting_process()
{
    int IPB, JPB, IPH, JPH;
    double Dhbz,Dhb;
    double Dbq, Dhs;
    double psi0;
    double X2,Y2;
    double nx,ny;
    bool Sflag=false;
    double Xq,Yq,Zq,Xs,Ys,Zs,Xr,Yr,Zr,mu,psiq; 

    int i,j;
    int  Ifinal,Jfinal;

    IPB=(int)floor(Xb/DGM);
    JPB=(int)floor(Yb/DGM);
    IPH=(int)floor(Xh/DGM);
    JPH=(int)floor(Yh/DGM);

    Mug= pow(1.83,2)/(2.0*Gspeed);

    // init mesh frame..
    Dhbz=sqrt( pow(Xh-Xb,2) + pow(Yh-Yb,2) + pow(Zh-Zb,2));
    Dhb= sqrt ( pow(Xh-Xb,2) + pow(Yh-Yb,2) );

    if ( Dhb == 0.0 ) 
    {
	printf("Ball and hole cup positions are the same\n");    
	return 0;      
    }
    else
	psi0 = PI/2.0;

    RTA=(rta_type**)malloc(sizeof(rta_type)*TMAX); //

    for(j=0;j<RTA_MAX;j++)
	RTA[j]=(rta_type**)malloc(sizeof(rta_type)*RTA_SPLIT_MAX);

    bta = (bta_type***)malloc(sizeof(bta_type**) * TMAX ); 
    for (j = 0; j < TMAX; j++) 
    {
	bta[j] = (bta_type**)malloc(sizeof(bta_type*) * TMAX); 
	for (i = 0; i < TMAX; i++)
	    bta[j][i] = (bta_type*)malloc(sizeof(bta_type) * TMAX); 
    }

    if ( Yh != Yb ) 
    {
	psi0=tanh((Xh-Xb)/(Yh-Yb));
    }
//   elsae
//   {
    RTA[1][1].EK=g*(Zh-Zb)+Mug*(Dhb+Dextra);
    RTA[1][1].psi=psi0;
//  }

    printf("EK:%f, psi:%f\n",RTA[1][1].EK,RTA[1][1].psi);

    if (RTA[1][1].EK <0 ) 
	goto Abnormal_1;

    X2=(IPB+1)*DGM;
    Y2=JPB*DGM;
    nx=MG[JPB][IPB].nx1;    
    ny=MG[JPB][IPB].ny1;    
    nz=MG[JPB][IPB].nz1;    

    if (Xb + Yb >= X2 + Y2 )
    {
	nx=MG[JPB][IPB].nx2;    
	ny=MG[JPB][IPB].ny2;    
	nz=MG[JPB][IPB].nz2;    
    }

    if ( MG[JPB][IPB].terrain == 1 ||  MG[JPB][IPB].terrain == 2 ) 
	goto NoPutting;


    i=1;
    j=1;

//    Sflag=Rolling(i,j,Xb,Yb,Zb,Xh,Yh,Mug,nz,RTA,bta,MG);  
    Rolling(i,j,RTA[i][j].EK,RTA[i][j].psi,&Sflag);
    if (Sflag==true)
    {
	Ifinal=i;
	Jfinal=j;
	goto FINAL;
    }

#if 1	
    do
    {
	i++;
	//        Sflag=false;
	Xq=RTA[i][j].Xq;
	Yq=RTA[i][j].Yq;
	Zq=RTA[i][j].Zq;
	Xs=RTA[i][j].Xs;
	Ys=RTA[i][j].Ys;
	Zs=RTA[i][j].Zs;
	Xq=RTA[i][j].Xq;
	Dbq = sqrt ( pow(Xb-Xq,2) + pow(Yb-Yq,2));
	Dhs = sqrt ( pow(Xh-Xs,2) + pow(Yh-Ys,2));

	mu=Mug; //check??

	RTA[i][j].EK=RTA[i][j].EK + mu*(Dhb-Dbq)+mu*(Dextra-Dhs);
	if (RTA[i][j].EK < 0) 
	    goto Abnormal_1;

	psiq=PI/2.0;
	if ( Xq != Xb ) 
	    psiq=tanh((Yq-Yb)/(Xq-Xb));            
	//        else
	RTA[i][j].psi= RTA[i][j].psi +  RTA[i][j].psi-psiq;
//	Sflag=Rolling(i,j,Xb,Yb,Zb,Xh,Yh,Mug,nz,RTA,bta,MG);  

         Rolling(i,j,RTA[i][j].EK,RTA[i][j].psi,&Sflag);  
	if (Sflag == true )
	{
	    Ifinal=i;
	    Jfinal=j; // 1        
	    //            break; 
	    goto FINAL;
	}
	else if ( Sflag == false)
	{
	    if (i >= TMAX ) 
	    {
		Ifinal=i;
		Jfinal=j;    
		goto Abnormal_2;
	    }
	    else
		continue;
	}
    }while(1);
#endif

FINAL:
    if (Sflag==true) 
    {
	printf("%d\n",__LINE__);  
	Xq=RTA[Ifinal][Jfinal].Xq;    
	Yq=RTA[Ifinal][Jfinal].Yq;
	Zq=RTA[Ifinal][Jfinal].Zq;
	Xs=RTA[Ifinal][Jfinal].Xs;
	Ys=RTA[Ifinal][Jfinal].Ys;
	Zs=RTA[Ifinal][Jfinal].Zs;
	Xr=RTA[Ifinal][Jfinal].Xr;
	Yr=RTA[Ifinal][Jfinal].Yr;
	Zr=RTA[Ifinal][Jfinal].Zr;

	display_bta(Ifinal,Jfinal,bta);
	return;    
    }

Abnormal_1:
Abnormal_2:
    if (Sflag==false)
    {


    }

NoPutting:    

	return 0;
}


void display_bta(int i,int j, bta_type*** bta)
{
    int k;

    printf("Ball Trajectry Area\n");
    for(k=0;k<TMAX;k++)
    {
	printf("(%d,%d):%f,%f,%f\n",i,j, bta[i][j][k].x,bta[i][j][k].y,bta[i][j][k].z);
    }

}


void main()
{
    int size,i;
    int xSize,ySize;
//    double Gspeed,Xb,Yb,Zb,Xh,Yh,Zh;

    xSize=570;
    ySize=550;

    Gspeed=2.7;
    Xb=20.2;
    Yb=10.2;
//    Zb=0.10;
    Xh=30.2;
    Yh=20.3;
//    Zh=0.20;


    size=xSize*ySize;

    MG = (mesh_frame**)malloc(ySize * sizeof(mesh_frame));

    for(i=0; i<ySize; i++)
	MG[i]=(mesh_frame*)malloc(xSize * sizeof(mesh_frame));

    get_csv( 1,xSize,ySize,MG);  // get attrib
    get_csv( 0,xSize,ySize,MG);  // get height

    initialize_mesh(MG,xSize,ySize);
    putting_process();

}

