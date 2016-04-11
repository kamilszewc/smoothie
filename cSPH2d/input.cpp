#ifndef SPH_INPUT
#define SPH_INPUT

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sph.h"
#include "hlp.h"

#include <iostream>
#include <string>
#include <vector>
#include "Tinyxml\tinyxml2.h"


void read_parameters_from_xml(const char *filename, Parameters *par)
{
	using namespace tinyxml2;

	XMLDocument xmlDocument;
	xmlDocument.LoadFile(filename);

	std::cout << filename << std::endl;

	XMLElement* xmlElementRoot = xmlDocument.FirstChildElement("sph");

	XMLElement* xmlElementParameters = xmlElementRoot->FirstChildElement("domain")->FirstChildElement("parameters");

	for (XMLElement* xmlElement = xmlElementParameters->FirstChildElement("parameter"); xmlElement != NULL; xmlElement = xmlElement->NextSiblingElement("parameter"))
	{
		const char* name = xmlElement->Attribute("name");
		if (!strcmp(name, "HDR"))
		{
			par->HDR = atoi(xmlElement->GetText());
		}
		if (!strcmp(name, "N"))
		{
			par->N = atoi(xmlElement->GetText());
		}
		if (!strcmp(name, "NCX"))
		{
			par->NCX = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "NCY"))
		{
			par->NCY = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "XCV"))
		{
			par->XCV = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "YCV"))
		{
			par->YCV = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "DT"))
		{
			par->DT = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "END_TIME"))
		{
			par->END_TIME = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "G_X"))
		{
			par->G_X = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "G_Y"))
		{
			par->G_Y = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "INTERVAL_TIME"))
		{
			par->INTERVAL_TIME = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_BOUNDARY_CONDITION"))
		{
			par->T_BOUNDARY_CONDITION = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_BOUNDARY_PERIODICITY"))
		{
			par->T_BOUNDARY_PERIODICITY = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_HEAT_TRANSFER"))
		{
			par->T_HEAT_TRANSFER = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_HYDROSTATIC_PRESSURE"))
		{
			par->T_HYDROSTATIC_PRESSURE = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_INTERFACE_CORRECTION"))
		{
			par->T_INTERFACE_CORRECTION = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_MODEL"))
		{
			par->T_MODEL = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_SURFACE_TENSION"))
		{
			par->T_SURFACE_TENSION = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_TEMP_E"))
		{
			par->T_TEMP_E = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_TEMP_S"))
		{
			par->T_TEMP_S = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_TEMP_N"))
		{
			par->T_TEMP_N = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_TEMP_W"))
		{
			par->T_TEMP_W = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "T_TIME_STEP"))
		{
			par->T_TIME_STEP = atoi(xmlElement->GetText());
		}
		else if (!strcmp(name, "TEMP_E"))
		{
			par->TEMP_E = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "TEMP_W"))
		{
			par->TEMP_W = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "TEMP_S"))
		{
			par->TEMP_S = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "TEMP_N"))
		{
			par->TEMP_N = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "V_E"))
		{
			par->V_E = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "V_N"))
		{
			par->V_N = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "V_S"))
		{
			par->V_S = atof(xmlElement->GetText());
		}
		else if (!strcmp(name, "V_W"))
		{
			par->V_W = atof(xmlElement->GetText());
		}
	}

	par->H = 0.5 * par->XCV / (double)par->NCX;
	par->NC = par->NCX * par->NCY;
	par->I_H = 1.0 / par->H;
	par->DH = 0.01 * par->H;
	par->KNORM = (double)(M_1_PI * pow2(par->I_H));
	par->GKNORM = (double)(M_1_PI * pow4(par->I_H));
	par->DR = par->H / par->HDR;

	XMLElement* xmlElementPhases = xmlElementRoot->FirstChildElement("domain")->FirstChildElement("phases");
	for (XMLElement* xmlElement = xmlElementPhases->FirstChildElement("phase"); xmlElement != NULL; xmlElement = xmlElement->NextSiblingElement("phase"))
	{
		const char* id_string = xmlElement->Attribute("id");
		par->PHASES.push_back(atoi(id_string));
	}

	if (par->T_SURFACE_TENSION > 0)
	{
		par->SURFACE_TENSION_COEFFICIENT = new InterPhaseParameter(par->PHASES);

		XMLElement* xmlElementSurfaceTensionCoefficients = xmlElementRoot->FirstChildElement("domain")->FirstChildElement("interphase-parameters")->FirstChildElement("surface-tension-coefficient");
		for (XMLElement* xmlElement = xmlElementSurfaceTensionCoefficients->FirstChildElement("value"); xmlElement != NULL; xmlElement = xmlElement->NextSiblingElement("value"))
		{
			const char* phaseId1_string = xmlElement->Attribute("phaseId1");
			const char* phaseId2_string = xmlElement->Attribute("phaseId2");
			const char* value_string = xmlElement->GetText();

			int phaseId1 = atoi(phaseId1_string);
			int phaseId2 = atoi(phaseId2_string);
			double value = atof(value_string);

			par->SURFACE_TENSION_COEFFICIENT->Set(phaseId1, phaseId2, value);
		}
	}

	if (par->T_INTERFACE_CORRECTION > 0)
	{
		par->INTERFACE_CORRECTION_COEFFICIENT = new InterPhaseParameter(par->PHASES);

		XMLElement* xmlElementInterfaceCorrectionCoefficients = xmlElementRoot->FirstChildElement("domain")->FirstChildElement("interphase-parameters")->FirstChildElement("interface-correction-coefficient");
		for (XMLElement* xmlElement = xmlElementInterfaceCorrectionCoefficients->FirstChildElement("value"); xmlElement != NULL; xmlElement = xmlElement->NextSiblingElement("value"))
		{
			const char* phaseId1_string = xmlElement->Attribute("phaseId1");
			const char* phaseId2_string = xmlElement->Attribute("phaseId2");
			const char* value_string = xmlElement->GetText();

			int phaseId1 = atoi(phaseId1_string);
			int phaseId2 = atoi(phaseId2_string);
			double value = atof(value_string);

			par->INTERFACE_CORRECTION_COEFFICIENT->Set(phaseId1, phaseId2, value);
		}
	}
	
}


std::string read_phases_from_xml(const char *filename, Parameters* par)
{
	using namespace tinyxml2;

	XMLDocument xmlDocument;
	xmlDocument.LoadFile(filename);

	XMLElement* xmlElementRoot = xmlDocument.FirstChildElement("sph");

	XMLElement* xmlElementPhases = xmlElementRoot->FirstChildElement("domain")->FirstChildElement("phases");


	XMLPrinter printer;
	xmlElementPhases->Accept(&printer);

	return printer.CStr();
}

std::vector<char*> split(char *str, const char* sep)
{
	std::vector<char*> vec;

	char* tokens;
	tokens = strtok(str, sep);
	while (tokens != NULL)
	{
		vec.push_back(tokens);
		tokens = strtok(NULL, sep);
	}

	return vec;
}

void read_particles_from_xml_file(const char* filename, Particle* p, Parameters* par)
{
	using namespace tinyxml2;

	XMLDocument xmlDocument;
	xmlDocument.LoadFile(filename);

	XMLElement* xmlElementRoot = xmlDocument.FirstChildElement("sph");

	XMLElement* xmlElementParticles = xmlElementRoot->FirstChildElement("particles");

	for (XMLElement* xmlElement = xmlElementParticles->FirstChildElement("field"); xmlElement != NULL; xmlElement = xmlElement->NextSiblingElement("field"))
	{
		const char* name = xmlElement->Attribute("name");
		char* stringValues = (char*)xmlElement->GetText();
		std::vector<char*> splitedStringValues = split(stringValues, " ");

		if (!strcmp(name, "id"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].id = atoi(splitedStringValues[i]);
		}
		if (!strcmp(name, "x-position"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].pos.x = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "y-position"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].pos.y = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "phase-id"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].phaseId = atoi(splitedStringValues[i]);
		}
		if (!strcmp(name, "x-velocity"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].vel.x = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "y-velocity"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].vel.y = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "density"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].d = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "initial density"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].di = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "dynamic viscosity"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].mi = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "sound speed"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].s = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "gamma"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].gamma = atof(splitedStringValues[i]);
		}
		if (!strcmp(name, "equation of state coefficient"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].b = atof(splitedStringValues[i]);
		}

		if (!strcmp(name, "color function"))
		{
			for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].c = atof(splitedStringValues[i]);
		}
		/*if (par->T_SURFACE_TENSION > 0)
		{
			if (!strcmp(name, "color function"))
			{
				for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].c = atof(splitedStringValues[i]);
			}
		}*/

		if (par->T_HEAT_TRANSFER > 0)
		{
			if (!strcmp(name, "temperature"))
			{
				for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].t = atof(splitedStringValues[i]);
			}
			if (!strcmp(name, "heat conductivity"))
			{
				for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].co = atof(splitedStringValues[i]);
			}
			if (!strcmp(name, "heat capacity"))
			{
				for (unsigned int i = 0; i < splitedStringValues.size(); i++) p[i].cv = atof(splitedStringValues[i]);
			}
		}
	}
	for (int i = 0; i < par->N; i++)
	{
		p[i].nu = p[i].mi / p[i].di;
		p[i].m = par->XCV * par->YCV * p[i].di / par->N;
		p[i].b = p[i].di * p[i].s * p[i].s / p[i].gamma;
		p[i].b = p[0].b;
		if (par->T_HEAT_TRANSFER > 0)
		{
			p[i].e = p[i].t * p[i].cv;
		}
	}
	
}





void read_raw_files(const char *filename, Particle *p, Parameters *par)
{};

void read_parameters(const char *filename, Parameters *par)
{};

void read_raw_files(const char *filename)
{
}

void read_parameters(const char *filename)
{
}


#endif
