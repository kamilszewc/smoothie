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
    /// Interaction logic for EditPositionWindow.xaml
    /// </summary>
    public partial class EditPositionWindow : Window
    {
        private Domain _domain;
        private Phase _phase;
        private PageDesigner _parent;
        private static bool _isInitialized = false;

        public EditPositionWindow()
        {
            InitializeComponent();
        }

        public EditPositionWindow(Domain domain, string selectedPhaseName, PageDesigner parent)
            : this()
        {
            _domain = domain;
            _phase = domain.GetPhase(selectedPhaseName);
            _parent = parent;
            _isInitialized = true;
            this.Closing += new CancelEventHandler(OnClose);

            TextBoxEditPositionWindow.Text = _phase.Location;
        }

        private void ButtonEditPositionApplyChanges_Clicked(object sender, RoutedEventArgs e)
        {
            string oldLocation = _phase.Location;
            try
            {
                _phase.Location = TextBoxEditPositionWindow.Text.Trim();
                _parent.TextBoxPhasePosition.Text = TextBoxEditPositionWindow.Text.Trim();
                _parent.UpdatePlot();
                this.Close();
            }
            catch (TextToMathParsingException ex)
            {
                _phase.Location = oldLocation;
                _parent.TextBoxPhasePosition.Text = oldLocation;
                TextBoxEditPositionWindow.Background = Brushes.MistyRose;
                MessageBox.Show("The statement can not be parsed correctly. Remember to use x and y variables only.");
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
