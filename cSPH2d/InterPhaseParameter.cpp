#include "InterPhaseParameter.h"

#include <iostream>

InterPhaseParameter::InterPhaseParameter(std::vector<int> phases)
{
	int numberOfPhases = phases.size();

	_values.resize(numberOfPhases);
	for (unsigned int i = 0; i < numberOfPhases; i++)
	{
		_values[i].resize(numberOfPhases);
	}

	_idToPhaseId.resize(numberOfPhases);
	for (unsigned int i = 0; i < numberOfPhases; i++)
	{
		_idToPhaseId[i] = phases[i];
	}

	for (unsigned int i = 0; i < numberOfPhases; i++)
	{
		for (unsigned int j = 0; j < numberOfPhases; j++)
		{
			_values[i][j] = 0.0;
		}
	}
}

double InterPhaseParameter::Get(int phaseId1, int phaseId2)
{
	int id1, id2;
	for (unsigned int i = 0; i < _idToPhaseId.size(); i++)
	{
		if (_idToPhaseId[i] == phaseId1)
		{
			id1 = i;
		}
		if (_idToPhaseId[i] == phaseId2)
		{
			id2 = i;
		}
	}
	return _values[id1][id2];
}

void InterPhaseParameter::Set(int phaseId1, int phaseId2, double value)
{
	int id1, id2;
	for (unsigned int i = 0; i < _idToPhaseId.size(); i++)
	{
		if (_idToPhaseId[i] == phaseId1)
		{
			id1 = i;
		}
		if (_idToPhaseId[i] == phaseId2)
		{
			id2 = i;
		}
	}

	_values[id1][id2] = value;
	_values[id2][id1] = value;
}

int InterPhaseParameter::Size()
{
	return _values.size();
}


int InterPhaseParameter::IdToPhaseId(int id)
{
	return _idToPhaseId[id];
}

int InterPhaseParameter::PhaseIdtoId(int phaseId)
{
	int id;
	for (unsigned int i = 0; i < _idToPhaseId.size(); i++)
	{
		if (_idToPhaseId[i] == phaseId)
		{
			id = i;
		}
	}
	return id;
}

double InterPhaseParameter::GetHighestValue()
{
	double highestValue = 0.0;
	for (unsigned int i = 0; i < _values.size(); i++)
	{
		for (unsigned int j = 0; j < _values.size(); j++)
		{
			if (_values[i][j] >= highestValue)
			{
				highestValue = _values[i][j];
			}
		}
	}

	return highestValue;
}