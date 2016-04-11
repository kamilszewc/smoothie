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
using System.Diagnostics;
using System.Windows.Threading;
using System.IO;
using System.Globalization;

namespace Smoothie
{
    /// <summary>
    /// Interaction logic for PageSimulation.xaml
    /// </summary>
    public partial class PageSimulation : Page
    {
        private string _filename = "";
        private string _outputDirectory;
        private Process _process;
        private bool _isSimulationStarted = false;

        DispatcherTimer _timer;

        private SimulationPlotModels _plotModels;

        public PageSimulation()
        {
            _plotModels = new SimulationPlotModels();
            DataContext = _plotModels;
            InitializeComponent();

            System.Windows.Application.Current.Exit += new ExitEventHandler(OnExit);

            SetComboBoxNumberOfThreads();

            _timer = new DispatcherTimer();
            _timer.Tick += new EventHandler(timer_Tick);
            _timer.Interval = new TimeSpan(0, 0, 1);

            _process = new Process();
        }

        public PageSimulation(string filename, string outputDirectory)
            : this()
        {
            _filename = filename;
            LabelSimulationOpenFile.Text = _filename;

            _outputDirectory = "";
            string[] splitedFilename = _filename.Split('\\');
            for (int i = 0; i < splitedFilename.Length - 1; i++)
            {
                _outputDirectory += splitedFilename[i] + "\\";
            }
            LabelSimulationOutputDirectory.Text = _outputDirectory;
        }

        private void SetComboBoxNumberOfThreads()
        {
            Smoothie.Informations.Hardware hardware = new Smoothie.Informations.Hardware();
            int maxNumberOfThreads = hardware.GetTotalNumberOfLogicalProcessors();
            ItemCollection itemCollection = ComboBoxNumberOfThreads.Items;
            itemCollection.Clear();

            for (int i=1; i<=maxNumberOfThreads; i++)
            {
                ComboBoxItem comboBoxItem = new ComboBoxItem();
                comboBoxItem.Content = i.ToString();
                itemCollection.Add(comboBoxItem);
            }
            ComboBoxNumberOfThreads.SelectedIndex = maxNumberOfThreads-1;
        }

        private void ComboBoxNumberOfThreads_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem selectedItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                int numberOfThreads = Convert.ToInt32(selectedItem.Content);
                System.IO.File.WriteAllText(".numberOfThreads", numberOfThreads.ToString());
            }
            catch
            { }
        }

        private void ButtonSimulationOpenFile_Clicked(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
            dialog.DefaultExt = ".xml";
            dialog.Filter = "Smoothie XML document|*.xml";

            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                _filename = dialog.FileName;
                LabelSimulationOpenFile.Text = _filename;

                _outputDirectory = "";
                string[] splitedFilename = _filename.Split('\\');
                for (int i=0; i<splitedFilename.Length-1; i++)
                {
                    _outputDirectory += splitedFilename[i] + "\\";
                }
                LabelSimulationOutputDirectory.Text = _outputDirectory;

            }
        }

        private void ButtonSimulationOutputDirectory_Clicked(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.SelectedPath = "C:\\";

            System.Windows.Forms.DialogResult result = dialog.ShowDialog();
            if (result.ToString() == "OK")
            {
                _outputDirectory = dialog.SelectedPath;
                LabelSimulationOutputDirectory.Text = dialog.SelectedPath;
            }
        }

        private void ButtonSimulationStartSimulation_Clicked(object sender, RoutedEventArgs e)
        {
            if (_filename != "")
            {
                if ((_isSimulationStarted == false) || ((_isSimulationStarted == true) && (_process.HasExited)))
                {
                    _process = new Process();
                    try
                    {
                        //MessageBox.Show(_filename + " " + _outputDirectory);
                        //_process.StartInfo.FileName = "C:\\Users\\Kamil\\Source\\Repos\\SphDesigner\\SPH\\Release\\cSPH2d.exe";
                        _process.StartInfo.FileName = "cSPH2d.exe";
                        _process.StartInfo.Arguments = '"' + _filename.Replace("\\", "\\\\") + '"' + " " + '"' + _outputDirectory.Replace("\\", "\\\\") + '"';
                        //_process.Start("C:\\Users\\Kamil\\Source\\Repos\\SphDesigner\\SPH\\Release\\cSPH2d.exe");
                        _process.StartInfo.UseShellExecute = false;
                        _process.StartInfo.CreateNoWindow = true;
                        _process.Start();
                        _isSimulationStarted = true;

                        _timer.Start();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.Message);
                    }
                }
            }
            else
            {
                MessageBox.Show("File is not choosen");
            }
        }

        private void ButtonSimulationStopSimulation_Clicked(object sender, RoutedEventArgs e)
        {
            if ( (_isSimulationStarted == true) && (!_process.HasExited) )
            {
                _timer.Stop();
                _process.Kill();
                _isSimulationStarted = false;
                StartButton.IsEnabled = true;
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            if (_process.HasExited)
            {
                StartButton.IsEnabled = true;
            }
            else
            {
                StartButton.IsEnabled = false;
            }

            _plotModels.UpdateFPS();
            PlotFPS.InvalidateFlag += 1;

            _plotModels.UpdateADT();
            PlotADT.InvalidateFlag += 1;

            _plotModels.UpdateKE();
            PlotKE.InvalidateFlag += 1;


            //string filename = "C:\\Users\\Kamil\\Source\\Repos\\SphDesigner\\SPH\\Smoothie\\bin\\Release\\timeToEnd.out";
            string filename = "timeToEnd.out";
            StreamReader streamReader = new StreamReader(filename);
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
                    double elapsedTime = Convert.ToDouble(splitedLine[1], CultureInfo.InvariantCulture);
                    double timeToEnd = Convert.ToDouble(splitedLine[2], CultureInfo.InvariantCulture);
                    double totalTime = Convert.ToDouble(splitedLine[3], CultureInfo.InvariantCulture);

                    LabelElapsedTime.Content = timeToString(elapsedTime);
                    LabelTotalTime.Content = timeToString(totalTime);
                    LabelTimeToEnd.Content = timeToString(timeToEnd);
                }
            }
            streamReader.Close();
        }

        private string timeToString(double time)
        {
            string hour = Convert.ToString( (int)(time) / 3600, CultureInfo.InvariantCulture );
            string minutes = Convert.ToString( ((int)(time) % 3600) / 60 , CultureInfo.InvariantCulture);
            string seconds = Convert.ToString( ((int)(time) % 3600) % 60, CultureInfo.InvariantCulture);

            return hour + "h" + minutes + "m" + seconds + "s";
        }

        private void OnExit(object sender, ExitEventArgs e)
        {
            if (_isSimulationStarted == true)
            {
                try
                {
                    _timer.Stop();
                    _process.Kill();
                    _isSimulationStarted = false;
                }
                catch
                { }
            }
        }
    }
}
