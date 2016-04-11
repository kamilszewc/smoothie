/* All right reserved by Kamil Szewc, Gdansk 2009 */
#ifndef SPH_STEP_EULER
#define SPH_STEP_EULER

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
void hydrostatic_press_gauge(Particle*, Parameters*);
void surface_tension(int *, int *, Particle*, Parameters*);

void part_int(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void dev_v2(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void density(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void volume(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void shepard(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void shepard2(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void grad_a(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);

void add_Particle();

void wcsph(Particle* p, Parameters* par)
{
	int i;
	int *head, *list;
	
    double *s_fx, *s_fy, *s_stx, *s_sty, *s_u, *s_v, *s_d, *s_x, *s_y;

	head = (int *)calloc(par->NC, sizeof(int));
	list = (int *)calloc(par->N, sizeof(int));
	
	s_fx = (double *)calloc(par->N, sizeof(double));
	s_fy = (double *)calloc(par->N, sizeof(double));
	s_stx = (double *)calloc(par->N, sizeof(double));
	s_sty = (double *)calloc(par->N, sizeof(double));
	s_u = (double *)calloc(par->N, sizeof(double));
	s_v = (double *)calloc(par->N, sizeof(double));
	s_d = (double *)calloc(par->N, sizeof(double));
	s_x = (double *)calloc(par->N, sizeof(double));
	s_y = (double *)calloc(par->N, sizeof(double));

	set_linked_list(head, list, p, par);
	
	if (par->T_INTEGRATION_SCHEME > 0) {
		for (i = 0; i<par->N; i++) {
			ph[i].pos.x = p[i].pos.x; ph[i].pos.y = p[i].pos.y;
			ph[i].vel.x = p[i].vel.x; ph[i].vel.y = p[i].vel.y;
			ph[i].d = p[i].d; ph[i].p = p[i].p;
			ph[i].e = p[i].e; ph[i].t = p[i].t;
			if (STEP == 1) {
				pv[i].pos.x = p[i].pos.x; pv[i].pos.y = p[i].pos.y;
				pv[i].vel.x = p[i].vel.x; pv[i].vel.y = p[i].vel.y;
				pv[i].d = p[i].d; pv[i].p = p[i].p;
				pv[i].e = p[i].e; pv[i].t = p[i].t;
			}
		}
	}
		
	if (par->T_SURFACE_TENSION > 0) {
		for (i = 0; i < par->N; i++) {
			p[i].n.x = 0.0;
			p[i].n.y = 0.0;
			p[i].cu = 0.0;
			p[i].st.x = 0.0;
			p[i].st.y = 0.0;
		}
	}

	if ( (STEP==1) && (S_RESTARTED==0) ) {
		clean_rh(p, par);
		for (i = 0; i<par->N; i++) {
			p[i].vol = p[i].m / p[i].d;
			p[i].vol0 = p[i].m / p[i].d;
			p[i].vollog = 0.0;
		}
		//set_rhoe(head, list, p, par, &shepard);
		for (i = 0; i<par->N; i++) {
			//p[i].di = rh[i].dp / rh[i].dpc;
			//p[i].d = rh[i].dp / rh[i].dpc;
			//p[i].dr = rh[i].dp / rh[i].dpc;
			p[i].b = p[0].b;
		}
		clean_rh(p, par);
	}

	/* To comment or uncomment*/
	switch (par->T_MODEL) {
	case 0:
		break;
	case 1:
		break;
	case 3:
		break;
	case 2:
		clean_rh(p, par);
		set_rhoe(head, list, p, par, &volume);
		break;
	default:
		printf("Blad\n");
	}
	/* Up to */

#ifdef _OPENMP
#pragma omp parallel for private(i) shared(p, par, pv, ph, rh, STEP, T_BOUSSINESQ)
#endif
	for (i = 0; i < par->N; i++) {
		/* Density */
		switch (par->T_MODEL) {
		case 0:
			break;
		case 1:
			break;
		case 3:
			p[i].o = pow2(p[i].m / p[i].d);
			break;
		case 2:
			p[i].d = p[i].m * rh[i].d;
			p[i].o = pow2(1.0/rh[i].d);
			break;
		}
			
		if (p[i].d < ZERO)
		{
			printf("Problem with density (wcsph.cpp)\n");
			exit(1);
		}

		/*if (T_BOUSSINESQ == 2) {
			help = p[i].d/p[i].di;
			p[i].di = p[i].dr*(1.0 - 0.071*p[i].t);
			p[i].d = help*p[i].di;
			p[i].m = par->XCV * par->YCV * p[i].di/(double)par->N;
		}*/

		switch (par->T_MODEL) {
		case 0:
		case 1:
		case 2:
		case 3:
			p[i].p = p[i].b * (pow(p[i].d / p[i].di, p[i].gamma) - 1.0);
			break;
		}
	}

	if (par->T_RENORMALIZE_PRESSURE == 1) press_gauge(p, par);

	clean_rh(p, par);
	if (par->T_HYDROSTATIC_PRESSURE != 0) {
		volforce(head, list, p, par);
		if ((par->T_HYDROSTATIC_PRESSURE == 1) && (par->T_RENORMALIZE_PRESSURE == 1)) hydrostatic_press_gauge(p, par);
	}
	set_rhoe(head, list, p, par, &part_int);

	if (par->T_SURFACE_TENSION > 0) surface_tension(head, list, p, par);

#ifdef _OPENMP
	#pragma omp parallel for private(i) shared(par, p, ph, pv, rh, T_BOUSSINESQ, STEP)
#endif
	for (i = 0; i < par->N; i++) {
		/* Velocity */
		if ( (par->T_INTEGRATION_SCHEME == 0) || (par->T_INTEGRATION_SCHEME == 2) ) {
			p[i].vel.x += par->DT * ( (par->G_X + rh[i].fx) + (p[i].st.x + rh[i].u) + rh[i].uc );
			p[i].vel.y += par->DT * ( (par->G_Y + rh[i].fy) + (p[i].st.y + rh[i].v) + rh[i].vc );
			if ( (par->T_MODEL == 0) || (par->T_MODEL == 1) ) p[i].d += par->DT * rh[i].d;
			if (par->T_MODEL == 3) p[i].d += p[i].m * par->DT * rh[i].d;
			if (par->T_INTEGRATION_SCHEME == 2) {
			    s_u[i] = rh[i].u; s_v[i] = rh[i].v;
			    s_fx[i] = rh[i].fx; s_fy[i] = rh[i].fy;
			    s_stx[i] = p[i].st.x; s_sty[i] = p[i].st.y;
			    s_d[i] = rh[i].d;   
			    s_x[i] = rh[i].x; s_y[i] = rh[i].y;
			}
			
			if (par->T_HEAT_TRANSFER == 1) {
				if (par->T_MODEL < 2) {
					p[i].e += par->DT * rh[i].e;
				} else {
					p[i].e += par->DT * rh[i].e;
				}
				p[i].t = p[i].e/p[i].cv;
				//if (T_BOUSSINESQ == 1) {
				//	p[i].vel.y -= DT * 0.071 * G_Y * p[i].t; /* 0.071 -> 1000.0 */
				//}
			}
			p[i].pos.x += par->DT * (p[i].vel.x + rh[i].x);
			p[i].pos.y += par->DT * (p[i].vel.y + rh[i].y);
		}
		else if (par->T_INTEGRATION_SCHEME == 1) {
			ph[i].pos.x = p[i].pos.x; ph[i].pos.y = p[i].pos.y;
			ph[i].vel.x = p[i].vel.x; ph[i].vel.y = p[i].vel.y;
			ph[i].e = p[i].e; ph[i].t = p[i].t;
			
			if ( (STEP % 50 == 0) || (STEP == 1) ) {
				p[i].vel.x += par->DT * ((par->G_X + rh[i].fx) + (p[i].st.x + rh[i].u));
				p[i].vel.y += par->DT * ((par->G_Y + rh[i].fy) + (p[i].st.y + rh[i].v));
				if ((par->T_MODEL == 0) || (par->T_MODEL == 1)) p[i].d += par->DT * rh[i].d;
				if (par->T_HEAT_TRANSFER == 1) {
					p[i].e += par->DT * rh[i].e;
					p[i].t = p[i].e/p[i].cv;
					//if (T_BOUSSINESQ == 1) {
					//	p[i].vel.y -= DT * 0.071 * G_Y * p[i].t; /* 0.071 -> 1000.0 */
					//}
				}
			} else {
				p[i].vel.x = pv[i].vel.x + 2.0 * par->DT * ((par->G_X + rh[i].fx) + (p[i].st.x + rh[i].u));
				p[i].vel.y = pv[i].vel.y + 2.0 * par->DT * ((par->G_Y + rh[i].fy) + (p[i].st.y + rh[i].v));
				if ((par->T_MODEL == 0) || (par->T_MODEL == 1)) p[i].d = pv[i].d + 2.0 * par->DT * rh[i].d;
				if (par->T_HEAT_TRANSFER == 1) {
					p[i].e = pv[i].e + 2.0 * par->DT * rh[i].e;
					p[i].t = p[i].e/p[i].cv;
					//if (T_BOUSSINESQ == 1) {
					//	p[i].vel.y -= DT * 0.071 * (-100.0) * p[i].t; /* 0.071 -> 1000.0 */
					//}
				}
			}
			p[i].pos.x += par->DT * (p[i].vel.x + rh[i].x) + 0.5 * pow2(par->DT) * ((par->G_X + rh[i].fx) + (p[i].st.x + rh[i].u));
			p[i].pos.y += par->DT * (p[i].vel.y + rh[i].y) + 0.5 * pow2(par->DT) * ((par->G_Y + rh[i].fy) + (p[i].st.y + rh[i].v));
		} else {
			printf("ERROR\n");
		}
			
		if (par->T_BOUNDARY_PERIODICITY != 0) {
			if (p[i].pos.x> par->XCV)  p[i].pos.x += -par->XCV;
			if (p[i].pos.x <= 0.0) p[i].pos.x += par->XCV;
		} else {
			if (p[i].pos.x> par->XCV) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x = 2.0 * par->XCV - (p[i].pos.x + ZERO);
			};
			if (p[i].pos.x<= 0.0) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x= -(p[i].pos.x+ZERO);
			};
		}
		if (par->T_BOUNDARY_PERIODICITY == 1) {
			if (p[i].pos.y> par->YCV)  p[i].pos.y += -par->YCV;
			if (p[i].pos.y <= 0.0) p[i].pos.y += par->YCV;
		} else {
			if (p[i].pos.y> par->YCV) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y = 2.0 * par->YCV - (p[i].pos.y + ZERO);
			};	
			if (p[i].pos.y<= 0.0) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y= -(p[i].pos.y+ZERO);
			};
		}
	}
	
	if (par->T_SURFACE_TENSION > 0) {
		for (i = 0; i < par->N; i++) {
			p[i].n.x = 0.0;
			p[i].n.y = 0.0;
			p[i].cu = 0.0;
			p[i].st.x = 0.0;
			p[i].st.y = 0.0;
		}
	}
	
/* CORRECTOR */
	if (par->T_INTEGRATION_SCHEME == 2) {
	clean_rh(p, par);
	switch (par->T_MODEL) {
		case 0: 
			break;
		case 1:
			break;
		case 3:
			break;
		case 2:
			set_rhoe(head, list, p, par, &volume);
			break;
		default:
			printf("Blad\n");
	}

	
	
	#ifdef _OPENMP
	#pragma omp parallel for private(i) shared(par, p, pv, ph, rh, STEP, T_BOUSSINESQ)
#endif
	for (i = 0; i < par->N; i++) {
		/* Density */

		switch (par->T_MODEL) {
			case 0:
				break;
			case 1:
				break;
			case 3:
				p[i].o = pow2(p[i].m / p[i].d);
				break;
			case 2:
				p[i].d = p[i].m * rh[i].d;
				p[i].o = pow2(1.0/rh[i].d);
				break;
		}
			
		if (p[i].d < ZERO) printf("Problem with density (step.c)\n");

		/*if (T_BOUSSINESQ == 2) {
			help = p[i].d/p[i].di;
			p[i].di = p[i].dr*(1.0 - 0.071*p[i].t);
			p[i].d = help*p[i].di;
			p[i].m = XCV * YCV * p[i].di/(double)par->N;
		}*/

		switch (par->T_MODEL) {
		case 0:
		case 1:
		case 2:
		case 3:
			p[i].p = p[i].b * (pow(p[i].d / p[i].di, p[i].gamma) - 1.0);
			break;
		}
	}

	if (par->T_RENORMALIZE_PRESSURE == 1) press_gauge(p, par);

	clean_rh(p, par);
	if (par->T_HYDROSTATIC_PRESSURE != 0) {
		volforce(head, list, p, par);
		if ((par->T_HYDROSTATIC_PRESSURE == 1) && (par->T_RENORMALIZE_PRESSURE == 1)) hydrostatic_press_gauge(p, par);
	}
	set_rhoe(head, list, p, par, &part_int);

	if (par->T_SURFACE_TENSION > 0) surface_tension(head, list, p, par);
	
	for (i = 0; i < par->N; i++) {
		p[i].vel.x = ph[i].vel.x + 0.5 * par->DT * ((par->G_X + rh[i].fx) + (p[i].st.x + rh[i].u) + rh[i].uc +
					  (par->G_X + s_fx[i]) + (s_stx[i] + s_u[i])  );
		p[i].vel.y = ph[i].vel.y + 0.5 * par->DT * ((par->G_Y + rh[i].fy) + (p[i].st.y + rh[i].v) + rh[i].vc +
					  (par->G_Y + s_fy[i]) + (s_sty[i] + s_v[i])  );
		if (par->T_MODEL < 2) p[i].d = ph[i].d + 0.5 * par->DT * (rh[i].d + s_d[i]);
		if (par->T_MODEL == 3) p[i].d = ph[i].d + 0.5 * par->DT * (p[i].m*rh[i].d + s_d[i]);
		p[i].pos.x = ph[i].pos.x + 0.5 * par->DT * (p[i].vel.x + rh[i].x + ph[i].vel.x + s_x[i]);
		p[i].pos.y = ph[i].pos.y + 0.5 * par->DT * (p[i].vel.y + rh[i].y + ph[i].vel.y + s_y[i]);
	    
		if (par->T_BOUNDARY_PERIODICITY != 0) {
			if (p[i].pos.x> par->XCV)  p[i].pos.x += -par->XCV;
			if (p[i].pos.x <= 0.0) p[i].pos.x += par->XCV;
		} else {
			if (p[i].pos.x> par->XCV) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x = 2.0 * par->XCV - (p[i].pos.x + ZERO);
			};
			if (p[i].pos.x<= 0.0) {
				p[i].vel.x = -p[i].vel.x;
				p[i].pos.x= -(p[i].pos.x+ZERO);
			};
		}
		if (par->T_BOUNDARY_PERIODICITY == 1) {
			if (p[i].pos.y> par->YCV)  p[i].pos.y += -par->YCV;
			if (p[i].pos.y <= 0.0) p[i].pos.y += par->YCV;
		} else {
			if (p[i].pos.y> par->YCV) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y = 2.0 * par->YCV - (p[i].pos.y + ZERO);
			};	
			if (p[i].pos.y<= 0.0) {
				p[i].vel.y = -p[i].vel.y;
				p[i].pos.y= -(p[i].pos.y+ZERO);
			};
		}
	}
}
	
	if (par->T_INTEGRATION_SCHEME > 0) {
		for (i=0; i<par->N; i++) {
			pv[i].pos.x = ph[i].pos.x; pv[i].pos.y = ph[i].pos.y;
			pv[i].vel.x = ph[i].vel.x; pv[i].vel.y = ph[i].vel.y;
			pv[i].d = ph[i].d; pv[i].p = ph[i].p;
			pv[i].e = ph[i].e; pv[i].t = ph[i].t;
		}
	}
	
	free(head);
	free(list);
	free(s_fx); 
	free(s_fy);
	free(s_stx); 
	free(s_sty);
	free(s_u);
	free(s_v);
	free(s_d);
	free(s_x);
	free(s_y);
}

#endif
