#if !defined(__SURFACE_TENSION_COEFFICIENT__)
#define __SURFACE_TENSION_COEFFICIENT__

#include <vector>

class InterPhaseParameter
{
private:
	std::vector<std::vector<double>> _values;
	std::vector<int> _idToPhaseId;
public:
	InterPhaseParameter(std::vector<int>);
	double Get(int, int);
	void Set(int, int, double);
	int Size();
	int IdToPhaseId(int);
	int PhaseIdtoId(int);
	double GetHighestValue();
};

#endif