/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_STEP_ISPH_PPS_POZORSKI
#define SPH_STEP_ISPH_PPS_POZORSKI

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "../sph.h"
#include "../hlp.h"

void find_b_part(int *, int *);
void set_linked_list(int *, int *, Particle*, Parameters*);
void set_rhoe(int*, int*, Particle*, Parameters*, void (*function) (unsigned int, unsigned int, unsigned int, Particle*, Parameters*));
void clean_rh(Particle*, Parameters*);
void volforce(int *, int *, Particle*, Parameters*);
void press_gauge(Particle*, Parameters*);

void viscosity(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void dev_v2(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void press(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void hpress(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void density(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void volume(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);

void surface_tension(int *, int *, Particle*, Parameters*);
void linear(double *, int);

void dev_v_sparse(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void dev_v_sparse_density(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);

void isph_pps_pozorski(Particle* p, Parameters* par)
{
	int i;
	double *P, *P2;
	double mean_p;
	int *head, *list;

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
		for (i = 0; i<par->N; i++) {
			p[i].di = p[i].m * rh[i].d;
			p[i].d = p[i].di;
			p[i].dr = p[i].di;
		}
	}

	clean_rh(p, par);

	if (par->T_HYDROSTATIC_PRESSURE != 0) {
		volforce(head, list, p, par);
		if (par->T_HYDROSTATIC_PRESSURE == 1) set_rhoe(head, list, p, par, &hpress);
	}

	set_rhoe(head, list, p, par, &viscosity);

	if (par->T_SURFACE_TENSION > 0) surface_tension(head, list, p, par);

#ifdef _OPENMP
	#pragma omp parallel for private(i) shared(par, p, rh)
#endif
	for (i = 0; i<par->N; i++) {
		p[i].vel.x += par->DT * ((p[i].st.x + rh[i].u) + (par->G_X + rh[i].fx));
		p[i].vel.y += par->DT * ((p[i].st.y + rh[i].v) + (par->G_Y + rh[i].fy));
	}

	clean_rh(p, par);

	b = (double *)calloc(par->N, sizeof(double));
	P = (double *)calloc(par->N, sizeof(double));
	P2 = (double *)calloc(par->N, sizeof(double));
	ncol = (int *)calloc(par->N, sizeof(int));
	M = (atom **)malloc(par->N * sizeof(size_t));

	set_rhoe(head, list, p, par, &dev_v_sparse);

#ifdef _OPENMP
	#pragma omp parallel for private(i) shared(par, p, b, P)
#endif
	for (i = 0; i < par->N; i++) {
		P[i] = p[i].p;
		b[i] = b[i] / par->DT;
	}

	linear(P, par->N);

	if (par->T_RENORMALIZE_PRESSURE == 1) {
		mean_p = 0.0;
		for (i = 0; i < par->N; i++) {
			p[i].p = P[i];
			mean_p += P[i];
		}
		mean_p = mean_p / (double)par->N;
		for (i = 0; i<par->N; i++) p[i].p -= mean_p;

		mean_p = 0.0;
		for (i = 0; i < par->N; i++) {
			if (mean_p > p[i].p) mean_p = p[i].p;
		}
		for (i = 0; i <par->N; i++) p[i].p -= mean_p;
	}

	clean_rh(p, par);
	set_rhoe(head, list, p, par, &press);

#ifdef _OPENMP
	#pragma omp parallel for private(i) shared(par, p, rh)
#endif
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

	for (i = 0; i<par->N; i++) {
		if (ncol[i] > 0) free(M[i]);
	}
	free(M);
	free(ncol);
	M = (atom **)malloc(par->N * sizeof(size_t));
	ncol = (int *)calloc(par->N, sizeof(int));

	set_linked_list(head, list, p, par);
	clean_rh(p, par);
	set_rhoe(head, list, p, par, &volume);
	for (i = 0; i<par->N; i++) {
		p[i].dr = p[i].m * rh[i].d;
		b[i] = 0.0;
	}

	clean_rh(p, par);
	set_rhoe(head, list, p, par, &dev_v_sparse_density);

	for (i = 0; i<par->N; i++) {
		P2[i] = p[i].pp;
		b[i] += (p[i].di-p[i].dr);
		/*printf("(%d %f), ", i, b[i]);*/
	}

	linear(P2, par->N);

	if (par->T_RENORMALIZE_PRESSURE == 1) {
		mean_p = 0.0;
		for (i = 0; i < par->N; i++) {
			p[i].p = P2[i];
			mean_p += P2[i];
		}
		mean_p = mean_p / (double)par->N;
		for (i = 0; i<par->N; i++) p[i].p -= mean_p;

		mean_p = 0.0;
		for (i = 0; i < par->N; i++) {
			if (mean_p > p[i].p) mean_p = p[i].p;
		}
		for (i = 0; i < par->N; i++) {
			p[i].p -= mean_p;
			p[i].pp = p[i].p;
		}
	}

	clean_rh(p, par);
	set_rhoe(head, list, p, par, &press);

	for (i = 0; i < par->N; i++) {
		p[i].pos.x= p[i].pos.x+ rh[i].u;/* * pow2(DT) * 0.5;*/
		p[i].pos.y= p[i].pos.y+ rh[i].v;/* * pow2(DT) * 0.5;*/
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

	clean_rh(p, par);
	set_rhoe(head, list, p, par, &volume);
	for (i = 0; i<par->N; i++) {
		p[i].d = p[i].m * rh[i].d;
	}

	for (i = 0; i<par->N; i++) p[i].p = P[i];

	for (i = 0; i<par->N; i++) {
		if (ncol[i] > 0) free(M[i]);
	}
	free(M);
	free(ncol);
	free(b);
	free(P);
	free(P2);
	free(head);
	free(list);
}

#endif

