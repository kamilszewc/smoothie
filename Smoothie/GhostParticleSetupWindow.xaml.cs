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
    /// Interaction logic for GhostParticleSetupWindow.xaml
    /// </summary>
    public partial class GhostParticleSetupWindow : Window
    {
        Domain _domain;
        static bool _isInitialized = false;

        public GhostParticleSetupWindow()
        {
            InitializeComponent();
            _isInitialized = true;
            this.Closing += new CancelEventHandler(OnClose);
        }

        public GhostParticleSetupWindow(Domain domain)
            : this()
        {
            _domain = domain;

            DoubleUpDownVelocityNX.Value = _domain.GetParameter("V_N");
            DoubleUpDownVelocityEY.Value = _domain.GetParameter("V_E");
            DoubleUpDownVelocitySX.Value = _domain.GetParameter("V_S");
            DoubleUpDownVelocityWY.Value = _domain.GetParameter("V_W");
            ComboBoxDesignerTemperatureTypeN.SelectedIndex = _domain.GetParameter("T_TEMP_N");
            ComboBoxDesignerTemperatureTypeE.SelectedIndex = _domain.GetParameter("T_TEMP_E");
            ComboBoxDesignerTemperatureTypeS.SelectedIndex = _domain.GetParameter("T_TEMP_S");
            ComboBoxDesignerTemperatureTypeW.SelectedIndex = _domain.GetParameter("T_TEMP_W");
            DoubleUpDownTemperatureN.Value = _domain.GetParameter("TEMP_N");
            DoubleUpDownTemperatureE.Value = _domain.GetParameter("TEMP_E");
            DoubleUpDownTemperatureS.Value = _domain.GetParameter("TEMP_S");
            DoubleUpDownTemperatureW.Value = _domain.GetParameter("TEMP_W");

        }


        private void ComboBoxDesignerTemperatureTypeN_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem comboBoxItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (comboBoxItem.Content.ToString())
                {
                    case "Constant":
                        DoubleUpDownTemperatureN.IsEnabled = true;
                        break;
                    case "Adiabatic":
                        DoubleUpDownTemperatureN.IsEnabled = false;
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerTemperatureTypeE_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem comboBoxItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (comboBoxItem.Content.ToString())
                {
                    case "Constant":
                        DoubleUpDownTemperatureE.IsEnabled = true;
                        break;
                    case "Adiabatic":
                        DoubleUpDownTemperatureE.IsEnabled = false;
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerTemperatureTypeS_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem comboBoxItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (comboBoxItem.Content.ToString())
                {
                    case "Constant":
                        DoubleUpDownTemperatureS.IsEnabled = true;
                        break;
                    case "Adiabatic":
                        DoubleUpDownTemperatureS.IsEnabled = false;
                        break;
                }
            }
            catch
            { }
        }

        private void ComboBoxDesignerTemperatureTypeW_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                ComboBoxItem comboBoxItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                switch (comboBoxItem.Content.ToString())
                {
                    case "Constant":
                        DoubleUpDownTemperatureW.IsEnabled = true;
                        break;
                    case "Adiabatic":
                        DoubleUpDownTemperatureW.IsEnabled = false;
                        break;
                }
            }
            catch
            { }
        }

        private void ButtonGhostParticleSetupWindowApplyChanges_OnClick(object sender, RoutedEventArgs e)
        {
            _domain.SetParameter("V_N", Convert.ToDouble(DoubleUpDownVelocityNX.Value));
            _domain.SetParameter("V_E", Convert.ToDouble(DoubleUpDownVelocityEY.Value));
            _domain.SetParameter("V_S", Convert.ToDouble(DoubleUpDownVelocitySX.Value));
            _domain.SetParameter("V_W", Convert.ToDouble(DoubleUpDownVelocityWY.Value));
            _domain.SetParameter("T_TEMP_N", ComboBoxDesignerTemperatureTypeN.SelectedIndex);
            _domain.SetParameter("T_TEMP_E", ComboBoxDesignerTemperatureTypeE.SelectedIndex);
            _domain.SetParameter("T_TEMP_S", ComboBoxDesignerTemperatureTypeS.SelectedIndex);
            _domain.SetParameter("T_TEMP_W", ComboBoxDesignerTemperatureTypeW.SelectedIndex);
            _domain.SetParameter("TEMP_N", Convert.ToDouble(DoubleUpDownTemperatureN.Value));
            _domain.SetParameter("TEMP_E", Convert.ToDouble(DoubleUpDownTemperatureE.Value));
            _domain.SetParameter("TEMP_S", Convert.ToDouble(DoubleUpDownTemperatureS.Value));
            _domain.SetParameter("TEMP_W", Convert.ToDouble(DoubleUpDownTemperatureW.Value));
            this.Close();
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
