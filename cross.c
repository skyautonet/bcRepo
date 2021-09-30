#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#define PRN_RESULT printf(" Xc,Yc:%f,%f\n-------------------------------\n", c.x, c.y) 

typedef struct {
 double x;
 double y;
} position_type;


static int lower_mesh=0;
static int upper_mesh=1;


/*************************************************************************************************

 Input parameter  : d <-- PD, a <---- PA 
 Output parameter : c.x <---- Xc, c.y <---- Yc

****************************************************************************************************/
#define DGM 0.1
position_type find_cross(position_type d, position_type a)
{
	position_type c;
	double XMc,YMc,  XLc,YLc, XTc,YTc, XRc,YRc, XBc,YBc; 
	double slope = 0.0;
	int flag;
	int IPD,JPD,IPA,JPA;
	double X2,Y2;
        position_type M[3];  

	c.x=0.0;
	c.y=0.0;

	if ( d.x != a.x )
		slope = (d.y-a.y)/(d.x - a.x);


	M[0].x = floor(d.x);
	M[0].y = floor(d.y);

	M[1].x = floor(a.x);
	M[1].y = floor(a.y);

	if ( M[0].y+M[0].x+1.0 < d.x + d.y )
          flag = upper_mesh;
        else 
          flag = lower_mesh;

	IPD=(int)floor(d.x);  
	JPD=(int)floor(d.y);  
	IPA=(int)floor(a.x);  
	JPA=(int)floor(a.y); 

	X2=IPD*DGM+DGM;
	Y2=IPD*DGM;

//	X2=IPD+DGM;
//	Y2=IPD;
	
	printf(" XD,YD:%f,%f\n XA,YA:%f,%f\n", d.x,d.y,a.x,a.y );  

	if (IPD != IPA || JPD != JPA || (d.x+d.y-X2-Y2)*(a.x+a.y-X2-Y2) <0)
	 goto CROSSED;       
        else
        {
	    c.x=a.x;
	    c.y=a.y;
	//    printf("moved in same Mesh area!\n");
	    return c;
	}

//	printf("slope:%f = %f / %f \n",slope,(d.y-a.y),(d.x-a.x));
//	printf(" XD,YD:%f,%f\n XA,YA:%f,%f\n X1,Y1:%f,%f\n X2,Y2:%f,%f\n", d.x,d.y,a.x,a.y,M[0].x, M[0].y,M[1].x, M[1].y);  
//	printf(" XD,YD:%f,%f\n XA,YA:%f,%f slope:%f \n", d.x,d.y,a.x,a.y,slope);

	
CROSSED:
        
	if ( flag == lower_mesh ) 
	{

		M[1].x = M[0].x + 1.0;
		M[1].y = M[0].y;
	
//		printf("X1,Y1:%f,%f\n X2,Y2:%f,%f\n", M[0].x, M[0].y,M[1].x, M[1].y);  

		if (d.x == a.x )
		{
			if ( a.y < d.y )
			{
				c.x = d.x;
				c.y = M[0].y; 			
//				printf("0: Xc,Yc = {%f,%f}\n", c.x, c.y); 
			} 
			else if (a.y > d.y)
			{
				c.x = d.x;
				c.y = d.x * (-1.0)  + M[1].x + M[1].y; 
//				printf("1: Xc,Yc = {%f,%f}\n", c.x, c.y); 
				return c;	
			} 
		} 
		else if (d.y == a.y )
		{
			if (a.x < d.x )
			{
				c.x = M[0].x; 
				c.y = d.y;
			}
			else if ( a.x > d.x)
			{
				c.x = d.y * (-1.0)  + M[1].x + M[1].y; 
				c.y = d.y;  
			}
		} 
		else if ( (d.x + d.y) == (a.x + a.y) && (a.x <= d.x )) 
		{
			c.x = M[0].x;
			c.y = M[0].x * (-1.0)  + d.x + d.y;
		} 
		else if ( (d.x + d.y) == (a.x + a.y) && (a.x >= d.x ))
		{
			c.y = M[0].y ;
			c.x = M[1].y * (-1.0) + d.x + d.y;
		}
		else
		{

			XMc = ( d.x * (d.y - a.y) + (d.x -a.x)*(M[1].x+M[1].y - d.y))/((d.x-a.x)+(d.y-a.y));        
			YMc=  XMc * (-1.0)  + M[1].x + M[1].y;
			
//			printf(" XMC, YMC = %f,%f\n", XMc, YMc);

			if ( ( M[0].x < XMc && XMc < M[1].x ) && ( M[0].y  < YMc ) &&  ( a.y > d.y) ) //a && YMc < M[2].y ))
			{
				c.x = XMc;
				c.y = YMc;
				
//				printf("2 Xc,Yc = {%f,%f}\n", c.x, c.y); 
			}
			else
                        {
				XLc = M[0].x;
				YLc = slope * (XLc - d.x ) + d.y;
//				if (( M[0].y < YLc && YLc < M[1].y ) && a.x < d.x ) 
				if (( M[0].y < YLc ) && a.x < d.x ) 
				{
					c.x = XLc;
					c.y = YLc;

//					printf("3 Xc,Yc = {%f,%f}\n", c.x, c.y); 
				}
                                else
                                { 
					YBc = M[0].y;
					XBc = (M[0].y -d.y)/slope + d.x;  // YLc
//					printf("YLC=%f, slope=%f\n",YLc,slope);
//					printf("4 XBc, YBc={%f,%f}\n", XBc,YBc);  
		
					if ( ( M[0].x < XBc && XBc < M[1].x ) && (a.y < d.y ) ) 
					{
						c.x = XBc;
						c.y = YBc;
//						printf("4 Xc,Yc = {%f,%f}\n", c.x, c.y); 
					}

				}

			}
		}

	}
	else if ( (flag == upper_mesh) ) 
	{
//		printf("upper_mesh\n");
		
		M[1].x = M[0].x;
		M[1].y = M[0].y + 1.0;
		M[2].x = M[1].x + 1.0; 

		if (d.x == a.x )
		{
			if ( a.y >  d.y )
			{
				c.x = d.x;
				c.y = M[1].y;
			} 
			else if (a.y < d.y)
			{
				c.x = d.x;
				c.y = d.x * (-1.0)  + M[1].x + M[1].y;
			} 
		} 
		else if (d.y == a.y )
		{
			if (a.x > d.x )
			{
				c.x = M[2].x;
				c.y = d.y;
			}
			else if ( a.x < d.x)
			{
				c.x = d.y * (-1.0)  + M[1].x + M[1].y;
				c.y = d.y;  
			}
		} 
		else if ( (d.x + d.y) == (a.x + a.y) && ( a.x > d.x ) )   // check 
		{
			c.x = M[2].x;
			c.y = M[2].x * (-1.0)  + d.x + d.y;
//			printf("case 3\n");	
                }
                else if ( (d.x + d.y) == (a.x + a.y) && ( a.x < d.x ) )
		{
			c.y = M[1].y;
			c.x = M[1].y * (-1.0) + d.x + d.y;
//			printf("case 4\n");
		}
		else
		{

			XMc = ( d.x * (d.y - a.y) + (d.x -a.x)*(M[1].x+M[1].y - d.y))/((d.x-a.x)+(d.y-a.y));        
			YMc = XMc * (-1.0) + M[1].x + M[1].y;

			printf(" XMC, YMC = %f,%f\n", XMc, YMc);

//			printf(" M[0].x, M[2].x, M[0].y, a.y, d.y : %f,%f,%f,%f,%f \n", M[0].x, M[2].x, M[0].y, a.y , a.x );

//			if ( ( M[0].x < XMc && XMc < M[1].x ) && ( M[0].y < YMc && YMc < M[1].y) )
			if ( ( M[0].x < XMc && XMc < M[2].x ) && ( M[0].y < YMc ) && ( a.y < d.y) )
			{
				c.x = XMc;
				c.y = YMc;
                                printf("2 Xc,Yc = {%f,%f}\n", c.x, c.y);
			}
			else	
			{

				XRc = M[2].x;
				YRc = slope * (XRc - d.x ) + d.y;

//				if ( M[0].y < YRc && YRc < M[1].y &&  a.x >  d.x ) 
				if ( M[0].y < YRc && YRc < M[1].y &&  a.x >  d.x ) 
				{
					c.x = XRc;
					c.y = YRc;
                                        printf("3 Xc,Yc = {%f,%f}\n", c.x, c.y);
				} 	
				else
				{	
					YTc = M[1].y;
//					XTc = (YTc-d.y) / slope + d.x;
					XTc = (M[1].y - d.y) / slope + d.x;

					if ( ( M[0].x < XTc && XTc < M[2].x ) && (a.y > d.y ) ) 
					{
						c.x = XTc;
						c.y = YTc;
                                                printf("4 Xc,Yc = {%f,%f}\n", c.x, c.y);
					}

				}
			}
		}
	}

	return c;
}

/*
  Input : dx - depature position X
          dy - depature position Y
          ax - arrival position X
          ay - arrival position Y

  Out :   Print out : contact position X,Y 

*/

void search_point(double dx, double dy, double ax, double ay)
{

   position_type pa,pd,c;

   pd.x = dx;
   pd.y = dy;
   pa.x = ax;
   pa.y = ay;

   c=find_cross(pd,pa);
   PRN_RESULT;
}

#ifdef RUN_CROSS
void  main( void )
{

#if 0
     printf("\n\n lower mesh  *******************************************\n");
     search_point(1.2,1.1,1.3,1.2); 
     search_point(1.2,1.1,1.2,2.6); 
     search_point(1.2,1.1,1.2,0.4); 
     search_point(1.2,1.1,0.6,1.1); 
     search_point(1.2,1.1,2.6,1.1); 
     search_point(1.2,1.1,0.8,1.5); 
     search_point(1.2,1.1,1.5,0.8); 
     search_point(1.2,1.1,1.5,2.6); 
     search_point(1.2,1.1,0.7,1.3); 
     search_point(1.2,1.1,1.1,0.3); 
     search_point(1.2,1.1,2.6,0.9); 
     
     printf("\n\n upper mesh *******************************************\n");
     search_point(1.8,1.9,1.8,2.4); 
     search_point(1.8,1.9,1.8,0.9); 
     search_point(1.8,1.9,2.8,1.9); 
     search_point(1.8,1.9,0.8,1.9); 
     search_point(1.8,1.9,1.5,2.2); 
     search_point(1.8,1.9,2.2,1.5); 
     search_point(1.8,1.9,1.4,1.3); 
     search_point(1.8,1.9,2.8,1.3); 
     search_point(1.8,1.9,1.7,2.1); 
#else

// search_point(1.2,2.2,1.215046,2.223799); 
//     search_point(1.215046,2.223799,1.189741,2.283249); 
//     search_point(0.900056,2.675294,0.767555,2.842444 ); 
     search_point(1.2,1.1,1.3,1.2); 
     search_point(20.2,10.2,20.23,10.22); 
     #endif 

}
#endif
