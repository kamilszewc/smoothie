using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Smoothie.Informations
{
    class License
    {
        private string _licenseNewBSD =
            "Copyright (c) 2014-2015, Kamil Szewc \n" +
            "All rights reserved.\n" +
            "Redistribution and use in source and binary forms, with or without " +
            "modification, are permitted provided that the following conditions are met:\n" +
            "  * Redistribution of source code must retain the above copyright " +
            "notice, this list of conditions and the following disclaimer\n" +
            "  * Redistribution in binary form must reproduce the above copyright " +
            "notice, this list of conditions and the following disclaimer in the " +
            "documentation and/or other materials provided with the distribution.\n" +
            "  * Neither the name of the Institute of Fluid-Flow Machinery, Polish " +
            "Academy of Sciences nor the names of its contributors may be used to endorse " +
            "or promote products derived from this software without specific prior " +
            "written permission\n\n" +
            "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND " +
            "ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED " +
            "WARRANTIES OF METCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE " +
            "DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY " +
            "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES " +
            "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; " +
            "LOSS OF USE, DATA, OR PROFITS; OR BUISNESS INTERRUPTION) HOWEVER CAUSED AND " +
            "ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT " +
            "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS " +
            "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";

        private string _licenseMy =
            "1. Smoothie is licensed to you (on a limited, non-exclusive, personal, non-transferable " +
            "and royalty-free license) under which you are free to use, copy, distribute, modify and transmit " +
            "Smoothie PROVIDED THAT you only do so for NON-COMMERCIAL purposes " +
            "(without charging a fee to any third party) and PROVIDED THAT you attribute the work to us " +
            "(at least) mentioning our name, including an appropriate copyright notice and providing a link " +
            "to our website located at XXX.\n\n" +
            "2. You may install Smoothie on multiple devices in multiple locations PROVIDED THAT " +
            "you always use the Software for non-commercial purposes and otherwise in accordance with License terms.\n\n" +
            "3. An appropriate copyright notice for the purposes of this License shall take " +
            "the following form:\n Copyright (c) 2014 Kamil Szewc, IMP PAN.\n\n" +
            "4. This license shall be governed by and interpreted in accordance with Polish law.\n\n" +
            "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND " +
            "ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED " +
            "WARRANTIES OF METCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE " +
            "DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY " +
            "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES " +
            "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; " +
            "LOSS OF USE, DATA, OR PROFITS; OR BUISNESS INTERRUPTION) HOWEVER CAUSED AND " +
            "ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT " +
            "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS " +
            "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n" +
            "PLEASE NOTE THAT THIS LICENSE IS INTENDED FOR NON-COMMERCIAL USE OF SMOOTHIE ONLY. " +
            "IF YOU INTEND TO USE SMOOTHIE FOR A COMMERCIAL PURPOSE, PLEASE CONTACT kszewc@imp.gda.pl " +
            "TO ARRANGE A COLLABORATION AGREEMENT WITH US BASED ON OUR COMMERCIAL LICENSE TERMS.\n";

        public License()
        {

        }

        public string GetLicense()
        {
            return _licenseNewBSD;
        }
    }
}
