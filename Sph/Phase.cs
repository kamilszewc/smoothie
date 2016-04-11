using System;
using System.Collections.Generic;
using System.Collections;
using System.Globalization;
using System.Text;

namespace Sph
{
    [Serializable()]
    public class TextToMathParsingException : System.Exception
    {
        public TextToMathParsingException() : base() { }
        public TextToMathParsingException(string message) : base(message) { }
        public TextToMathParsingException(string message, System.Exception inner) : base(message, inner) { }
        protected TextToMathParsingException(System.Runtime.Serialization.SerializationInfo info,
        System.Runtime.Serialization.StreamingContext context) { }
    }

    public class Phase
    {
        private int _id;
        private string _name;
        private string _location;
        private List<Field> _fields;
        private List<Particle> _particles;
        private byte[] _color;

        static private int _numberOfCreatedPhases = 0;

        public Phase(int id, string name, string location, byte[] color)
        {
            _id = id;
            _name = name;
            _location = location;
            _fields = new List<Field>();
            _color = color;
            _numberOfCreatedPhases++;
        }

        public Phase(int id, string name, string location)
            : this(id, name, location, new byte[]{255,0,0,0})
        { }

        public Phase(string name, string location)
            : this(_numberOfCreatedPhases, name, location)
        { }

        public Phase(string name)
            : this(name, "")
        { }


        public void AddField(string name, string type, string value)
        {
            _fields.Add(new Field(name, type, value));
        }

        public void DelField(string name)
        {
            _fields.RemoveAll(val => val.Name == name);
        }

        public Field GetField(string name)
        {
            return _fields.Find(val => val.Name == name);
        }

        public List<Field> GetFields()
        {
            return _fields;
        }

        public void GenerateParticles(IList<Position> positions)
        {
            _particles = new List<Particle>();
            foreach (Position position in positions)
            {
                if (this.HasPosition(position))
                {
                    Hashtable fields = new Hashtable();
                    foreach (Field field in _fields)
                    {
                        var value = field.ParseValueInPosition(position);
                        fields.Add(field.Name, value);
                    }
                    _particles.Add(new Particle(Particle.GetNumberOfCreatedParticles(), _id, position.X, position.Y, position.Z, fields));
                }
            }
        }


        /// <summary>
        /// Returns particle of given id
        /// </summary>
        /// <param name="id">Id of particle</param>
        public Particle GetParticle(int id)
        {
            return _particles.Find(particle => particle.Id == id);
        }

        public List<Particle> Particles
        {
            set
            {
                _particles = new List<Particle>();
                _particles = value;
            }
            get { return _particles; }
        }

        public int Id
        {
            get { return _id; }
        }

        public string Name
        {
            set { _name = value; }
            get { return _name; }
        }

        public string Location
        {
            set { _location = value; }
            get { return _location; }
        }

        public byte[] Color
        {
            set { _color = value; }
            get { return _color; }
        }

        public bool HasPosition(Position position)
        {
            if (_location == "")
            {
                return true;
            }
            else
            {
                try
                {
                    //string alocation = "y>x";
                    string[] locationSplitedX = _location.Split('x');
                    StringBuilder stringBuilderX = new StringBuilder();

                    int i = 1;
                    foreach (string val in locationSplitedX)
                    {
                        stringBuilderX.Append(val);
                        if (i < locationSplitedX.Length)
                        {
                            stringBuilderX.Append(Convert.ToString(position.X, CultureInfo.InvariantCulture));
                        }
                        i++;
                    }
                    string location = stringBuilderX.ToString();

                    string[] valueSplitedY = location.Split('y');
                    StringBuilder stringBuilderY = new StringBuilder();
                    i = 1;
                    foreach (string val in valueSplitedY)
                    {
                        stringBuilderY.Append(val);
                        if (i < valueSplitedY.Length)
                        {
                            stringBuilderY.Append(Convert.ToString(position.Y, CultureInfo.InvariantCulture));
                        }
                        i++;
                    }
                    location = stringBuilderY.ToString();

                    var parser = YAMP.Parser.Parse(location);
                    var result = parser.Execute(); 

                    if (result.ToString() == "0")
                    {
                        return false;
                    }
                    else
                    {
                        return true;
                    }
                }
                catch
                {
                    throw new TextToMathParsingException();
                }
            }
        }

        public int GetNumberOfParticles()
        {
            return _particles.Count;
        }

        public override string ToString()
        {
            return "Phase " + _name;
        }

        public static void ResetNumberOfCreatedPhases()
        {
            _numberOfCreatedPhases = 0;
        }

        public static int GetNumberOfCreatedPhases()
        {
            return _numberOfCreatedPhases;
        }
    }
}
