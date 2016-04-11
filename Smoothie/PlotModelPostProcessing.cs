using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OxyPlot;
using OxyPlot.Series;
using OxyPlot.Axes;
using Sph;

namespace Smoothie
{
    class PlotModelPostProcessing
    {
        public PlotModel PlotModelPP { set; get; }
        private LinearAxis linearAxisX { set; get; }
        private LinearAxis linearAxisY { set; get; }
        private HeatMapSeries heatMapSeries { set; get; }

        public PlotModelPostProcessing()
        {
            PlotModelPP = new PlotModel();
            
            //var linearColorAxis = new LinearColorAxis();
            //linearColorAxis.HighColor = OxyColors.Gray;
            //linearColorAxis.LowColor = OxyColors.Black;
            //linearColorAxis.Position = AxisPosition.Right;
            //PlotModelPP.Axes.Add(linearColorAxis);

            linearAxisX = new LinearAxis();
            linearAxisX.Position = AxisPosition.Bottom;
            linearAxisX.Minimum = 0.0;
            linearAxisX.Maximum = 1.0;
            PlotModelPP.Axes.Add(linearAxisX);

            linearAxisY = new LinearAxis();
            linearAxisY.Position = AxisPosition.Left;
            linearAxisY.Minimum = 0.0;
            linearAxisY.Maximum = 1.0;
            PlotModelPP.Axes.Add(linearAxisY);

            //heatMapSeries = new HeatMapSeries();
            //heatMapSeries.X0 = 0.0;
            //heatMapSeries.Y0 = 0.0;
            //heatMapSeries.X1 = 1.0;
            //heatMapSeries.Y1 = 1.0;
            //heatMapSeries.Interpolate = true;
            //heatMapSeries.Data = new Double[2,2];
            //heatMapSeries.Data[0, 0] = 0.0;
            //heatMapSeries.Data[0, 1] = 0.0;
            //heatMapSeries.Data[1, 0] = 0.0;
            //heatMapSeries.Data[1, 1] = 0.0;
            //PlotModelPP.Series.Add(heatMapSeries);
        }

        public void UpdateMap(string field, double[,] values, double x0, double x1, double y0, double y1)
        {
            PlotModelPP.Title = field;

            heatMapSeries = new HeatMapSeries();
            heatMapSeries.Interpolate = true;
            heatMapSeries.Data = values;
            heatMapSeries.X0 = x0;
            heatMapSeries.X1 = x1;
            heatMapSeries.Y0 = y0;
            heatMapSeries.Y1 = y1;
            PlotModelPP.Series.Clear();
            PlotModelPP.Series.Add(heatMapSeries);

            PlotModelPP.Axes.Clear();

            var linearColorAxis = new LinearColorAxis();
            linearColorAxis.HighColor = OxyColors.Gray;
            linearColorAxis.LowColor = OxyColors.Black;
            linearColorAxis.Position = AxisPosition.Right;
            PlotModelPP.Axes.Add(linearColorAxis);

            linearAxisX = new LinearAxis();
            linearAxisX.Position = AxisPosition.Bottom;
            linearAxisX.Minimum = x0;
            linearAxisX.Maximum = x1;
            PlotModelPP.Axes.Add(linearAxisX);

            linearAxisY = new LinearAxis();
            linearAxisY.Position = AxisPosition.Left;
            linearAxisY.Minimum = y0;
            linearAxisY.Maximum = y1;
            PlotModelPP.Axes.Add(linearAxisY);
        }

        public void UpdateParticles(string field, Domain domain, double x0, double x1, double y0, double y1)
        {
            List<Phase> phases = domain.GetPhases();


            PlotModelPP.Series.Clear();
            PlotModelPP.Axes.Clear();
            PlotModelPP.Title = field;

            
            foreach (Phase phase in phases)
            {
                List<Particle> particles = phase.Particles;
                byte[] color = phase.Color;

                ScatterSeries scatterSeries = new ScatterSeries();
                scatterSeries.MarkerSize = 1.5;
                scatterSeries.MarkerType = MarkerType.Circle;
                scatterSeries.MarkerFill = OxyColor.FromArgb(color[0], color[1], color[2], color[3]);

                foreach (Particle particle in particles)
                {
                    scatterSeries.Points.Add(new DataPoint(particle.X, particle.Y));
                }

                PlotModelPP.Series.Add(scatterSeries);
            }
            

            linearAxisX = new LinearAxis();
            linearAxisX.Position = AxisPosition.Bottom;
            linearAxisX.Minimum = x0;
            linearAxisX.Maximum = x1;
            PlotModelPP.Axes.Add(linearAxisX);

            linearAxisY = new LinearAxis();
            linearAxisY.Position = AxisPosition.Left;
            linearAxisY.Minimum = y0;
            linearAxisY.Maximum = y1;
            PlotModelPP.Axes.Add(linearAxisY);
        }
    }
}
