using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Smoothie.Informations
{
    class Version
    {
        private string _version = "1.1.0 (14-03-2015)";

        public Version()
        {
        }

        public string GetVersion()
        {
            return _version;
        }
    }
}
