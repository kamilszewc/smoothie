#include "TimeStep.h"
#include "hlp.h"

TimeStep::TimeStep(Particle* p, Parameters* par)
{
	_p = p;
	_par = par;
	_initialTimeStep = par->DT;
}

void TimeStep::SetInitialTimeStep(double initialTimeStep)
{
	_initialTimeStep = initialTimeStep;
}

double TimeStep::GetInitialTimeStep()
{
	return _initialTimeStep;
}

double TimeStep::Get()
{
	if (_par->T_TIME_STEP == 0)
	{
		return _initialTimeStep;
	}
	else
	{
		// Advection ?
		double highestVelocity = 0.0;
		double highestSoundSpeed = 0.0;
		for (int i = 0; i < _par->N; i++)
		{
			double particleVelocity = sqrt(pow2(_p[i].vel.x) + pow2(_p[i].vel.y));
			if (particleVelocity > highestVelocity)
			{
				highestVelocity = particleVelocity;
			}

			if (_p[i].s > highestSoundSpeed)
			{
				highestSoundSpeed = _p[i].s;
			}
		}

		double timeStep1 = 0.25 * _par->H / (highestSoundSpeed + highestVelocity);

		// Viscosity
		double highestKinematicViscosity = 0.0;
		for (int i = 0; i < _par->N; i++)
		{
			double particleKinematicViscosity = _p[i].mi / _p[i].d;
			if (particleKinematicViscosity > highestKinematicViscosity)
			{
				highestKinematicViscosity = particleKinematicViscosity;
			}
		}

		double timeStep2 = 0.125 * pow2(_par->H) / highestKinematicViscosity;

		// Acceleration
		double highestAcceleration = sqrt(pow2(_par->G_X) + pow2(_par->G_Y));
		double timeStep3 = 0.25 * sqrt( _par->H / highestAcceleration );

		// Surface-tension
		double highestSurfaceTensionCoefficient = 0.0;
		if (_par->T_SURFACE_TENSION > 0)
		{
			highestSurfaceTensionCoefficient = _par->SURFACE_TENSION_COEFFICIENT->GetHighestValue();

		}

		double highestDensity = 0.0;
		for (int i = 0; i < _par->N; i++)
		{
			if (_p[i].d >= highestDensity)
			{
				highestDensity = _p[i].d;
			}
		}
		double smallestDensity = highestDensity;
		for (int i = 0; i < _par->N; i++)
		{
			if (_p[i].d <= smallestDensity)
			{
				smallestDensity = _p[i].d;
			}
		}
		double timeStep4 = 0.25 * sqrt(smallestDensity * pow3(_par->H) / (2.0*M_PI*highestSurfaceTensionCoefficient));


		if ((timeStep1 <= timeStep2) && (timeStep1 <= timeStep3) && (timeStep1 <= timeStep4))
		{
			if (_par->T_MODEL == 2) return timeStep1;
			else return timeStep1 * 0.1;
		}
		else if ((timeStep2 <= timeStep1) && (timeStep2 <= timeStep3) && (timeStep2 <= timeStep4))
		{
			if (_par->T_MODEL == 2) return timeStep1;
			else return timeStep2 * 0.1;
		}
		else if ((timeStep3 <= timeStep1) && (timeStep3 <= timeStep2) && (timeStep3 <= timeStep4))
		{
			if (_par->T_MODEL == 2) return timeStep1;
			else return timeStep3 * 0.1;
		}
		else if ((timeStep4 <= timeStep1) && (timeStep4 <= timeStep2) && (timeStep4 <= timeStep3))
		{
			if (_par->T_MODEL == 2) return timeStep1;
			else return timeStep4 * 0.1;
		}
		else
		{
			return 0.0;
		}
	}
}