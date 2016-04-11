
#include "StatisticsPrinter.h"
#include <iostream>

StatisticsPrinter::StatisticsPrinter(time_t time, double numericalTime, int stepNumber, Particle* p, Parameters* par)
{
	_timeAtLastPrintOut = time;
	_initialTime = time;
	_numericalTimeAtLastPrintOut = numericalTime;
	_stepNumberAtLastPrintOut = stepNumber;
	_par = par;
	_p = p;

	std::ofstream file1("framesPerSecond.out", std::ios::out);
	if (file1.is_open())
	{
		file1 << 0.0 << " " << 0.0 << " " << 0.0 << std::endl;
		file1.close();
	}

	std::ofstream file2("averageDT.out", std::ios::out);
	if (file2.is_open())
	{
		file2.close();
	}

	std::ofstream file3("timeToEnd.out", std::ios::out);
	if (file3.is_open())
	{
		file3 << 0.0 << " " << 0.0 << " " << 0.0 << " " << 0.0 << std::endl;
		file3.close();
	}

	std::ofstream file4("kineticEnergy.out", std::ios::out);
	if (file4.is_open())
	{
		file4 << 0.0 << " " << 0.0 << " " << GetKineticEnergy() << std::endl;
		file4.close();
	}
}

void StatisticsPrinter::Print(double numericalTime, int stepNumber)
{
	int numberOfSteps = stepNumber - _stepNumberAtLastPrintOut;
	double timeDifference = difftime(time(NULL), _timeAtLastPrintOut);
	double numericalTimeDifference = numericalTime - _numericalTimeAtLastPrintOut;

	double framesPerSecond = (double)numberOfSteps / timeDifference;
	std::ofstream file1("framesPerSecond.out", std::ios::app);
	if (file1.is_open())
	{
		file1 << numericalTime << " " << difftime(time(NULL), _initialTime) << " " << framesPerSecond << std::endl;
		file1.close();
	}

	double averageDT = (numericalTime - _numericalTimeAtLastPrintOut) / (double)(stepNumber - _stepNumberAtLastPrintOut);
	std::ofstream file2("averageDT.out", std::ios::app);
	if (file2.is_open())
	{
		file2 << numericalTime << " " << difftime(time(NULL), _initialTime) << " " << averageDT << std::endl;
		file2.close();
	}

	double totalTime = difftime(time(NULL), _initialTime) * (_par->END_TIME) / (numericalTime);
	double timeToEnd = totalTime - difftime(time(NULL), _initialTime);
	std::ofstream file3("timeToEnd.out", std::ios::app);
	if (file3.is_open())
	{
		file3 << numericalTime << " " << difftime(time(NULL), _initialTime) << " " << timeToEnd << " "<< totalTime << std::endl;
		file3.close();
	}

	std::ofstream file4("kineticEnergy.out", std::ios::app);
	if (file4.is_open())
	{
		file4 << numericalTime << " " << difftime(time(NULL), _initialTime) << " " << GetKineticEnergy() << std::endl;
		file4.close();
	}

	_timeAtLastPrintOut = time(NULL);
	_numericalTimeAtLastPrintOut = numericalTime;
	_stepNumberAtLastPrintOut = stepNumber;
}

double StatisticsPrinter::GetKineticEnergy()
{
	double kineticEnergy = 0.0;

	for (int i = 0; i < _par->N; i++)
	{
		kineticEnergy += 0.5 * _p[i].m * (pow2(_p[i].vel.x) + pow2(_p[i].vel.y));
	}

	return kineticEnergy;
}