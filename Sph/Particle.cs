using System;
using System.Collections;
using System.Collections.Generic;

namespace Sph
{
    public class Particle : Position
    {
        private int _id;
        private int _phaseId;
        private Hashtable _fields;

        private static int _numberOfCreatedParticles = 0;

        public Particle(Hashtable fields)
            : base(0.0, 0.0, 0.0)
        {
            _fields = fields;
            try
            {
                _id = (int)_fields["id"];
                _phaseId = (int)_fields["phase-id"];
                _x = (double)fields["x-position"];
                _y = (double)fields["y-position"];
            }
            catch
            {
                Console.WriteLine("Error");
            }

            //try
            //{
            //    _z = (double)fields["z-position"];
            //}
            //catch
            //{
                //_z = 0.0;
                //_fields["z-position"] = 0.0;
            //}

            _numberOfCreatedParticles++;
        }

        public Particle(int id, int phaseId, double x, double y, double z, Hashtable fields)
            : base(0.0, 0.0, 0.0)
        {
            _fields = fields;
            Id = id;
            PhaseId = phaseId;
            X = x;
            Y = y;
            Z = z;

            _numberOfCreatedParticles++;
        }

        public Particle(int id, double x, double y, double z, Hashtable fields)
            : this(id, 0, x, y, z, fields)
        { }

        public Particle(int id, double x, double y, double z)
            : base(x, y, z)
        {
            _id = id;
            _fields = new Hashtable();
            _fields.Add("id", id);
            _fields.Add("x-position", x);
            _fields.Add("y-position", y);
            _fields.Add("z-position", z);
            _numberOfCreatedParticles++;
        }

        public Particle(int id, double x, double y)
            : this(id, x, y, 0.0)
        { }


        public int Id
        {
            set { _id = value; _fields["id"] = value; }
            get { return _id; }
        }

        public int PhaseId
        {
            set { _phaseId = value; _fields["phase-id"] = value; }
            get { return _phaseId; }
        }

        public override double X
        {
            set { _x = value; _fields["x-position"] = value; }
            get { return _x; }
        }

        public override double Y
        {
            set { _y = value; _fields["y-position"] = value; }
            get { return _y; }
        }

        public override double Z
        {
            set { _z = value; _fields["z-position"] = value; }
            get { return _z; }
        }

        public dynamic this[string key]
        {
            set
            {
                Set(key, value);
            }
            get
            {
                return Get(key);
            }
        }

        public dynamic Get(string key)
        {
            return _fields[key];
        }

        public void Set(string key, dynamic value)
        {
            _fields[key] = value;
            switch (key)
            {
                case "id":
                    _id = value;
                    break;
                case "phase-id":
                    _phaseId = value;
                    break;
                case "x-position":
                    _x = value;
                    break;
                case "y-position":
                    _y = value;
                    break;
                case "z-position":
                    _z = value;
                    break;
                default:
                    break;
            }
        }

        public double GetDouble(string key)
        {
            return (double)_fields[key];
        }

        public List<string> GetListOfFields()
        {
            List<string> listOfField = new List<string>();

            foreach (DictionaryEntry entry in _fields)
            {
                listOfField.Add((string)entry.Key);
            }

            return listOfField;
        }

        public override string ToString()
        {
            return "Particle " + Convert.ToString(_id) + " position=" + base.ToString() + " phaseId=" + Convert.ToString(_phaseId);
        }


        public static int GetNumberOfCreatedParticles()
        {
            return _numberOfCreatedParticles;
        }
    }
}
