/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_SPARSE_MATRIX
#define SPH_SPARSE_MATRIX

#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "sph.h"

INLINE void add_atom(int c, int r, double val, atom *M[], int ncol[])
{
        if (ncol[r] == 0) M[r] = (atom *)malloc(sizeof(atom));
        else M[r] = (atom *)realloc(M[r], (ncol[r]+1) * sizeof(atom));
        M[r][ncol[r]].col = c;
        M[r][ncol[r]].val = val;
        ncol[r] += 1;
}

void add_element(int c, int r, double val, atom *M[], int ncol[])
{
        int i;
        int ver = 0;
        for (i=0; i<ncol[r]; i++) {
                if (M[r][i].col == c) {
                        M[r][i].val += val;
                        ver = 1;
                        break;
                }
        }
        if (ver == 0) add_atom(c, r, val, M, ncol);
}

double get_element(int c, int r, atom *M[], int ncol[])
{
	int i;
	double val = 0.0;
	for (i=0; i<ncol[r]; i++) {
		if (M[r][i].col == c) {
			val = M[r][i].val;
			break;
		}
	}
	return val;
}

void matrix_vector_product(double *y, atom *M[], int ncol[], double x[], int N)
{
        int i, j;
		double tmp;
#ifdef _OPENMP
		#pragma omp parallel for private(i, tmp)
#endif
        for (i=0; i<N; i++) {
                tmp = 0.0;
                for (j=0; j<ncol[i]; j++) {
                        tmp += M[i][j].val * x[M[i][j].col];
                }
				y[i] = tmp;
        }
}

#endif

