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
using System.Drawing;
using System.Globalization;
using OxyPlot;
using OxyPlot.Series;
using Sph;

namespace Smoothie
{
    [Serializable()]
    public class InvalidName : System.Exception
    {
        public InvalidName() : base() { }
        public InvalidName(string message) : base(message) { }
        public InvalidName(string message, System.Exception inner) : base(message, inner) { }
        protected InvalidName(System.Runtime.Serialization.SerializationInfo info,
        System.Runtime.Serialization.StreamingContext context) { }
    }


    public delegate void DataChangedHandler(object sender, EventArgs args);

    /// <summary>
    /// Interaction logic for PageDesigner.xaml
    /// </summary>
    public partial class PageDesigner : Page
    {
        private string _filename;
        private Domain _domain;
        private int _defaultPhaseNoIndicator = 0;
        private string _selectedPhaseName;
        private int _selectedPhaseId;

        public event DataChangedHandler DataChangedEvent;

        private PlotModelDesigner _plotModel;

        public PageDesigner()
        {
            _plotModel = new PlotModelDesigner();
            DataContext = _plotModel;
            InitializeComponent();
            SetupNewDomain();

            //_plotModel.UpdateModel();
        }

        private void ButtonDesignerNewFile_Clicked(object sender, RoutedEventArgs e)
        {
            SetupNewDomain();
        }

        private void SetupNewDomain()
        {
            ListBoxDesignerPhases.Items.Clear();
            _domain = new Domain();
            _filename = "";
            LabelFileName.Text = _filename;
            DoubleUpDownDomainSizeX.Value = 1.0;
            DoubleUpDownDomainSizeY.Value = 1.0;
            DoubleUpDownDomainSizeZ.Value = 1.0;
            IntegerUpDownNumOfCellsX.Value = 16;
            IntegerUpDownNumOfCellsY.Value = 16;
            IntegerUpDownNumOfCellsZ.Value = 16;
            ComboBoxDesignerDomainBoundaries.SelectedIndex = 1; _domain.SetParameter("T_BOUNDARY_CONDITION", 1);
            ComboBoxDesignerDomainPeriodicity.SelectedIndex = 0; _domain.SetParameter("T_BOUNDARY_PERIODICITY", 0);
            ComboBoxDesignerFormalizm.SelectedIndex = 0; _domain.SetParameter("T_MODEL", 0);
            ComboBoxDesignerSurfaceTension.SelectedIndex = 0; _domain.SetParameter("T_SURFACE_TENSION", 0);
            ComboBoxDesignerInterfaceCorrection.SelectedIndex = 0; _domain.SetParameter("T_INTERFACE_CORRECTION", 0);
            ComboBoxDesignerHeatTransfer.SelectedIndex = 0; _domain.SetParameter("T_HEAT_TRANSFER", 0);
            DoubleUpDownDomainGravityX.Value = 0.0; _domain.SetParameter("G_X", 0.0);
            DoubleUpDownDomainGravityY.Value = 0.0; _domain.SetParameter("G_Y", 0.0);
            DoubleUpDownDomainGravityZ.Value = 0.0; _domain.SetParameter("G_Z", 0.0);
            ComboBoxDesignerHydrostaticPressure.SelectedIndex = 0; _domain.SetParameter("T_HYDROSTATIC_PRESSURE", 0);
            ComboBoxDesignerTimeStepType.SelectedIndex = 0; _domain.SetParameter("T_TIME_STEP", 0);
            DoubleUpDownDomainTimeStep.Value = 0.0001; _domain.SetParameter("DT", 0.0001);
            DoubleUpDownDomainSimulationTime.Value = 1.0; _domain.SetParameter("END_TIME", 1.0);
            DoubleUpDownDomainResultSaveInterval.Value = 0.01; _domain.SetParameter("INTERVAL_TIME", 0.01);
            DoubleUpDownDomainHDR.Value = 2.0; _domain.HDR = 2.0;

            //GhostParticleSetup
            _domain.SetParameter("V_N", 0.0);
            _domain.SetParameter("V_E", 0.0);
            _domain.SetParameter("V_S", 0.0);
            _domain.SetParameter("V_W", 0.0);
            _domain.SetParameter("T_TEMP_N", 1);
            _domain.SetParameter("T_TEMP_E", 1);
            _domain.SetParameter("T_TEMP_S", 1);
            _domain.SetParameter("T_TEMP_W", 1);
            _domain.SetParameter("TEMP_N", 0.0);
            _domain.SetParameter("TEMP_E", 0.0);
            _domain.SetParameter("TEMP_S", 0.0);
            _domain.SetParameter("TEMP_W", 0.0);

            _domain.AddInterPhaseParameter("surface-tension-coefficient");
            _domain.AddInterPhaseParameter("interface-correction-coefficient");

            _domain.GenerateParticles();
            UpdatePlot();
        }

        private void ButtonDesignerOpenFile_Clicked(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
            dialog.DefaultExt = ".xml";
            dialog.Filter = "Smoothie XML document|*.xml";

            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                ListBoxDesignerPhases.Items.Clear();
                //_domain.GenerateParticles();
                _filename = dialog.FileName;
                _domain = new Domain(_filename);

                LabelFileName.Text = _filename;
                DoubleUpDownDomainSizeX.Value = _domain["XCV"];
                DoubleUpDownDomainSizeY.Value = _domain["YCV"];
                //DoubleUpDownDomainSizeZ.Value = _domain["ZCV"];
                IntegerUpDownNumOfCellsX.Value = _domain["NCX"];
                IntegerUpDownNumOfCellsY.Value = _domain["NCY"];
                //IntegerUpDownNumOfCellsZ.Value = _domain["NCZ"];
                ComboBoxDesignerDomainBoundaries.SelectedIndex = _domain["T_BOUNDARY_CONDITION"];
                ComboBoxDesignerDomainPeriodicity.SelectedIndex = _domain["T_BOUNDARY_PERIODICITY"];
                ComboBoxDesignerFormalizm.SelectedIndex = _domain["T_MODEL"];
                ComboBoxDesignerSurfaceTension.SelectedIndex = _domain["T_SURFACE_TENSION"];
                ComboBoxDesignerInterfaceCorrection.SelectedIndex = _domain["T_INTERFACE_CORRECTION"];
                ComboBoxDesignerHeatTransfer.SelectedIndex = _domain["T_HEAT_TRANSFER"];
                DoubleUpDownDomainGravityX.Value = _domain["G_X"];
                DoubleUpDownDomainGravityY.Value = _domain["G_Y"];
                //DoubleUpDownDomainGravityZ.Value = 0.0; _domain.SetParameter("G_Z", 0.0);
                ComboBoxDesignerHydrostaticPressure.SelectedIndex = _domain["T_HYDROSTATIC_PRESSURE"];
                ComboBoxDesignerTimeStepType.SelectedIndex = _domain["T_TIME_STEP"];
                DoubleUpDownDomainTimeStep.Value = _domain["DT"];
                DoubleUpDownDomainSimulationTime.Value = _domain["END_TIME"];
                DoubleUpDownDomainResultSaveInterval.Value = _domain["INTERVAL_TIME"];
                DoubleUpDownDomainHDR.Value = _domain.HDR;

                //ListBoxDesignerPhases.Items.Clear();
                foreach (Phase phase in _domain.GetPhases())
                {
                    TextBlock textBlock = new TextBlock();
                    textBlock.Text = phase.Name;
                    ListBoxDesignerPhases.Items.Add(textBlock);
                    _defaultPhaseNoIndicator++;
                }

                

                UpdatePlot();
            }
        }

        private void ButtonDesignerSaveFileAs_Clicked(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.SaveFileDialog dialog = new Microsoft.Win32.SaveFileDialog();
            dialog.DefaultExt = ".xml";
            dialog.Filter = "Smoothie XML document|*.xml";

            Nullable<bool> result = dialog.ShowDialog();

            if (result == true)
            {
                _domain.GenerateParticles();
                _filename = dialog.FileName;
                _domain.WriteToXml(_filename);
                LabelFileName.Text = _filename;
                PhasesOverlapingInfo();
            }
        }

        private void ButtonDesignerSaveFile_Clicked(object sender, RoutedEventArgs e)
        {
            if (_filename == "")
            {
                Microsoft.Win32.SaveFileDialog dialog = new Microsoft.Win32.SaveFileDialog();
                dialog.DefaultExt = ".xml";
                dialog.Filter = "Smoothie XML document|*.xml";

                Nullable<bool> result = dialog.ShowDialog();

                if (result == true)
                {
                    _domain.GenerateParticles();
                    _filename = dialog.FileName;
                    _domain.WriteToXml(_filename);
                    LabelFileName.Text = _filename;
                    PhasesOverlapingInfo();
                }
            }
            else
            {
                _domain.GenerateParticles();
                _domain.WriteToXml(_filename);
                PhasesOverlapingInfo();
            }
        }

        private void PhasesOverlapingInfo()
        {
            List<Position> positions = _domain.GetParticlePositions();
            foreach (Position position in positions)
            {
                int indicator = 0;
                List<Phase> phases = _domain.GetPhases();
                foreach (Phase phase in phases)
                {
                    if (phase.HasPosition(position))
                    {
                        indicator++;
                    }
                }
                if (indicator > 1)
                {
                    MessageBox.Show("Created phases overlap! Correct it if this is unintentional.", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
                    break;
                }
            }
        }

        private void ComboBoxDesignerDimensions_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBoxItem choosenComboBox = (sender as ComboBox).SelectedItem as ComboBoxItem;
            //MessageBox.Show(choosenComboBox.Content.ToString());
            //string text = ComboBoxDesignerDimensions.SelectedValue.ToString();
            //TextBoxCanvasOutput.Text = "poko";
        }

        private void ButtonDesignerValidation_Clicked(object sender, RoutedEventArgs e)
        {
            double domainSizeX = Convert.ToDouble(DoubleUpDownDomainSizeX.Value);
            double domainSizeY = Convert.ToDouble(DoubleUpDownDomainSizeY.Value);
            double domainSizeZ = Convert.ToDouble(DoubleUpDownDomainSizeZ.Value);
            int numberOfCellsX = Convert.ToInt32(IntegerUpDownNumOfCellsX.Value);
            int numberOfCellsY = Convert.ToInt32(IntegerUpDownNumOfCellsY.Value);
            int numberOfCellsZ = Convert.ToInt32(IntegerUpDownNumOfCellsZ.Value);
            try
            {
                _domain.SetSize(domainSizeX, domainSizeY, numberOfCellsX, numberOfCellsY);
                ButtonDesignerValidation.IsEnabled = false;
                UpdatePlot();

                TextBlockH.Text = "H=" + Convert.ToString(_domain.H, CultureInfo.InvariantCulture);
                TextBlockN.Text = "N=" + Convert.ToString(_domain.GetParticlePositions().Count);
            }
            catch (InvalidAspectRatio ex)
            {
                MessageBox.Show("Invalid aspect ratio.");
            }
        }

       

        private void DomainValues_Changed(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                ButtonDesignerValidation.IsEnabled = true;
            }
            catch
            { }
        }
        
        

        private void ComboBoxDesignerDomainBoundaries_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem domainBoundaries = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (domainBoundaries.Content.ToString())
                {
                    case "None":
                        _domain.SetParameter("T_BOUNDARY_CONDITION", 0);
                        ButtonDesignerGhostParticleSetup.IsEnabled = false;
                        break;
                    case "Ghost particles":
                        _domain.SetParameter("T_BOUNDARY_CONDITION", 1);
                        ButtonDesignerGhostParticleSetup.IsEnabled = true;
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerDomainPeriodicity_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem domainPeriodicity = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (domainPeriodicity.Content.ToString())
                {
                    case "None":
                        _domain.SetParameter("T_BOUNDARY_PERIODICITY", 0);
                        break;
                    case "Left-right, up-down":
                        _domain.SetParameter("T_BOUNDARY_PERIODICITY", 1);
                        break;
                    case "Left-right":
                        _domain.SetParameter("T_BOUNDARY_PERIODICITY", 2);
                        break;
                }
            }
            catch
            { }
        }

        private void ButtonDesignerGhostParticleSetup_OnClick(object sender, RoutedEventArgs e)
        {
            if (GhostParticleSetupWindow.IsInitialized() == false)
            {
                var window = new GhostParticleSetupWindow(_domain);
                window.Show();
            }
        }

        private void ComboBoxDesignerFormalizm_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem formalizm = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (formalizm.Content.ToString())
                {
                    case "Standard":
                        _domain.SetParameter("T_MODEL", 0);
                        break;
                    case "Colagrossi and Landrini (2003)":
                        _domain.SetParameter("T_MODEL", 1);
                        break;
                    case "Hu and Adams (2006)":
                        _domain.SetParameter("T_MODEL", 2);
                        break;
                    case "Szewc and Olejnik (2015)":
                        _domain.SetParameter("T_MODEL", 3);
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerSurfaceTension_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem surfaceTension = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (surfaceTension.Content.ToString())
                {
                    case "None":
                        _domain.SetParameter("T_SURFACE_TENSION", 0);
                        ButtonDesignerSurfaceTension.IsEnabled = false;
                        break;
                    case "CSF Monaghan (1995)":
                        _domain.SetParameter("T_SURFACE_TENSION", 1);
                        ButtonDesignerSurfaceTension.IsEnabled = true;
                        break;
                    case "CSF Morris (2000)":
                        _domain.SetParameter("T_SURFACE_TENSION", 2);
                        ButtonDesignerSurfaceTension.IsEnabled = true;
                        break;
                    case "SSF Hu and Adams (2006)":
                        _domain.SetParameter("T_SURFACE_TENSION", 3);
                        ButtonDesignerSurfaceTension.IsEnabled = true;
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerInterfaceCorrection_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem surfaceTension = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (surfaceTension.Content.ToString())
                {
                    case "None":
                        _domain.SetParameter("T_INTERFACE_CORRECTION", 0);
                        ButtonDesignerInterfaceCorrection.IsEnabled = false;
                        break;
                    case "Szewc et al. (2014)":
                        _domain.SetParameter("T_INTERFACE_CORRECTION", 1);
                        ButtonDesignerInterfaceCorrection.IsEnabled = true;
                        break;
                }
            }
            catch
            { }
        }
        
        private void ComboBoxDesignerHeatTransfer_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem heatTransfer = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (heatTransfer.Content.ToString())
                {
                    case "None":
                        _domain.SetParameter("T_HEAT_TRANSFER", 0);
                        break;
                    case "Cleary (1998)":
                        _domain.SetParameter("T_HEAT_TRANSFER", 1);
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerHydrostaticPressure_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem hydrostaticPressure = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (hydrostaticPressure.Content.ToString())
                {
                    case "None":
                        _domain.SetParameter("T_HYDROSTATIC_PRESSURE", 0);
                        break;
                    case "Szewc et al. (2011)":
                        _domain.SetParameter("T_HYDROSTATIC_PRESSURE", 1);
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerTimeStepType_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem timeStep = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (timeStep.Content.ToString())
                {
                    case "Static":
                        _domain.SetParameter("T_TIME_STEP", 0);
                        DoubleUpDownDomainTimeStep.IsEnabled = true;
                        break;
                    case "Dynamic":
                        _domain.SetParameter("T_TIME_STEP", 1);
                        DoubleUpDownDomainTimeStep.IsEnabled = false;
                        break;
                }
            }
            catch
            { }
        }

        private void ButtonDesignerAddPhase_Clicked(object sender, RoutedEventArgs e)
        {
            string phaseName = "New phase " + Convert.ToString(_defaultPhaseNoIndicator);
            //ListBoxDesignerPhases.Items.Add(phaseName);
            TextBlock textBlock = new TextBlock();
            textBlock.Text = phaseName;
            ListBoxDesignerPhases.Items.Add(textBlock);
            Phase phase = _domain.AddPhase(phaseName);
            phase.AddField("x-velocity", "double", "0.0");
            phase.AddField("y-velocity", "double", "0.0");
            phase.AddField("z-velocity", "double", "0.0");
            phase.AddField("density", "double", "1.0");
            phase.AddField("initial density", "double", "1.0");
            phase.AddField("dynamic viscosity", "double", "0.01");
            phase.AddField("sound speed", "double", "10.0");
            phase.AddField("equation of state coefficient", "double", "15.0");
            phase.AddField("gamma", "double", "7.0");
            phase.AddField("temperature", "double", "0.0");
            phase.AddField("heat capacity", "double", "0.0");
            phase.AddField("heat conductivity", "double", "0.0");
            phase.AddField("color function", "double", phase.Id.ToString());
            _defaultPhaseNoIndicator++;
        }

        private void ButtonDesignerDelPhase_Clicked(object sender, RoutedEventArgs e)
        {
            try
            {
                int id = ListBoxDesignerPhases.Items.IndexOf(ListBoxDesignerPhases.SelectedItems[0]);
                ListBoxDesignerPhases.Items.RemoveAt(id);
                _domain.RemovePhase(_selectedPhaseName);
                _selectedPhaseName = "";
                _selectedPhaseId = -1;
                GroupBoxDesignerPhaseToolbox.Header = "Phase is not selected";
            }
            catch (ArgumentOutOfRangeException ex)
            {
                MessageBox.Show("Exception");
            }
        }

        private void ColorPickerEditPhase_SelectedColorChanged(object sender, RoutedEventArgs e)
        {
            Xceed.Wpf.Toolkit.ColorPicker colorPicker = sender as Xceed.Wpf.Toolkit.ColorPicker;
            Color color = colorPicker.SelectedColor;

            Phase selectedPhase = _domain.GetPhase(_selectedPhaseName);
            selectedPhase.Color[0] = color.A;
            selectedPhase.Color[1] = color.R;
            selectedPhase.Color[2] = color.G;
            selectedPhase.Color[3] = color.B;
            UpdatePlot();
        }

        private void ListBoxDesignerPhases_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                //_selectedPhaseName = ListBoxDesignerPhases.SelectedItems[0];
                _selectedPhaseId = ListBoxDesignerPhases.Items.IndexOf(ListBoxDesignerPhases.SelectedItems[0]);
                TextBlock textBlock = ListBoxDesignerPhases.Items[_selectedPhaseId] as TextBlock;
                _selectedPhaseName = textBlock.Text;
                Phase phase = _domain.GetPhase(_selectedPhaseName);
                GroupBoxDesignerPhaseToolbox.Header = "Toolbox for " + _selectedPhaseName;
                TextBoxPhaseName.Text = _selectedPhaseName;
                if (phase.Location == "")
                {
                    TextBoxPhasePosition.Text = "Whole domain";
                }
                else
                {
                    TextBoxPhasePosition.Text = phase.Location;
                }
                ColorPickerEditPhase.SelectedColor = Color.FromArgb(phase.Color[0], phase.Color[1], phase.Color[2], phase.Color[3]);
                UpdatePlot();
            }
            catch
            {
            }
        }

        private void TextBoxPhaseName_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                TextBox textBox = sender as TextBox;

                foreach (Phase ph in _domain.GetPhases())
                {
                    if (ph.Name == textBox.Text)
                    {
                        throw new InvalidName("Phase name exists");
                    }
                }

                GroupBoxDesignerPhaseToolbox.Header = "Toolbox for " + textBox.Text;
                Phase phase = _domain.GetPhase(_selectedPhaseName);
                phase.Name = textBox.Text;

                TextBlock textBlock = ListBoxDesignerPhases.Items[_selectedPhaseId] as TextBlock;
                textBlock.Text = phase.Name;
                _selectedPhaseName = phase.Name;
            }
            catch
            {
            }
        }

        private void ButtonPhaseEditFields_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (EditFieldsWindow.IsInitialized() == false)
                {
                    var window = new EditFieldsWindow(_domain, _selectedPhaseName);
                    window.Show();
                }
            }
            catch (ArgumentOutOfRangeException ex)
            {
            }
        }

        private void ButtonPhasePosition_Clicked(object sender, RoutedEventArgs e)
        {
            try
            {
                if (EditPositionWindow.IsInitialized() == false)
                {
                    var window = new EditPositionWindow(_domain, _selectedPhaseName, this);
                    window.Show();
                }
            }
            catch (ArgumentOutOfRangeException ex)
            {
            }
        }

        private void ButtonDesignerSurfaceTension_Click(object sedner, RoutedEventArgs e)
        {
            try
            {
                if (EditSurfaceTensionWindow.IsInitialized() == false)
                {
                    var window = new EditSurfaceTensionWindow(_domain, _selectedPhaseName);
                    window.Show();
                }
            }
            catch (ArgumentOutOfRangeException ex)
            {
            }
        }

        private void ButtonDesignerInterfaceCorrection_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (EditInterfaceCorrectionWindow.IsInitialized() == false)
                {
                    var window = new EditInterfaceCorrectionWindow(_domain, _selectedPhaseName);
                    window.Show();
                }
            }
            catch (ArgumentOutOfRangeException ex)
            {
            }
        }

        public void UpdatePlot()
        {
            //_domain.GenerateParticles();
            List<Position> phasePositions;
            Phase phase;
            Color color;
            bool isFullResolution = true;
            try
            {
                phase = _domain.GetPhase(_selectedPhaseName);
                color = Color.FromArgb(phase.Color[0], phase.Color[1], phase.Color[2], phase.Color[3]);

                phasePositions = new List<Position>();
                List<Position> positions = _domain.GetParticlePositions();
                if (positions.Count > 18000)
                {
                    positions = _domain.GetParticleLoosePositions();
                    isFullResolution = false;
                }
                
                foreach (Position position in positions)
                {
                    if (phase.HasPosition(position))
                    {
                        phasePositions.Add(position);
                    }
                }
               
            }
            catch (TextToMathParsingException e)
            {
                throw e;
            }
            catch
            {
                phasePositions = new List<Position>();
                color = Color.FromArgb(0,0,0,0);
            }

            //MessageBox.Show(phasePositions.Count.ToString());
            _plotModel.UpdateModel(_domain["XCV"], _domain["YCV"], phasePositions, color, isFullResolution);

            if ( _domain["XCV"] > _domain["YCV"] )
            {
                Plot1.Width = 600;
                Plot1.Height = Convert.ToInt32(600.0 * _domain["YCV"] / _domain["XCV"]);
            }
            else
            {
                Plot1.Height = 600;
                Plot1.Width = Convert.ToInt32(600.0 * _domain["XCV"] / _domain["YCV"]);
            }
            
            Plot1.InvalidateFlag += 1;
        }

        private void DoubleUpDownDomainGravityX_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                _domain["G_X"] = DoubleUpDownDomainGravityX.Value;
            }
            catch (System.NullReferenceException ex)
            { }
        }

        private void DoubleUpDownDomainGravityY_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                _domain["G_Y"] = DoubleUpDownDomainGravityY.Value;
            }
            catch (System.NullReferenceException ex)
            { }
        }

        private void DoubleUpDownDomainTimeStep_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                _domain["DT"] = DoubleUpDownDomainTimeStep.Value;
            }
            catch (System.NullReferenceException ex)
            { }
        }

        private void DoubleUpDownDomainSimulationTime_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                _domain["END_TIME"] = DoubleUpDownDomainSimulationTime.Value;
            }
            catch (System.NullReferenceException ex)
            { }
        }

        private void DoubleUpDownDomainResultSaveInterval_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                _domain["INTERVAL_TIME"] = DoubleUpDownDomainResultSaveInterval.Value;
            }
            catch (System.NullReferenceException ex)
            { }
        }

        private void DoubleUpDownDomainHDR_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            try
            {
                _domain.HDR = (double)DoubleUpDownDomainHDR.Value;
                _domain.SetSize(_domain.XCV, _domain.YCV, _domain["NCX"], _domain["NCY"]);
                //ButtonDesignerValidation.IsEnabled = false;
                UpdatePlot();
                _domain.GenerateParticles();
                TextBlockH.Text = "H=" + Convert.ToString(_domain.H, CultureInfo.InvariantCulture);
                TextBlockN.Text = "N=" + Convert.ToString(_domain.GetParticlePositions().Count);
            }
            catch (System.NullReferenceException ex)
            { }
        }
    }
}
