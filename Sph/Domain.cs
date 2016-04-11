using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.Text;
using System.IO;
using System.IO.Compression;
using System.Globalization;

namespace Sph
{
    /// <summary>
    /// Exception to indicate the inproper aspect ratio (NCX/NCY vs XCV/YCV).
    /// </summary>
    [Serializable()]
    public class InvalidAspectRatio : System.Exception
    {
        public InvalidAspectRatio() : base() { }
        public InvalidAspectRatio(string message) : base(message) { }
        public InvalidAspectRatio(string message, System.Exception inner) : base(message, inner) { }
        protected InvalidAspectRatio(System.Runtime.Serialization.SerializationInfo info,
        System.Runtime.Serialization.StreamingContext context) { }
    }

    [Serializable()]
    public class InvalidParameterName : System.Exception
    {
        public InvalidParameterName() : base() { }
        public InvalidParameterName(string message) : base(message) { }
        public InvalidParameterName(string message, System.Exception inner) : base(message, inner) { }
        protected InvalidParameterName(System.Runtime.Serialization.SerializationInfo info,
        System.Runtime.Serialization.StreamingContext context) { }
    }

    public class Domain
    {
        private List<Phase> _phases;
        private List<Position> _particlePositions;
        private List<Position> _looseParticlePositions;
        private SortedList _parameters;
        private SortedList _interPhaseParameters;
        private double _hdr;
        private double _h;
        private double _domainSizeX;
        private double _domainSizeY;
        private int _numberOfCellsX;
        private int _numberOfCellsY;

        /// <summary>
        /// Initialize a new instance of Domain class.
        /// </summary>
        /// <param name="hdr">Used to set h/dr (HDR)</param>
        /// <param name="h">Used to set the smoothing length (H)</param>
        /// <param name="xcv">Used to set horizontal size of domain (XCV)</param>
        /// <param name="ycv">Used to set vertical size of domain (YCV)</param>
        public Domain(double hdr, double domainSizeX, double domainSizeY, int numberOfCellsX, int numberOfCellsY)
        {
            _hdr = hdr;
            try
            {
                SetSize(domainSizeX, domainSizeY, numberOfCellsX, numberOfCellsY);
            }
            catch (InvalidAspectRatio e)
            {
                throw e;
            }

            _phases = new List<Phase>();

            InitParticlePositions();

            _parameters = new SortedList();
            _interPhaseParameters = new SortedList();
        }

        public Domain()
            : this(2.0, 1.0, 1.0, 16, 16)
        { }

        public Domain(string filename)
        {
            _parameters = new SortedList();
            _interPhaseParameters = new SortedList();
            ReadFromXml(filename);
            InitParticlePositions();
        }

        public void SetSize(double domainSizeX, double domainSizeY, int numberOfCellsX, int numberOfCellsY)
        {
            if (IsProperAspectRatio(domainSizeX, domainSizeY, numberOfCellsX, numberOfCellsY))
            {
                _domainSizeX = domainSizeX;
                _domainSizeY = domainSizeY;
                _numberOfCellsX = numberOfCellsX;
                _numberOfCellsY = numberOfCellsY;
                _h = 0.5 * domainSizeX / numberOfCellsX;

                InitParticlePositions();
            }
            else
            {
                throw new InvalidAspectRatio("Incompatible aspect ratio");
            }
        }

        private bool IsProperAspectRatio(double domainSizeX, double domainSizeY, int numberOfCellsX, int numberOfCellsY)
        {
            double cellsAspectRatio = (double)numberOfCellsX / (double)numberOfCellsY;
            double edgesAspectRatio = domainSizeX / domainSizeY;
            if (cellsAspectRatio == edgesAspectRatio)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// h/dr
        /// </summary>
        public double HDR
        {
            set { _hdr = value; }
            get { return _hdr; }
        }

        /// <summary>
        /// Smoothing length.
        /// </summary>
        public double H
        {
            get { return _h; }
        }

        /// <summary>
        /// Horizontal size of domain.
        /// </summary>
        public double XCV
        {
            get { return _domainSizeX; }
        }

        /// <summary>
        /// Vertical size of domain.
        /// </summary>
        public double YCV
        {
            get { return _domainSizeY; }
        }

        /// <summary>
        /// Initialize particle positions in domain.
        /// </summary>
        private void InitParticlePositions()
        {
            _particlePositions = new List<Position>();
            _looseParticlePositions = new List<Position>();

            double dr = H / HDR;
            int nx = (int)(XCV / dr);
            int ny = (int)(YCV / dr);
            for (int i = 0; i < nx; i++)
                for (int j = 0; j < ny; j++)
                {
                    Position position = new Position(0.5 * dr + i * dr, 0.5 * dr + j * dr);
                    _particlePositions.Add(position);
                }

            if (nx < ny)
            {
                nx = 150;
                ny = nx * Convert.ToInt32(YCV / XCV);
            }
            else
            {
                ny = 150;
                nx = ny * Convert.ToInt32(XCV / YCV);
            }
            dr = XCV / nx;
            for (int i = 0; i < nx; i++)
                for (int j = 0; j < ny; j++)
                {
                    Position position = new Position(0.5 * dr + i * dr, 0.5 * dr + j * dr);
                    _looseParticlePositions.Add(position);
                }
        }

        public Phase AddPhase(int id, string name, string location, byte[] color)
        {
            Phase phase = new Phase(id, name, location, color);
            _phases.Add(phase);
            return phase;
        }

        public Phase AddPhase(int id, string name, string location)
        {
            Phase phase = new Phase(id, name, location);
            _phases.Add(phase);
            return phase;
        }
        /// <summary>
        /// Add a new phase into domain.
        /// </summary>
        /// <param name="name">Used to set up a new phase name.</param>
        /// <returns>Returns instance of added phase.</returns>
        public Phase AddPhase(string name)
        {
            Phase phase = new Phase(name);
            _phases.Add(phase);
            return phase;
        }

        /// <summary>
        /// Returns phase instance of given id.
        /// </summary>
        /// <param name="id">Used to indicate phase id.</param>
        public Phase GetPhase(int id)
        {
            int listId = _phases.FindIndex(phase => phase.Id == id);
            return _phases[listId];
        }

        /// <summary>
        /// Returns phase instance of given name
        /// </summary>
        /// <param name="name">Used to indicate name of phase</param>
        public Phase GetPhase(string name)
        {
            int id = _phases.FindIndex(phase => phase.Name == name);
            return _phases[id];
        }

        public List<Phase> GetPhases()
        {
            return _phases;
        }

        public InterPhaseCoefficients AddInterPhaseParameter(string name)
        {
            InterPhaseCoefficients interPhaseParameter = new InterPhaseCoefficients(_phases);
            _interPhaseParameters.Add(name, interPhaseParameter);
            return interPhaseParameter;
        }

        public InterPhaseCoefficients GetInterPhaseParameter(string name)
        {
            return (InterPhaseCoefficients)_interPhaseParameters[name];
        }

        public void RemoveInterPhaseParameter(string name)
        {
            _interPhaseParameters.Remove(name);
        }

        public void RemovePhase(int id)
        {
            int listId = _phases.FindIndex(phase => phase.Id == id);
            _phases.RemoveAt(listId);
        }

        public void RemovePhase(string name)
        {
            int id = _phases.FindIndex(phase => phase.Name == name);
            _phases.RemoveAt(id);
        }

        /// <summary>
        /// Returns list of all particles in domain
        /// </summary>
        public List<Particle> GetParticles()
        {
            List<Particle> particles = new List<Particle>();
            foreach (Phase phase in _phases)
            {
                List<Particle> phaseParticles = phase.Particles;
                foreach (Particle particle in phaseParticles)
                {
                    particles.Add(particle);
                }
            }
            return particles;
        }

        public List<Position> GetParticlePositions()
        {
            return _particlePositions;
        }

        public List<Position> GetParticleLoosePositions()
        {
            return _looseParticlePositions;
        }

        /// <summary>
        /// Generate particles in domain
        /// </summary>
        public void GenerateParticles()
        {
            int n = 0;
            foreach (Phase phase in _phases)
            {
                phase.GenerateParticles(_particlePositions);
                n += phase.GetNumberOfParticles();
            }
            this["N"] = n;
        }

        /// <summary>
        /// Adds a new parameter.
        /// </summary>
        /// <param name="name">Used to set a name of parameter.</param>
        /// <param name="value">Used to set a value.</param>
        public void AddParameter(string name, dynamic value)
        {
            if ((name == "H") || (name == "HDR") || (name == "XCV") || (name == "YCV") ||
                 (name == "NCX") || (name == "NCY"))
            {
                throw new InvalidParameterName("Illegal name of parameter. You tried to override a basic field value.");
            }
            else
            {
                _parameters.Add(name, value);
            }
        }

        /// <summary>
        /// Removes a parameter of given name.
        /// </summary>
        /// <param name="name">Name of parameter.</param>
        public void RemoveParameter(string name)
        {
            _parameters.Remove(name);
        }

        /// <summary>
        /// Set up a new value of parameter of given name.
        /// </summary>
        /// <param name="name">Name of parameter.</param>
        /// <param name="value">New value.</param>
        public void SetParameter(string name, dynamic value)
        {
            if ( (name == "XCV") || (name == "YCV") ||
                 (name == "NCX") || (name == "NCY"))
            {
                throw new InvalidParameterName("Illegal name of parameter. You tried to override a basic field value.");
            }
            else
            {
                _parameters[name] = value;
            }
        }

        public dynamic this[string name]
        {
            set
            {
                SetParameter(name, value);
            }
            get
            {
                return GetParameter(name);
            }
        }

        /// <summary>
        /// Returns a value of given parameter.
        /// </summary>
        /// <param name="name">Name of parameter.</param>
        public dynamic GetParameter(string name)
        {
            switch (name)
            {
                case "H":
                    return _h;
                case "HDR":
                    return _hdr;
                case "XCV":
                    return _domainSizeX;
                case "YCV":
                    return _domainSizeY;
                case "NCX":
                    return _numberOfCellsX;
                case "NCY":
                    return _numberOfCellsY;
                default:
                    return _parameters[name];
            }
        }

        /// <summary>
        /// Returns SortedList of all parameters.
        /// </summary>
        public SortedList GetParameters()
        {
            return _parameters;
        }

        public void WriteToXml(string filename, bool compression)
        {
            // Create xml document
            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.LoadXml("<sph>" +
                                "  <domain>" +
                                "    <parameters></parameters>" +
                                "    <phases></phases>" +
                                "    <interphase-parameters></interphase-parameters>" +
                                "  </domain>" +
                                "  <particles></particles>" +
                                "</sph>");

            XmlElement root = xmlDocument.DocumentElement;

            // Create xml declaration
            XmlDeclaration xmlDeclaration = xmlDocument.CreateXmlDeclaration("1.0", null, null);
            xmlDocument.InsertBefore(xmlDeclaration, root);

            // Create xml nodes for parameters
            XmlNode xmlDomain = root.SelectSingleNode("domain");
            XmlNode xmlParameters = xmlDomain.SelectSingleNode("parameters");
            XmlNode xmlPhases = xmlDomain.SelectSingleNode("phases");
            XmlNode xmlInterPhaseParameters = xmlDomain.SelectSingleNode("interphase-parameters");
            XmlNode xmlParticles = root.SelectSingleNode("particles");

            SortedList basicParameters = new SortedList();
            basicParameters.Add("HDR", _hdr);
            basicParameters.Add("XCV", _domainSizeX);
            basicParameters.Add("YCV", _domainSizeY);
            basicParameters.Add("NCX", _numberOfCellsX);
            basicParameters.Add("NCY", _numberOfCellsY);

            foreach (DictionaryEntry entry in basicParameters)
            {
                XmlNode xmlNode = xmlDocument.CreateNode(XmlNodeType.Element, "parameter", null);
                xmlNode.InnerText = Convert.ToString(entry.Value, CultureInfo.InvariantCulture);

                XmlAttribute xmlAttributeName = xmlDocument.CreateAttribute("name");
                xmlAttributeName.Value = (string)entry.Key;
                xmlNode.Attributes.Append(xmlAttributeName);

                XmlAttribute xmlAttributeType = xmlDocument.CreateAttribute("type");
                if (entry.Value is double)
                {
                    xmlAttributeType.Value = "double";
                }
                else if (entry.Value is int)
                {
                    xmlAttributeType.Value = "int";
                }
                else
                {
                    xmlAttributeType.Value = "string";
                }
                xmlNode.Attributes.Append(xmlAttributeType);

                xmlParameters.AppendChild(xmlNode);
            }

            foreach (DictionaryEntry entry in _parameters)
            {
                XmlNode xmlNode = xmlDocument.CreateNode(XmlNodeType.Element, "parameter", null);
                xmlNode.InnerText = Convert.ToString(entry.Value, CultureInfo.InvariantCulture);

                XmlAttribute xmlAttributeName = xmlDocument.CreateAttribute("name");
                xmlAttributeName.Value = (string)entry.Key;
                xmlNode.Attributes.Append(xmlAttributeName);

                XmlAttribute xmlAttributeType = xmlDocument.CreateAttribute("type");
                if (entry.Value is double)
                {
                    xmlAttributeType.Value = "double";
                }
                else if (entry.Value is int)
                {
                    xmlAttributeType.Value = "int";
                }
                else
                {
                    xmlAttributeType.Value = "string";
                }
                xmlNode.Attributes.Append(xmlAttributeType);

                xmlParameters.AppendChild(xmlNode);
            }

            foreach (Phase phase in _phases)
            {
                XmlNode xmlNode = xmlDocument.CreateNode(XmlNodeType.Element, "phase", null);

                XmlAttribute xmlAttributeName = xmlDocument.CreateAttribute("name");
                xmlAttributeName.Value = phase.Name;
                xmlNode.Attributes.Append(xmlAttributeName);

                XmlAttribute xmlAttributeId = xmlDocument.CreateAttribute("id");
                xmlAttributeId.Value = Convert.ToString(phase.Id);
                xmlNode.Attributes.Append(xmlAttributeId);

                xmlPhases.AppendChild(xmlNode);

                XmlNode xmlColor = xmlDocument.CreateNode(XmlNodeType.Element, "color", null);
                XmlAttribute xmlAttributeColorA = xmlDocument.CreateAttribute("alpha");
                xmlAttributeColorA.Value = Convert.ToString(phase.Color[0]);
                xmlColor.Attributes.Append(xmlAttributeColorA);

                XmlAttribute xmlAttributeColorR = xmlDocument.CreateAttribute("red");
                xmlAttributeColorR.Value = Convert.ToString(phase.Color[1]);
                xmlColor.Attributes.Append(xmlAttributeColorR);

                XmlAttribute xmlAttributeColorG = xmlDocument.CreateAttribute("green");
                xmlAttributeColorG.Value = Convert.ToString(phase.Color[2]);
                xmlColor.Attributes.Append(xmlAttributeColorG);

                XmlAttribute xmlAttributeColorB = xmlDocument.CreateAttribute("blue");
                xmlAttributeColorB.Value = Convert.ToString(phase.Color[3]);
                xmlColor.Attributes.Append(xmlAttributeColorB);

                xmlNode.AppendChild(xmlColor);

                XmlNode xmlLocation = xmlDocument.CreateNode(XmlNodeType.Element, "location", null);
                xmlLocation.InnerText = phase.Location;
                xmlNode.AppendChild(xmlLocation);

                XmlNode xmlFields = xmlDocument.CreateNode(XmlNodeType.Element, "fields", null);
                xmlNode.AppendChild(xmlFields);

                foreach (Field field in phase.GetFields())
                {
                    XmlNode xmlField = xmlDocument.CreateNode(XmlNodeType.Element, "field", null);
                    xmlField.InnerText = field.Value;

                    XmlAttribute xmlAttributeFieldName = xmlDocument.CreateAttribute("name");
                    xmlAttributeFieldName.Value = field.Name;
                    xmlField.Attributes.Append(xmlAttributeFieldName);

                    XmlAttribute xmlAttributeFieldType = xmlDocument.CreateAttribute("type");
                    xmlAttributeFieldType.Value = field.Type;
                    xmlField.Attributes.Append(xmlAttributeFieldType);

                    xmlFields.AppendChild(xmlField);
                }
            }

            foreach (DictionaryEntry entry in _interPhaseParameters)
            {
                string interPhaseParametersName = (string)entry.Key;
                InterPhaseCoefficients interPhaseParameters = entry.Value as InterPhaseCoefficients;
                XmlNode xmlIPPs = xmlDocument.CreateNode(XmlNodeType.Element, interPhaseParametersName, null);
                xmlInterPhaseParameters.AppendChild(xmlIPPs);

                interPhaseParameters.Update();
                foreach (InterPhaseCoefficiant interPhaseParameter in interPhaseParameters.Get())
                {
                    XmlNode xmlValue = xmlDocument.CreateNode(XmlNodeType.Element, "value", null);
                    xmlValue.InnerText = Convert.ToString(interPhaseParameter.Value, CultureInfo.InvariantCulture);

                    XmlAttribute xmlAttributePhaseId1 = xmlDocument.CreateAttribute("phaseId1");
                    xmlAttributePhaseId1.Value = interPhaseParameter.Phase1.Id.ToString();
                    xmlValue.Attributes.Append(xmlAttributePhaseId1);

                    XmlAttribute xmlAttributePhaseId2 = xmlDocument.CreateAttribute("phaseId2");
                    xmlAttributePhaseId2.Value = interPhaseParameter.Phase2.Id.ToString();
                    xmlValue.Attributes.Append(xmlAttributePhaseId2);

                    xmlIPPs.AppendChild(xmlValue);
                }
            }

            foreach (string name in GetParticles()[0].GetListOfFields())
            {
                XmlNode xmlField = xmlDocument.CreateNode(XmlNodeType.Element, "field", null);

                XmlAttribute xmlAttributeFieldName = xmlDocument.CreateAttribute("name");
                xmlAttributeFieldName.Value = name;
                xmlField.Attributes.Append(xmlAttributeFieldName);

                XmlAttribute xmlAttributeFieldType = xmlDocument.CreateAttribute("type");
                var fieldValue = GetParticles()[0][name];
                string fieldType = "";
                if (fieldValue is int)
                {
                    fieldType = "int";
                }
                else if (fieldValue is double)
                {
                    fieldType = "double";
                }
                xmlAttributeFieldType.Value = fieldType;
                xmlField.Attributes.Append(xmlAttributeFieldType);

                StringBuilder stringBuilder = new StringBuilder();

                foreach (Particle particle in GetParticles())
                {
                    stringBuilder.Append(Convert.ToString(particle[name], CultureInfo.InvariantCulture));
                    stringBuilder.Append(" ");
                }
                xmlField.InnerText = stringBuilder.ToString();

                xmlParticles.AppendChild(xmlField);
            }


            if (compression == true)
            {
                FileStream fileStream = new FileStream(filename, System.IO.FileMode.Create);
                GZipStream compressedFileStream = new GZipStream(fileStream, CompressionLevel.Optimal);
                xmlDocument.Save(compressedFileStream);
                compressedFileStream.Close();
                fileStream.Close();
            }
            else
            {
                xmlDocument.Save(filename);
            }

        }

        public void WriteToXml(string filename)
        {
            string[] splitedFilename = filename.Split('.');
            string suffix = splitedFilename[splitedFilename.Length - 1];
            if (suffix == "gz")
            {
                WriteToXml(filename, true);
            }
            else
            {
                WriteToXml(filename, false);
            }
        }


        public void ReadFromXml(string filename)
        {
            XmlDocument xmlDocument = new XmlDocument();

            string[] splitedFilename = filename.Split('.');
            string suffix = splitedFilename[splitedFilename.Length - 1];
            if (suffix == "gz")
            {
                FileStream fileStream = new FileStream(filename, System.IO.FileMode.Open);
                GZipStream compressedFileStream = new GZipStream(fileStream, CompressionMode.Decompress);
                xmlDocument.Load(compressedFileStream);
                compressedFileStream.Close();
                fileStream.Close();
            }
            else
            {
                xmlDocument.Load(filename);
            }




            XmlNode xmlDomain = xmlDocument.DocumentElement.SelectSingleNode("domain");
            XmlNode xmlParameters = xmlDomain.SelectSingleNode("parameters");
            XmlNode xmlPhases = xmlDomain.SelectSingleNode("phases");
            XmlNode xmlInterPhaseParameters = xmlDomain.SelectSingleNode("interphase-parameters");
            XmlNode xmlParticles = xmlDocument.DocumentElement.SelectSingleNode("particles");
            

            double domainSizeX = 0.0;
            double domainSizeY = 0.0;
            int numberOfCellsX = 0;
            int numberOfCellsY = 0;

            foreach (XmlNode xmlParameter in xmlParameters)
            {
                var parameterName = xmlParameter.Attributes["name"].Value;
                var parameterType = xmlParameter.Attributes["type"].Value;
                dynamic parameterValue;
                if (parameterType == "double")
                {
                    parameterValue = Convert.ToDouble(xmlParameter.InnerText, CultureInfo.InvariantCulture);
                }
                else if (parameterType == "int")
                {
                    parameterValue = Convert.ToInt32(xmlParameter.InnerText, CultureInfo.InvariantCulture);
                }
                else
                {
                    parameterValue = Convert.ToString(xmlParameter.InnerText, CultureInfo.InvariantCulture);
                }

                switch (parameterName)
                {
                    case "HDR":
                        HDR = parameterValue;
                        break;
                    case "XCV":
                        domainSizeX = parameterValue;
                        break;
                    case "YCV":
                        domainSizeY = parameterValue;
                        break;
                    case "NCX":
                        numberOfCellsX = parameterValue;
                        break;
                    case "NCY":
                        numberOfCellsY = parameterValue;
                        break;
                    default:
                        AddParameter(parameterName, parameterValue);
                        break;
                }

            }
            SetSize(domainSizeX, domainSizeY, numberOfCellsX, numberOfCellsY);

            _phases = new List<Phase>(xmlPhases.ChildNodes.Count);
            Phase.ResetNumberOfCreatedPhases();

            foreach (XmlNode xmlPhase in xmlPhases)
            {
                var phaseName = xmlPhase.Attributes["name"].Value;
                var phaseId = Convert.ToInt32(xmlPhase.Attributes["id"].Value);
                var phaseLocation = xmlPhase["location"].InnerText.Trim();
                byte[] color = new byte[4];
                foreach (XmlAttribute xmlAttributeColor in xmlPhase["color"].Attributes)
                {
                    switch (xmlAttributeColor.Name)
                    {
                        case "alpha":
                            color[0] = Convert.ToByte(xmlAttributeColor.Value);
                            break;
                        case "red":
                            color[1] = Convert.ToByte(xmlAttributeColor.Value);
                            break;
                        case "green":
                            color[2] = Convert.ToByte(xmlAttributeColor.Value);
                            break;
                        case "blue":
                            color[3] = Convert.ToByte(xmlAttributeColor.Value);
                            break;
                    }
                }

                Phase phase = AddPhase(phaseId, phaseName, phaseLocation, color);

                XmlNode xmlPhaseFields = xmlPhase.SelectSingleNode("fields");
                foreach (XmlNode xmlPhaseField in xmlPhaseFields)
                {
                    var phaseFieldName = xmlPhaseField.Attributes["name"].Value;
                    var phaseFieldType = xmlPhaseField.Attributes["type"].Value;
                    var phaseFieldValue = xmlPhaseField.InnerText;
                    phase.AddField(phaseFieldName, phaseFieldType, phaseFieldValue);
                }
            }

            XmlNode xmlSurfaceTensionCoefficient = xmlInterPhaseParameters.SelectSingleNode("surface-tension-coefficient");
            InterPhaseCoefficients surfaceTensionCoefficients = AddInterPhaseParameter("surface-tension-coefficient");
            surfaceTensionCoefficients.Update();
            foreach(XmlNode xmlValue in xmlSurfaceTensionCoefficient)
            {
                int phaseId1 = Convert.ToInt32(xmlValue.Attributes["phaseId1"].Value);
                int phaseId2 = Convert.ToInt32(xmlValue.Attributes["phaseId2"].Value);
                double value = Convert.ToDouble(xmlValue.InnerText, CultureInfo.InvariantCulture);
                Phase phase1 = GetPhase(phaseId1);
                Phase phase2 = GetPhase(phaseId2);
                surfaceTensionCoefficients.Set(phase1.Name, phase2.Name, value);
            }

            XmlNode xmlInterfaceCorrectionCoefficient = xmlInterPhaseParameters.SelectSingleNode("interface-correction-coefficient");
            InterPhaseCoefficients interfaceCorrectionCoefficients = AddInterPhaseParameter("interface-correction-coefficient");
            interfaceCorrectionCoefficients.Update();
            foreach (XmlNode xmlValue in xmlInterfaceCorrectionCoefficient)
            {
                int phaseId1 = Convert.ToInt32(xmlValue.Attributes["phaseId1"].Value);
                int phaseId2 = Convert.ToInt32(xmlValue.Attributes["phaseId2"].Value);
                double value = Convert.ToDouble(xmlValue.InnerText, CultureInfo.InvariantCulture);
                Phase phase1 = GetPhase(phaseId1);
                Phase phase2 = GetPhase(phaseId2);
                interfaceCorrectionCoefficients.Set(phase1.Name, phase2.Name, value);
            }

            List<string> particleFields = new List<string>();
            List<string> particleTypes = new List<string>();
            List<string[]> particleFieldValues = new List<string[]>();
            foreach (XmlNode xmlParticleField in xmlParticles)
            {
                particleFields.Add(xmlParticleField.Attributes["name"].Value);
                particleTypes.Add(xmlParticleField.Attributes["type"].Value);
                particleFieldValues.Add(xmlParticleField.InnerText.Trim().Split());
            }

            List<Particle> particles = new List<Particle>(particleFieldValues[0].Length);
            for (int i = 0; i < particleFieldValues[0].Length; i++)
            {
                Hashtable fields = new Hashtable();
                for (int j = 0; j < particleFields.Count; j++)
                {
                    string particleFieldName = particleFields[j];
                    string particleFieldType = particleTypes[j];
                    dynamic particleFieldValue;
                    switch (particleFieldType)
                    {
                        case "double":
                            particleFieldValue = Convert.ToDouble(particleFieldValues[j][i], CultureInfo.InvariantCulture);
                            break;
                        case "int":
                            particleFieldValue = Convert.ToInt32(particleFieldValues[j][i], CultureInfo.InvariantCulture);
                            break;
                        default:
                            particleFieldValue = particleFieldValues[j][i];
                            break;
                    }

                    fields.Add(particleFieldName, particleFieldValue);
                }
                particles.Add(new Particle(fields));
            }

            foreach (Phase phase in _phases)
            {
                List<Particle> particlesInPhase = new List<Particle>();
                int phaseId = phase.Id;
                foreach (Particle particle in particles)
                {
                    if (particle.PhaseId == phaseId)
                    {
                        particlesInPhase.Add(particle);
                    }
                }
                phase.Particles = particlesInPhase;
            }

        }
    }
}