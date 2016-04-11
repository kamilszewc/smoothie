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
using System.Windows.Shapes;
using System.ComponentModel;
using Sph;

namespace Smoothie
{
    /// <summary>
    /// Interaction logic for EditFieldsWindow.xaml
    /// </summary>
    public partial class EditFieldsWindow : Window
    {
        private Domain _domain;
        private Phase _phase;
        private static bool _isInitialized = false;

        public EditFieldsWindow()
        {
            InitializeComponent();
            
        }

        public EditFieldsWindow(Domain domain, string selectedPhaseName)
            : this()
        {
            _domain = domain;
            _phase = domain.GetPhase(selectedPhaseName);
            _isInitialized = true;
            this.Closing += new CancelEventHandler(OnClose);

            TextBoxFieldXVelocity.Text = _phase.GetField("x-velocity").Value;
            TextBoxFieldYVelocity.Text = _phase.GetField("y-velocity").Value;
            TextBoxFieldZVelocity.Text = _phase.GetField("z-velocity").Value;
            TextBoxFieldDensity.Text = _phase.GetField("density").Value;
            TextBoxFieldKinematicViscosity.Text = _phase.GetField("dynamic viscosity").Value;
            TextBoxFieldSoundSpeed.Text = _phase.GetField("sound speed").Value;
            TextBoxFieldGamma.Text = _phase.GetField("gamma").Value;
            TextBoxFieldTemperature.Text = _phase.GetField("temperature").Value;
            TextBoxFieldHeatCapacity.Text = _phase.GetField("heat capacity").Value;
            TextBoxFieldConductivity.Text = _phase.GetField("heat conductivity").Value;

            if (_domain["T_HEAT_TRANSFER"] > 0)
            {
                TextBoxFieldTemperature.IsEnabled = true;
                TextBoxFieldConductivity.IsEnabled = true;
                TextBoxFieldHeatCapacity.IsEnabled = true;
            }
            else
            {
                TextBoxFieldTemperature.IsEnabled = false;
                TextBoxFieldConductivity.IsEnabled = false;
                TextBoxFieldHeatCapacity.IsEnabled = false;
            }
        }

        private void ButtonFieldApplyChanges_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _phase.GetField("x-velocity").Value = TextBoxFieldXVelocity.Text;
                _phase.GetField("y-velocity").Value = TextBoxFieldYVelocity.Text;
                _phase.GetField("z-velocity").Value = TextBoxFieldZVelocity.Text;
                _phase.GetField("density").Value = TextBoxFieldDensity.Text;
                _phase.GetField("initial density").Value = TextBoxFieldDensity.Text;
                _phase.GetField("dynamic viscosity").Value = TextBoxFieldKinematicViscosity.Text;
                _phase.GetField("sound speed").Value = TextBoxFieldSoundSpeed.Text;
                _phase.GetField("gamma").Value = TextBoxFieldGamma.Text;
                _phase.GetField("temperature").Value = TextBoxFieldTemperature.Text;
                _phase.GetField("heat capacity").Value = TextBoxFieldHeatCapacity.Text;
                _phase.GetField("heat conductivity").Value = TextBoxFieldConductivity.Text;
                this.Close();
            }
            catch
            {

            }
        }

        public new static bool IsInitialized()
        {
            return _isInitialized;
        }

        public void OnClose(object sender, CancelEventArgs e)
        {
            _isInitialized = false;
        }
    }
}
