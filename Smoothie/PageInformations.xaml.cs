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
using Smoothie.Informations;

namespace Smoothie
{
    /// <summary>
    /// Interaction logic for PageInformations.xaml
    /// </summary>
    public partial class PageInformations : Page
    {
        public PageInformations()
        {
            InitializeComponent();
            PrintInformations();
        }

        private void PrintInformations()
        {
            Informations.Version version = new Informations.Version();
            TextBlockVersion.Text = "Version: " + version.GetVersion();

            Hardware hardware = new Hardware();
            TextBlockCPUInfo.Text = hardware.GetCPUs();
            TextBlockGPUInfo.Text = hardware.GetGPUs();

            License license = new License();
            TextBlockLicense.Text = license.GetLicense();
        }
    }
}
