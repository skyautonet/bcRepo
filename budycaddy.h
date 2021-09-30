#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
    double x;
    double y;
} position_type;

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


typedef struct _rta
{
    double EK;
    double psi;
    double kq;
    double Xq;
    double Yq;
    double Zq;
    double dhq;
    double ks;
    double Xs;
    double Ys;
    double Zs;
    double dhs;
    double kr;
    double Xr;
    double Yr;
    double Zr;
    double drk;
} rta_type;


typedef struct _bta
{
    double x;
    double y;
    double z;
    int    timestamp;
}bta_type;

position_type find_cross(position_type d, position_type a);
void search_point(double dx, double dy, double ax, double ay);
void get_normal_vector( double* z, normal_vector* nv);
void print_mesh(int x,int y, mesh_frame** MG);
//bool Rolling(int i,int j,double Xb,double Yb,double Zb,double Xh,double Yh,double Mug,double nz, rta_type** rta, bta_type*** bta, mesh_frame** MG);
void Rolling(int i,int j,double Ekin, double Apsi, bool* Sflag );
//int initial_putting_process(double Gspeed, double Xb,double Yb,double Zb,double Xh,double Yh,double Zh);
int initial_putting_process();
void display_bta(int i,int j, bta_type*** bta);


