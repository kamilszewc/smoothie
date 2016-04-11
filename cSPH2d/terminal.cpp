/*
* terminalOptions.cpp
*
*  Created on: 03-04-2014
*      Author: Kamil Szewc (kamil.szewc@gmail.com)
*/

#include <iostream>
#include <cstring>
#include <string>

std::string programFullName = "CppSph2D - C++ Smoothed Particle Hydrodynamics solver";

std::string programName = "CppSph2D";

std::string license =
"Copyright (c) 2012-2014 Kamil Szewc (Institute of Fluid-Flow Machinery, PAS)\n"
"All right reserved.\n"
"\n"
"Redistribution and use in source and binary forms, with or without\n"
"modification, are permitted provided that the following conditions are met:\n"
"  * Redistribution of source code must retain the above copyright\n"
"    notice, this list of conditions and the following disclaimer\n"
"  * Redistribution in binary form must reproduce the above copyright\n"
"    notice, this list of conditions and the following disclaimer in the\n"
"    documentation and/or other materials provided with the distribution.\n"
"  * Neither the name of the Institute of Fluid-Flow Machinery, Polish\n"
"    Academy of Sciences nor the names of its contributors may be used to endorse\n"
"    or promote products derived from this software without specific prior\n"
"    written permission\n"
"\n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND\n"
"ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n"
"WARRANTIES OF METCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
"DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY\n"
"DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n"
"(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"
"LOSS OF USE, DATA, OR PROFITS; OR BUISNESS INTERRUPTION) HOWEVER CAUSED AND\n"
"ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
"(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"
"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n";

std::string shortLicense = "Copyright (c) 2012-2014 Kamil Szewc (New BSD License)";

std::string version = "1.0";

std::string authors[] = { "Kamil Szewc" };
std::string affiliations[] = { "Institute of Fluid-Flow Machinery, PAS" };
int numberOfAuthors = 1;

void printProgramFullName()
{
	std::cout << programFullName << std::endl;
}

void printLicense()
{
	std::cout << license << std::endl;
}

void printShortLicense()
{
	std::cout << shortLicense << std::endl;
}

void printVersion()
{
	std::cout << "Version " << version << std::endl;
}

void printAuthors()
{
	for (int i = 0; i < numberOfAuthors; i++) {
		std::cout << authors[i] << "\t" << affiliations[i] << std::endl;
	}
}

void printHelp()
{
	printProgramFullName();
	printShortLicense();
	std::cout << "Additional options:" << std::endl;
	std::cout << "  -version \t\t" << "Show version" << std::endl;
	std::cout << "  -license \t\t" << "Show license" << std::endl;
	std::cout << "  -short-license \t" << "Show shorten version of license" << std::endl;
}

void terminal(int argc, char *argv[])
{
	switch (argc) {
	case 1:
		break;
	case 2:
		if (!strcmp(argv[1], "-help") || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) printHelp();
		else if (!strcmp(argv[1], "-version")) printVersion();
		else if (!strcmp(argv[1], "-license")) printLicense();
		else if (!strcmp(argv[1], "-short-license")) printShortLicense();
		else if (!strcmp(argv[1], "-authors")) printAuthors();
		else {
			std::cerr << "Incorrect option. ";
			std::cerr << "See 'cusph --help'." << std::endl;
		}
		exit(EXIT_SUCCESS);
	case 4:
		break;
	default:
		std::cerr << "Incorrect number of arguments. ";
		std::cerr << "See 'cusph --help'." << std::endl;
		exit(EXIT_FAILURE);
	}
}
