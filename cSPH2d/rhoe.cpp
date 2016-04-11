/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_RHOE
#define SPH_RHOE

#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "sph.h"

void set_linked_list(int *, int *);
void cell_int(const unsigned int ic, const unsigned int jc, int *, int *, Particle*, Parameters*, void (*function) (unsigned int, unsigned int, unsigned int, Particle*, Parameters*));

void clean_rh(Particle* p, Parameters* par)
{
	int i;
#ifdef _OPENMP
	#pragma omp parallel for private(i) shared(par,rh)
#endif
	for (i = 0; i < par->N; i++) {
		rh[i].x = 0.0;
		rh[i].y = 0.0;
		rh[i].u = 0.0; rh[i].uc = 0.0;
		rh[i].v = 0.0; rh[i].vc = 0.0;
		rh[i].d = 0.0; rh[i].dc = 0.0; rh[i].dp = 0.0; rh[i].dpc = 0.0;
		rh[i].b = 0.0;
		rh[i].fx = 0.0;
		rh[i].fy = 0.0;
		p[i].st.x = 0.0;
		p[i].st.y = 0.0;
		rh[i].sig = 0.0; rh[i].sigw = 0.0;
		rh[i].is_00 = 0.0; rh[i].is_11 = 0.0; rh[i].is_01 = 0.0; rh[i].is_10 = 0.0;
		rh[i].cs = 0.0;
		rh[i].csw = 0.0;
		rh[i].cw = 0.0;
		rh[i].e = 0.0;
		rh[i].ssr_00 = 0.0; rh[i].ssr_01 = 0.0; rh[i].ssr_10 = 0.0; rh[i].ssr_11 = 0.0;
		rh[i].qx = 0.0;
		rh[i].qy = 0.0;
	}
}

void set_rhoe(int *head, int *list, Particle* p, Parameters* par, void (*function) (unsigned int, unsigned int, unsigned int, Particle*, Parameters*))
{	/* Steps between cells */
	int x1c, x2c, y1c, y2c, ic;

#ifdef _OPENMP
	unsigned int a,b;
	for (a = 0; a<3; a++)
        for (b = 0; b<2; b++)
	#pragma omp parallel private(x1c, y1c, ic, x2c, y2c) shared(par, list, head, p)
	{
//        #pragma omp for schedule(auto) collapse(2) nowait
			#pragma omp for nowait
			for (x1c = a; x1c<par->NCX; x1c += 3) {
	        for (y1c = b; y1c<par->NCY; y1c += 2) {
			ic = x1c + par->NCX*y1c;
			if (head[ic] >= 0) {
				cell_int(ic, ic, head, list, p, par, function);
				x2c = x1c + 1;
				if (x2c < par->NCX) cell_int(ic, ic + 1, head, list, p, par, function);
				y2c = y1c + 1;
				if (y2c < par->NCY) {
					cell_int(ic, ic + par->NCX, head, list, p, par, function);
					x2c = x1c - 1;
					if (x2c >= 0) cell_int(ic, ic + par->NCX - 1, head, list, p, par, function);
					x2c = x1c + 1;
					if (x2c < par->NCX) cell_int(ic, ic + par->NCX + 1, head, list, p, par, function);
				}
			}
		}
	}
	}
#else
	for (ic = 0; ic < par->NC; ic++) {
		x1c = ic % par->NCX;
		y1c = (ic - x1c) / par->NCX;
		if (head[ic] >= 0) {
			cell_int(ic, ic, head, list, p, par, function);
			x2c = x1c + 1;
			if (x2c < par->NCX) cell_int(ic, ic + 1, head, list, p, par, function);
			y2c = y1c + 1;
			if (y2c < par->NCY) {
				cell_int(ic, ic + par->NCX, head, list, p, par, function);
				x2c = x1c - 1;
				if (x2c >= 0) cell_int(ic, ic + par->NCX - 1, head, list, p, par, function);
				x2c = x1c + 1;
				if (x2c <par->NCX) cell_int(ic, ic + par->NCX + 1, head, list, p, par, function);
			}
		}
	}
#endif

	if (par->T_BOUNDARY_PERIODICITY != 0) {	/* Left-right periodic boundary */
		x1c = par->NCX - 1;
/*#ifdef _OPENMP
		for (b=0; b<2; b++)
		#pragma omp parallel for private(y1c, ic) shared(x1c, NYC, NXC, head, list, p)
		for (y1c = b; y1c < NYC; y1c += 2) {
#else
		for (y1c = 0; y1c < NYC; y1c++) {
#endif*/
		for (y1c = 0; y1c < par->NCY; y1c++) {
			ic = x1c + y1c * par->NCX;
			if (head[ic] >= 0) {
				cell_int(ic, ic + 1 - par->NCX, head, list, p, par, function);
				if (y1c > 0) cell_int(ic, ic + 1 - 2 * par->NCX, head, list, p, par, function);
				if (y1c < par->NCY - 1) cell_int(ic, ic + 1, head, list, p, par, function);
			}
		}
	}

	if (par->T_BOUNDARY_PERIODICITY == 1) {	/* Left-right periodic boundary */
		y1c = par->NCY - 1;
		/*#ifdef _OPENMP
		for (b=0; b<2; b++)
		#pragma omp parallel for private(y1c, ic) shared(x1c, NYC, NXC, head, list, p)
		for (y1c = b; y1c < NYC; y1c += 2) {
		#else
		for (y1c = 0; y1c < NYC; y1c++) {
		#endif*/
		for (x1c = 0; x1c < par->NCX; x1c++) {
			int ic1 = x1c + y1c * par->NCX;
			int ic2 = x1c;
			if (head[ic2] >= 0) {
				cell_int(ic1, ic2, head, list, p, par, function);
				if (x1c > 0) cell_int(ic1, ic2 - 1, head, list, p, par, function);
				if (x1c < par->NCY - 1) cell_int(ic1, ic2 + 1, head, list, p, par, function);
				if (x1c == 0) cell_int(ic1, par->NCX - 1, head, list, p, par, function);
				if (x1c == par->NCX - 1) cell_int(ic1, 0, head, list, p, par, function);
			}
		}
	}

}

#endif
