/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef __CELL_INT__
#define __CELL_INT__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sph.h"

void cell_int(const unsigned int ic, const unsigned int jc, int *head, int *list, Particle* p, Parameters *par, void(*function) (unsigned int, unsigned int, unsigned int, Particle*, Parameters*))
{	/* Steps between Particles in cells */
	volatile int i = head[ic];
	volatile int j = head[jc];

	volatile unsigned int NC = (unsigned)par->NC;
	volatile unsigned int NCX = (unsigned)par->NCX;
	volatile unsigned int icModNCX = ic % par->NCX;
	volatile unsigned int jcModNCX = jc % par->NCX;
	volatile unsigned int NCmin1 = NC - 1;
	volatile unsigned int NCXmin1 = NCX - 1;
	volatile unsigned int NCminNCX = NC - NCX;

	switch (par->T_BOUNDARY_CONDITION)
	{
	case 0:
		break;
	case 1:
		while (i >= 0) {
			while (j >= 0) {
				function(i, j, 0, p, par);

				if (par->T_BOUNDARY_PERIODICITY != 1) {
					if ((ic >= NCminNCX) && (jc >= NCminNCX)) { /* N */
						function(i, j, 1, p, par);
						if (i != j) function(j, i, 1, p, par);
					}
					else if ((ic < NCX) && (jc < NCX)) { /* S */
						function(i, j, 3, p, par);
						if (i != j) function(j, i, 3, p, par);
					}
				}
				if (par->T_BOUNDARY_PERIODICITY == 0) {
					
					if ((icModNCX == 0) && (jcModNCX == 0)) {	/* W */
						function(i, j, 4, p, par);
						if (i != j) function(j, i, 4, p, par);
					}
					else if ((icModNCX == NCXmin1) && (jcModNCX == NCXmin1)) {	/* E */
						function(i, j, 2, p, par);
						if (i != j) function(j, i, 2, p, par);
					}

					if (ic == jc) // Corners
					{
						if (ic == NCmin1) {	    /* NE */
							function(i, j, 5, p, par);
							if (i != j) function(j, i, 5, p, par);
						}
						else if (ic == NCXmin1) {   /* SE */
							function(i, j, 6, p, par);
							if (i != j) function(j, i, 6, p, par);
						}
						else if (ic == 0) {	            /* SW */
							function(i, j, 7, p, par);
							if (i != j) function(j, i, 7, p, par);
						}
						else if (ic == NCminNCX) { /* NW */
							function(i, j, 8, p, par);
							if (i != j) function(j, i, 8, p, par);
						}
					}
				}

				j = list[j];
			}
			i = list[i];
			if (ic != jc) j = head[jc];
			if (ic == jc) j = i;
		}
		break;
	default:
		perror("Type of boundaries is not defined (T_BOUNDARY_CONDITION)");
		exit(EXIT_FAILURE);
	}
}

#endif
