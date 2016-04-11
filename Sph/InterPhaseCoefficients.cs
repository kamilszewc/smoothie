using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sph
{
    public class InterPhaseCoefficiant
    {
        private Phase _phase1;
        private Phase _phase2;
        private double _value;

        public InterPhaseCoefficiant(Phase phase1, Phase phase2, double value)
        {
            _phase1 = phase1;
            _phase2 = phase2;
            _value = value;
        }

        public double Value
        {
            set { _value = value; }
            get { return _value; }
        }

        public Phase Phase1
        {
            get { return _phase1; }
        }

        public Phase Phase2
        {
            get { return _phase2; }
        }
    }


    public class InterPhaseCoefficients
    {
        List<Phase> _phases;

        List<InterPhaseCoefficiant> _interPhaseCoefficiants;

        public InterPhaseCoefficients(List<Phase> phases)
        {
            _phases = phases;
            _interPhaseCoefficiants = new List<InterPhaseCoefficiant>();
            Update();
        }

        public void Add(string phaseName1, string phaseName2, double value)
        {
            int id1 = _phases.FindIndex(phase => phase.Name == phaseName1);
            int id2 = _phases.FindIndex(phase => phase.Name == phaseName2);
            Phase phase1 = _phases[id1];
            Phase phase2 = _phases[id2];
            this.Add(phase1, phase1, value);
        }

        public void Add(Phase phase1, Phase phase2, double value)
        {
            _interPhaseCoefficiants.Add(new InterPhaseCoefficiant(phase1, phase2, value));
        }

        public void Set(string phaseName1, string phaseName2, double value)
        {
            int id = _interPhaseCoefficiants.FindIndex(val => (val.Phase1.Name == phaseName1) && (val.Phase2.Name == phaseName2));
            if (id < 0)
            {
                id = _interPhaseCoefficiants.FindIndex(val => (val.Phase1.Name == phaseName2) && (val.Phase2.Name == phaseName1));
            }
            _interPhaseCoefficiants[id].Value = value;
        }

        public void Remove(string phaseName1, string phaseName2)
        {
            int id = _interPhaseCoefficiants.FindIndex(val => (val.Phase1.Name == phaseName1) && (val.Phase2.Name == phaseName2));
            _interPhaseCoefficiants.RemoveAt(id);
        }

        public void Remove(string phaseName)
        {
            while(true)
            {
                int id = _interPhaseCoefficiants.FindIndex(val => (val.Phase1.Name == phaseName) || (val.Phase2.Name == phaseName));
                if (id >= 0)
                {
                    _interPhaseCoefficiants.RemoveAt(id);
                }
                else
                {
                    break;
                }
            }
        }

        public InterPhaseCoefficiant Get(string phaseName1, string phaseName2)
        {
            int id = _interPhaseCoefficiants.FindIndex(val => (val.Phase1.Name == phaseName1) && (val.Phase2.Name == phaseName2));
            if (id < 0)
            {
                id = _interPhaseCoefficiants.FindIndex(val => (val.Phase1.Name == phaseName2) && (val.Phase2.Name == phaseName1));
            }
            return _interPhaseCoefficiants[id];
        }

        public List<InterPhaseCoefficiant> Get()
        {
            return _interPhaseCoefficiants;
        }

        public void Update()
        {

            for (int i=0; i < _phases.Count(); i++)
            {
                for (int j=i+1; j < _phases.Count(); j++)
                {
                    InterPhaseCoefficiant interPhaseCoefficiant;
                    try
                    {
                        interPhaseCoefficiant = this.Get(_phases[i].Name, _phases[j].Name);
                    }
                    catch
                    {
                        interPhaseCoefficiant = new InterPhaseCoefficiant(_phases[i], _phases[j], 0.0);
                        _interPhaseCoefficiants.Add(interPhaseCoefficiant);
                    }
                }
            }

            List<string> namesToRemove = new List<string>();
            foreach (InterPhaseCoefficiant interPhaseCoefficiant in _interPhaseCoefficiants)
            {
                string phaseName1 = interPhaseCoefficiant.Phase1.Name;
                string phaseName2 = interPhaseCoefficiant.Phase2.Name;

                bool boolName1 = false;
                bool boolName2 = false;
                foreach (Phase phase in _phases)
                {
                    string phaseName = phase.Name;
                    if (phaseName == phaseName1) boolName1 = true;
                    if (phaseName == phaseName2) boolName2 = true;
                }

                if (boolName1 == false) namesToRemove.Add(phaseName1);
                if (boolName2 == false) namesToRemove.Add(phaseName2);
            }
            foreach (string phaseName in namesToRemove)
            {
                this.Remove(phaseName);
            }
        }

        public int Count()
        {
            return _interPhaseCoefficiants.Count();
        }
    }
}
