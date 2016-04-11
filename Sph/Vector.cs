using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sph
{
    public class Vector
    {
        protected double _x;
        protected double _y;
        protected double _z;

        public Vector(double x, double y, double z)
        {
            _x = x;
            _y = y;
            _z = z;
        }

        public virtual double X
        {
            set { _x = value; }
            get { return _x; }
        }

        public virtual double Y
        {
            set { _y = value; }
            get { return _y; }
        }

        public virtual double Z
        {
            set { _z = value; }
            get { return _z; }
        }

        public static Vector operator +(Vector v1, Vector v2)
        {
            return new Vector(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
        }

        public static Vector operator -(Vector v1, Vector v2)
        {
            return new Vector(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
        }

        public static double operator *(Vector v1, Vector v2)
        {
            return (v1.X * v2.X) + (v1.Y * v2.Y) + (v1.Z * v1.Z);
        }

        public double Length()
        {
            return Math.Sqrt(this * this);
        }

        public static double Distance(Vector v1, Vector v2)
        {
            return (v1 - v2).Length();
        }

        public override string ToString()
        {
            return "(" + Convert.ToString(_x) + ", " + Convert.ToString(_y) + ", " + Convert.ToString(_z) + ")";
        }
    }
}

