/*
* domain.cpp
*
*  Created on : 01 - 10 - 2013
* Author : Kamil Szewc(kamil.szewc@gmail.com)
*/

#define _CRT_SECURE_NO_WARNINGS

#include "Domain.h"
#include <random>

void read_raw_files(const char *filename, Particle *p, Parameters *par);
void read_parameters(const char *filename, Parameters *par);
void read_parameters_from_xml(const char *filename, Parameters *par);
std::string read_phases_from_xml(const char *filename, Parameters *par);
void read_particles_from_xml_file(const char* filename, Particle* p, Parameters* par);

void write_parameters(const char* filename, Parameters *par);
void write_particles(const char* filename, Particle *p, Parameters *par);
void write_phase(const char *filename, Particle *p, Parameters *par, int PHASE);
void write_to_xml(const char* filename, Particle *p, Parameters *par, std::string);

Domain::Domain(char *filename) {
	_outputDirectory = "Results/";
	/*time = atof(argv[1]);
	read_parameters(argv[3], &par);
	p = new Particle[par.N];
	read_raw_files(argv[2], p, &par);*/
	time = 0.0;
	InitDefaultParameters(2.0);
	read_parameters_from_xml(filename, &par);
	p = new Particle[par.N];
	SetParticles();
	read_particles_from_xml_file(filename, p, &par);
	phasesXmlInformation = read_phases_from_xml(filename, &par);
}

Domain::Domain(int CASE, double HDR, double H)
{
	_outputDirectory = "Results/";
	InitDefaultGeometry(CASE);

	par.H = H;

	par.NCX = (int)(0.5 * par.XCV / par.H);
	par.NCY = (int)(0.5 * par.YCV / par.H);

	time = 0.0;

	if (!IsConsistentWithSearchAlgorithm()) {
		std::cerr << "NCX or NCY is not consistent with search algorithm." << std::endl;
		std::cerr << "Due to the GPU efficiency the search algorithm requires NCX" << std::endl;
		std::cerr << "and NCY to be power of 2." << std::endl;
		std::cerr << "This limitation is planned to be removed in newer releases." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!IsConsistentWithGeometry()) {
		std::cerr << "Domain is geometrically not consistent." << std::endl;
		exit(EXIT_FAILURE);
	}
	par.NC = par.NCX * par.NCY;

	InitDefaultParameters(HDR);

	p = new Particle[par.N];
	InitCase(CASE);

	std::ostringstream stream;
	stream << "    <phases>\n";
	for (unsigned int i = 0; i < par.PHASES.size(); i++)
	{
		stream << "      <phase name=\"no name\" id=\"" << par.PHASES[i] << "\" />\n";
	}
	stream << "    </phases>";
	phasesXmlInformation = stream.str();
}

Domain::Domain(int CASE, double HDR, int MIN_NC)
{
	_outputDirectory = "Results/";
	//read_parameters_from_xml("Results/0.100000.xml", &par);
	//std::string test = read_phases_from_xml("Results/0.100000.xml", &par);
	//read_particles_from_xml_file("Results/0.100000.xml", p, &par);

	InitDefaultGeometry(CASE);
	if (par.XCV > par.YCV) {
		par.NCY = MIN_NC;
		par.NCX = (int)(par.XCV / par.YCV) * MIN_NC;
	}
	else {
		par.NCX = MIN_NC;
		par.NCY = (int)(par.YCV / par.XCV) * MIN_NC;
	}
	time = 0.0;

	if (!IsConsistentWithSearchAlgorithm()) {
		std::cerr << "NCX or NCY is not consistent with search algorithm." << std::endl;
		std::cerr << "Due to the GPU efficiency the search algorithm requires NCX" << std::endl;
		std::cerr << "and NCY to be power of 2." << std::endl;
		std::cerr << "This limitation is planned to be removed in newer releases." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!IsConsistentWithGeometry()) {
		std::cerr << "Domain is geometrically not consistent." << std::endl;
		exit(EXIT_FAILURE);
	}
	par.NC = par.NCX * par.NCY;

	InitDefaultParameters(HDR);

	p = new Particle[par.N];
	InitCase(CASE);


	std::ostringstream stream;
	stream << "    <phases>\n";
	for (unsigned int i = 0; i < par.PHASES.size(); i++)
	{
		stream << "      <phase name=\"no name\" id=\"" << par.PHASES[i] << "\"/>\n";
	}
	stream << "    </phases>";
	phasesXmlInformation = stream.str();
}

Domain::Domain(int CASE, double HDR, int NCX, int NCY)
{
	_outputDirectory = "Results/";
	time = 0.0;

	InitDefaultGeometry(CASE);

	par.NCX = NCX;
	par.NCY = NCY;
	if (!IsConsistentWithSearchAlgorithm()) {
		std::cerr << "NCX or NCY is not consistent with search algorithm." << std::endl;
		std::cerr << "Due to the GPU efficiency the search algorithm requires NCX" << std::endl;
		std::cerr << "and NCY to be power of 2." << std::endl;
		std::cerr << "This limitation is planned to be removed in newer releases." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!IsConsistentWithGeometry()) {
		std::cerr << "Domain is geometrically not consistent." << std::endl;
		exit(EXIT_FAILURE);
	}
	par.NC = par.NCX * par.NCY;

	InitDefaultParameters(HDR);

	p = new Particle[par.N];
	InitCase(CASE);

	std::ostringstream stream;
	stream << "    <phases>\n";
	for (unsigned int i = 0; i < par.PHASES.size(); i++)
	{
		stream << "      <phase name=\"no name\" id=\"" << par.PHASES[i] << "\"/>\n";
	}
	stream << "    </phases>";
	phasesXmlInformation = stream.str();
}

void Domain::InitDefaultGeometry(int CASE)
{
	switch (CASE) {
	case 0: par.XCV = 1.0; par.YCV = 1.0; break;
	case 1: par.XCV = 1.0; par.YCV = 1.0; break;
	case 2: par.XCV = 1.0; par.YCV = 1.0; break;
	case 3: par.XCV = 1.0; par.YCV = 1.0; break;
	case 4: par.XCV = 1.0; par.YCV = 2.0; break;
	case 5: par.XCV = 1.0; par.YCV = 2.0; break;
	case 6: par.XCV = 1.0; par.YCV = 1.0; break;
	case 7: par.XCV = 6.0; par.YCV = 12.0; break;
	case 8: par.XCV = 1.0; par.YCV = 1.0; break;
	case 10:par.XCV = 1.0; par.YCV = 1.0; break;
	default:
		std::cerr << "Undefined case no. " << CASE << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Domain::InitDefaultParameters(double HDR)
{
	par.HDR = HDR;
	par.T_MODEL = 2;
	par.V_N = 0.0; par.V_E = 0.0; par.V_S = 0.0; par.V_W = 0.0;
	par.G_X = 0.0; par.G_Y = 0.0;
	par.T_BOUNDARY_PERIODICITY = 0;
	par.T_INTEGRATION_SCHEME = 0;
	par.T_BOUNDARY_CONDITION = 1;
	par.T_INTEGRATION_SCHEME = 0;
	par.T_HYDROSTATIC_PRESSURE = 0;
	par.DT = 0.002;
	par.END_TIME = 10.0;
	par.INTERVAL_TIME = par.DT * 100.0;
	par.T_TIME_STEP = 0;
	par.T_INTERFACE_CORRECTION = 0;
	//par.INTERFACE_CORRECTION = 0.0;
	par.T_SURFACE_TENSION = 0;
	par.T_RENORMALIZE_PRESSURE = 0;
	par.T_XSPH = 0;
	par.XSPH = 0.0;
	par.H = 0.5 * par.XCV / (double)par.NCX;
	par.I_H = 1.0 / par.H;
	par.DH = 0.01*par.H;
	par.KNORM = (double)(M_1_PI * pow2(par.I_H));
	par.GKNORM = (double)(M_1_PI * pow4(par.I_H));
	par.DR = par.H / HDR;
	par.NX = (int)(par.XCV / par.DR);
	par.NY = (int)(par.YCV / par.DR);
	par.N = par.NX * par.NY;

	par.T_TEMP_N = 0;
	par.T_TEMP_E = 0;
	par.T_TEMP_S = 0;
	par.T_TEMP_W = 0;
	par.TEMP_N = 0.0;
	par.TEMP_E = 0.0;
	par.TEMP_S = 0.0;
	par.TEMP_W = 0.0;

	
	//par.SURFACE_TENSION_COEFFICIENT = new InterPhaseParameter(par.NUMBER_OF_PHASES);
}

void Domain::InitCase(int CASE)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0.0, 1.0);

	switch (CASE) {
	case 0: // Static box
		par.PHASES.push_back(0);
		SetParticles();
		break;
	case 1: // Poisseuille flow
		par.T_BOUNDARY_PERIODICITY = 2;
		par.G_X = 1.0;
		par.DT = 0.00005;
		par.INTERVAL_TIME = 0.02;
		par.PHASES.push_back(0);
		SetParticles();
		for (int i = 0; i < par.N; i++) {
			p[i].nu = 1.0;
			p[i].mi = 1.0;
		}
		break;
	case 2: //Couette flow
		par.V_N = 1.0;
		par.T_BOUNDARY_PERIODICITY = 2;
		par.DT = 0.00001;
		par.INTERVAL_TIME = 0.01;
		par.PHASES.push_back(0);
		SetParticles();
		for (int i = 0; i < par.N; i++) {
			p[i].nu = 1.0;
			p[i].mi = 1.0;
		}
		break;
	case 3: // Taylor-Green Vortex
		par.T_BOUNDARY_PERIODICITY = 1;
		par.DT = 0.00001;
		par.INTERVAL_TIME = 0.01;
		par.PHASES.push_back(0);
		SetParticles();
		for (int i = 0; i < par.N; i++) {
			p[i].nu = 0.01;
			p[i].mi = 0.01;
			p[i].vel.x = -cos(2.0*M_PI*p[i].pos.x)*sin(2.0*M_PI*p[i].pos.y);
			p[i].vel.y = sin(2.0*M_PI*p[i].pos.x)*cos(2.0*M_PI*p[i].pos.y);
		}
		break;

	case 4: // Lid-driven cavity Re=100
		par.V_N = 1.0;
		par.END_TIME = 10.0;
		par.DT = 0.00005;
		par.INTERVAL_TIME = 0.1;
		par.PHASES.push_back(0);
		SetParticles();
		for (int i = 0; i < par.N; i++) {
			p[i].d = 1.0;
			p[i].di = 1.0;
			p[i].m = par.XCV * par.YCV * p[i].d / par.N;
			p[i].nu = 0.01;
			p[i].mi = 0.01;
		}
		break;
	case 5: // Rayleigh-Taylor instability
		par.END_TIME = 5.0;
		par.DT = 0.00005;
		par.INTERVAL_TIME = 0.1;
		par.G_Y = -1.0;
		par.PHASES.push_back(0);
		SetParticles();
		for (int i = 0; i<par.N; i++) {
			p[i].nu = 1.0 / 420.0;
			if (p[i].pos.y >(1.0 - 0.15*sin(2.0*M_PI*p[i].pos.x))) {
				p[i].phaseId = 1;
				p[i].d = 1.8;
				p[i].di = 1.8;
				p[i].c = 1.0;
			}
			else {
				p[i].phaseId = 0;
				p[i].d = 1.0;
				p[i].di = 1.0;
				p[i].c = 0.0;

			}
			p[i].mi = p[i].nu * p[i].d;
			p[i].m = par.XCV * par.YCV * p[i].d / par.N;
			p[i].s = 15.0;
			p[i].gamma = 7.0;
			p[i].b = p[i].s*p[i].s*p[i].di / p[i].gamma;
			p[i].m = p[i].m * (double)pow(1.0 + 1.8*(2.0 - p[i].pos.y) / p[i].b, 1.0 / 7.0);
			if (par.T_MODEL != 2) p[i].d = p[i].d * (double)pow(1.0 + 1.8*(2.0 - p[i].pos.y) / p[i].b, 1.0 / 7.0);
		}
		break;
	case 6: // Square-droplet deformation
		par.END_TIME = 2.0;
		par.DT = 0.00005;
		par.INTERVAL_TIME = 0.01;
		par.T_SURFACE_TENSION = 3;
		par.PHASES.push_back(0);
		par.PHASES.push_back(1);
		SetParticles();
		for (int i = 0; i < par.N; i++) {
			if ((p[i].pos.x > 0.2) && (p[i].pos.x < 0.8)
				&& (p[i].pos.y > 0.2) && (p[i].pos.y < 0.8)) {
				
				p[i].phaseId = 1;
				p[i].c = 1.0;
			}
			else {
				p[i].phaseId = 0;
				p[i].c = 0.0;
			}
			p[i].m = par.XCV * par.YCV * p[i].d / par.N;
			p[i].s = 15.0;
			p[i].b = p[i].s*p[i].s*p[i].di / p[i].gamma;
			p[i].mi = 0.2; p[i].nu = 0.2;
		}
		par.SURFACE_TENSION_COEFFICIENT = new InterPhaseParameter(par.PHASES);
		par.SURFACE_TENSION_COEFFICIENT->Set(0,1,1.0);

		break;
	case 7: // Bubble rising in water
		par.END_TIME = 6.0;
		par.DT = 0.000025;
		par.INTERVAL_TIME = 0.05;
		par.T_SURFACE_TENSION = 1;
		par.T_INTERFACE_CORRECTION = 1;
		//par.INTERFACE_CORRECTION = 0.5;
		par.G_Y = -1.0;
		par.T_RENORMALIZE_PRESSURE = 0;
		par.T_XSPH = 0;
		par.XSPH = 0.0;
		SetParticles();
		for (int i = 0; i < par.N; i++) {
			if (pow2(p[i].pos.x - 3.0) + pow2(p[i].pos.y - 2.0) < 1.0) {
				p[i].phaseId = 1;
				p[i].d = 0.001;
				p[i].di = 0.001;
				p[i].m = par.XCV * par.YCV * p[i].d / par.N;
				p[i].c = 1.0;
				p[i].s = 400.0;
				p[i].nu = (double)(100.0*sqrt(8.0) / 5.0);
				p[i].mi = p[i].nu * p[i].d;
				p[i].gamma = 1.4;
				p[i].b = p[i].s*p[i].s*p[i].di / p[i].gamma;
			}
			else {
				p[i].phaseId = 0;
				p[i].d = 1.0;
				p[i].di = 1.0;
				p[i].m = par.XCV * par.YCV * p[i].d / par.N;
				p[i].c = 0.0;
				p[i].s = 28.28;
				p[i].nu = (double)(sqrt(8.0) / 5.0);
				p[i].mi = p[i].nu * p[i].d;
				p[i].gamma = 7.0;
				p[i].b = p[i].s*p[i].s*p[i].di / p[i].gamma;
			}
			p[i].b = p[0].s*p[0].s*p[0].di / p[0].gamma;
			if (pow2(p[i].pos.x - 3.0) + pow2(p[i].pos.y - 2.0) < 1.0) {
				p[i].m = p[i].m * (double)pow(1.0 + 10.0 / p[i].b, 1.0 / 1.4);
			}
			else {
				p[i].m = p[i].m * (double)pow(1.0 + (12.0 - p[i].pos.y) / p[i].b, 1.0 / 7.0);
			}

			par.INTERFACE_CORRECTION_COEFFICIENT = new InterPhaseParameter(par.PHASES);
			par.INTERFACE_CORRECTION_COEFFICIENT->Set(0, 1, 0.5);
		}
		break;
	case 8: // Hamiltonian stabilization
		par.END_TIME = 2.0;
		par.DT = 0.0000005;
		par.INTERVAL_TIME = 0.001;
		par.T_SURFACE_TENSION = 0;
		par.PHASES.push_back(0);
		par.PHASES.push_back(1);
		SetParticles();
		
		for (int i = 0; i < par.N; i++)
		{
			p[i].pos.x = dist(gen);
			p[i].pos.y = dist(gen);
		}

		for (int i = 0; i < par.N; i++) 
		{
			if ((p[i].pos.x > 0.2) && (p[i].pos.x < 0.8)
				&& (p[i].pos.y > 0.2) && (p[i].pos.y < 0.8)) 
			{

				p[i].phaseId = 1;
				p[i].c = 1.0;
				p[i].gamma = 1.4;
				p[i].d = 0.01;
				p[i].di = 0.01;
			}
			else 
			{
				p[i].phaseId = 0;
				p[i].c = 0.0;
				p[i].gamma = 7.0;
				p[i].d = 1.0;
				p[i].di = 1.0;
			}
			p[i].m = par.XCV * par.YCV * p[i].d / par.N;
			p[i].s = 15.0;
			p[i].b = p[0].s*p[0].s*p[0].di / p[0].gamma;
			p[i].mi = 0.2; p[i].nu = 0.2;
		}
		par.SURFACE_TENSION_COEFFICIENT = new InterPhaseParameter(par.PHASES);
		par.SURFACE_TENSION_COEFFICIENT->Set(0, 1, 0.0);

		break;
	case 10: /* Heat conduction in box (square) */
		par.T_HEAT_TRANSFER = 1;
		par.T_TEMP_N = 1; 
		par.T_TEMP_S = 1; 
		par.T_TEMP_W = 0; 
		par.T_TEMP_E = 0;
		par.TEMP_E = 1.0; 
		par.TEMP_W = 0.0;
		par.END_TIME = 1.0;
		par.INTERVAL_TIME = 0.1;
		par.DT = 0.0002;
		SetParticles();
		for (int i = 0; i<par.N; i++) {
			p[i].d = 1000.0;
			p[i].di = 1000.0;
			p[i].m = par.XCV * par.YCV * p[i].di / (double)par.N;
			p[i].nu = 0.2;
			p[i].mi = p[i].nu * p[i].d;
			p[i].cv = 1.0;
			p[i].co = 1.0;
			if ((p[i].pos.x > 0.5)) {
				p[i].t = 1.0;
			}
			else p[i].t = 0.0;
			p[i].e = p[i].t*p[i].cv;
		}
		break;
	default:
		break;
	}

}


void Domain::SetParticles()
{
	double dx = par.XCV / (double)(par.NX);
	double dy = par.YCV / (double)(par.NY);
	double x0 = 0.5 * dx;
	double y0 = 0.5 * dy;
	int l = 0;
	for (int i = 0; i < par.NX; i++) {
		for (int j = 0; j < par.NY; j++) {
			p[l].id = l; p[l].phaseId = 0;
			p[l].pos.x = x0 + dx * (double)(i); p[l].rh_pos.x = 0.0f;
			p[l].pos.y = y0 + dx * (double)(j); p[l].rh_pos.y = 0.0f;
			p[l].vel.x = 0.0f; p[l].rh_vel.x = 0.0f;
			p[l].vel.y = 0.0f; p[l].rh_vel.y = 0.0f;
			p[l].m = 1.0 / (double)(par.N);
			p[l].p = 0.0;
			p[l].d = 1.0; p[l].di = 1.0; p[l].rh_d = 0.0;
			p[l].nu = 0.01; p[l].mi = 0.01;
			p[l].gamma = 7.0; p[l].s = 10.0;
			p[l].b = p[l].s * p[l].s * p[l].di / p[l].gamma;
			p[l].o = pow2(p[l].m / p[l].d);
			p[l].c = 0.0;
			p[l].n.x = 0.0; p[l].n.y = 0.0f; p[l].n.z = 0.0f;
			p[l].na = 0;
			p[l].cu = 0.0;
			p[l].st.x = 0.0; p[l].st.y = 0.0f;
			p[l].cs = 0.0;
			p[l].cw = 0.0;
			l++;
		}
	}
}

Particle* Domain::GetParticles() { return p; }

Parameters* Domain::GetParameters() { return &par; }

void Domain::SetModel(int MODEL) { par.T_MODEL = MODEL; }

double* Domain::GetTime() { return &time; }

const char* Domain::GetOutputDirectory()
{
	return _outputDirectory;
}

void Domain::SetOutputDirectory(const char* outputDirectory)
{
	_outputDirectory = outputDirectory;
}

bool Domain::IsConsistentWithGeometry()
{
	if ((double)(par.NCX) / (double)(par.NCY) == (par.XCV / par.YCV)) {
		return true;
	}
	else {
		return false;
	}
}

bool Domain::IsConsistentWithSearchAlgorithm()
{
	if ((par.NCX % 2 == 0) && (par.NCY % 2 == 0)) {
		return true;
	}
	else {
		return false;
	}
}

void Domain::WriteParametersToFile(const char* filename)
{
	write_parameters(filename, &par);
}

void Domain::WriteParametersToFile()
{
	WriteParametersToFile("results/parameters.dat");
}

void Domain::WriteParticlesToFile(const char* filename)
{
	write_particles(filename, p, &par);
}

void Domain::WriteParticlesToFile()
{
	std::stringstream filenameStream;
	filenameStream << _outputDirectory << std::fixed << time << ".dat";
	std::string filenameString = filenameStream.str();
	WriteParticlesToFile(filenameString.c_str());
}

void Domain::WritePhaseToFile(const char *filename, int phaseId)
{
	write_phase(filename, p, &par, phaseId);
}

void Domain::WritePhaseToFile(int phaseId)
{
	std::stringstream filenameStream;
	filenameStream << _outputDirectory << std::fixed << time << "_ph" << phaseId << ".dat";
	std::string filenameString = filenameStream.str();
	WritePhaseToFile(filenameString.c_str(), phaseId);
}

void Domain::WriteToXmlFile(const char* filename)
{
	write_to_xml(filename, p, &par, phasesXmlInformation);
}

void Domain::WriteToXmlFile()
{
	std::stringstream filenameStream;
	filenameStream << _outputDirectory << std::fixed << time << ".xml";
	std::string filenameString = filenameStream.str();
	WriteToXmlFile(filenameString.c_str());
}

double Domain::GetAndWriteKinetic(const char *filename)
{
	FILE *file;

	double kinetic = 0.0;
	for (int i = 0; i < par.N; i++) {
		kinetic += 0.5 * p[i].m * (pow2(p[i].vel.x) + pow2(p[i].vel.y));
	}

	if ((file = fopen(filename, "a+"))) {
		fprintf(file, "%.10f %.10f\n", time, kinetic);
		fclose(file);
	}
	else {
		std::cerr << "Output kinetic error: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	return kinetic;
}

double Domain::GetAndWriteKinetic()
{
	std::stringstream filenameStream;
	filenameStream << _outputDirectory << "kinetic.dat";
	std::string filenameString = filenameStream.str();
	return GetAndWriteKinetic(filenameString.c_str());
}

int Domain::GetSizeOfParticles()
{
	return sizeof(Particle)*par.N;
}

int Domain::GetSizeOfParameters()
{
	return sizeof(Parameters);
}

Domain::~Domain()
{
	delete[] p;
}
