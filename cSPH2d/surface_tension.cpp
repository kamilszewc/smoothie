/* All right reserved by Kamil Szewc, Gdansk 2011 */

#ifndef SPH_SURFACE_TENSION
#define SPH_SURFACE_TENSION

#include <math.h>
#include "sph.h"
#include "hlp.h"

void set_rhoe(int*, int*, Particle*, Parameters*, void (*function) (unsigned int, unsigned int, unsigned int, Particle*, Parameters*));
void curvature(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void normal_after_smoothed(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);
void surface_tension_accel(unsigned int, unsigned int, unsigned int, Particle*, Parameters*);

void surface_tension(int *head, int *list, Particle *p, Parameters *par)
{
    int i;
	
	switch (par->T_SURFACE_TENSION)
	{
	case 1:
		break;
	case 2:
	case 3:
		set_rhoe(head, list, p, par, &normal_after_smoothed);
		break;
	}

    double eps = 0.0;
    for (i = 0; i < par->N; i++) 
	{
        p[i].nn = sqrt( pow2(p[i].n.x) + pow2(p[i].n.y) );
        eps += p[i].nn;
    }
   /*eps *= EPS / N;*/
	eps = 0.01 / par->H;
	for (i = 0; i < par->N; i++) 
	{
		if (p[i].nn > eps) p[i].na = 1;
		else p[i].na = 0;
		if ((par->T_SURFACE_TENSION == 1) && (p[i].nn > ZERO)) p[i].na = 1;
    }

	if (par->T_SURFACE_TENSION != 3)
	{
		set_rhoe(head, list, p, par, &curvature);
		
		if (par->T_SURFACE_TENSION == 2) for (i = 0; i<par->N; i++) 
		{
			if (rh[i].cw > ZERO) p[i].cu = p[i].cu/rh[i].cw;
		}
			
		for (i = 0; i < par->N; i++) 
		{
            if ( (p[i].nn > ZERO) && (rh[i].sigw > 0.0) ) 
			{
				double st = rh[i].sig / rh[i].sigw;
                p[i].st.x = (st/p[i].d) * p[i].cu * p[i].n.x;
                p[i].st.y = (st/p[i].d) * p[i].cu * p[i].n.y;
            }
        }
    }

		
	if (par->T_SURFACE_TENSION == 3) 
	{
        double help = 0.0;
		for (i = 0; i<par->N; i++) 
		{
            double dc2 = pow2(p[i].n.x) + pow2(p[i].n.y);
            if ( (dc2 > ZERO) && (rh[i].sigw > 0.0) ) 
			{
				double st = rh[i].sig / rh[i].sigw;
				double cons = st / sqrt(dc2);
                rh[i].is_00 = cons * ( dc2 - p[i].n.x * p[i].n.x );
                rh[i].is_11 = cons * ( dc2 - p[i].n.y * p[i].n.y );
                rh[i].is_10 = cons * ( - p[i].n.x * p[i].n.y );
                rh[i].is_01 = cons * ( - p[i].n.y * p[i].n.x );
            }
            if (sqrt(dc2) > help) help = sqrt(dc2);
        }
		set_rhoe(head, list, p, par, &surface_tension_accel);
    }

}
#endif
