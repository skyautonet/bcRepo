#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define Mesh_ResolutionX    0.1
//#define Mesh_ResolutionY    0.1
#define DGM                 0.1 // 그린근처의 격자구조 간격

#define HEIGHT_FILE "height_0_0_x_387_442_y_194_251_100mm.csv"
#define ATTRIB_FILE "attr_0_0_x_387_442_y_194_251_100mm.csv"

typedef enum 
{
    GREEN=1,
    FAIRWAY,
    ROUGH,
    BUNKER,
    TBOX,
    HAZARD,
    ROAD,
    OBJECT,
    OUTBOUND 

} terrain_type;

typedef struct _mesh_frame
{
    double x;
    double y;
    double z;
    terrain_type terrain;
    double nx1;
    double ny1;
    double nz1;
    double nx2;
    double ny2;
    double nz2;   
} mesh_frame;


typedef struct _normal_vector
{
    double nx1;
    double ny1;
    double nz1;
        
    double nx2;
    double ny2;
    double nz2;
} normal_vector;

static int kkk;
void get_normal_vector( double* z, normal_vector* nv)  
{
    double n1,n2;

//    printf("%s start [%d] \n",__func__, kkk++);

    n1 = sqrt( pow( z[1] - z[2] , 2 ) + pow ( z[1] - z[3] , 2 ) + pow(DGM, 2)) ; 
    n2 = sqrt( pow( z[4] - z[3] , 2 ) + pow ( z[4] - z[2] , 2 ) + pow(DGM, 2)) ;

    nv->nx1 = ( z[1] - z[2] ) / n1;   
    nv->ny1 = ( z[1] - z[3] ) / n1;   
    nv->nz1 =  DGM / n1;
    
    nv->nx2 = ( z[4] - z[3] ) / n2;   
    nv->ny2 = ( z[4] - z[2] ) / n2;   
    nv->nz2 = DGM / n2;   

#if 0
    printf("z1: %f,z2: %f,z3: %f, z4: %f\n",z[1],z[2],z[3],z[4]); 
    printf("n1 : %f, n2: %f\n", n1, n2);
    printf("nx1: %f, ny1:%f, nz1:%f\n", lower_normal_vector->nx1,lower_normal_vector->ny1,lower_normal_vector->nz1);
    printf("nx2: %f, ny2:%f, nz2:%f\n", upper_normal_vector->nx2,upper_normal_vector->ny2,upper_normal_vector->nz2);
    printf("---------------------------------------------\n");
#endif    
}

int get_csv(int type, int xSize, int ySize, mesh_frame** MG)
{
   FILE *fp;
   int cnt,i,j;
   int bSize;
   char* fname, *p;
   char *str_tmp;
   double min_h=0;  
         
   i=j=0; 
   

   if (type == 0 ) // get height
   {
     bSize=10*xSize+100;
     fname = HEIGHT_FILE;
   }
   else
   {
     bSize=2*xSize+100;
     fname = ATTRIB_FILE;
   }

   str_tmp=malloc(bSize);

   fp = fopen(fname,"r"); 
   
   if (fp<0)
   {
     printf("%s file not found\n", fname);    
     return -1;    
   }

   printf("get_csv %s\n",fname);      

   while( !feof(fp) && i < ySize )
   {
       fgets(str_tmp,bSize,fp);
       p = strtok(str_tmp,",");


       while (p!= NULL && j< xSize )
       {
            if(type) 
              MG[i][j].terrain=atoi(p);
            else
            {
              MG[i][j].z=atof(p);
              if (MG[i][j].z <= min_h || (i==0 && j==0) )
                  min_h = MG[i][j].z;
            }
              
            j++;
            p=strtok(NULL,",");
       }
       j=0;
       i++;
   }

//    MG[0][0].z = MG[0][0].z - min_h;
//    printf("min_h:%f mg(0,0):%f \n",min_h, MG[0][0].z );

    if (type==0)
     for(i=0;i<ySize;i++)
         for(j=0;j<xSize;j++)
             MG[i][j].z = (MG[i][j].z - min_h)/1000.0;

   free(str_tmp); 
   return 1;
}

initialize_mesh(mesh_frame** MG, int xSize, int ySize )
{
    int i,j;
    double z[5];

    printf("initialize_mesh start\n");

    normal_vector nv;

    for(i=0;i<ySize;i++)  // y
     for(j=0;j<xSize;j++)  // x
     {
        MG[i][j].y = i * DGM;
        MG[i][j].x = j * DGM;  
//        MG[i][j]->z = csv(i,j,1); 
//        MG[i][j]->terrain = csv(i,j,2);
     }

   
    for(i=0;i<ySize; i++) // y
     for(j=0;j<xSize; j++) // x
     {
         z[0] = 0;
         z[1] = MG[i][j].z;
          
         if (j+1>=xSize || i+1 >= ySize)
         {
             z[2]=0;
             z[3]=0; 
             z[4]=0;
         }
         else
         {
            z[2] = MG[i][j+1].z;
            z[3] = MG[i+1][j].z;
            z[4] = MG[i+1][j+1].z;  
         }

         get_normal_vector(z, &nv);      
         MG[i][j].nx1=nv.nx1;  
         MG[i][j].ny1=nv.ny1;  
         MG[i][j].nz1=nv.nz1;  
         MG[i][j].nx2=nv.nx2;  
         MG[i][j].ny2=nv.ny2;  
         MG[i][j].nz2=nv.nz2;  
     }

}

//void print_mesh(int x,int y, mesh_frame** MG)
void print_mesh(double a,double b, mesh_frame** MG)
{
    int x,y;

    x=(int)floor(a);
    y=(int)floor(b);

    printf("----Mesh Gride (%d,%d)---------\n",x,y);
    printf(" MG(%d,%d,1):%f\n",x,y,MG[y][x].x);
    printf(" MG(%d,%d,2):%f\n",x,y,MG[y][x].y);
    printf(" MG(%d,%d,3):%f\n",x,y,MG[y][x].z);
    printf(" MG(%d,%d,4):%d\n",x,y,MG[y][x].terrain);
    printf(" MG(%d,%d,5):%f\n",x,y,MG[y][x].nx1);
    printf(" MG(%d,%d,6):%f\n",x,y,MG[y][x].ny1);
    printf(" MG(%d,%d,7):%f\n",x,y,MG[y][x].nz1);
    printf(" MG(%d,%d,8):%f\n",x,y,MG[y][x].nx2);
    printf(" MG(%d,%d,9):%f\n",x,y,MG[y][x].ny2);
    printf(" MG(%d,%d,10):%f\n",x,y,MG[y][x].nz2);
    
}

#ifdef RUN_MESH
void main()
{
     mesh_frame** MG;
     int size,i;

     int xSize,ySize;

     xSize=570;
     ySize=550;

     size=xSize*ySize;
         
     MG = (mesh_frame**)malloc(ySize * sizeof(mesh_frame));
     
     for(i=0; i<ySize; i++)
        MG[i]=(mesh_frame*)malloc(xSize * sizeof(mesh_frame));
                 
     get_csv( 1,xSize,ySize,MG);  // get attrib
     get_csv( 0,xSize,ySize,MG);  // get height
    
     initialize_mesh(MG,xSize,ySize);


//     print_mesh(0.7,0.8,MG);
  //   print_mesh(1.2,2.2,MG);
  //   print_mesh(5.2,3.3,MG);
     print_mesh(102.2,52.2,MG);
     print_mesh(200.2,100.2,MG);
     print_mesh(201.2,100.2,MG);
     print_mesh(202.2,100.2,MG);
     print_mesh(203.2,100.2,MG);
     free(MG);
    
 }
#endif 

