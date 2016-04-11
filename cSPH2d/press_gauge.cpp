/* All right reserved by Kamil Szewc, Gdansk 2011 */

#ifndef __SPH_PRESS_GAUGE__
#define __SPH_PRESS_GAUGE__

#include "sph.h"

void press_gauge(Particle *p, Parameters *par) 
{
    int i;
    double mean_p = 0.0;

	for (i = 0; i < par->N; i += 10) {
        mean_p += p[i].p;
	mean_p += p[i+1].p;
	mean_p += p[i+2].p;
	mean_p += p[i+3].p;
	mean_p += p[i+4].p;
	mean_p += p[i+5].p;
	mean_p += p[i+6].p;
	mean_p += p[i+7].p;
	mean_p += p[i+8].p;
	mean_p += p[i+9].p;
    }
	mean_p = mean_p / (double)par->N;
	for (i = 0; i<par->N; i++) p[i].p -= mean_p;

    mean_p = 0.0;
	for (i = 0; i < par->N; i++) {
        if (mean_p > p[i].p) mean_p = p[i].p;
    }
	for (i = 0; i < par->N; i++) p[i].p -= mean_p;
}

void hydrostatic_press_gauge(Particle* p, Parameters *par) 
{
    int i;
    double mean_p = 0.0;

	for (i = 0; i < par->N; i += 10) {
        mean_p += p[i].hp;
	mean_p += p[i+1].hp;
	mean_p += p[i+2].hp;
	mean_p += p[i+3].hp;
	mean_p += p[i+4].hp;
	mean_p += p[i+5].hp;
	mean_p += p[i+6].hp;
	mean_p += p[i+7].hp;
	mean_p += p[i+8].hp;
	mean_p += p[i+9].hp;
    }
	mean_p = mean_p / (double)par->N;
	for (i = 0; i<par->N; i++) p[i].hp -= mean_p;

    mean_p = 0.0;
	for (i = 0; i < par->N; i++) {
        if (mean_p > p[i].hp) mean_p = p[i].hp;
    }
	for (i = 0; i < par->N; i++) p[i].hp -= mean_p;
}

#endif
