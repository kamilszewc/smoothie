/* All right reserved by Kamil Szewc, Canberra 2014 */

#if !defined(__OUTPUT_H__)
#define __OUTPUT_H__

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "sph.h"
#include "hlp.h"

/* Write Particle data to file */
void write_particles(const char *filename, Particle *p, Parameters *par)
{	/* Function for output data into raw files */
	FILE *file;

	if ((file = fopen(filename, "w"))) {
		for (int i = 0; i < par->N; i++) {
			fprintf(file, "%d %d ", p[i].id, p[i].phaseId);
			fprintf(file, "%.15f %.15f %.15f %.15f ", p[i].pos.x, p[i].pos.y, p[i].vel.x, p[i].vel.y);
			fprintf(file, "%.15f %.15f %.15f %.15f %.15f %.15f %.15f ", p[i].m, p[i].p, p[i].d, p[i].di, p[i].o, p[i].nu, p[i].mi);
			fprintf(file, "%.15f %.15f %.15f %.15f ", p[i].gamma, p[i].s, p[i].b, p[i].c);
			fprintf(file, "%.15f %.15f %d %.15f ", p[i].n.x, p[i].n.y, p[i].na, p[i].cu);
			fprintf(file, "%.15f %.15f %.15f %.15f\n", p[i].st.x, p[i].st.y, p[i].t, rh[i].e);
		}
		fclose(file);
	}
	else {
		std::cerr << "Output error: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
}

/* Write information about given phase (phaseId) to file */
void write_phase(const char *filename, Particle *p, Parameters *par, int phaseId)
{	/* Function for output data into raw files */
	FILE *file;

	if ((file = fopen(filename, "w"))) {
		for (int i = 0; i < par->N; i++) {
			if (p[i].phaseId == phaseId) {
				fprintf(file, "%d %d ", p[i].id, p[i].phaseId);
				fprintf(file, "%.15f %.15f %.15f %.15f ", p[i].pos.x, p[i].pos.y, p[i].vel.x, p[i].vel.y);
				fprintf(file, "%.15f %.15f %.15f %.15f %.15f %.15f %.15f ", p[i].m, p[i].p, p[i].d, p[i].di, p[i].o, p[i].nu, p[i].mi);
				fprintf(file, "%.15f %.15f %.15f %.15f ", p[i].gamma, p[i].s, p[i].b, p[i].c);
				fprintf(file, "%.15f %.15f %d %.15f ", p[i].n.x, p[i].n.y, p[i].na, p[i].cu);
				fprintf(file, "%.15f %.15f\n", p[i].st.x, p[i].st.y);
			}
		}
		fclose(file);
	}
	else {
		std::cerr << "Output phase error: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
}

void write_parameters(const char* filename, Parameters *par)
{
	FILE *file;

	if ((file = fopen(filename, "w"))) {
		fprintf(file, "T_MODEL\t%d\n", par->T_MODEL);
		//fprintf(file, "NX\t%d\n", par->NX);
		//fprintf(file, "NY\t%d\n", par->NY);
		fprintf(file, "N\t%d\n", par->N);
		fprintf(file, "NCX\t%d\n", par->NCX);
		fprintf(file, "NCY\t%d\n", par->NCY);
		fprintf(file, "NC\t%d\n", par->NC);
		fprintf(file, "XCV\t%.15f\n", par->XCV);
		fprintf(file, "YCV\t%.15f\n", par->YCV);
		fprintf(file, "V_N\t%.15f\n", par->V_N);
		fprintf(file, "V_E\t%.15f\n", par->V_E);
		fprintf(file, "V_S\t%.15f\n", par->V_S);
		fprintf(file, "V_W\t%.15f\n", par->V_W);
		fprintf(file, "G_X\t%.15f\n", par->G_X);
		fprintf(file, "G_Y\t%.15f\n", par->G_Y);
		fprintf(file, "T_BOUNDARY_PERIODICITY\t%d\n", par->T_BOUNDARY_PERIODICITY);
		fprintf(file, "H\t%.15f\n", par->H);
		fprintf(file, "I_H\t%.15f\n", par->I_H);
		fprintf(file, "DH\t%.15f\n", par->DH);
		fprintf(file, "DR\t%.15f\n", par->DR);
		fprintf(file, "KNORM\t%.15f\n", par->KNORM);
		fprintf(file, "GKNORM\t%.15f\n", par->GKNORM);
		fprintf(file, "DT\t%.15f\n", par->DT);
		fprintf(file, "END_TIME\t%.15f\n", par->END_TIME);
		fprintf(file, "T_INTERFACE_CORRECTION\t%d\n", par->T_INTERFACE_CORRECTION);
		//fprintf(file, "INTERFACE_CORRECTION\t%.15f\n", par->INTERFACE_CORRECTION);
		fprintf(file, "T_SURFACE_TENSION\t%d\n", par->T_SURFACE_TENSION);
		fprintf(file, "T_XSPH\t%d\n", par->T_XSPH);
		fprintf(file, "XSPH\t%.15f\n", par->XSPH);

	}
	else {
		std::cerr << "Output parameters error: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	fclose(file);
}



template<class T> std::string parse_parameter(std::string name, std::string type, T value)
{
	std::string str = "      <parameter name=\"" + name + "\" type=\"" + type + "\">";
	std::ostringstream stream;
	//str = str + std::to_string(value); //C++11
	stream << value << "</parameter>";
	return str + stream.str();
}

std::string parse_field(std::string name, std::string type)
{
	return "    <field name=\"" + name + "\" type=\"" + type + "\">";
}

void write_to_xml(const char* filename, Particle *p, Parameters *par, std::string phasesXmlInformation)
{
	std::ofstream file;
	file.open(filename);

	if (file.is_open())
	{
		file << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << std::endl;
		file << "<sph>" << std::endl;
		file << "  <domain>" << std::endl;
		file << "     <parameters>" << std::endl;
		file << parse_parameter<double>("HDR", "double", par->HDR) << std::endl;
		file << parse_parameter<int>("NCX", "int", par->NCX) << std::endl;
		file << parse_parameter<int>("NCY", "int", par->NCY) << std::endl;
		file << parse_parameter<int>("NXC", "int", par->NCX) << std::endl;
		file << parse_parameter<int>("NYC", "int", par->NCY) << std::endl;
		file << parse_parameter<double>("XCV", "double", par->XCV) << std::endl;
		file << parse_parameter<double>("YCV", "double", par->YCV) << std::endl;
		file << parse_parameter<double>("DT", "double", par->DT) << std::endl;
		file << parse_parameter<double>("INTERVAL_TIME", "double", par->INTERVAL_TIME) << std::endl;
		file << parse_parameter<int>("N", "int", par->N) << std::endl;
		file << parse_parameter<int>("T_BOUNDARY_CONDITION", "int", par->T_BOUNDARY_CONDITION) << std::endl;
		file << parse_parameter<int>("T_BOUNDARY_PERIODICITY", "int", par->T_BOUNDARY_PERIODICITY) << std::endl;
		file << parse_parameter<int>("T_MODEL", "int", par->T_MODEL) << std::endl;
		//file << parse_parameter<int>("NX", "int", par->NX) << std::endl;
		//file << parse_parameter<int>("NY", "int", par->NY) << std::endl;
		
		file << parse_parameter<int>("T_TIME_STEP", "int", par->T_TIME_STEP) << std::endl;
		file << parse_parameter<double>("END_TIME", "double", par->END_TIME) << std::endl;
		file << parse_parameter<double>("G_X", "double", par->G_X) << std::endl;
		file << parse_parameter<double>("G_Y", "double", par->G_Y) << std::endl;

		file << parse_parameter<double>("V_N", "double", par->V_N) << std::endl;
		file << parse_parameter<double>("V_E", "double", par->V_E) << std::endl;
		file << parse_parameter<double>("V_S", "double", par->V_S) << std::endl;
		file << parse_parameter<double>("V_W", "double", par->V_W) << std::endl;
		
		//file << parse_parameter<double>("H", "double", par->H) << std::endl;
		//file << parse_parameter<double>("I_H", "double", par->I_H) << std::endl;
		//file << parse_parameter<double>("DH", "double", par->DH) << std::endl;
		//file << parse_parameter<double>("DR", "double", par->DR) << std::endl;
		//file << parse_parameter<double>("KNORM", "double", par->KNORM) << std::endl;
		//file << parse_parameter<double>("GKNORM", "double", par->GKNORM) << std::endl;
		
		file << parse_parameter<int>("T_HYDROSTATIC_PRESSURE", "int", par->T_HYDROSTATIC_PRESSURE) << std::endl;
		
		file << parse_parameter<int>("T_INTERFACE_CORRECTION", "int", par->T_INTERFACE_CORRECTION) << std::endl;
		//file << parse_parameter<double>("INTERFACE_CORRECTION", "double", par->INTERFACE_CORRECTION) << std::endl;
		file << parse_parameter<int>("T_SURFACE_TENSION", "int", par->T_SURFACE_TENSION) << std::endl;
		file << parse_parameter<int>("T_XSPH", "int", par->T_XSPH) << std::endl;
		file << parse_parameter<double>("XSPH", "double", par->XSPH) << std::endl;
		file << parse_parameter<double>("T_HEAT_TRANSFER", "int", par->T_HEAT_TRANSFER) << std::endl;
		file << parse_parameter<double>("T_TEMP_N", "int", par->T_TEMP_N) << std::endl;
		file << parse_parameter<double>("T_TEMP_E", "int", par->T_TEMP_E) << std::endl;
		file << parse_parameter<double>("T_TEMP_S", "int", par->T_TEMP_S) << std::endl;
		file << parse_parameter<double>("T_TEMP_W", "int", par->T_TEMP_W) << std::endl;
		file << parse_parameter<double>("TEMP_N", "double", par->TEMP_N) << std::endl;
		file << parse_parameter<double>("TEMP_E", "double", par->TEMP_E) << std::endl;
		file << parse_parameter<double>("TEMP_S", "double", par->TEMP_S) << std::endl;
		file << parse_parameter<double>("TEMP_W", "double", par->TEMP_W) << std::endl;
		file << "    </parameters>" << std::endl;
		file << phasesXmlInformation << std::endl;
		
		file << "    <interphase-parameters>" << std::endl;
		if (par->T_SURFACE_TENSION > 0)
		{
			file << "      <surface-tension-coefficient>" << std::endl;
			for (int i = 0; i < par->SURFACE_TENSION_COEFFICIENT->Size(); i++)
			{
				for (int j = i + 1; j < par->SURFACE_TENSION_COEFFICIENT->Size(); j++)
				{
					int id1 = par->SURFACE_TENSION_COEFFICIENT->IdToPhaseId(i);
					int id2 = par->SURFACE_TENSION_COEFFICIENT->IdToPhaseId(j);
					file << "        <value phaseId1=\"" << id1 << "\" phaseId2=\"" << id2 << "\" >";
					file << par->SURFACE_TENSION_COEFFICIENT->Get(id1, id2) << "</value>" << std::endl;
				}
			}
			file << "      </surface-tension-coefficient>" << std::endl;
		}
		else
		{
			file << "      <surface-tension-coefficient />" << std::endl;
		}
		if (par->T_INTERFACE_CORRECTION > 0)
		{
			file << "      <interface-correction-coefficient>" << std::endl;
			for (int i = 0; i < par->INTERFACE_CORRECTION_COEFFICIENT->Size(); i++)
			{
				for (int j = i + 1; j < par->INTERFACE_CORRECTION_COEFFICIENT->Size(); j++)
				{
					int id1 = par->INTERFACE_CORRECTION_COEFFICIENT->IdToPhaseId(i);
					int id2 = par->INTERFACE_CORRECTION_COEFFICIENT->IdToPhaseId(j);
					file << "        <value phaseId1=\"" << id1 << "\" phaseId2=\"" << id2 << "\" >";
					file << par->INTERFACE_CORRECTION_COEFFICIENT->Get(id1, id2) << "</value>" << std::endl;
				}
			}
			file << "      </interface-correction-coefficient>" << std::endl;
		}
		else
		{
			file << "      <interface-correction-coefficient />" << std::endl;
		}
		file << "    </interphase-parameters>" << std::endl;
		
		file << "  </domain>" << std::endl;

		file << "  <particles>" << std::endl;

		file << parse_field("id", "int");
		for (int i = 0; i<par->N; i++) { file << p[i].id << " "; }
		file << "</field>" << std::endl;

		file << parse_field("x-position", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].pos.x << " "; }
		file << "</field>" << std::endl;

		file << parse_field("y-position", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].pos.y << " "; }
		file << "</field>" << std::endl;

		file << parse_field("phase-id", "int");
		for (int i = 0; i<par->N; i++) { file << p[i].phaseId << " "; }
		file << "</field>" << std::endl;

		file << parse_field("x-velocity", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].vel.x << " "; }
		file << "</field>" << std::endl;

		file << parse_field("y-velocity", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].vel.y << " "; }
		file << "</field>" << std::endl;

		file << parse_field("mass", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].m << " "; }
		file << "</field>" << std::endl;

		file << parse_field("pressure", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].p << " "; }
		file << "</field>" << std::endl;

		file << parse_field("density", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].d << " "; }
		file << "</field>" << std::endl;

		file << parse_field("initial density", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].di << " "; }
		file << "</field>" << std::endl;

		file << parse_field("dynamic viscosity", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].mi << " "; }
		file << "</field>" << std::endl;

		file << parse_field("gamma", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].gamma << " "; }
		file << "</field>" << std::endl;

		file << parse_field("sound speed", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].s << " "; }
		file << "</field>" << std::endl;

		file << parse_field("equation of state coefficient", "double");
		for (int i = 0; i<par->N; i++) { file << p[i].b << " "; }
		file << "</field>" << std::endl;

		if (par->T_SURFACE_TENSION > 0)
		{
			file << parse_field("color function", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].c << " "; }
			file << "</field>" << std::endl;

			file << parse_field("x-normal vector", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].n.x << " "; }
			file << "</field>" << std::endl;

			file << parse_field("y-normal vector", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].n.y << " "; }
			file << "</field>" << std::endl;

			file << parse_field("curvature influence indicator", "int");
			for (int i = 0; i<par->N; i++) { file << p[i].na << " "; }
			file << "</field>" << std::endl;

			file << parse_field("curvature", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].cu << " "; }
			file << "</field>" << std::endl;

			file << parse_field("x-surface tension", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].st.x << " "; }
			file << "</field>" << std::endl;

			file << parse_field("y-surface tension", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].st.y << " "; }
			file << "</field>" << std::endl;
		}

		if (par->T_HEAT_TRANSFER > 0)
		{
			file << parse_field("temperature", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].t << " "; }
			file << "</field>" << std::endl;

			file << parse_field("heat conductivity", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].co << " "; }
			file << "</field>" << std::endl;

			file << parse_field("heat capacity", "double");
			for (int i = 0; i<par->N; i++) { file << p[i].cv << " "; }
			file << "</field>" << std::endl;
		}

		file << "  </particles>" << std::endl;
		file << "</sph>" << std::endl;
		file.close();
	}
}


#endif
