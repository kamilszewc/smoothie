#if !defined(__TIME_STEP_H__)
#define __TIME_STEP_H__

#include "sph.h"

class TimeStep
{
private:
	Particle* _p;
	Parameters* _par;
	double _initialTimeStep;
public:
	TimeStep(Particle*, Parameters*);
	void SetInitialTimeStep(double);
	double GetInitialTimeStep();
	double Get();
};


#endif