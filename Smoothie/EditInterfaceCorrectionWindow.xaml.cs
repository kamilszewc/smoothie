﻿using System;
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
using Xceed.Wpf.Toolkit;
using Sph;

namespace Smoothie
{
    /// <summary>
    /// Interaction logic for EditInterfaceCorrectionWindow.xaml
    /// </summary>
    public partial class EditInterfaceCorrectionWindow : Window
    {
        static bool _isInitialized = false;
        Domain _domain;
        string _selectedPhaseName;

        public EditInterfaceCorrectionWindow()
        {
            InitializeComponent();
            _isInitialized = true;
            this.Closing += new CancelEventHandler(OnClose);
        }

        public EditInterfaceCorrectionWindow(Domain domain, string selectedPhaseName)
            : this()
        {
            _domain = domain;
            _selectedPhaseName = selectedPhaseName;
            InterPhaseCoefficients interfaceCorrectionCoefficients = _domain.GetInterPhaseParameter("interface-correction-coefficient");
            interfaceCorrectionCoefficients.Update();
            int number = interfaceCorrectionCoefficients.Count();
            foreach (InterPhaseCoefficiant interPhaseCoefficiant in interfaceCorrectionCoefficients.Get())
            {
                StackPanel stackPanel = new StackPanel();
                stackPanel.Orientation = Orientation.Horizontal;

                TextBox textBoxPhase1 = new TextBox();
                textBoxPhase1.Text = interPhaseCoefficiant.Phase1.Name;
                textBoxPhase1.Width = 120;
                stackPanel.Children.Add(textBoxPhase1);

                TextBox textBoxPhase2 = new TextBox();
                textBoxPhase2.Text = interPhaseCoefficiant.Phase2.Name;
                textBoxPhase1.Width = 120;
                stackPanel.Children.Add(textBoxPhase2);

                DoubleUpDown doubleUpDown = new DoubleUpDown();
                doubleUpDown.Value = interPhaseCoefficiant.Value;
                doubleUpDown.Minimum = 0.0;
                doubleUpDown.FormatString = "N";
                doubleUpDown.Width = 60;
                doubleUpDown.CultureInfo = System.Globalization.CultureInfo.InvariantCulture;
                stackPanel.Children.Add(doubleUpDown);

                ListBoxInterfaceCorrection.Items.Add(stackPanel);
            }
        }

        private void ButtonApplyChanges_Click(object sender, RoutedEventArgs e)
        {
            InterPhaseCoefficients interfaceCorrectionCoefficients = _domain.GetInterPhaseParameter("interface-correction-coefficient");

            foreach (StackPanel stackPanel in ListBoxInterfaceCorrection.Items)
            {
                TextBox textBoxPhase1 = stackPanel.Children[0] as TextBox;
                string phaseName1 = textBoxPhase1.Text;

                TextBox textBoxPhase2 = stackPanel.Children[1] as TextBox;
                string phaseName2 = textBoxPhase2.Text;

                DoubleUpDown doubleUpDown = stackPanel.Children[2] as DoubleUpDown;
                double value = (double)doubleUpDown.Value;

                interfaceCorrectionCoefficients.Set(phaseName1, phaseName2, value);
            }
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
