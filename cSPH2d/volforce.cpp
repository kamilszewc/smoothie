/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_MESH
#define SPH_MESH

#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "sph.h"
#include "hlp.h"

double kernel_volume(double q) 
{
	double k;
	/*if (q <= 2.0)
		k = 0.375 * q * q - 1.5 * q + 1.5;
	else
		k = 0.0;*/
	if (q <= 2.0)
		k = -0.5*q + 1.0;
	else
		k = 0.0;
	return k;
}

double xxx(int k, int ci, int cj, double DX, double DY, int nx, int ny, double *fcx, double *fcy, Particle* p, Parameters* par)
{
	int cc;
	double q, kern, xx, yy;

	cc = ci + cj * nx;
	xx = par->XCV * (((double)(ci)+0.5) / (double)(nx));
	yy = par->YCV * (((double)(cj)+0.5) / (double)(ny));
	q = sqrt( pow2(p[k].pos.x- xx) + pow2(p[k].pos.y- yy) ) / (0.5*DX);
	kern = kernel_volume(q);
	rh[k].fx += fcx[cc] * kern;
	rh[k].fy += fcy[cc] * kern;
	return kern;	
}

struct {                        /*        u        */
	double u;                 /*    ---------    */
	double d;                 /*    |       |    */
	double l;                 /*  l |       | r  */
	double r;                 /*    |       |    */
} typedef pressure;             /*    ---------    */
                                /*        d        */
void volforce(int *head, int *list, Particle* p, Parameters* par)
{
	int i, j, k, c;
	double x,y,dx,dy;

	int nx = par->NCX * 2;//(int)((double)(par->XCV / par->DR) / PART_IN_GRAVIT_CELL); /* nx, ny - num. of cells */
	int ny = par->NCY * 2;//(int)((double)(par->YCV / par->DR) / PART_IN_GRAVIT_CELL); /* to comp gravit. force  */
	double DX = par->XCV / (double)(nx);
	double DY = par->YCV / (double)(ny);

	double *de = (double *)calloc(nx * ny, sizeof(double));
	int *num = (int *)calloc(nx * ny, sizeof(int));
	pressure *pe = (pressure *)calloc(nx * ny, sizeof(pressure));
	double *fcx = (double *)calloc(nx * ny, sizeof(double));
	double *fcy = (double *)calloc(nx * ny, sizeof(double));

	/* Begin: averaged density in cell (#meandensincell) */
	for (k = 0; k < par->N; k++) {
		i = (int)(p[k].pos.x/ DX);
		j = (int)(p[k].pos.y/ DY);
		c = i + nx * j;
		de[c] += p[k].d;
		num[c] += 1;
	}
	for (c = 0; c < nx * ny; c++) {
		if (num[c] > 0) {
			de[c] = de[c] / num[c];
		}
	}
	/* End: averaged density in cell */

	/* Begin: horizontal pressure (#horizpress) */
	for (c=0; c<nx*ny; c++) pe[c].u = APRESSURE;
	for (i=0; i<nx; i++) {
		for (j=ny-1; j>=0; j--) {
			c = i + nx*j;
			pe[c].d = pe[c].u - de[c] * DY * par->G_Y;
			if (c>=nx) pe[c-nx].u = pe[c].d;
		}
	}
	/* End: horizontal pressure */

	/* Begin: vertical pressure (#vertpress) */
	for (c=0; c<nx*ny; c++) {
		if ( (c%nx) == (nx-1) ) {
			if ((par->T_BOUNDARY_PERIODICITY != 0)) {
				pe[c].r = 0.25*(pe[c].u + pe[c].d + pe[c-nx+1].u + pe[c-nx+1].d);
				pe[c-nx+1].l = pe[c].r;
			} else {
				pe[c].r = 0.5*(pe[c].u + pe[c].d);
			}
		} else {
			pe[c].r = 0.25*(pe[c].u + pe[c].d + pe[c+1].u + pe[c+1].d); 
			if ((par->T_BOUNDARY_PERIODICITY == 0) && (c%nx == 0)) {
				pe[c].l = 0.5*(pe[c].u + pe[c].d);
			}
			pe[c+1].l = pe[c].r;
		}
	}
	/* End: vertical pressure */

	switch (par->T_HYDROSTATIC_PRESSURE) {
	case 1:
		/* Begin: interpolation on Particle (#intofpressureincell) */
		for (k = 0; k<par->N; k++) {
			double px, py;
			i = (int)(p[k].pos.x/ DX);
			j = (int)(p[k].pos.y/ DY);
			c = i + nx * j;
			x = par->XCV *( (double)(c%nx) / (double)(nx) );
			y = par->YCV *((double)((c - c%nx) / nx) / (double)(ny));
			dx = p[k].pos.x- x;
			dy = p[k].pos.y- y;
			px = ((pe[c].r - pe[c].l)/DX)*dx + pe[c].l;
			py = ((pe[c].u - pe[c].d)/DY)*dy + pe[c].d;
			p[k].hp = 0.5*(px + py);
		}
		/* End: interpolation on Particle */
		break;
	case 0:
		/* Begin: comp forces in centers of cells (#hydroforceincell) */
#ifdef _OPENMP
	#pragma omp parallel for private(c) shared(fcx, fcy, pe, DX, DY, nx, ny)
#endif
		for (c=0; c<nx*ny; c++) {
			fcx[c] = (pe[c].l - pe[c].r)/DX;
			fcy[c] = (pe[c].d - pe[c].u)/DY;
		}
		/* End: comp forces in centers of cells */
	

		/* Begin: force interpolation on Particle (#forceintonpart) */
#ifdef _OPENMP
	#pragma omp parallel for private(k, i, j, c, x, y, dx, dy) shared(p, par, nx, ny, DX, DY, rh, fcx, fcy)
#endif
		for (k = 0; k < par->N; k++) {
			int na=-1, nb=-1, nc=-1, nd=-1;
			double fxu, fxd, fyu, fyd;
			i = (int)(p[k].pos.x/ DX);
			j = (int)(p[k].pos.y/ DY);
			c = i + nx * j;
			x = par->XCV * (((double)(c%nx) + 0.5) / (double)(nx));
			y = par->YCV * (((double)((c - c%nx) / nx) + 0.5) / (double)(ny));
			dx = p[k].pos.x- x;
			dy = p[k].pos.y- y;

			if ( (dx >= 0.0) && (dy >= 0.0) ) { /* I */
				na = c; nb = c+1; nc = c+nx+1; nd = c+nx;
				if ( (i==nx-1) && (j==ny-1) ) { na = c-nx-1; nb = c-nx; nc = c; nd = c-1;/*III*/ }
				if ( (i!=nx-1) && (j==ny-1) ) { na = c-nx; nb = c-nx+1; nc = c+1; nd = c;/*II*/ }
				if ( (i==nx-1) && (j!=ny-1) ) { na = c-1; nb = c; nc = c+nx; nd = c+nx-1;/*IV*/ } 
			}
			if ( (dx >= 0.0) && (dy < 0.0) ) { /* II */
				na = c-nx; nb = c-nx+1; nc = c+1; nd = c;
				if ( (i==nx-1) && (j==0) ) { na = c-1; nb = c; nc = c+nx; nd = c+nx-1;/*IV*/ };
				if ( (i!=nx-1) && (j==0) ) { na = c; nb = c+1; nc = c+nx+1; nd = c+nx;/*I*/ };
				if ( (i==nx-1) && (j!=0) ) { na = c-nx-1; nb = c-nx; nc = c; nd = c-1;/*III*/ };
			}
			if ( (dx < 0.0) && (dy < 0.0) ) { /* III */
				na = c-nx-1; nb = c-nx; nc = c; nd = c-1;
				if ( (i==0) && (j==0) ) { na = c; nb = c+1; nc = c+nx+1; nd = c+nx; /*I*/};
				if ( (i!=0) && (j==0) ) { na = c-1; nb = c; nc = c+nx; nd = c+nx-1; /*IV*/ };
				if ( (i==0) && (j!=0) ) { na = c-nx; nb = c-nx+1; nc = c+1; nd = c; /*II*/ };
			}
			if ( (dx < 0.0) && (dy >= 0.0) ) { /* IV */
				na = c-1; nb = c; nc = c+nx; nd = c+nx-1;
				if ( (i==0) && (j==ny-1) ) { na = c-nx; nb = c-nx+1; nc = c+1; nd = c;/*II*/ };
				if ( (i==0) && (j!=ny-1) ) { na = c; nb = c+1; nc = c+nx+1; nd = c+nx;/*I*/ };
				if ( (i!=0) && (j==ny-1) ) { na = c-nx-1; nb = c-nx; nc = c; nd = c-1;/*III*/ };
			}
			if (dx < 0.0) {
				fxu = ( (fcx[nc] - fcx[nd])*dx/DX ) + fcx[nc];
				fyu = ( (fcy[nc] - fcy[nd])*dx/DX ) + fcy[nc];
				fxd = ( (fcx[nb] - fcx[na])*dx/DX ) + fcx[nb];
				fyd = ( (fcy[nb] - fcy[na])*dx/DX ) + fcy[nb];
			} else {
				fxu = ( (fcx[nc] - fcx[nd])*dx/DX ) + fcx[nd];
				fyu = ( (fcy[nc] - fcy[nd])*dx/DX ) + fcy[nd];
				fxd = ( (fcx[nb] - fcx[na])*dx/DX ) + fcx[na];
				fyd = ( (fcy[nb] - fcy[na])*dx/DX ) + fcy[na];
			}
	
			if (dy < 0.0) {
				rh[k].fx = ( (fxu-fxd)*dy/DY + fxu )/p[k].d;
				rh[k].fy = ( (fyu-fyd)*dy/DY + fyu )/p[k].d;
			} else {
				rh[k].fx = ( (fxu-fxd)*dy/DY + fxd )/p[k].d;
				rh[k].fy = ( (fyu-fyd)*dy/DY + fyd )/p[k].d;
			}

		}
		/* End: force interpolation on Particle */


		/* Begin: zeroth order force interpolation on Particle () */
		/*for (k = 0; k < N; k++) {
			i = (int)(p[k].pos.x/ DX);
			j = (int)(p[k].pos.y/ DY);
			c = i + nx * j;
			rh[k].fx = fcx[c]/p[k].d;
			rh[k].fy = fcy[c]/p[k].d;
		}*/
		/* End: zeroth order force interpolation on Particle */
		break;
	case 2:
#ifdef _OPENMP
	#pragma omp parallel for private(c) shared(fcx, fcy, pe, DX, DY, nx, ny)
#endif
		for (c=0; c<nx*ny; c++) {
			fcx[c] = (pe[c].l - pe[c].r)/DX;
			fcy[c] = (pe[c].d - pe[c].u)/DY;
		}

/*#ifdef _OPENMP
	#pragma omp parallel for private(k)
#endif*/
		for (k = 0; k < par->N; k++) {
			double norm = 0.0;
			int ci, cj;

			ci = (int)(p[k].pos.x/ DX); /* Central cell */
			cj = (int)(p[k].pos.y/ DY);
			norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			ci = (int)(p[k].pos.x/ DX) - 1; /* Wall */
			cj = (int)(p[k].pos.y/ DY);
			if (ci >= 0) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			ci = (int)(p[k].pos.x/ DX) + 1; /* Wall */
			cj = (int)(p[k].pos.y/ DY);
			if (ci <= nx - 1) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			ci = (int)(p[k].pos.x/ DX); /* Wall */
			cj = (int)(p[k].pos.y/ DY) - 1;
			if (cj >= 0) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			ci = (int)(p[k].pos.x/ DX); /* Wall */
			cj = (int)(p[k].pos.y/ DY) + 1;
			if (cj <= ny - 1) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			ci = (int)(p[k].pos.x/ DX) - 1; /* Corner */
			cj = (int)(p[k].pos.y/ DY) - 1;
			if ((ci >= 0) && (cj >= 0)) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			ci = (int)(p[k].pos.x/ DX) + 1; /* Corner */
			cj = (int)(p[k].pos.y/ DY) + 1;
			if ((ci <= nx - 1) && (cj <= ny - 1)) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);
			ci = (int)(p[k].pos.x/ DX) - 1; /* Corner */
			cj = (int)(p[k].pos.y/ DY) + 1;
			if ((ci >= 0) && (cj <= ny - 1)) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			ci = (int)(p[k].pos.x/ DX) + 1; /* Corner */
			cj = (int)(p[k].pos.y/ DY) - 1;
			if ((ci <= nx - 1) && (cj >= 0)) norm += xxx(k, ci, cj, DX, DY, nx, ny, fcx, fcy, p, par);

			rh[k].fx = rh[k].fx / (norm*p[k].d);
			rh[k].fy = rh[k].fy / (norm*p[k].d);
		}
		break;
	default:
		break;
	}

	free(de);
	free(num);
	free(pe);
	free(fcx);
	free(fcy);
}

#endif
