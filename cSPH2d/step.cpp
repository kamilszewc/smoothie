/* All right reserved by Kamil Szewc, Gdansk 2009 */

#ifndef SPH_STEP
#define SPH_STEP

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "sph.h"

void wcsph(Particle*, Parameters*);
void sph_general(Particle*, Parameters*);
void isph_pps(Particle*, Parameters*);
void isph_grid(Particle*, Parameters*);
void isph_pps_pozorski(Particle*, Parameters*);

void step(Particle* p, Parameters *par)
{
	switch (par->T_MODEL) {
	case 0:
	case 1:
	case 2:
	case 3:
		wcsph(p, par);
		break;
	case 4: // General
		sph_general(p, par);
		break;
	case 5:
		isph_pps(p, par);
		break;
	case 6:
		isph_pps_pozorski(p, par);
		break;
	case 7:
		isph_grid(p, par);
		break;
	default:
		perror("Type of time step is not defined (T_MODEL)");
		exit(EXIT_FAILURE);
	}

}

#endif
