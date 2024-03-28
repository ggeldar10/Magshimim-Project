using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace teamViewerGui
{
    /// <summary>
    /// Interaction logic for UserInput.xaml
    /// </summary>
    public partial class UserInput : Window
    {
        public string IpResult { get; set; }
        private bool firstFocus = true;
        private bool programClose = false;
        public UserInput()
        {
            InitializeComponent();
        }

        private void TextBox_GotFocus(object sender, RoutedEventArgs e)
        {
            if (firstFocus)
            {
                textBoxIp.Text = "";
                firstFocus = false;
            }
        }

        private void TextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (textBoxIp.Text == "")
            {
                firstFocus = true;
                textBoxIp.Text = "Enter Here";
            }
        }

        // confirm button
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            string regexPattern = @"^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
            if (!Regex.IsMatch(textBoxIp.Text, regexPattern))
            {
                errorMsg.Visibility = Visibility.Visible;
                return;
            }
            this.programClose = true;
            this.IpResult = textBoxIp.Text;
            this.Close();
            
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (programClose)
            {
                DialogResult = true;
            }
            else
            {
                DialogResult = false;
            }
        }
    }
}
