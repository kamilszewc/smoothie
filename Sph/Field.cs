using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Globalization;

namespace Sph
{
    public class Field
    {
        private string _name;
        private string _type;
        private string _value;

        /// <summary>
        /// Initialize a new instance of Field
        /// </summary>
        /// <param name="name">Used to set name</param>
        /// <param name="type">Used to set type</param>
        /// <param name="value">Used to set value</param>
        public Field(string name, string type, string value)
        {
            _name = name;
            _type = type;
            _value = value;
        }

        public string Name
        {
            set { _name = value; }
            get { return _name; }
        }

        public string Type
        {
            set { _type = value; }
            get { return _type; }
        }

        public string Value
        {
            set { _value = value; }
            get { return _value; }
        }

        public dynamic ParseValueInPosition(Position position)
        {
            string[] locationSplitedX = _value.Split('x');
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


            if (_type == "int")
            {
                int result = (parser.Execute() as YAMP.ScalarValue).IntValue;
                return Convert.ToInt32(result, CultureInfo.InvariantCulture);
            }
            else
            {
                double result = (parser.Execute() as YAMP.ScalarValue).Value;
                return Convert.ToDouble(result, CultureInfo.InvariantCulture);
            }
        }
    }
}
