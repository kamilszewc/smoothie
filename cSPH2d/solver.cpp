/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_SOLVER
#define SPH_SOLVER

#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sph.h"

void matrix_vector_product(double *y, atom *M[], int ncol[], double x[], int N);
double get_element(int c, int r, atom *M[], int ncol[]);

double vsv(double *x1, double *x2, int n)
{
	int i;
	double sum;
	sum = 0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+:sum)
#endif
	for (i = 0; i < n; i++) {
		sum = sum + x1[i] * x2[i];
	}
	return sum;
}

void sv(double *y, double alpha, double *x, int n)
{
	int i;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (i = 0; i < n; i++) {
		y[i] = alpha * x[i];
	}
}

void vmv(double *y, double *x1, double *x2, int n)
{
	int i;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (i = 0; i < n; i++) {
		y[i] = x1[i] - x2[i];
	}
}

void vpv(double *y, double *x1, double *x2, int n)
{
	int i;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (i = 0; i < n; i++) {
		y[i] = x1[i] + x2[i];
	}
}

void vcopy(double *y, double *x, int n)
{
	int i;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (i = 0; i < n; i++) {
		y[i] = x[i];
	}
}

double res_2_sum(double *r, int n)
{
	int i;
	double res;
	
	res = 0.0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+:res)
#endif
	for (i = 0; i < n; i++) {
		res += r[i]*r[i];
	}

	return res;
}

double residuum(double *x, int n)
{
	double *r;
	double *y;
	double result;
	r = (double *)malloc(n * sizeof(double));
	y = (double *)malloc(n * sizeof(double));

	matrix_vector_product(y, M, ncol, x, n);
	vmv(r, y, b, n);
	result = res_2_sum(r, n);

	free(r);
	free(y);

	return result;
}

void unpre_bcgstab(double *x, int n)
{
	double *y, *r, *ar;
	double rho0, rho1, alpha, om;
	double *v, *pe;
	int i;
	double beta, *s, *t;
	double res0, res1;

	double *y1;

	y = (double *)calloc(n, sizeof(double));
	r = (double *)calloc(n, sizeof(double));
	ar = (double *)calloc(n, sizeof(double));
	v = (double *)calloc(n, sizeof(double));
	pe = (double *)calloc(n, sizeof(double));
	s = (double *)calloc(n, sizeof(double));
	t = (double *)calloc(n, sizeof(double));
	y1 = (double *)calloc(n, sizeof(double));

	matrix_vector_product(y, M, ncol, x, n);
	vmv(r, b, y, n);
	vcopy(ar, r, n);

	rho0 = 1.0;
	rho1 = 1.0;
	alpha = 1.0;
	om = 1.0;
	for (i = 0; i < n; i++) {
		v[i] = 0.0;
		pe[i] = 0.0;
	};

	res0 = res_2_sum(r, n);
	res1 = res0;

	i = 0;
	while ( sqrt(res1/res0) > 1E-7 /*i < 200*/ ) {
		/* Poczatek iteracji */
		rho1 = vsv(ar, r, n);
		beta = (rho1 / rho0) * (alpha / om);
		sv(y, om, v, n);
		vmv(y, pe, y, n);
		sv(y, beta, y, n);
		vpv(pe, r, y, n);
		matrix_vector_product(v, M, ncol, pe, n);
		alpha = rho1 / vsv(ar, v, n);
		sv(y, alpha, v, n);
		vmv(s, r, y, n);
		matrix_vector_product(t, M, ncol, s, n);
		om = vsv(t, s, n) / vsv(t, t, n);
		sv(y, om, s, n);

		sv(y1, alpha, pe, n);

		vpv(y, y1, y, n);

		vpv(x, x, y, n);	/* Tutaj oblicznie */

		sv(y, om, t, n);
		vmv(r, s, y, n);

		rho0 = rho1;
		/* Koniec iteracji */

		res1 = res_2_sum(r, n);
		i++;
		if (i%500 == 0) printf("res=%e\n", sqrt(res1/n));
	} 
	printf("res0 = %e, res1 = %e, res1/res0 = %e, M = %d\n", sqrt(res0/n), sqrt(res1/n), sqrt(res1/res0), i);
	

	free(y);
	free(r);
	free(ar);
	free(v);
	free(pe);
	free(s);
	free(t);
	free(y1);
}

void sor(double *x, int n) {
	double s;
	int i,j,k;
	double *y;
	double omega=1.4;
	double res0, res1=0.0;
	y = (double *)malloc(n * sizeof(double));

	res0 = sqrt(residuum(x, n)/(double)n);

	k=0;
	do {
		for(i=0; i<n; i++) {
			s = 0.0;
			for(j=0; j<i; j++) s += get_element(j, i, M, ncol)*x[j]; /* TO CHECK */
			for(j=i+1; j<n; j++) s+= get_element(j, i, M, ncol)*y[j]; /* TO CHECK */
			y[i] = (1.0-omega)*x[i] + omega*(b[i]-s)/get_element(i, i, M, ncol);
		}
		vcopy(x,y,n);
		if (k%20 == 0) res1 = sqrt(residuum(x, n)/(double)n);
		
		k++;
	} while (res1 > 1E-6 /*k<200*/);
	printf("res0=%e, res1=%e res0/res1=%e, k=%d\n", res0, res1, res1/res0, k);

	free(y);
}

void linear(double *x, int n)
{
	T_LIN = 0;
	switch (T_LIN) {
	case 0:
		unpre_bcgstab(x, n);
		break;
	case 1:
		sor(x, n);
		break;
	default:
		break;
	}
}

#endif
