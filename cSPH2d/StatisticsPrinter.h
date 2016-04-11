#if !defined(__STATISTICS_PRINTER_H__)
#define __STATISTICS_PRINTER_H__

#include <fstream>
#include <ctime>
#include "sph.h"
#include "hlp.h"

class StatisticsPrinter
{
private:
	time_t _timeAtLastPrintOut;
	time_t _initialTime;
	int _stepNumberAtLastPrintOut;
	double _numericalTimeAtLastPrintOut;
	Particle* _p;
	Parameters* _par;

	double GetKineticEnergy();
public:
	StatisticsPrinter(time_t, double, int, Particle*, Parameters*);
	void Print(double, int);
};

#endif