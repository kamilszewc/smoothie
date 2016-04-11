using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sph;

namespace Postprocessing
{

    public class ArtificialMesh
    {
        Domain _domain;
        List<List<Particle>> _hashList;

        public ArtificialMesh(Domain domain)
        {
            _domain = domain;

            _hashList = new List<List<Particle>>();
            for (int i=0; i < _domain["NCX"] * _domain["NCY"]; i++)
            {
                _hashList.Add(new List<Particle>());
            }
            GenerateHashList();
        }

        private void GenerateHashList()
        {
            List<Particle> particles = _domain.GetParticles();
            for (int i=0; i<particles.Count; i++)
            {
                int hashId = GetHashIdFromParticlePosition(particles[i] as Position);
                _hashList[hashId].Add(particles[i]);
            }
        }

        public List<Particle> GetParticlesFromCell(int hashId)
        {
            return _hashList[hashId];
        }

        public List<Particle> GetParticlesFromCell(int nx, int ny)
        {
            int hashId = GetHashIdFromCellCoordinates(nx, ny);
            return GetParticlesFromCell(hashId);
        }

        public int GetHashIdFromCellCoordinates(int nx, int ny)
        {
            if ( (nx < _domain["NCX"]) && (ny < _domain["NCY"]) && (nx >= 0) && (ny >= 0) )
            {
                return nx + _domain["NCX"] * ny;
            }
            else
            {
                throw new IndexOutOfRangeException("nx or ny out of range");
            }
        }

        public int GetHashIdFromParticlePosition(Position position)
        {
            if ((position.X <= _domain["XCV"]) && (position.X >= 0.0)
              && (position.Y <= _domain["YCV"]) && (position.Y >= 0.0))
            {
                int nx = (int)(0.5 * position.X / _domain["H"]);
                int ny = (int)(0.5 * position.Y / _domain["H"]);

                return GetHashIdFromCellCoordinates(nx, ny);
            }
            else
            {
                throw new IndexOutOfRangeException("position out of range");
            }
        }

        public List<int> GetNeighbouringCellsHashIds(int hashId)
        {
            List<int> hashIds = new List<int>();

            int nx = hashId % _domain["NCX"];
            int ny = (hashId - nx) / _domain["NCX"];

            for (int i = nx-1; i <= nx+1; i++)
            {
                for (int j = ny-1; j <= ny+1; j++)
                {
                    try
                    {
                        int neighbouringHashId = GetHashIdFromCellCoordinates(i, j);
                        hashIds.Add(neighbouringHashId);
                    }
                    catch (IndexOutOfRangeException ex)
                    {
                    }
                }
            }

            return hashIds;
        }

        public List<int> GetNeighbouringCellsHashIds(Position position)
        {
            int hashId = GetHashIdFromParticlePosition(position);
            return GetNeighbouringCellsHashIds(hashId);
        }

        public List<List<Particle>> GetNeighbouringCellsHashList(Position position)
        {
            List<int> neighbouringCellsHashIds = GetNeighbouringCellsHashIds(position);
            List<List<Particle>> hashList= new List<List<Particle>>();

            foreach (int hashId in neighbouringCellsHashIds)
            {
                hashList.Add(_hashList[hashId]);
            }

            return hashList;
        }

        public List<Particle> GetNeighbouringCellsParticles(Position position)
        {
            List<List<Particle>> hashList = GetNeighbouringCellsHashList(position);
            List<Particle> particles = new List<Particle>();

            foreach (List<Particle> listOfParticles in hashList)
            {
                foreach (Particle particle in listOfParticles)
                {
                    particles.Add(particle);
                }
            }

            return particles;
        }
    }
}
