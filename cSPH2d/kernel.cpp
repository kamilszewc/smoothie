/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_PART_INT
#define SPH_PART_INT

#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cmath>
#include <iostream>
//#include <amdlibm.h> 

#include "sph.h"
#include "hlp.h"

void add_element(int c, int r, double val, atom *M[], int ncol[]);


/***********************************************************************
 *  Functions to compute kernel and gradient of kernel value:          *
 *  - kernel(double q): return value of W(r,h) (q=|r|/h).                *
 *  - grad_of_kernel(double x, double q): return value of grad_x W(r,h)    *
 ***********************************************************************/
INLINE double pse_kernel(double q)
{
	double k;
	if (q <= 2.0)
		k = 0.835 / (1 + q*q);
	else
		k = 0.0;

	return k;
}

INLINE double kernel(double q)
{				/* Kernel functions */
	double k;

	switch (T_KERNEL) {
	case 0:		/* Gaussian */
		if (q <= 2.0)
			k = KNORM * exp(-q * q);
		else
			k = 0.0;
		break;

	case 1:		/* Quadratic */
		if (q <= 2.0)
			k = KNORM * (0.375 * q * q - 1.5 * q + 1.5);
		else
			k = 0.0;
		break;

	case 2:		/* Cubic spline */
		if (q <= 1.0)
			k = KNORM * ((10.0 - 15.0 * q * q + 7.5 * q * q * q) * (1.0 / 7.0));
		else if (q < 2.0)
			k = KNORM * (pow3(2.0 - q) * (2.5 / 7.0));
		else
			k = 0.0;
		break;

	case 3:		/* Quintic */
		if (q <= 2.0)
			k = KNORM * (0.21875 * pow4(2.0 - q) * (q + 0.5));
		else
			k = 0.0;
		break;
	case 4:		/* Quintic - Morris, Fox, Zhu, JCP 136, 1997 */
		if (q <= 1.0)
			k = KNORM * (0.014644351464435146 * (pow5(3.0 - q) - 6.0*pow5(2.0 - q) + 15.0*pow5(1.0 - q)));
		else if (q <= 2.0)
			k = KNORM * (0.014644351464435146 * (pow5(3.0 - q) - 6.0*pow5(2.0 - q)));
		else if (q <= 3.0)
			k = KNORM * (0.014644351464435146 * pow5(3.0 - q));
		else
			k = 0.0;
		break;
	default:
		perror("Type of kernel is not defined (T_KERNEL)");
		exit(EXIT_FAILURE);
	}
	return k;
}

INLINE double grad_of_kernel(double x, double q)
{				/* Grad of kernel functions */
	double gk;

	switch (T_KERNEL) {
	case 0:		/* Gaussian */
		if (q <= 2.0)
			gk = GKNORM * (-2.0 * x * exp(-q * q));
		else
			gk = 0.0;
		break;

	case 1:		/* Quadratic */
		if (q == 0.0)
			gk = 0.0;
		else if (q <= 2.0)
			gk = GKNORM * (x * (0.75 - 1.5 / q));
		else
			gk = 0.0;
		break;

	case 2:		/* Cubic spline */
		if (q <= 1.0)
			gk = GKNORM * ((-30.0 + 22.5 * q) * x * (1.0 / 7.0));
		else if (q <= 2.0)
			gk = GKNORM * ((-7.5 / 7.0) * pow2(2.0 - q) * (x / q));
		else
			gk = 0.0;
		break;

	case 3:		/* Quintic */
		if (q < 2.0)
			gk = GKNORM * (-1.09375 * x * pow3(2.0 - q));
		else
			gk = 0.0;
		break;
	case 4:		/* Quintic - Morris, Fox, Zhu, JCP 136, 1997 */
#define K_CASE_4_NORM 0.073221757322175729
		if (q < ZERO)
			gk = 0.0;
		else if (q < 1.0)
			gk = GKNORM * (-K_CASE_4_NORM * (x / q) * (pow4(3.0 - q) - 6.0*pow4(2.0 - q) + 15.0*pow4(1.0 - q)));
		else if (q < 2.0)
			gk = GKNORM * (-K_CASE_4_NORM * (x / q) * (pow4(3.0 - q) - 6.0*pow4(2.0 - q)));
		else if (q < 3.0)
			gk = GKNORM * (-K_CASE_4_NORM * (x / q) * pow4(3.0 - q));
		else
			gk = 0.0;
		break;
	default:
		perror("Type of kernel is not defined (T_KERNEL)");
		exit(EXIT_FAILURE);
	}
	return gk;
}


/***********************************************************************
 *  Functions to compute r_a-r_b, u_a-u_b and t_a-t_b:                 *
 *  - static void interaction: r_a-r_b, u_a-u_b                        *
 *  - static void interaction_temperature: t_a-t_b                     *
 ***********************************************************************/

static void interaction(unsigned int i, unsigned int j, double *x, double *y, double *u, double *v, int T, Particle*p, Parameters* par)
{
	switch (T) {
	case 0: /* Normal Particles */
		*x = p[i].pos.x - p[j].pos.x;
		*y = p[i].pos.y - p[j].pos.y;
		*u = p[i].vel.x - p[j].vel.x;
		*v = p[i].vel.y - p[j].vel.y;
		break;
	case 1:		/* N */
		*x = p[i].pos.x - p[j].pos.x;
		*y = 2.0 * par->YCV - p[i].pos.y - p[j].pos.y;
		*u = 2.0 * par->V_N - p[i].vel.x - p[j].vel.x;
		*v = -p[i].vel.y - p[j].vel.y;
		break;
	case 2:		/* E */
		*x = 2.0 * par->XCV - p[i].pos.x - p[j].pos.x;
		*y = p[i].pos.y - p[j].pos.y;
		*u = -p[i].vel.x - p[j].vel.x;
		*v = 2.0 * par->V_E - p[i].vel.y - p[j].vel.y;
		break;
	case 3:		/* S */
		*x = p[i].pos.x - p[j].pos.x;
		*y = -p[i].pos.y - p[j].pos.y;
		*u = 2.0 * par->V_S - p[i].vel.x - p[j].vel.x;
		*v = -p[i].vel.y - p[j].vel.y;
		break;
	case 4:		/* W */
		*x = -p[i].pos.x - p[j].pos.x;
		*y = p[i].pos.y - p[j].pos.y;
		*u = -p[i].vel.x - p[j].vel.x;
		*v = 2.0 * par->V_W - p[i].vel.y - p[j].vel.y;
		break;
	case 5:		/* NE */
		*x = 2.0 * par->XCV - p[i].pos.x - p[j].pos.x;
		*y = 2.0 * par->YCV - p[i].pos.y - p[j].pos.y;
		*u = /*2.0 * V_N*/ -p[i].vel.x - p[j].vel.x;
		*v = /*2.0 * V_E*/ -p[i].vel.y - p[j].vel.y;
		break;
	case 6:		/* SE */
		*x = 2.0 * par->XCV - p[i].pos.x - p[j].pos.x;
		*y = -p[i].pos.y - p[j].pos.y;
		*u = /*2.0 * V_S*/ -p[i].vel.x - p[j].vel.x;
		*v = /*2.0 * V_E*/ -p[i].vel.y - p[j].vel.y;
		break;
	case 7:		/* SW */
		*x = -p[i].pos.x - p[j].pos.x;
		*y = -p[i].pos.y - p[j].pos.y;
		*u = /*2.0 * V_S*/ -p[i].vel.x - p[j].vel.x;
		*v = /*2.0 * V_W*/ -p[i].vel.y - p[j].vel.y;
		break;
	case 8:		/* NW */
		*x = -p[i].pos.x - p[j].pos.x;
		*y = 2.0 * par->YCV - p[i].pos.y - p[j].pos.y;
		*u = /*2.0 * V_N*/ -p[i].vel.x - p[j].vel.x;
		*v = /*2.0 * V_W*/ -p[i].vel.y - p[j].vel.y;
		break;
	default:
		break;
	}

	switch (par->T_BOUNDARY_PERIODICITY)
	{
	case 0:
		break;
	case 1:
		if (*x > 6.0 * par->H) *x -= par->XCV;
		if (*x < -6.0 * par->H) *x += par->XCV;
		if (*y >  6.0 * par->H) *y -= par->YCV;
		if (*y < -6.0 * par->H) *y += par->YCV;
		break;
	case 2:
		if (*x >  6.0 * par->H) *x -= par->XCV;
		if (*x < -6.0 * par->H) *x += par->XCV;
		break;
	}

	/*if ((par->T_BOUNDARY_PERIODICITY != 0) && (*x >  6.0 * par->H)) *x -= par->XCV;
	if ((par->T_BOUNDARY_PERIODICITY != 0) && (*x < -6.0 * par->H)) *x += par->XCV;
	if ((par->T_BOUNDARY_PERIODICITY == 1) && (*y >  6.0 * par->H)) *y -= par->YCV;
	if ((par->T_BOUNDARY_PERIODICITY == 1) && (*y < -6.0 * par->H)) *y += par->YCV;*/
}

static void interaction_temperature(unsigned int i, unsigned int j, double *t, unsigned int T, Particle* p, Parameters* par)
{
	switch (T) {
	case 0: /* Normal Particles */
		*t = p[i].t - p[j].t;
		break;
	case 1:		/* N */
		if (par->T_TEMP_N == 0) *t = 2.0*par->TEMP_N - p[j].t - p[i].t;
		if (par->T_TEMP_N == 1) *t = 0.0;
		break;
	case 2:		/* E */
		if (par->T_TEMP_E == 0) *t = 2.0*par->TEMP_E - p[j].t - p[i].t;
		if (par->T_TEMP_E == 1) *t = 0.0;
		break;
	case 3:		/* S */
		if (par->T_TEMP_S == 0) *t = 2.0*par->TEMP_S - p[j].t - p[i].t;
		if (par->T_TEMP_S == 1) *t = 0.0;
		break;
	case 4:		/* W */
		if (par->T_TEMP_W == 0) *t = 2.0*par->TEMP_W - p[j].t - p[i].t;
		if (par->T_TEMP_W == 1) *t = 0.0;
		break;
	case 5:		/* NE */
		if (par->T_TEMP_E == 0) *t = 0.5*(par->TEMP_E + par->TEMP_N) - p[j].t;
		if (par->T_TEMP_N == 1) *t = 0.0;
		break;
	case 6:		/* SE */
		if (par->T_TEMP_S == 0) *t = 0.5*(par->TEMP_E + par->TEMP_S) - p[j].t;
		if (par->T_TEMP_S == 1) *t = 0.0;
		break;
	case 7:		/* SW */
		if (par->T_TEMP_S == 0) *t = 0.5*(par->TEMP_W + par->TEMP_S) - p[j].t;
		if (par->T_TEMP_S == 1) *t = 0.0;
		break;
	case 8:		/* NW */
		if (par->T_TEMP_N == 0) *t = 0.5*(par->TEMP_W + par->TEMP_N) - p[j].t;
		if (par->T_TEMP_N == 1) *t = 0.0;
		break;
	default:		/* Normal Particles */
		*t = p[i].t - p[j].t;
	}
}


/***********************************************************************
 *  WCSPH kernels                                                      *
 ***********************************************************************/

void part_int(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v;
	interaction(i, j, &x, &y, &u, &v, T, p, par);

	double r2 = x * x + y * y;
	double q = sqrt(r2) * par->I_H;
	double gkx = grad_of_kernel(x, q);
	double gky = grad_of_kernel(y, q);

	double g = 0.0;
	if ((par->T_SURFACE_TENSION > 0) || (par->T_XSPH == 1)) g = kernel(q);

	double pres = 0.0;
	double hpres = 0.0;
	double visc = 0.0;
	double force = 0.0;
	switch (par->T_MODEL) {
	case 0: /* Standard */
		pres = (p[i].p / pow2(p[i].d)) + (p[j].p / pow2(p[j].d));
		if (par->T_HYDROSTATIC_PRESSURE == 1) hpres = (p[i].hp / pow2(p[i].d)) + (p[j].hp / pow2(p[j].d));
		if ((par->T_INTERFACE_CORRECTION == 1) && (p[i].c != p[j].c)) pres += par->INTERFACE_CORRECTION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * (p[i].p / pow2(p[i].d)) + (p[j].p / pow2(p[j].d));
		visc = 8.0 * (u * x + v * y) * (p[i].nu + p[j].nu) / ((p[i].d + p[j].d) * (r2 + par->DH));
		force = visc - pres;
		break;
	case 1: /* Colagrossi & Landrini */
		pres = (p[i].p + p[j].p) / (p[i].d * p[j].d);
		if (par->T_HYDROSTATIC_PRESSURE == 1) hpres = (p[i].hp + p[j].hp) / (p[i].d * p[j].d);
		if ((par->T_INTERFACE_CORRECTION == 1) && (p[i].c != p[j].c)) pres += par->INTERFACE_CORRECTION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * (fabs(p[i].p) + fabs(p[j].p)) / (p[i].d * p[j].d);
		visc = 8.0 * (u * x + v * y) * (p[i].nu + p[j].nu) / ((p[i].d + p[j].d) * (r2 + par->DH));
		force = visc - pres;
		break;
	case 3: /* Hu & Adams */
		pres = (p[i].p*p[i].o) + (p[j].p*p[j].o);
		if (par->T_HYDROSTATIC_PRESSURE == 1) hpres = (p[i].hp*p[i].o) + (p[j].hp*p[j].o);
		if ((par->T_INTERFACE_CORRECTION == 1) && (p[i].c != p[j].c)) pres += par->INTERFACE_CORRECTION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * (fabs(p[i].o) + fabs(p[j].o));
		visc = 2.0*(p[i].mi*p[j].mi)*(p[i].o + p[j].o)*(x*gkx + y*gky) / ((r2 + par->DH) * (p[i].mi + p[j].mi));
		break;
	}

	double t = 0.0;
	if (par->T_HEAT_TRANSFER == 1) {
		interaction_temperature(i, j, &t, T, p, par);
		if (par->T_MODEL < 2) {
			t = 4.0 * (p[i].co*p[j].co / (p[i].co + p[j].co)) * t * (x*gkx + y*gky) / (p[i].d*p[j].d*(r2 + par->DH));
		}
		else {
			t = 2.0 * (p[i].co*p[j].co / (p[i].co + p[j].co)) * (p[i].o + p[j].o) * t * (x*gkx + y*gky) / (r2 + par->DH);
		}
	}

	switch (T) {
	case 0:
		switch (par->T_MODEL) {
		case 0:
		case 1:
			rh[i].u += p[j].m * force * gkx;
			rh[j].u -= p[i].m * force * gkx;
			rh[i].v += p[j].m * force * gky;
			rh[j].v -= p[i].m * force * gky;
			/*rh[i].u += p[j].m * (-pres * gkx + visc * u);
			rh[j].u -= p[i].m * (-pres * gkx + visc * u);
			rh[i].v += p[j].m * (-pres * gky + visc * v);
			rh[j].v -= p[i].m * (-pres * gky + visc * v);*/
			if ((par->T_HYDROSTATIC_PRESSURE == 1)) {
				rh[i].fx += p[j].m * (-hpres * gkx);
				rh[j].fx -= p[i].m * (-hpres * gkx);
				rh[i].fy += p[j].m * (-hpres * gky);
				rh[j].fy -= p[i].m * (-hpres * gky);
			}
			break;
		case 2:
		case 3:
			rh[i].u += (visc * u - pres * gkx) / p[i].m; /*+ (visc * u * gk - pres * gkx) / p[i].m;*/
			rh[j].u -= (visc * u - pres * gkx) / p[j].m; /*+ (visc * u * gk - pres * gkx) / p[j].m;*/
			rh[i].v += (visc * v - pres * gky) / p[i].m; /*+ (visc * v * gk - pres * gky) / p[i].m;*/
			rh[j].v -= (visc * v - pres * gky) / p[j].m; /*+ (visc * v * gk - pres * gky) / p[j].m;*/
			if ((par->T_HYDROSTATIC_PRESSURE == 1)) {
				rh[i].fx += (-hpres * gkx) / p[i].m;
				rh[j].fx -= (-hpres * gkx) / p[j].m;
				rh[i].fy += (-hpres * gky) / p[i].m;
				rh[j].fy -= (-hpres * gky) / p[j].m;
			}
			break;
		}

		if (par->T_MODEL < 2){
			rh[i].d += p[j].m * (u * gkx + v * gky) * p[i].d / p[j].d;
			rh[j].d += p[i].m * (u * gkx + v * gky) * p[j].d / p[i].d;
		} else if (par->T_MODEL == 3) {
			rh[i].d += (u * gkx + v * gky);
			rh[j].d += (u * gkx + v * gky);
		}

		if (par->T_XSPH == 1) {
			rh[i].x += 0.5 * p[j].m * u * g / p[j].d;
			rh[i].y += 0.5 * p[j].m * v * g / p[j].d;
			rh[j].x -= 0.5 * p[i].m * u * g / p[i].d;
			rh[j].y -= 0.5 * p[i].m * v * g / p[i].d;
		}

		if (par->T_HEAT_TRANSFER == 1) {
			if (par->T_MODEL < 2) {
				rh[i].e += p[j].m * t;
				rh[j].e -= p[i].m * t;
			}
			else {
				rh[i].e += t / p[i].m;
				rh[j].e -= t / p[j].m;
			}
		}

		if (par->T_SURFACE_TENSION > 0) {
			if (par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) > 0.0)
			{
				rh[i].sig += p[j].m * par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * g / p[j].d;
				rh[i].sigw += p[j].m * g / p[j].d;
				if (i != j) rh[j].sig += p[i].m *par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * g / p[i].d;
				if (i != j) rh[j].sigw += p[i].m * g / p[i].d;
			}
			switch (par->T_SURFACE_TENSION) {
			case 1:
				p[i].n.x -= p[j].m * p[j].c * gkx / (p[j].d * abs(par->PHASES[0] - par->PHASES[1]));
				p[j].n.x += p[i].m * p[i].c * gkx / (p[i].d * abs(par->PHASES[0] - par->PHASES[1]));
				p[i].n.y -= p[j].m * p[j].c * gky / (p[j].d * abs(par->PHASES[0] - par->PHASES[1]));
				p[j].n.y += p[i].m * p[i].c * gky / (p[i].d * abs(par->PHASES[0] - par->PHASES[1]));
				break;
			case 2:
			case 3:
				rh[i].cs += p[j].m * p[j].c * g / p[j].d;
				if (i != j) rh[j].cs += p[i].m * p[i].c * g / p[i].d;
				break;
			default:
				break;
			}
		}
		break;
	default:
		switch (par->T_MODEL) {
		case 0:
		case 1:
			rh[j].u -= p[i].m * force * gkx;
			rh[j].v -= p[i].m * force * gky;
			if ((par->T_HYDROSTATIC_PRESSURE == 1)) {
				rh[j].fx -= p[i].m * (-hpres * gkx);
				rh[j].fy -= p[i].m * (-hpres * gky);
			}
			break;
		case 2:
		case 3:
			rh[j].u -= (visc * u - pres * gkx) / p[j].m;
			rh[j].v -= (visc * v - pres * gky) / p[j].m;
			if ((par->T_HYDROSTATIC_PRESSURE == 1)) {
				rh[j].fx -= (-hpres * gkx) / p[j].m;
				rh[j].fy -= (-hpres * gky) / p[j].m;
			}
			break;
		}

		switch (T) {
		case 1:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 2:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 3:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 4:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 5:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 6:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 7:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 8:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
		default:
			break;
		}

		if (par->T_MODEL < 2) {
			rh[j].d += p[i].m * (u*gkx + v *gky) * p[j].d / p[i].d;
		} else if (par->T_MODEL == 3) {
			rh[j].d += (u * gkx + v * gky);
		}

		if (par->T_XSPH == 1) {
			rh[j].x -= 0.5 * p[i].m * u * g / p[i].d;
			rh[j].y -= 0.5 * p[i].m * v * g / p[i].d;
		}

		if (par->T_HEAT_TRANSFER == 1) {
			if (par->T_MODEL < 2) {
				rh[j].e -= p[i].m * t;
			}
			else {
				rh[j].e -= t / p[j].m;
			}
		}

		if (par->T_SURFACE_TENSION > 0) {
			if (par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) > 0.0)
			{
				rh[i].sig += p[j].m * par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * g / p[j].d;
				rh[i].sigw += p[j].m * g / p[j].d;
			}
			switch (par->T_SURFACE_TENSION) {
			case 1:
				p[j].n.x += p[i].m * p[i].c * gkx / (p[i].d * abs(par->PHASES[0] - par->PHASES[1]));
				p[j].n.y += p[i].m * p[i].c * gky / (p[i].d * abs(par->PHASES[0] - par->PHASES[1]));
				break;
			case 2:
			case 3:
				rh[j].cs += p[i].m * p[i].c * g / p[i].d;
				/*rh[j].csw += p[i].m * 1.0 * g / p[i].d;*/
				break;
			default:
				break;
			}
		}
	}
}

void part_int_tartakovsky(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v;
	interaction(i, j, &x, &y, &u, &v, T, p, par);

	double r2 = x * x + y * y;
	double q = sqrt(r2) * par->I_H;
	double gkx = grad_of_kernel(x, q);
	double gky = grad_of_kernel(y, q);

	double pres = 0.0;
	double visc = 0.0;
	
	pres = (p[i].p/pow2(p[i].o)) + (p[j].p/pow2(p[j].o));
	visc = 2.0*(p[i].mi*p[j].mi)*(1.0/pow2(p[i].o) + 1.0/pow2(p[j].o))*(x*gkx + y*gky) / ((r2 + par->DH) * (p[i].mi + p[j].mi));

	double c2 = 0.0001;
	double k = 1.0;
	if (p[i].phaseId != p[j].phaseId)
	{
		k = 1.0;
	}

	switch (T) {
	case 0:
		rh[i].u += (visc * u - pres * gkx) / p[i].m; /*+ (visc * u * gk - pres * gkx) / p[i].m;*/
		rh[j].u -= (visc * u - pres * gkx) / p[j].m; /*+ (visc * u * gk - pres * gkx) / p[j].m;*/
		rh[i].v += (visc * v - pres * gky) / p[i].m; /*+ (visc * v * gk - pres * gky) / p[i].m;*/
		rh[j].v -= (visc * v - pres * gky) / p[j].m; /*+ (visc * v * gk - pres * gky) / p[j].m;*/
		rh[i].x += 2.0 * k * c2 * gkx / p[i].m;
		rh[j].x -= 2.0 * k * c2 * gkx / p[j].m;
		rh[i].y += 2.0 * k * c2 * gky / p[i].m;
		rh[j].y -= 2.0 * k * c2 * gky / p[j].m;
		break;
	default:
		rh[j].u -= (visc * u - pres * gkx) / p[j].m;
		rh[j].v -= (visc * v - pres * gky) / p[j].m;
		rh[j].x -= 2.0 * k * c2 * gkx / p[j].m;
		rh[j].y -= 2.0 * k * c2 * gky / p[j].m;
	}
}

void density(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, k;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	k = kernel(sqrt(x * x + y * y) * par->I_H);

	switch (T) {
	case 0:
		rh[i].d += p[j].m * k;
		if (i != j) rh[j].d += p[i].m * k;
		break;
	default:
		rh[j].d += p[i].m * k;
	}
}

void volume(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, k;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	k = kernel(sqrt(x * x + y * y) * par->I_H);
	//k = kernel(hypot(x, y) * par->I_H);

	switch (T) {
	case 0:
		rh[i].d += k;
		if (i != j) rh[j].d += k;
		break;
	default:
		rh[j].d += k;
	}
}

void shepard(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, k;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	k = kernel(sqrt(x * x + y * y) * par->I_H);
	//k = kernel(hypot(x, y) * par->I_H);

	switch (T) {
	case 0:
		rh[i].dc += k * p[j].vol;
		if (i != j) rh[j].dc += k * p[i].vol;
		break;
	default:
		rh[j].dc += k * p[i].vol;
	}

	if (p[i].c == p[j].c) {
		switch (T) {
		case 0:
			rh[i].dp += p[j].m * k;
			rh[i].dpc += k * p[j].vol;
			if (i != j) rh[j].dp += p[i].m * k;
			if (i != j) rh[j].dpc += k * p[i].vol;
			break;
		default:
			rh[j].dp += p[i].m * k;
			rh[j].dpc += k * p[i].vol;
		}
	}
}

void shepard2(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, k;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	k = kernel(sqrt(x * x + y * y) * par->I_H);

	/*switch (T) {
	case 0:
	rh[i].dc += k * p[j].m / p[j].d;
	if (i != j) rh[j].dc += k * p[i].m / p[i].d;
	break;
	default:
	rh[j].dc += k * p[i].m / p[i].d;
	}*/

	if (p[i].c == p[j].c) {
		switch (T) {
		case 0:
			rh[i].dp += /*p[j].m */ k;
			rh[i].dpc += k * p[j].m / p[j].d;
			if (i != j) rh[j].dp += /*p[i].m */ k;
			if (i != j) rh[j].dpc += k * p[i].m / p[i].d;
			break;
		default:
			rh[j].dp += /*p[i].m */ k;
			rh[j].dpc += k * p[i].m / p[i].d;
		}
	}
}

/***********************************************************************
 *  ISPH kernels                                                       *
 ***********************************************************************/

void viscosity(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, g = 0.0;
	double visc;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);
	if ((par->T_XSPH == 1) || (par->T_SURFACE_TENSION > 0)) g = kernel(q);

	/*visc = 8.0 * (u * x + v * y) * (p[i].nu + p[j].nu) / ( (p[i].d + p[j].d) * (r2 + DH));*/
	visc = (x * gkx + y * gky) * (p[i].mi + p[j].mi) / (p[i].d*p[j].d*(r2 + par->DH));

	switch (T) {
	case 0:
		/*rh[i].u += p[j].m * visc * gkx;
		rh[j].u -= p[i].m * visc * gkx;
		rh[i].v += p[j].m * visc * gky;
		rh[j].v -= p[i].m * visc * gky;*/
		rh[i].u += p[j].m * visc * u;
		rh[j].u -= p[i].m * visc * u;
		rh[i].v += p[j].m * visc * v;
		rh[j].v -= p[i].m * visc * v;

		if (par->T_SURFACE_TENSION > 0) {
			rh[i].sig += p[j].m * par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * g / p[j].d;
			if (i != j) rh[j].sig += p[i].m * par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * g / p[i].d;
			switch (par->T_SURFACE_TENSION) {
			case 1:
				p[i].n.x -= p[j].m * p[j].c * gkx / p[j].d;
				p[j].n.x += p[i].m * p[i].c * gkx / p[i].d;
				p[i].n.y -= p[j].m * p[j].c * gky / p[j].d;
				p[j].n.y += p[i].m * p[i].c * gky / p[i].d;
				break;
			case 2:
			case 3:
				rh[i].cs += p[j].m * p[j].c * g / p[j].d;
				if (i != j) rh[j].cs += p[i].m * p[i].c * g / p[i].d;
				break;
			default:
				break;
			}
		}

		break;
	default:
		/*rh[j].u -= p[i].m * visc * gkx;
		rh[j].v -= p[i].m * visc * gky;*/
		rh[j].u -= p[i].m * visc * u;
		rh[j].v -= p[i].m * visc * v;

		if (par->T_SURFACE_TENSION > 0) {
			rh[i].sig += p[j].m * par->SURFACE_TENSION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * g / p[j].d;
			switch (par->T_SURFACE_TENSION) {
			case 1:
				p[j].n.x += p[i].m * p[i].c * gkx / p[i].d;
				p[j].n.y += p[i].m * p[i].c * gky / p[i].d;
				break;
			case 2:
			case 3:
				rh[j].cs += p[i].m * p[i].c * g / p[i].d;
				/*rh[j].csw += p[i].m * 1.0 * g / p[i].d;*/
				break;
			default:
				break;
			}
		}
	}
}

void press(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, pres;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	/*pres = (p[i].p / (p[i].d * p[i].d)) + (p[j].p / (p[j].d * p[j].d));*/
	pres = (p[i].p + p[j].p) / (p[i].d * p[j].d);
	if ((par->T_INTERFACE_CORRECTION == 1) && (p[i].c != p[j].c)) pres += par->INTERFACE_CORRECTION_COEFFICIENT->Get(p[i].phaseId, p[j].phaseId) * (fabs(p[i].p) + fabs(p[j].p)) / (p[i].d * p[j].d);

	switch (T) {
	case 0:
		rh[i].u -= p[j].m * pres * gkx;
		rh[j].u += p[i].m * pres * gkx;
		rh[i].v -= p[j].m * pres * gky;
		rh[j].v += p[i].m * pres * gky;

		break;
	default:
		rh[j].u += p[i].m * pres * gkx;
		rh[j].v += p[i].m * pres * gky;
	}
}

void hpress(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, pres;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	/*pres = (p[i].p / (p[i].d * p[i].d)) + (p[j].p / (p[j].d * p[j].d));*/
	pres = (p[i].hp + p[j].hp) / (p[i].d * p[j].d);

	switch (T) {
	case 0:
		rh[i].fx -= p[j].m * pres * gkx;
		rh[j].fx += p[i].m * pres * gkx;
		rh[i].fy -= p[j].m * pres * gky;
		rh[j].fy += p[i].m * pres * gky;

		break;
	default:
		rh[j].fx += p[i].m * pres * gkx;
		rh[j].fy += p[i].m * pres * gky;
	}
}

void dev_v2(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, q, gkx, gky, put;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	q = sqrt(x * x + y * y) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);
	put = u * gkx + v * gky;

	switch (T) {
	case 0:
		rh[i].d -= p[j].m * put;
		rh[j].d -= p[i].m * put;
		break;
	default:
		switch (T) {
		case 1:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 2:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 3:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 4:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 5:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 6:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 7:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 8:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
		default:
			break;
		}
		rh[j].d -= p[i].m * (u * gkx + v * gky);
		break;
	}
}

void dev_v_sparse(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, mod, put;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	/*visc = 8.0 * (p[i].mi*p[j].mi) * (p[i].o+p[j].o) * (u*x + v*y)/( (r2 + DH) * (p[i].mi+p[j].mi) );*/
	/* rh[j].u -= (visc * u - pres * gkx) / p[j].m; */
	mod = 4.0 * (x * gkx + y * gky) / ((r2 + DH) * (p[i].di + p[j].di));
	put = u * gkx + v * gky;

	switch (T) {
	case 0:
		b[i] -= p[j].m * put / p[j].di;
		b[j] -= p[i].m * put / p[i].di;
		if (i != j) {
			add_element(i, j, -p[j].m * mod / p[j].di, M, ncol);
			add_element(j, i, -p[i].m * mod / p[i].di, M, ncol);
			add_element(i, i, p[j].m * mod / p[j].di, M, ncol);
			add_element(j, j, p[i].m * mod / p[i].di, M, ncol);
		}
		break;
	default:
		switch (T) {
		case 1:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 2:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 3:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 4:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 5:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 6:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 7:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 8:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
		default:
			break;
		}
		b[j] -= p[i].m * (u * gkx + v * gky) / p[i].di;
		add_element(j, j, p[i].m * mod / p[i].di, M, ncol);
		b[j] += par->DT * p[i].m * p[i].p * mod / p[i].di;
		break;
	}
}

void dev_v_sparse_density(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, mod;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	mod = 2.0 * (x * gkx + y * gky) * (p[i].d + p[j].d) / ((r2 + par->DH) * (p[i].di + p[j].di));

	switch (T) {
	case 0:
		if (i != j) {
			add_element(i, j, -p[j].m * mod / p[j].di, M, ncol);
			add_element(j, i, -p[i].m * mod / p[i].di, M, ncol);
			add_element(i, i, p[j].m * mod / p[j].di, M, ncol);
			add_element(j, j, p[i].m * mod / p[i].di, M, ncol);
		}
		break;
	default:
		add_element(j, j, p[i].m * mod / p[i].di, M, ncol);
		b[j] += p[i].m * p[i].pp * mod / p[i].di;
		break;
	}
}

void dev_v_test(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, g, mod;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);
	g = kernel(q);

	mod = 2.0 * (x * gkx + y * gky) / ((r2 + par->DH));

	switch (T) {
	case 0:
		b[i] -= p[j].m * g / p[j].d;
		if (i != j) b[j] -= p[i].m * g / p[i].d;

		if (i != j) {
			add_element(i, j, -p[j].m * mod, M, ncol);
			add_element(j, i, -p[i].m * mod, M, ncol);
			add_element(i, i, p[j].m * mod, M, ncol);
			add_element(j, j, p[i].m * mod, M, ncol);
		}

		break;
	default:
		add_element(j, j, p[i].m * mod, M, ncol);
		break;
	}
}

void dev_v_test_n(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, g, mod;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);
	g = kernel(q);

	mod = 2.0 * (x * gkx + y * gky) / ((r2 + par->DH));

	switch (T) {
	case 0:
		b[i] -= p[j].m * g / p[j].d;
		if (i != j) b[j] -= p[i].m * g / p[i].d;

		if (i != j) {
			add_element(i, j, -p[j].m * mod, M, ncol);
			add_element(j, i, -p[i].m * mod, M, ncol);
			add_element(i, i, p[j].m * mod, M, ncol);
			add_element(j, j, p[i].m * mod, M, ncol);
		}

		break;
	default:
		break;
	}
}



/***********************************************************************
 *  Surface tension kernels                                            *
 ***********************************************************************/

void normal_after_smoothed(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v;
	interaction(i, j, &x, &y, &u, &v, T, p, par);

	double q = sqrt(x * x + y * y) * par->I_H;
	double gkx = grad_of_kernel(x, q);
	double gky = grad_of_kernel(y, q);
	double put_d_i = (rh[i].cs - rh[j].cs) * (p[i].m / p[i].d) / abs(par->PHASES[0] - par->PHASES[1]);
	double put_d_j = (rh[i].cs - rh[j].cs) * (p[j].m / p[j].d) / abs(par->PHASES[0] - par->PHASES[1]);

	switch (T) {
	case 0:
		p[i].n.x += gkx * put_d_j;
		p[j].n.x += gkx * put_d_i;
		p[i].n.y += gky * put_d_j;
		p[j].n.y += gky * put_d_i;
		break;
	default:
		p[j].n.x += gkx * put_d_i;
		p[j].n.y += gky * put_d_i;
		break;
	}
}

void surface_tension_accel(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, q, put_00, put_11, put_01, put_10, gkx, gky;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	q = sqrt(x * x + y * y) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);
	/*put_00 = ( rh[i].is_00/pow2(p[i].n) ) + ( rh[j].is_00/pow2(p[j].n) );
	put_10 = ( rh[i].is_10/pow2(p[i].n) ) + ( rh[j].is_10/pow2(p[j].n) );
	put_01 = ( rh[i].is_01/pow2(p[i].n) ) + ( rh[j].is_01/pow2(p[j].n) );
	put_11 = ( rh[i].is_11/pow2(p[i].n) ) + ( rh[j].is_11/pow2(p[j].n) );*/
	put_00 = (rh[i].is_00*p[i].o) + (rh[j].is_00*p[j].o);
	put_10 = (rh[i].is_10*p[i].o) + (rh[j].is_10*p[j].o);
	put_01 = (rh[i].is_01*p[i].o) + (rh[j].is_01*p[j].o);
	put_11 = (rh[i].is_11*p[i].o) + (rh[j].is_11*p[j].o);

	switch (T) {
	case 0:
		p[i].st.x += (put_00*gkx + put_01*gky) / p[i].m;
		p[j].st.x -= (put_00*gkx + put_01*gky) / p[j].m;
		p[i].st.y += (put_11*gky + put_10*gkx) / p[i].m;
		p[j].st.y -= (put_11*gky + put_10*gkx) / p[j].m;
		break;
	default:
		p[j].st.x -= (put_00*gkx + put_01*gky) / p[j].m;
		p[j].st.y -= (put_11*gky + put_10*gkx) / p[j].m;
		break;
	}
}

void curvature(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y;
	if ((p[i].na == 1) && (p[j].na == 1)) {

		switch (T) {
		case 0:
			x = p[i].pos.x - p[j].pos.x;
			y = p[i].pos.y - p[j].pos.y;
			break;
		case 1:		/* N */
			x = p[i].pos.x - p[j].pos.x;
			y = 2.0 * par->YCV - p[i].pos.y - p[j].pos.y;
			break;
		case 2:		/* E */
			x = 2.0 * par->XCV - p[i].pos.x - p[j].pos.x;
			y = p[i].pos.y - p[j].pos.y;
			break;
		case 3:		/* S */
			x = p[i].pos.x - p[j].pos.x;
			y = -p[i].pos.y - p[j].pos.y;
			break;
		case 4:		/* W */
			x = -p[i].pos.x - p[j].pos.x;
			y = p[i].pos.y - p[j].pos.y;
			break;
		case 5:		/* NE */
			x = 2.0 * par->XCV - p[i].pos.x - p[j].pos.x;
			y = 2.0 * par->YCV - p[i].pos.y - p[j].pos.y;
			break;
		case 6:		/* NW */
			x = -p[i].pos.x - p[j].pos.x;
			y = 2.0 * par->YCV - p[i].pos.y - p[j].pos.y;
			break;
		case 7:		/* SE */
			x = 2.0 * par->XCV - p[i].pos.x - p[j].pos.x;
			y = -p[i].pos.y - p[j].pos.y;
			break;
		case 8:		/* WS */
			x = -p[i].pos.x - p[j].pos.x;
			y = -p[i].pos.y - p[j].pos.y;
			break;
		default:
			perror("Error (curvature.c)");
			exit(EXIT_FAILURE);
		}

		double r2 = x * x + y * y;
		double q = sqrt(r2) * par->I_H;
		double gkx = grad_of_kernel(x, q);
		double gky = grad_of_kernel(y, q);

		double g = 0.0;
		if (par->T_SURFACE_TENSION == 2) g = kernel(q);

		double diffx = ((p[i].n.x / p[i].nn) - (p[j].n.x / p[j].nn)) * gkx;
		double diffy = ((p[i].n.y / p[i].nn) - (p[j].n.y / p[j].nn)) * gky;

		double vol_i = p[i].m / p[i].d;
		double vol_j = p[j].m / p[j].d;

		switch (T) {
		case 0:
			p[i].cu += diffx * vol_j;
			p[i].cu += diffy * vol_j;
			p[j].cu += diffx * vol_i;
			p[j].cu += diffy * vol_i;
			if (par->T_SURFACE_TENSION == 2) {
				rh[i].cw += g * vol_j;
				if (i != j) rh[j].cw += g * vol_i;
			}
		default:
			p[j].cu += diffx * vol_i;
			p[j].cu += diffy * vol_i;
			if (par->T_SURFACE_TENSION == 2) rh[j].cw += g * vol_i;
		}
	}

}


void grad_a(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, pres;
	double al = 0.05*par->H;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	pres = (p[i].a + p[j].a);

	switch (T) {
	case 0:
		rh[i].uc -= al * p[j].m * pres * gkx / p[j].d;
		rh[j].uc += al * p[i].m * pres * gkx / p[i].d;
		rh[i].vc -= al * p[j].m * pres * gky / p[j].d;
		rh[j].vc += al * p[i].m * pres * gky / p[i].d;
		break;
	default:
		rh[j].uc += al * p[i].m * pres * gkx / p[i].m;
		rh[j].vc += al * p[i].m * pres * gky / p[i].m;
	}
}


/* VdW*/
void part_int_vdw(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, g = 0.0, t = 0.0;
	double pres = 0.0, visc = 0.0/*, gk=0.0*/, force = 0.0;
	double hpres = 0.0;
	double C2 = 2.0;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	/*H = 2.0*H;
	I_H = 1.0 / H;
	q = sqrt(r2) * I_H;
	gkxH = grad_of_kernel(x, q);
	gkyH = grad_of_kernel(y, q);
	H = 0.5*H;
	I_H = 1.0 / H;*/

	pres = (p[i].p*p[i].o) + (p[j].p*p[j].o);
	if ((par->T_HYDROSTATIC_PRESSURE == 1)) hpres = (p[i].hp*p[i].o) + (p[j].hp*p[j].o);
	//if ((p[i].c != p[j].c)) pres -= 0.9 * (2.0*C2);
	visc = 2.0*(p[i].mi*p[j].mi)*(p[i].o + p[j].o)*(x*gkx + y*gky) / ((r2 + par->DH) * (p[i].mi + p[j].mi));

	if (par->T_HEAT_TRANSFER == 1) {
		interaction_temperature(i, j, &t, T, p, par);
		t = 2.0 * (p[i].co*p[j].co / (p[i].co + p[j].co)) * (p[i].o + p[j].o) * t * (x*gkx + y*gky) / (r2 + par->DH);
	}

	switch (T) {
	case 0:
		rh[i].u += (visc * u - pres * gkx  /*2.0 * C2 *gkxH*/) / p[i].m;
		rh[j].u -= (visc * u - pres * gkx  /*2.0 * C2 *gkxH*/) / p[j].m;
		rh[i].v += (visc * v - pres * gky  /*2.0 * C2 *gkyH*/) / p[i].m;
		rh[j].v -= (visc * v - pres * gky  /*2.0 * C2 *gkyH*/) / p[j].m;
		if ((par->T_HYDROSTATIC_PRESSURE == 1)) {
			rh[i].fx += (-hpres * gkx) / p[i].m;
			rh[j].fx -= (-hpres * gkx) / p[j].m;
			rh[i].fy += (-hpres * gky) / p[i].m;
			rh[j].fy -= (-hpres * gky) / p[j].m;
		}

		if (par->T_HEAT_TRANSFER == 1) {
			rh[i].e += t / p[i].m;
			rh[j].e -= t / p[j].m;
		}
		break;
	default:
		rh[j].u -= (visc * u - pres * gkx /*+ 2.0 * C2 *gkxH*/) / p[j].m;
		rh[j].v -= (visc * v - pres * gky /*+ 2.0 * C2 *gkyH*/) / p[j].m;
		if ((par->T_HYDROSTATIC_PRESSURE == 1)) {
			rh[j].fx -= (-hpres * gkx) / p[j].m;
			rh[j].fy -= (-hpres * gky) / p[j].m;
		}
		break;

		switch (T) {
		case 1:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 2:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 3:
			u = p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 4:
			u = -p[i].vel.x - p[j].vel.x;
			v = p[i].vel.y - p[j].vel.y;
			break;
		case 5:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 6:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 7:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
			break;
		case 8:
			u = -p[i].vel.x - p[j].vel.x;
			v = -p[i].vel.y - p[j].vel.y;
		default:
			break;
		}

		if (par->T_HEAT_TRANSFER == 1) {
			rh[j].e -= t / p[j].m;
		}
	}
}


void cohesion_vdw(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky;
	double C2 = 2.0;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);


	//if (p[i].c != p[j].c)

	switch (T) {
	case 0:
		rh[i].u += 2.0 * C2 *gkx / p[i].m;
		rh[j].u -= 2.0 * C2 *gkx / p[j].m;
		rh[i].v += 2.0 * C2 *gky / p[i].m;
		rh[j].v -= 2.0 * C2 *gky / p[j].m;
		break;
	default:
		rh[j].u -= 2.0 * C2 *gkx / p[j].m;
		rh[j].v -= 2.0 * C2 *gky / p[j].m;
	}
}

/* General SPH */
void shear_strain_rate_and_heat_flux(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, t;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	if (par->T_HEAT_TRANSFER == 1)
	{
		interaction_temperature(i, j, &t, T, p, par);
	}

	switch (T) {
	case 0:
		rh[i].ssr_00 += (p[j].m / p[j].d) * (u * gkx + v * gky) - (p[j].m / p[j].d) * (u * gkx + v * gky);
		rh[j].ssr_00 += (p[i].m / p[i].d) * (u * gkx + v * gky) - (p[i].m / p[i].d) * (u * gkx + v * gky);
		rh[i].ssr_01 += (p[j].m / p[j].d) * (u * gky + v * gkx);
		rh[j].ssr_01 += (p[i].m / p[i].d) * (u * gky + v * gkx);
		rh[i].ssr_10 += (p[j].m / p[j].d) * (u * gky + v * gkx);
		rh[j].ssr_10 += (p[i].m / p[i].d) * (u * gky + v * gkx);
		rh[i].ssr_11 += (p[j].m / p[j].d) * (u * gkx + v * gky) - (p[j].m / p[j].d) * (u * gkx + v * gky);
		rh[j].ssr_11 += (p[i].m / p[i].d) * (u * gkx + v * gky) - (p[j].m / p[j].d) * (u * gkx + v * gky);

		if (par->T_HEAT_TRANSFER == 1)
		{
			rh[i].qx -= p[i].co * (p[j].m / p[j].d) * t * gkx;
			rh[i].qy -= p[i].co * (p[j].m / p[j].d) * t * gky;
			rh[j].qx -= p[j].co * (p[i].m / p[i].d) * t * gkx;
			rh[j].qy -= p[j].co * (p[i].m / p[i].d) * t * gky;
		}

		break;
	default:
		rh[j].ssr_00 += (p[i].m / p[i].d) * (u * gkx + v * gky) - (p[i].m / p[i].d) * (u * gkx + v * gky);
		rh[j].ssr_01 += (p[i].m / p[i].d) * (u * gky + v * gkx);
		rh[j].ssr_10 += (p[i].m / p[i].d) * (u * gky + v * gkx);
		rh[j].ssr_11 += (p[i].m / p[i].d) * (u * gkx + v * gky) - (p[i].m / p[i].d) * (u * gkx + v * gky);

		if (par->T_HEAT_TRANSFER == 1)
		{
			rh[j].qx -= p[j].co * (p[i].m / p[i].d) * t * gkx;
			rh[j].qy -= p[j].co * (p[i].m / p[i].d) * t * gky;
		}
	}
}

void momentum_and_energy(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, g = 0.0, t = 0.0;
	double pres = 0.0, visc_00 = 0.0, visc_01 = 0.0, visc_10 = 0.0, visc_11 = 0.0, force = 0.0;
	double hpres = 0.0;
	double heat = 0.0;
	double C2 = 2.0;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	pres = (p[i].p + p[j].p) / (p[i].d*p[j].d);
	visc_00 = (p[i].nu * rh[i].ssr_00 + p[j].nu * rh[j].ssr_00) / (p[i].d*p[j].d);
	visc_01 = (p[i].nu * rh[i].ssr_01 + p[j].nu * rh[j].ssr_01) / (p[i].d*p[j].d);
	visc_10 = (p[i].nu * rh[i].ssr_10 + p[j].nu * rh[j].ssr_10) / (p[i].d*p[j].d);
	visc_11 = (p[i].nu * rh[i].ssr_11 + p[j].nu * rh[j].ssr_11) / (p[i].d*p[j].d);
	heat = ((rh[i].qx + rh[j].qx) * gkx + (rh[i].qy + rh[j].qy) * gky) / (p[i].d*p[j].d);

	switch (T) {
	case 0:
		rh[i].u -= p[j].m * (pres * gkx + visc_00 * gkx + visc_01 * gky);
		rh[j].u += p[i].m * (pres * gkx + visc_00 * gkx + visc_01 * gky);
		rh[i].v -= p[j].m * (pres * gky + visc_11 * gky + visc_10 * gkx);
		rh[j].v += p[i].m * (pres * gky + visc_11 * gky + visc_10 * gkx);

		if (par->T_HEAT_TRANSFER == 1)
		{
			rh[i].e += 0.5 * p[j].m * pres * (u * gkx + v * gky)
				//+ 0.5 * (p[i].nu / p[i].d) * (pow2(rh[i].ssr_00) + pow2(rh[i].ssr_11) + pow2(rh[i].ssr_01) + pow2(rh[i].ssr_10))
				+ p[j].m * heat;
			rh[j].e += 0.5 * p[i].m * pres * (u * gkx + v * gky)
				//+ 0.5 * (p[j].nu / p[j].d) * (pow2(rh[j].ssr_00) + pow2(rh[j].ssr_11) + pow2(rh[j].ssr_01) + pow2(rh[j].ssr_10))
				- p[i].m * heat;
		}

		break;
	default:
		rh[j].u += p[i].m * (pres * gkx + visc_00 * gkx + visc_01 * gky);
		rh[j].v += p[i].m * (pres * gky + visc_11 * gky + visc_10 * gkx);
		break;

		if (par->T_HEAT_TRANSFER == 1)
		{
			rh[j].e += 0.5 * p[i].m * pres * (u * gkx + v * gky)
				//+ 0.5 * (p[j].nu / p[j].d) * (pow2(rh[j].ssr_00) + pow2(rh[j].ssr_11) + pow2(rh[j].ssr_01) + pow2(rh[j].ssr_10))
				- p[i].m * heat;
		}
	}
}

void attraction(unsigned int i, unsigned int j, unsigned int T, Particle* p, Parameters* par)
{
	double x, y, u, v, r2, q, gkx, gky, g = 0.0, t = 0.0;
	double pres = 0.0;
	double A = 2.0;

	interaction(i, j, &x, &y, &u, &v, T, p, par);

	r2 = x * x + y * y;
	q = sqrt(r2) * par->I_H;
	gkx = grad_of_kernel(x, q);
	gky = grad_of_kernel(y, q);

	switch (T) {
	case 0:
		rh[i].u += 2.0 * A * p[j].m * gkx;
		rh[i].v += 2.0 * A * p[j].m * gky;
		rh[j].u -= 2.0 * A * p[i].m * gkx;
		rh[j].v -= 2.0 * A * p[i].m * gky;
		rh[i].e -= 2.0 * A * p[j].m * (u * gkx + v * gky);
		rh[j].e -= 2.0 * A * p[i].m * (u * gkx + v * gky);
		break;
	default:
		rh[j].u -= 2.0 * A * p[i].m * gkx;
		rh[j].v -= 2.0 * A * p[i].m * gky;
		rh[j].e -= 2.0 * A * p[i].m * (u * gkx + v * gky);

	}
}
#endif
