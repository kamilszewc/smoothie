/* All right reserved by Kamil Szewc, Gdansk 2009 */
#ifndef SPH_STEP_TARTAKOVSKY
#define SPH_STEP_TARTAKOVSKY

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "../sph.h"
#include "../hlp.h"

void set_linked_list(int *, int *, Particle*, Parameters*);
void set_rhoe(int*, int*, Particle*, Parameters*, void (*function) (unsigned int, unsigned int, unsigned int, Particle*, Parameters*));
void clean_rh(Particle*, Parameters*);
void volforce(int *, int *, Particle*, Parameters*);
void press_gauge(Particle*, Parameters*);

void volume(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void part_int_tartakovsky(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);

void sphTartakovskyEtAl(Particle* p, Parameters* par)
{
	int i;
	int *head, *list;

	head = (int *)calloc(par->NC, sizeof(int));
	list = (int *)calloc(par->N, sizeof(int));

	set_linked_list(head, list, p, par);

	clean_rh(p, par);

	set_rhoe(head, list, p, par, &volume);

	
#ifdef _OPENMP
#pragma omp parallel for private(i) shared(p, par, pv, ph, rh, STEP)
#endif
	for (i = 0; i < par->N; i++) {
		double K = 1.0;
		double B = 0.5;
		double A = 2.0;

		/* Density */
		p[i].d = p[i].m * rh[i].d;
		p[i].o = rh[i].d;

		if (p[i].d < ZERO) printf("Problem with density (step.c)\n");

		p[i].p = p[i].b * (pow(p[i].d / p[i].di, p[i].gamma) - 1.0);

		//p[i].p = p[i].d * K * p[i].t / (1.0 - p[i].d * B);// -A * pow2(p[i].d);
	}

	clean_rh(p, par);

	set_rhoe(head, list, p, par, &part_int_tartakovsky);

	#ifdef _OPENMP
#pragma omp parallel for private(i) shared(p, par, ph, pv, rh, STEP)
#endif
	for (i = 0; i < par->N; i++) {
		p[i].vel.x += par->DT * (par->G_X + rh[i].u + rh[i].x);
		p[i].vel.y += par->DT * (par->G_Y + rh[i].v + rh[i].y);

		p[i].pos.x += par->DT * p[i].vel.x;
		p[i].pos.y += par->DT * p[i].vel.y;

		if (par->T_BOUNDARY_PERIODICITY != 0) {
			if (p[i].pos.x> par->XCV)  p[i].pos.x += -par->XCV;
			if (p[i].pos.x <= 0.0) p[i].pos.x += par->XCV;
		}
		else
		{
			if (p[i].pos.x> par->XCV) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x = 2.0 * par->XCV - (p[i].pos.x + ZERO);
			};
			if (p[i].pos.x<= 0.0) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x= -(p[i].pos.x+ ZERO);
			};
		}
		if (par->T_BOUNDARY_PERIODICITY == 1) {
			if (p[i].pos.y> par->YCV)  p[i].pos.y += -par->YCV;
			if (p[i].pos.y <= 0.0) p[i].pos.y += par->YCV;
		}
		else
		{
			if (p[i].pos.y> par->YCV) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y = 2.0 * par->YCV - (p[i].pos.y + ZERO);
			};
			if (p[i].pos.y<= 0.0) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y= -(p[i].pos.y+ ZERO);
			};
		}
	}

	free(head);
	free(list);
}

#endif