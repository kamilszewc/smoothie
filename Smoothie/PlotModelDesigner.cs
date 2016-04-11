using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Media;
using OxyPlot;
using OxyPlot.Series;
using OxyPlot.Axes;
using Sph;

namespace Smoothie
{
    public class PlotModelDesigner
    {
        private PlotModel plotModel;

        public PlotModel PlotModel
        {
            get { return plotModel; }
            set { plotModel = value;}
        }

        public LinearAxis AxisX { get; set; }

        public LinearAxis AxisY { get; set; }

        public ScatterSeries ScatterSeries { get; set; }

        public PlotModelDesigner()
        {
            plotModel = new PlotModel();
            plotModel.PlotAreaBackground = OxyColors.Bisque;//OxyColors.AliceBlue;
            plotModel.PlotAreaBorderColor = OxyColors.Transparent;
            //plotModel.Title = "Title";

            ScatterSeries = new ScatterSeries();
            ScatterSeries.MarkerSize = 1.5;
            ScatterSeries.MarkerType = MarkerType.Circle;
            ScatterSeries.MarkerFill = OxyColors.Black;
            plotModel.Series.Add(ScatterSeries);

            
            AxisX = new LinearAxis();
            AxisX.Minimum = 0.0;
            AxisX.Maximum = 1.0;
            AxisX.TickStyle = TickStyle.None;
            AxisX.Position = AxisPosition.Bottom;
            AxisX.IsZoomEnabled = false;
            plotModel.Axes.Add(AxisX);

            AxisY = new LinearAxis();
            AxisY.Minimum = 0.0;
            AxisY.Maximum = 1.0;
            AxisY.TickStyle = TickStyle.None;
            AxisY.Position = AxisPosition.Left;
            AxisY.IsZoomEnabled = false;
            plotModel.Axes.Add(AxisY);
        }

        public void UpdateModel(double xcv, double ycv, List<Position> particlePositions, Color color, bool isFullResolution)
        {
            AxisX.Minimum = 0.0;
            AxisX.Maximum = xcv;

            AxisY.Minimum = 0.0;
            AxisY.Maximum = ycv;

            OxyColor oxyColor = OxyColor.FromArgb(color.A, color.R, color.G, color.B);

            ScatterSeries.MarkerFill = oxyColor;
            if (isFullResolution == false)
            {
                ScatterSeries.MarkerType = MarkerType.Square;
                ScatterSeries.MarkerStroke = oxyColor;
                ScatterSeries.MarkerSize = 2.0;
            }
            else
            {
                ScatterSeries.MarkerType = MarkerType.Circle;
                ScatterSeries.MarkerStroke = OxyColors.Transparent;
                ScatterSeries.MarkerSize = 1.5;
            }

            ScatterSeries.Points.Clear();
            foreach (Position position in particlePositions)
            {
                ScatterSeries.Points.Add(new DataPoint(position.X, position.Y));
            }
        }

        
    }
}
