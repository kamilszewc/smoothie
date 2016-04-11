using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sph;

namespace Postprocessing
{
    public class PostProcessor
    {
        Domain _domain;
        ArtificialMesh _artificialMesh;

        double _kernelNorm;
        double _massNorm;

        public PostProcessor(Domain domain)
        {
            _domain = domain;
            _artificialMesh = new ArtificialMesh(domain);

            _massNorm = _domain["XCV"] * _domain["YCV"] / _domain.GetParticles().Count;
            _kernelNorm = 1.0 / (Math.PI * _domain.H * _domain.H);
        }

        public double InterpolateAtPosition(Position position, string fieldName)
        {
            List<Particle> particles = _artificialMesh.GetNeighbouringCellsParticles(position);
            double value = 0.0;
            double norm = 0.0;

            foreach (Particle particle in particles)
            {
                Vector vector = (particle as Position) - position;
                double distance = vector.Length();
                double q = distance / _domain.H;
                double volume = _massNorm * particle.GetDouble("initial density") / particle.GetDouble("density");
                double kernel = Kernel(q);

                value += particle.GetDouble(fieldName) * kernel * volume;
                norm += kernel * volume;
            }

            return value / norm;
        }

        public List<double> InterpolateAtPosition(List<Position> positions, string fieldName)
        {
            List<double> values = new List<double>();

            foreach (Position position in positions)
            {
                values.Add(InterpolateAtPosition(position, fieldName));
            }

            return values;
        }

        public double[,] InterpolateAtPosition(object[,] positions, string fieldName)
        {
            double[,] values = new double[positions.GetLength(0), positions.GetLength(1)];

            for (int i=0; i<positions.GetLength(0); i++)
            {
                for (int j=0; j<positions.GetLength(1); j++)
                {
                    values[i, j] = InterpolateAtPosition(positions[i, j] as Position, fieldName);
                }
            }

            return values;
        }


        private double Kernel(double q)
        {
            if (q <= 2.0)
                return _kernelNorm * 0.21875 * Math.Pow(2.0 - q, 4) * (q + 0.5);
            else
                return 0.0;
        }
    }
}
