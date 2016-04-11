/*
 *  All right reserved by:
 *  Kamil Szewc,
 *  Institute of the Fluid-Flow Machinery, Polish Academy of Science, Gdańsk PL
 *  Universite Henri Poincare, ESSTIN, Nancy FR
 */

#ifndef SPH_MAIN
#define SPH_MAIN

#ifdef _OPENMP
#include <omp.h>
#endif
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include "sph.h"
#include "hlp.h"
#include "terminal.h"
#include "Domain.h"
#include "TimeStep.h"
#include "Device.h"
#include "StatisticsPrinter.h"

void set_case();
void output(double t, Particle*, Parameters*);
void step(Particle*, Parameters*);
double output_kinetic(double, Particle*, Parameters*);
void read_raw_files(const char *);
void read_parameters(const char *);

unsigned int NIC;

double XCV_0 = 0.0;
double YCV_0 = 0.0;

double H, H2, H3, H4;    
double I_H, I_H2, I_H3, I_H4;         
double DH;
double KNORM;
double GKNORM;
double DR;
  
unsigned int STEP;

unsigned int S_RESTARTED;

/*  Type of kernel:                  */
/*  0 - gaussian      1 - quadratic, */
/*  2 - cubic spline  3 - quintic    */
/*  4 - quintic Morris, Fox, Zhu     */
unsigned int T_KERNEL = 3;

/* Linear solver */
unsigned int T_LIN = 0;

unsigned int T_OUTPUT = 0;

/* Is the Bessinesq approx in use   */
/* 0 - off, 1 - standard Boussinesq */
/* 2 - new approach                 */
unsigned int T_BOUSSINESQ = 0;


double APRESSURE = 0.0; /* Atmospheric pressure */
double PART_IN_GRAVIT_CELL = 5.0;
double EPS = 1.0;

double Cell = 2.0;

rhoe *rh;
double *b;
int *ncol;
atom **M;
int nopic;

//Particle *p;
Particle *pv, *ph;

Particle *_p;

int main(int argc, char *argv[])
{
	//terminal(argc, argv);

	// CPU configuration
	Device device;
	device.SetNumberOfThreadsFromFile(".numberOfThreads");

	// Choice of a case and model
	int CASE = 5;
	int MODEL = 3;
	int OUT_PHASE = 0;

	// Declaration of CPU data variables
	Parameters *par;
	Particle *p;

	// Data INPUT
	Domain *domain;
	double *t;
	switch (argc) {
	case 1:
		domain = new Domain(CASE, 2.0, 16);
		par = domain->GetParameters();
		p = domain->GetParticles();
		t = domain->GetTime();
		par->T_MODEL = MODEL;
		domain->SetModel(MODEL);
		break;
	case 2:
		domain = new Domain(argv[1]);
		par = domain->GetParameters();
		p = domain->GetParticles();
		t = domain->GetTime();
		MODEL = par->T_MODEL;
		break;
	case 3:
		domain = new Domain(argv[1]);
		domain->SetOutputDirectory(argv[2]);
		par = domain->GetParameters();
		p = domain->GetParticles();
		t = domain->GetTime();
		MODEL = par->T_MODEL;
		break;
	default:
		std::cout << "-- Simulation did not start --" << std::endl;
		exit(EXIT_FAILURE);
	}
	rh = new rhoe[par->N];

	domain->WriteToXmlFile();
	if (argc == 1)
	{
		domain->WriteParticlesToFile();
		domain->WriteParametersToFile();
	}

	double t0 = *t;

	S_RESTARTED = 0;

	for (int i = 0; i < par->N; i++)
	{
		rh[i].x = 0.0; rh[i].y = 0.0; rh[i].u = 0.0; rh[i].v = 0.0; rh[i].uc = 0.0; rh[i].vc = 0.0;
		rh[i].d = 0.0; rh[i].dc = 0.0; rh[i].b = 0.0; rh[i].fx = 0.0; rh[i].fy = 0.0;
		p[i].st.x = 0.0; p[i].st.y = 0.0; rh[i].sig = 0.0; rh[i].sigw = 0.0;
		rh[i].is_00 = 0.0, rh[i].is_10 = 0.0, rh[i].is_01 = 0.0, rh[i].is_11 = 0.0;
		rh[i].cs = 0.0; rh[i].csw = 0.0; rh[i].cw = 0.0; rh[i].e = 0.0;
		rh[i].dp = 0.0; rh[i].dpc = 0.0;
		rh[i].ssr_00 = 0.0; rh[i].ssr_01 = 0.0; rh[i].ssr_10 = 0.0; rh[i].ssr_11 = 0.0;
		rh[i].qx = 0.0; rh[i].qy = 0.0;
	}

	H = par->H;

	H2 = pow2(H); H3 = pow3(H); H4 = pow4(H);
	I_H = 1.0 / H; I_H2 = pow2(I_H); I_H3 = pow3(I_H); I_H4 = pow4(I_H);/* Comp. 1/H             */
	DH = 0.01 * pow2(H);
	KNORM = M_1_PI * pow2(I_H);     /* Comp. normalization coeff of kernel */
	GKNORM = M_1_PI * pow4(I_H);    /* Comp. normalization coeff of grad kernel */
	DR = par->XCV / (double)par->NX;

	TimeStep timeStep(p, par);

	unsigned int timestep = 0;
	time_t cpu_timer_0 = time(NULL);
	time_t cpu_timer_1 = time(NULL);
	time_t cpu_timer_stat = time(NULL);

	STEP = 1;
	double timeOfLastPrintOut = 0.0;
	double timeOfLastStatPrintOut = 0.0;
	StatisticsPrinter statisticsPrinter(time(NULL), *t, STEP, p, par);
	while (*t < par->END_TIME) 
	{
		par->DT = timeStep.Get();
		step(p, par);	/* Make step in time. dt is setting inside */

		{  // Interaction with Smoothie
			int NUM_STEPS_FOR_STAT = 1000000 / par->N;

			if (STEP%NUM_STEPS_FOR_STAT == 0)
			{
				device.SetNumberOfThreadsFromFile(".numberOfThreads");
				statisticsPrinter.Print(*t, STEP);
			}
		}

		if ( ((*t - timeOfLastPrintOut + 0.5*par->DT) >= par->INTERVAL_TIME) )
		{
			timeOfLastPrintOut = *t;

			domain->WriteToXmlFile();
			if (argc == 1)
			{
				domain->WriteParticlesToFile();
			}

			double kinetic = domain->GetAndWriteKinetic();
			
			cpu_timer_1 = time(NULL);
			double cpu_etime = difftime(cpu_timer_1, cpu_timer_0);
			double cpu_rtime = cpu_etime * ((par->END_TIME) - t0) / (*t - t0);
			
			{ // Print informations about a current timestep
				std::cout << "Time: " << std::fixed << *t << " ";
				std::cout << "Elapsed time: " << (int)cpu_etime / 3600 << "h";
				std::cout << std::setfill('0') << std::setw(2) << ((int)cpu_etime % 3600) / 60 << "m";
				std::cout << std::setfill('0') << std::setw(2) << ((int)cpu_etime % 3600) % 60 << "s ";
				std::cout << "(" << (int)(100.0* *t / par->END_TIME) << "%) ";
				std::cout << "Total time: " << (int)cpu_rtime / 3600 << "h";
				std::cout << std::setfill('0') << std::setw(2) << ((int)cpu_rtime % 3600) / 60 << "m";
				std::cout << std::setfill('0') << std::setw(2) << ((int)cpu_rtime % 3600) % 60 << "s ";
				std::cout << "Kinetic: " << std::fixed << kinetic << std::endl;
			}
		}

		*t += par->DT;	/* Set point in time */
		timestep += 1;
		STEP++;
	}

	free(p);
	delete rh;

	if (par->T_INTEGRATION_SCHEME > 0) { free(pv); free(ph); }

	//delete domain;

	return 0;
}

#endif
