using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Globalization;
using OxyPlot;
using OxyPlot.Series;
using OxyPlot.Axes;
using Sph;

namespace Smoothie
{
    class SimulationPlotModels
    {
        public PlotModel PlotModelFPS { set; get; }
        public LinearAxis AxisXFPS { get; set; }
        public LinearAxis AxisYFPS { get; set; }
        public LineSeries LineSeriesFPS { get; set; }

        public PlotModel PlotModelADT { set; get; }
        public LinearAxis AxisXADT { get; set; }
        public LinearAxis AxisYADT { get; set; }
        public LineSeries LineSeriesADT { get; set; }

        public PlotModel PlotModelKE { set; get; }
        public LinearAxis AxisXKE { get; set; }
        public LinearAxis AxisYKE { get; set; }
        public LineSeries LineSeriesKE { get; set; }

        public SimulationPlotModels()
        {
            PlotModelFPS = new PlotModel();
            LineSeriesFPS = new LineSeries();
            //LineSeriesFPS.Points.Add(new DataPoint(0.0, 0.0));
            //LineSeriesFPS.Points.Add(new DataPoint(1.0, 1.0));
            //LineSeriesFPS.Points.Add(new DataPoint(2.0, 2.0));
            PlotModelFPS.Series.Add(LineSeriesFPS);

            AxisXFPS = new LinearAxis();
            AxisXFPS.Position = AxisPosition.Bottom;
            AxisXFPS.Title = "Time";
            PlotModelFPS.Axes.Add(AxisXFPS);

            AxisYFPS = new LinearAxis();
            AxisYFPS.Position = AxisPosition.Left;
            AxisYFPS.Title = "Frames per second";
            PlotModelFPS.Axes.Add(AxisYFPS);



            PlotModelADT = new PlotModel();
            LineSeriesADT = new LineSeries();
            //LineSeriesADT.Points.Add(new DataPoint(0.0, 0.0));
            //LineSeriesADT.Points.Add(new DataPoint(1.0, 2.0));
            //LineSeriesADT.Points.Add(new DataPoint(2.0, 5.0));
            PlotModelADT.Series.Add(LineSeriesADT);

            AxisXADT = new LinearAxis();
            AxisXADT.Position = AxisPosition.Bottom;
            AxisXADT.Title = "Time";
            PlotModelADT.Axes.Add(AxisXADT);

            AxisYADT = new LinearAxis();
            AxisYADT.Position = AxisPosition.Left;
            AxisYADT.Title = "Time step";
            PlotModelADT.Axes.Add(AxisYADT);



            PlotModelKE = new PlotModel();
            LineSeriesKE = new LineSeries();
            //LineSeriesKE.Points.Add(new DataPoint(0.0, 0.0));
            //LineSeriesKE.Points.Add(new DataPoint(1.0, 2.0));
            //LineSeriesKE.Points.Add(new DataPoint(2.0, 5.0));
            PlotModelKE.Series.Add(LineSeriesKE);

            AxisXKE = new LinearAxis();
            AxisXKE.Position = AxisPosition.Bottom;
            AxisXKE.Title = "Time";
            PlotModelKE.Axes.Add(AxisXKE);

            AxisYKE = new LinearAxis();
            AxisYKE.Position = AxisPosition.Left;
            AxisYKE.Title = "Kinetic energy";
            PlotModelKE.Axes.Add(AxisYKE);
        }
    

        public void UpdateFPS()
        {
            try
            {
                //string filename = "C:\\Users\\Kamil\\Source\\Repos\\SphDesigner\\SPH\\Smoothie\\bin\\Release\\framesPerSecond.out";
                string filename = "framesPerSecond.out";

                StreamReader streamReader = new StreamReader(filename);

                LineSeriesFPS.Points.Clear();
                while (true)
                {
                    string line = streamReader.ReadLine();
                    if (line == null)
                    {
                        break;
                    }
                    else
                    {
                        string[] splitedLine = line.Split();
                        double time = Convert.ToDouble(splitedLine[0], CultureInfo.InvariantCulture);
                        double value = Convert.ToDouble(splitedLine[2], CultureInfo.InvariantCulture);
                        LineSeriesFPS.Points.Add(new DataPoint(time, value));
                    }
                }

                streamReader.Close();
            }
            catch (System.IO.IOException e)
            {

            }
        }

        public void UpdateADT()
        {
            try
            {
                //string filename = "C:\\Users\\Kamil\\Source\\Repos\\SphDesigner\\SPH\\Smoothie\\bin\\Release\\averageDT.out";
                string filename = "averageDT.out";

                StreamReader streamReader = new StreamReader(filename);

                LineSeriesADT.Points.Clear();
                while (true)
                {
                    string line = streamReader.ReadLine();
                    if (line == null)
                    {
                        break;
                    }
                    else
                    {
                        string[] splitedLine = line.Split();
                        double time = Convert.ToDouble(splitedLine[0], CultureInfo.InvariantCulture);
                        double value = Convert.ToDouble(splitedLine[2], CultureInfo.InvariantCulture);
                        LineSeriesADT.Points.Add(new DataPoint(time, value));
                    }
                }

                streamReader.Close();
            }
            catch (System.FormatException e)
            {

            }
            catch (System.IO.IOException e)
            {

            }
        }

        public void UpdateKE()
        {
            try
            {
                //string filename = "C:\\Users\\Kamil\\Source\\Repos\\SphDesigner\\SPH\\Smoothie\\bin\\Release\\kineticEnergy.out";
                string filename = "kineticEnergy.out";

                StreamReader streamReader = new StreamReader(filename);

                LineSeriesKE.Points.Clear();
                while (true)
                {
                    string line = streamReader.ReadLine();
                    if (line == null)
                    {
                        break;
                    }
                    else
                    {
                        string[] splitedLine = line.Split();
                        double time = Convert.ToDouble(splitedLine[0], CultureInfo.InvariantCulture);
                        double value = Convert.ToDouble(splitedLine[2], CultureInfo.InvariantCulture);
                        LineSeriesKE.Points.Add(new DataPoint(time, value));
                    }
                }

                streamReader.Close();
            }
            catch (System.IO.IOException e)
            {

            }
        }
    }

}
