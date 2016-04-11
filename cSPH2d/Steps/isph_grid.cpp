/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_STEP_ISPH_GRID
#define SPH_STEP_ISPH_GRID

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "../sph.h"

void find_b_part(int *, int *);
void set_linked_list(int *, int *, Particle*, Parameters*);
void set_rhoe(int*, int*, Particle*, Parameters*, void (*function) (unsigned int, unsigned int, unsigned int, Particle*, Parameters*));
void clean_rh(Particle*, Parameters*);
void volforce(int *, int *, Particle*, Parameters*);
void press_gauge(Particle*, Parameters*);

void surface_tension(int *, int *, Particle*, Parameters*);

void viscosity(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void dev_v2(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void curvature(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void press(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void hpress(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void density(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void volume(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);

void solver2(double *, double *, int, int, double, int, Parameters*);
void general(double *, double *, double *, int, int, double, int, Parameters*);

void isph_grid(Particle* p, Parameters* par)
{
	int i,j,k,c,xc,yc,*num;
	double *P, *dens, *C;
	int *bound, *cell;
	int *head, *list;

	int nx, ny;
	double DX, DY, dx, dy, x, y;
	double pu, pd;
	int na, nb, nc, nd;

	bound = (int *)malloc(par->N * sizeof(int));
	cell = (int *)malloc(par->N * sizeof(int));

	head = (int *)calloc(par->NC, sizeof(int));
	list = (int *)calloc(par->N, sizeof(int));
	set_linked_list(head, list, p, par);

	if (par->T_SURFACE_TENSION > 0) {
		for (i = 0; i < par->N; i++) {
			p[i].n.x = 0.0;
			p[i].n.y = 0.0;
			p[i].cu = 0.0;
		}
	}

	if ( (STEP==1) && (S_RESTARTED==0) ) {
		clean_rh(p, par);
		set_rhoe(head, list, p, par, &volume);
		for (i=0; i<par->N; i++) {
			p[i].di = p[i].m * rh[i].d;
			p[i].d = p[i].di;
			p[i].dr = p[i].di;
		}
	}

	// TODO
	/*switch (T_WCSPH) {
	case 0:
		clean_rh(p, par);
		set_rhoe(head, list, p, par, &dev_v2);
		for (i = 0; i < par->N; i++) { p[i].d += par->DT * rh[i].d; };
		break;
	case 1:
		clean_rh(p, par);
		set_rhoe(head, list, p, par, &density);
		for (i = 0; i < par->N; i++) {p[i].d = rh[i].d;};
		break;
	case 2:
		break;
	default:
		perror("Blad");
		exit(EXIT_FAILURE);
	}*/
	clean_rh(p, par);

	if (par->T_HYDROSTATIC_PRESSURE != 0) {
		volforce(head, list, p, par);
		if (par->T_HYDROSTATIC_PRESSURE == 1) set_rhoe(head, list, p, par, &hpress);
	}

	set_rhoe(head, list, p, par, &viscosity);

	if (par->T_SURFACE_TENSION > 0) surface_tension(head, list, p, par);

	for (i = 0; i < par->N; i++) {
		p[i].vel.x += par->DT * ((p[i].st.x + rh[i].u) + (par->G_X + rh[i].fx));
		p[i].vel.y += par->DT * ((p[i].st.y + rh[i].v) + (par->G_Y + rh[i].fy));
	}

	nx = (int)(par->NX / Cell);
	ny = (int)(par->NY / Cell);
	DX = par->XCV / (double)(nx);
	DY = par->YCV / (double)(ny);

	P = (double *)calloc(nx*ny, sizeof(double));
	b = (double *)calloc(nx*ny, sizeof(double));
	C = (double *)calloc(nx*ny, sizeof(double));
	dens = (double *)calloc(nx*ny, sizeof(double));
	num = (int *)calloc(nx*ny, sizeof(int));

	clean_rh(p, par);
	set_linked_list(head, list, p, par);
	set_rhoe(head, list, p, par, &dev_v2);

	for (i=0; i<par->N; i++) p[i].du = rh[i].d/p[i].d;

	for (i = 0; i < par->N; i++) {
		xc = (int)(p[i].pos.x/ DX);
		yc = (int)(p[i].pos.y/ DY);
		c = xc + nx * yc;
		P[c] += p[i].p;
		b[c] += rh[i].d / (par->DT*p[i].d);
		dens[c] += p[i].d;
		num[c] += 1;
	}
	for (i = 0; i < nx * ny; i++) {
		if (num[i] > 0) {
			P[i] = P[i] / num[i];
			b[i] = b[i] / num[i];
			dens[i] = dens[i] / num[i];
		}
		C[i] = 1.0/dens[i];
	}

	solver2(P,b,nx,ny,DX,0, par);
	/*general(P,b,C,nx,ny,DX,0);*/

	for (k = 0; k < par->N; k++) {

		i = (int)(p[k].pos.x/ DX);
		j = (int)(p[k].pos.y/ DY);
		c = i + nx * j;
		x = par->XCV * (((double)(c%nx) + 0.5) / (double)(nx));
		y = par->YCV * (((double)((c - c%nx) / nx) + 0.5) / (double)(ny));
		dx = p[k].pos.x- x;
		dy = p[k].pos.y- y;

		na=-1, nb=-1, nc=-1, nd=-1;
		if ( (dx >= 0.0) && (dy >= 0.0) ) {
			na = c; nb = c+1; nc = c+nx+1; nd = c+nx;
			if ( (i==nx-1) && (j==ny-1) ) { na = c-nx-1; nb = c-nx; nc = c; nd = c-1; }
			if ( (i!=nx-1) && (j==ny-1) ) { na = c-nx; nb = c-nx+1; nc = c+1; nd = c; }
			if ( (i==nx-1) && (j!=ny-1) ) { na = c-1; nb = c; nc = c+nx; nd = c+nx-1; } 
		}
		if ( (dx >= 0.0) && (dy < 0.0) ) {
			na = c-nx; nb = c-nx+1; nc = c+1; nd = c;
			if ( (i==nx-1) && (j==0) ) { na = c-1; nb = c; nc = c+nx; nd = c+nx-1; };
			if ( (i!=nx-1) && (j==0) ) { na = c; nb = c+1; nc = c+nx+1; nd = c+nx; };
			if ( (i==nx-1) && (j!=0) ) { na = c-nx-1; nb = c-nx; nc = c; nd = c-1; };
		}
		if ( (dx < 0.0) && (dy < 0.0) ) {
			na = c-nx-1; nb = c-nx; nc = c; nd = c-1;
			if ( (i==0) && (j==0) ) { na = c; nb = c+1; nc = c+nx+1; nd = c+nx; };
			if ( (i!=0) && (j==0) ) { na = c-1; nb = c; nc = c+nx; nd = c+nx-1; };
			if ( (i==0) && (j!=0) ) { na = c-nx; nb = c-nx+1; nc = c+1; nd = c; };
		}
		if ( (dx < 0.0) && (dy >= 0.0) ) {
			na = c-1; nb = c; nc = c+nx; nd = c+nx-1;
			if ( (i==0) && (j==ny-1) ) { na = c-nx; nb = c-nx+1; nc = c+1; nd = c; };
			if ( (i==0) && (j!=ny-1) ) { na = c; nb = c+1; nc = c+nx+1; nd = c+nx; };
			if ( (i!=0) && (j==ny-1) ) { na = c-nx-1; nb = c-nx; nc = c; nd = c-1; };
		}
		if (dx < 0.0) {
			pu = ( (P[nc] - P[nd])*dx/DX ) + P[nc];
			pd = ( (P[nb] - P[na])*dx/DX ) + P[nb];
		} else {
			pu = ( (P[nc] - P[nd])*dx/DX ) + P[nd];
			pd = ( (P[nb] - P[na])*dx/DX ) + P[na];
		}

		if (dy < 0.0) {
			p[k].p = ( (pu-pd)*dy/DY ) + pu;
		} else {
			p[k].p = ( (pu-pd)*dy/DY ) + pd;
		}
	}

	/*for (i=0; i<N; i++) {
		xc = (int)(p[i].pos.x/ DX);
		yc = (int)(p[i].pos.y/ DY);
		c = xc + nx * yc;
		p[i].p = P[c];
	}*/

	if (par->T_RENORMALIZE_PRESSURE == 1) press_gauge(p, par);

	clean_rh(p, par);
	set_rhoe(head, list, p, par, &press);

	for (i = 0; i < par->N; i++) {
		p[i].vel.x += par->DT * rh[i].u;
		p[i].vel.y += par->DT * rh[i].v;
		p[i].pos.x = p[i].pos.x + p[i].vel.x * par->DT;
		p[i].pos.y = p[i].pos.y + p[i].vel.y * par->DT;
		if (par->T_BOUNDARY_PERIODICITY != 0) {
			if (p[i].pos.x> par->XCV) p[i].pos.x += -par->XCV;
			if (p[i].pos.x <= 0.0) p[i].pos.x += par->XCV;
		} else {
			if (p[i].pos.x> par->XCV) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x = 2.0 * par->XCV - p[i].pos.x;
			};
			if (p[i].pos.x<= 0.0) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x = -p[i].pos.x;
			};
		}
		if (par->T_BOUNDARY_PERIODICITY == 1) {
			if (p[i].pos.y> par->YCV) p[i].pos.y += -par->YCV;
			if (p[i].pos.y <= 0.0) p[i].pos.y += par->YCV;
		} else {
			if (p[i].pos.y> par->YCV) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y = 2.0 * par->YCV - p[i].pos.y;
			};
			if (p[i].pos.y<= 0.0) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y = -p[i].pos.y;
			};
		}
	}

	free(P);
	free(b);
	free(C);
	free(num);
	free(bound);
	free(cell);
	free(head);
	free(list);
}

#endif
