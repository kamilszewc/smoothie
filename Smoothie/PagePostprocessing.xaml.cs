using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Globalization;
using Sph;
using Postprocessing;

namespace Smoothie
{
    /// <summary>
    /// Interaction logic for PagePostprocessing.xaml
    /// </summary>
    public partial class PagePostprocessing : Page
    {
        private string _filename;
        private Domain _domain;
        private PostProcessor _postProcessor;
        private string _currentField = "velocity";

        private PlotModelPostProcessing _plotModel;

        public PagePostprocessing()
        {
            _plotModel = new PlotModelPostProcessing();
            DataContext = _plotModel;
            InitializeComponent();
        }

        private void ButtonPostProcessingOpenFile_Clicked(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
            dialog.DefaultExt = ".xml";
            dialog.Filter = "Smoothie XML document|*.xml";

            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                _filename = dialog.FileName.Replace("\\", "\\\\");
                _domain = new Domain(_filename);
                _postProcessor = new PostProcessor(_domain);

                LabelFileName.Text = dialog.FileName;

                DoubleUpDownLBX.Value = 0.0;
                DoubleUpDownLBY.Value = 0.0;
                TextBlockLTX.Text = "0.0";
                DoubleUpDownLTY.Value = _domain["YCV"];
                DoubleUpDownLTY.Maximum = _domain["YCV"];
                DoubleUpDownRBX.Value = _domain["XCV"];
                DoubleUpDownRBX.Maximum = _domain["XCV"];
                TextBlockRBY.Text = "0.0";

                IntegerUpDownNX.Value = _domain["NCX"] * 2;
                IntegerUpDownNY.Value = _domain["NCY"] * 2;

                ButtonXVelocity.IsEnabled = true;
                ButtonYVelocity.IsEnabled = true;
                ButtonVelocity.IsEnabled = true;
                ButtonDensity.IsEnabled = true;
                ButtonPressure.IsEnabled = true;

                if (_domain["T_HEAT_TRANSFER"] > 0) { ButtonTemperature.IsEnabled = true; }
                else { ButtonTemperature.IsEnabled = false; }

                ButtonParticlesPosition.IsEnabled = true;

                ButtonUpdatePlot.IsEnabled = true;
                ButtonSetDefaultValues.IsEnabled = true;

                IntegerUpDownNX.IsEnabled = true;
                IntegerUpDownNY.IsEnabled = true;
                DoubleUpDownLBX.IsEnabled = true;
                DoubleUpDownLBY.IsEnabled = true;
                DoubleUpDownLTY.IsEnabled = true;
                DoubleUpDownRBX.IsEnabled = true;

            }
        }

        private void DoubleUpDownLBX_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                TextBlockLTX.Text = Convert.ToString(DoubleUpDownLBX.Value, CultureInfo.InvariantCulture);
            }
            catch (System.NullReferenceException ex)
            { }
        }

        private void DoubleUpDownLBY_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                TextBlockRBY.Text = Convert.ToString(DoubleUpDownLBY.Value, CultureInfo.InvariantCulture);
            }
            catch (System.NullReferenceException ex)
            { }
        }

        private void ButtonXVelocity_Click(object sender, RoutedEventArgs e)
        {
            _currentField = "x-velocity";
            UpdatePlot();
        }

        private void ButtonYVelocity_Click(object sender, RoutedEventArgs e)
        {
            _currentField = "y-velocity";
            UpdatePlot();
        }

        private void ButtonVelocity_Click(object sender, RoutedEventArgs e)
        {
            _currentField = "velocity";
            UpdatePlot();
        }

        private void ButtonDensity_Click(object sender, RoutedEventArgs e)
        {
            _currentField = "density";
            UpdatePlot();
        }

        private void ButtonPressure_Click(object sender, RoutedEventArgs e)
        {
            _currentField = "pressure";
            UpdatePlot();
        }

        private void ButtonTemperature_Click(object sender, RoutedEventArgs e)
        {
            _currentField = "temperature";
            UpdatePlot();
        }

        private void ButtonParticlesPosition_Click(object sender, RoutedEventArgs e)
        {
            _currentField = "particles_position";
            UpdatePlot();
        }

        private void ButtonUpdatePlot_Click(object sender, RoutedEventArgs e)
        {
            UpdatePlot();
        }

        private void ButtonSetDefaultValues_Click(object sender, RoutedEventArgs e)
        {
            DoubleUpDownLBX.Value = 0.0;
            DoubleUpDownLBY.Value = 0.0;
            TextBlockLTX.Text = "0.0";
            DoubleUpDownLTY.Value = _domain["YCV"];
            DoubleUpDownRBX.Value = _domain["XCV"];
            TextBlockRBY.Text = "0.0";

            IntegerUpDownNX.Value = _domain["NCX"] * 2;
            IntegerUpDownNY.Value = _domain["NCY"] * 2;
        }

        

        private void UpdatePlot()
        {
            double x0 = (double)DoubleUpDownLBX.Value;
            double y0 = (double)DoubleUpDownLBY.Value;
            double x1 = (double)DoubleUpDownRBX.Value;
            double y1 = (double)DoubleUpDownLTY.Value;

            if (_currentField != "particles_position")
            {
                int nx = (int)IntegerUpDownNX.Value;
                int ny = (int)IntegerUpDownNY.Value;

                double dx = (x1 - x0) / nx;
                double dy = (y1 - y0) / ny;

                var values = new Double[nx, ny];
                //var positions = new object[nx, ny];

                
                for (int i = 0; i < nx; i++)
                {
                    for (int j = 0; j < ny; j++)
                    {
                        double x = x0 + 0.5 * dx + i * dx;
                        double y = y0 + 0.5 * dy + j * dy;
                        Position position = new Position(x, y);

                        if (_currentField == "velocity")
                        {
                            double u = _postProcessor.InterpolateAtPosition(position, "x-velocity");
                            double v = _postProcessor.InterpolateAtPosition(position, "y-velocity");
                            values[i, j] = Math.Sqrt(u * u + v * v);
                        }
                        else
                        {
                            //positions[i, j] = position;
                            values[i, j] = _postProcessor.InterpolateAtPosition(position, _currentField);
                        }
                    }

                }

                //values = _postProcessor.InterpolateAtPosition(positions, _currentField);

                _plotModel.UpdateMap(_currentField, values, x0, x1, y0, y1);
            }
            else
            {
                _plotModel.UpdateParticles(_currentField, _domain, x0, x1, y0, y1);
            }

            if (_domain["XCV"] > _domain["YCV"])
            {
                PlotPostProcessing.Width = 600;
                PlotPostProcessing.Height = Convert.ToInt32(600.0 * _domain["YCV"] / _domain["XCV"]);
            }
            else
            {
                PlotPostProcessing.Height = 600;
                PlotPostProcessing.Width = Convert.ToInt32(600.0 * _domain["XCV"] / _domain["YCV"]);
            }
            PlotPostProcessing.InvalidateFlag += 1;
        }
    }
}
