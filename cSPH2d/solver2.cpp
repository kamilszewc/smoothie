/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_STEP_SOLVER2
#define SPH_STEP_SOLVER2

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sph.h"
#include "hlp.h"


double res(int nx, int ny, double *u, double *P, double dx, int T, Parameters* par)
{
	double err, sum;
	int c0, i, j;
	err = 0.0; sum = 0.0;
	for (i=0; i<nx; i++) {
		for (j=0; j<ny; j++) {
			c0 = i + nx * j;
			if (T==1) {
				P[c0] = u[c0];
			}
			sum = -dx*dx*b[c0];
			if (par->T_BOUNDARY_PERIODICITY == 0) {
				if (i!=0)    sum += u[c0-1];
				else         sum += P[c0];
				if (i!=nx-1) sum += u[c0+1];
				else         sum += P[c0];
			} else {
				if (i!=0)    sum += u[c0-1];
				else         sum += u[c0+nx-1];
				if (i!=nx-1) sum += u[c0+1];
				else         sum += u[c0-nx+1];
			}
			if (par->T_BOUNDARY_PERIODICITY != 1) {
				if (j!=0)    sum += u[c0-nx];
				else         sum += P[c0];
				if (j!=ny-1) sum += u[c0+nx];
				else         sum += P[c0];
			} else {
				if (j!=0)    sum += u[c0-nx];
				else         sum += u[i + nx * (ny-1)];
				if (j!=ny-1) sum += u[c0+nx];
				else         sum += u[i - nx * (ny-1)];
			}
			err += pow2( sum*0.25 - u[c0] );

		}
	}
	return sqrt(err/(double)(nx*ny));
}


void solver2(double *P, double *b, int nx, int ny, double dx, int T, Parameters* par)
{
	int i, j, k, c0;
	int M;
	double *u;

	u = (double *) malloc(nx*ny * sizeof(double));

	for (i=0; i<nx*ny; i++) u[i] = P[i];
	
	M = 200000;
	for (k=0; k<M; k++) {
		if (T==1) {
			for (i=0; i<nx; i++) {
				for (j=0; j<ny; j++) {
					c0 = i + nx*j;
					if(i==0)   P[c0] = (4.0*u[c0]-u[c0+1])/3.0;
					if(i==nx-1)P[c0] = (4.0*u[c0]-u[c0-1])/3.0;
					if(j==0)   P[c0] = (4.0*u[c0]-u[c0+nx])/3.0;
					if(j==ny-1)P[c0] = (4.0*u[c0]-u[c0-nx])/3.0;
					if( (i==0) && (j==ny-1) ) P[c0] = u[c0];
					if( (i==0) && (j==0) ) P[c0] = u[c0];
					if( (i==nx-1) && (j==ny-1) ) P[c0] = u[c0];
					if( (i==nx-1) && (j==0) ) P[c0] = u[c0];
				}
			}
			P[0] = dx*dx*b[0];
		}
		for (i=0; i<nx; i++) {
			for (j=0; j<ny; j++) {
				c0 = i  + nx * j;
				u[c0] = -dx*dx*b[c0];
				if (par->T_BOUNDARY_PERIODICITY == 0) {
					if (i!=0)    u[c0] += u[c0-1];
					else         u[c0] += P[c0];
					if (i!=nx-1) u[c0] += u[c0+1];
					else         u[c0] += P[c0];
				} else {
					if (i!=0)    u[c0] += u[c0-1];
					else         u[c0] += u[c0+nx-1];
					if (i!=nx-1) u[c0] += u[c0+1];
					else	     u[c0] += u[c0-nx+1];
				}	
				if (par->T_BOUNDARY_PERIODICITY != 1) {
					if (j!=0)    u[c0] += u[c0-nx];
					else         u[c0] += P[c0];
					if (j!=ny-1) u[c0] += u[c0+nx];
					else         u[c0] += P[c0];
				} else {
					if (j!=0)    u[c0] += u[c0-nx];
					else         u[c0] += u[i + nx*(ny-1)];
					if (j!=ny-1) u[c0] += u[c0+nx];
					else         u[c0] += u[i - nx*(ny-1)];
				}
				u[c0] = u[c0]*0.25;
			}
		}
		if (k%50 == 0) {
			if ( res(nx, ny, u, P, dx, T, par) < 1E-6) break;
		}
	}
	if (k==M) {
		printf("Poisson failed res=%e num=%d\n", res(nx, ny, u, P, dx, T, par), k);
		/*exit(EXIT_FAILURE);*/
	}


	printf("After res1=%e num=%d\n", res(nx, ny, u, P, dx, T, par), k);
	/*printf("Num=%d", k);*/

	for (i=0; i<nx*ny; i++) P[i] = u[i];

	free(u);
}

/* General Poisson Solver based on Tao Pang "Metody obliczeniowe w fizyce" */
void general(double *P, double *b, double *C, int nx, int ny, double dx, int T, Parameters* par)
{
	int i, j, k;
	int c0;
	int M = 20000;
	double *u, *ul, *ur, *uu, *ub;
	double sum;
	double omega = 1.4;
	u = (double *) malloc(nx*ny * sizeof(double));
	ul = (double *) malloc(ny * sizeof(double));
	ur = (double *) malloc(ny * sizeof(double));
	uu = (double *) malloc(nx * sizeof(double));
	ub = (double *) malloc(nx * sizeof(double));

	for (i=0; i<nx*ny; i++)	u[i] = P[i];
	if (T==0) {
	    for (i=0; i<nx; i++) {
			ub[i] = u[i];
			uu[i] = u[i+nx*(ny-1)];
	    }
	    for (j=0; j<ny; j++) {
			ul[j] = u[nx*j];
			ur[j] = u[nx-1+nx*j];
	    }
	}

	for (k=0; k<M; k++) {
		if (T==1) {
			for (i=0; i<nx; i++) {
				ub[i] = (4.0*u[i]-u[i+nx])/3.0;
				uu[i] = (4.0*u[i+nx*(ny-1)]-u[i+nx*(ny-2)])/3.0;
			}
			for (j=0; j<ny; j++) {
				ul[j] = (4.0*u[nx*j]-u[nx*j+1])/3.0;
				ur[j] = (4.0*u[nx-1+nx*j]-u[nx-2+nx*j])/3.0;
			}
			/*ub[5] = 0.0;*/
		}
		for (i=0; i<nx; i++) {
			for (j=0; j<ny; j++) {
				c0 = i  + nx * j;
				sum = -2.0*dx*dx*b[c0];
				if (par->T_BOUNDARY_PERIODICITY == 0) {
					if (i!=0)    sum += u[c0-1] * (C[c0]+C[c0-1]);
					else         sum += ul[j] * (C[c0] + C[c0]);
					if (i!=nx-1) sum += u[c0+1] * (C[c0] + C[c0+1]);
					else         sum += ur[j] * (C[c0] + C[c0]);
				} else {
					if (i!=0)    sum += u[c0-1] * (C[c0]+C[c0-1]);
					else         sum += u[c0+nx-1] * (C[c0]+C[c0+nx-1]);
					if (i!=nx-1) sum += u[c0+1]* (C[c0]+C[c0+1]);
					else	     sum += u[c0-nx+1]* (C[c0]+C[c0-nx+1]);
				}
				if (par->T_BOUNDARY_PERIODICITY != 1) {
					if (j!=0)    sum += u[c0-nx]* (C[c0]+C[c0-nx]);
					else         sum += ub[i]* (C[c0]+C[c0]);
					if (j!=ny-1) sum += u[c0+nx]* (C[c0]+C[c0+nx]);
					else         sum += uu[i]* (C[c0]+C[c0]);
				} else {
					if (j!=0)    sum += u[c0-nx]* (C[c0]+C[c0-nx]);
					else         sum += u[i + nx*(ny-1)]* (C[c0]+C[i + nx*(ny-1)]);
					if (j!=ny-1) sum += u[c0+nx]* (C[c0]+C[c0+nx]);
					else         sum += u[i - nx*(ny-1)]* (C[c0]+C[i - nx*(ny-1)]);
				}
				u[c0] = (1-omega)*u[c0] + omega*sum/(8.0*C[c0]);

			}
		}
	}

	for (i=0; i<nx*ny; i++) P[i] = u[i];

	free(u); free(ul); free(ur); free(uu); free(ub);
}
#endif
