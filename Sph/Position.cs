using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sph
{
    public class Position : Vector
    {
        public Position(double x, double y, double z)
            : base(x, y, z)
        { }

        public Position(double x, double y)
            : base(x, y, 0.0)
        { }
    }
}
