/*
* domain.hpp
*
*  Created on: 01-10-2013
*      Author: Kamil Szewc (kamil.szewc@gmail.com)
*/
#if !defined(__DOMAIN_H__)
#define __DOMAIN_H__

#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include "sph.h"
#include "hlp.h"

class Domain {
private:
	Particle *p;
	Parameters par;
	double time;
	std::string phasesXmlInformation;
	const char* _outputDirectory;

	void InitDefaultGeometry(int CASE);
	void InitDefaultParameters(double HDR);
	void InitCase(int CASE);
	void SetParticles();
	bool IsConsistentWithGeometry();
	bool IsConsistentWithSearchAlgorithm();
public:
	Domain(char*);
	Domain(int, double, double);
	Domain(int, double, int);
	Domain(int, double, int, int);
	~Domain();

	Particle* GetParticles();
	Parameters* GetParameters();
	void SetModel(int);
	double* GetTime();
	const char* GetOutputDirectory();
	void SetOutputDirectory(const char *);
	void WriteParametersToFile(const char*);
	void WriteParametersToFile();
	void WriteParticlesToFile(const char*);
	void WriteParticlesToFile();
	void WritePhaseToFile(const char*, int);
	void WritePhaseToFile(int);
	void WriteToXmlFile(const char*);
	void WriteToXmlFile();
	double GetAndWriteKinetic(const char*);
	double GetAndWriteKinetic();
	int GetSizeOfParticles();
	int GetSizeOfParameters();
};

#endif