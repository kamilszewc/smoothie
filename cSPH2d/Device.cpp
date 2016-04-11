#ifdef _OPENMP
#include <omp.h>
#endif

#include <fstream>
#include <iostream>

#include "Device.h"

Device::Device()
{
	SetMaxNumberOfThreads();
}

void Device::SetNumberOfThreads(int numberOfThreads)
{
#ifdef _OPENMP
	_numberOfThreads = numberOfThreads;
	omp_set_num_threads(_numberOfThreads);
#else
	_numberOfThreads = 1;
#endif
}

void Device::SetNumberOfThreadsFromFile(const char* filename)
{
	std::ifstream file(filename);
	if (file.is_open())
	{
		int numberOfThreads;
		file >> numberOfThreads;
		file.close();

		SetNumberOfThreads(numberOfThreads);
	}
}

void Device::SetMaxNumberOfThreads()
{
#ifdef _OPENMP
	_numberOfThreads = omp_get_max_threads();
	omp_set_num_threads(_numberOfThreads);
#else
	_numberOfThreads = 1;
#endif
}