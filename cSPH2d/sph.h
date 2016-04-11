/* All right reserved by Kamil Szewc, Gdansk 2009 */
#if !defined(__SPH_H__)
#define __SPH_H__
//#define double double
#include <vector>
#include "InterPhaseParameter.h"

#ifndef INLINE
 #if defined __STRICT_ANSI__
  #define INLINE static
 #elif defined _WIN32
  #define INLINE static __forceinline
 #else
  #define INLINE static inline
 #endif
#endif

typedef struct double2
{
	double x;
	double y;
} double2;

typedef struct double3
{
	double x;
	double y;
	double z;
} double3;

struct {
    /* Fundamental */
	int id;
	int phaseId;
    double2 pos;   /* y-Position */  /* z-Position (axi) */
	double2 rh_pos;
    double2 vel;   /* x-Velocity */  /* r-Velocity (axi) */
	double2 rh_vel;
    double m;   /* Mass       */
    double p;   /* Pressure   */ double pp;
    double hp;  /* Hydrostatic pressure */
    double d;   /* Density    */
	double rh_d;
    double di;  /* Initial density */
    double dr;  /* Ref density */ 
    double nu;  /* Viscosity */ double nu0;
    double mi;  /* Viscosity */ double mi0;
    double o;
    double a;  /* surface */
    double vol; 
    double vol0;
    double vollog;

    double t;   /* Temperature */
    double co;  /* Conductivity */
    double cv;
    double e;  /* Internal energy */

    /* WCSPH only */
    double gamma; /* Gamma coefficient */
    double s; /* Sound speed */
    double chi; /* Added in two-phase flows to use in confined domains */
    double b; /* Coefficient of state equation */

	/* TODO */
	double cs;
	double cw;

    /* Multiphase */
    double c;   /* Color function */
	double3 n;
    double nn;  /* sqrt(nx*nx + ny*ny) */
    int  na;  /* Logical: is it the interface ? (1-yes, 0-no) */
    double cu;  /* Curvature */
    double sigw; /* Surface tension coefficient */
	double2 st;

    /* Stat */
    double dv;  /* dev u (1)  */
    double du;  /* dev u (2)  */
    
    /* Different boundary conditions */
    int k;   /* Kind of Particles BC */
} typedef Particle;

struct {
	double x;  /* rhoe x-Position (for XSPH) */
	double y;  /* rhoe y-Position (for YSPH) */
	double u;  /* rhoe x-Velocity */ double uc;
	double v;  /* rhoe y-Velocity */ double vc;
	double d;  /* rhoe density */ double dc; double dp; double dpc;
	double b;  /* for help */
	double fx; /* x-additional force (volume like force) */
	double fy; /* y-additional force (volume like force) */
	double sig, sigw; /* surface tension coefficient */
	double is_00, is_10, is_01, is_11;  /* interface stress */
	double cs; /* smoothed color function */
	double csw;
	double cw; /* Morris ST-curvature weighting function */
	double e; /* rhoe internal energy */
	double ssr_00, ssr_01, ssr_10, ssr_11; /* shear strain rate */
	double qx, qy;
} typedef rhoe;

typedef struct Parameters {
	double HDR;
	double H, I_H, DH, DR;     //
	int T_MODEL;
	int NX, NY;     //
	int N;          //
	int NCX, NCY;   //
	int NC;         //
	int T_BOUNDARY_CONDITION;
	int T_BOUNDARY_PERIODICITY;
	int T_INTERFACE_CORRECTION;    //
	//double INTERFACE_CORRECTION;    //
	double XCV, YCV; //
	double V_N, V_E, V_S, V_W; //
	double G_X, G_Y;         
	int T_INTEGRATION_SCHEME;	
	int T_HYDROSTATIC_PRESSURE;
	double KNORM;    //
	double GKNORM;   //
	double DT;       //
	double END_TIME;     //
	double INTERVAL_TIME;
	int T_TIME_STEP;
	int T_SURFACE_TENSION;//  
	int T_RENORMALIZE_PRESSURE; //
	int T_XSPH;     //
	double XSPH;     //
	double T_HEAT_TRANSFER;
	int T_TEMP_N;
	int T_TEMP_E;
	int T_TEMP_S;
	int T_TEMP_W;
	double TEMP_N;
	double TEMP_E;
	double TEMP_S;
	double TEMP_W;
	std::vector<int> PHASES;
	InterPhaseParameter* SURFACE_TENSION_COEFFICIENT;
	InterPhaseParameter* INTERFACE_CORRECTION_COEFFICIENT;
} Parameters;

struct {
    double val;
    int col;
    char pad[4];
} typedef atom;

#ifndef SPH_MAIN

extern unsigned int NIC;             /* Number of Particles in cell (begining) */

extern double XCV_0;          /* x-Size domain begining */
extern double YCV_0;          /* y-Size dimain begining */

extern double H, H2, H3, H4;  /* Smoothing length */
extern double I_H, I_H2, I_H3, I_H4; /* I_H = 1.0/H, I_H2 = 1.0/H^2 */
extern double DH;
extern double KNORM;          /* The part of normalization coefficient of kernel */
extern double GKNORM;         /* The part of normalization coefficient of grad kernel */
extern double DR;             /* Particles initial spatial resolution */

extern unsigned int STEP;       /* Time step number */

extern unsigned int S_RESTARTED;

/*  Type of kernel:                */
/*  0 - gaussian, 1 - quadratic,   */
/*  2 - cubic spline, 3 - quintic  */
/*  4 - quintic Morris, Fox, Zhu   */
extern unsigned int T_KERNEL;

/* Linear solver: */
extern unsigned int T_LIN;


extern unsigned int T_OUTPUT;

/* Is the Bessinesq approx in use   */
/* 0 - off, 1 - standard Boussinesq */
/* 2 - new approach                 */
extern unsigned int T_BOUSSINESQ;

extern double APRESSURE; /* Atmospheric pressure */
extern double PART_IN_GRAVIT_CELL;
extern double EPS;

extern rhoe *rh;
extern double *b;
extern int *ncol;
extern atom **M;
extern int nopic;

//extern Particle *p;
extern Particle *pv, *ph;

extern Particle *_p;
extern double *x_position_memory;
extern double *y_position_memory;

extern double Cell;

#endif

#endif