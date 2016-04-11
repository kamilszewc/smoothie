/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_LIST
#define SPH_LIST

#include <stdio.h>
/*#ifndef _DEBUG
    #include <omp.h>
#endif  _DEBUG */
#include "sph.h"

void set_linked_list(int *head, int *list, Particle* p, Parameters* par)
{
	int xc, yc, c;
	int i;
	for (i = 0; i < par->NC; i++) head[i] = -1;
	for (i = 0; i < par->N; i++) list[i] = -1;

	/*#pragma omp parallel for private(i,xc,yc,c) shared(head, list,p,I_H,NCX,NCY) */
	for (i = 0; i < par->N; i++) {
		if (T_KERNEL <= 3) { 
			xc = (int)(0.5 * (-XCV_0 + p[i].pos.x) * par->I_H);
			yc = (int)(0.5 * (-YCV_0 + p[i].pos.y) * par->I_H);
		} else {
			xc = (int)( (-XCV_0 + p[i].pos.x) * par->I_H / 3.0 );
			yc = (int)( (-YCV_0 + p[i].pos.y) * par->I_H / 3.0 );
		}
		c = xc + yc * par->NCX;
		if ((xc < par->NCX) && (yc < par->NCY) && (xc >= 0) && (yc >= 0)) {
			list[i] = head[c];
			head[c] = i;
		} else {
			/*printf("%i %i %i %i\n", xc, NCX, yc, NCY);
			printf("Particle outside boundaries !!! (list.c)\n");*/
		}
	}
}

//void find_b_part(int *bound, int *cell)
//{
//	int xc, yc, c;
//	int i;
//	int k;
//	for (i = 0; i < N; i++) {
//		cell[i] = -1;
//		bound[i] = -1;
//	}
//	k = 0;
//
//	/*#pragma omp parallel for private(i,xc,yc,c) shared(head, list,p,I_H,NCX,NCY) */
//	for (i = 0; i < N; i++) {
//		if (T_KERNEL <= 3) { 
//			xc = (int)(0.5 * (-XCV_0 + p[i].pos.x) * I_H);
//			yc = (int)(0.5 * (-YCV_0 + p[i].pos.y) * I_H);
//		} else {
//			xc = (int)( (-XCV_0 + p[i].pos.x) * I_H / 3.0 );
//			yc = (int)( (-YCV_0 + p[i].pos.y) * I_H / 3.0 );
//		}
//		c = xc + yc * NCX;
//		if ((xc < NCX) && (yc < NCY) && (xc >= 0) && (yc >= 0)) {
//			/* Additional numeration */
//			if ( (T_BOUNDARY_PERIODICITY != 1) && (c >= NC - NCX) )      { bound[k] = i; cell[k] = 1; k++; }
//			if ( (T_BOUNDARY_PERIODICITY == 0) && (c % NCX == NCX - 1) ) { bound[k] = i; cell[k] = 2; k++; }
//			if ( (T_BOUNDARY_PERIODICITY != 1) && (c < NCX) )            { bound[k] = i; cell[k] = 3; k++; }
//			if ( (T_BOUNDARY_PERIODICITY == 0) && (c % NCX == 0) )       { bound[k] = i; cell[k] = 4; k++; }
//			if ( (T_BOUNDARY_PERIODICITY == 0) ) {
//				if (c==NC-1)   { bound[k] = i; cell[k] = 5; k++; }
//				if (c==NCX-1)  { bound[k] = i; cell[k] = 6; k++; }
//				if (c==0)      { bound[k] = i; cell[k] = 7; k++; }
//				if (c==NC-NCX) { bound[k] = i; cell[k] = 8; k++; }
//			}
//		} else {
//			printf("Particle outside boundaries !!! (list.c)\n");
//		}
//	}
//	nopic = k;
//}

#endif
