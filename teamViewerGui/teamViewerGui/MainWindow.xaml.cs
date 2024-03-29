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

namespace teamViewerGui
{
    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            PipeManagerSingletone.getInstance().Listen();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            List<byte> bytes = new List<byte>();
            bytes.Add((byte)MODES.CONTROLLER);
            PipeManagerSingletone.getInstance().SendMessage(bytes);
            UserInput userInput = new UserInput();
            userInput.ShowDialog();
            if ((bool)userInput.DialogResult)
            {
                bytes.Clear();
                foreach (byte item in userInput.IpResult.ToList())
                {
                    bytes.Add(item);
                }
                PipeManagerSingletone.getInstance().SendMessage(new List<byte>(bytes);
                ControllerWindow window = new ControllerWindow();
                window.Show();
            }
            
        }
        
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            PipeManagerSingletone.getInstance().SendMessage(new List<byte> { (byte)MODES.CONTROLLER });
            ControlledWindow controlled = new ControlledWindow();
            controlled.Show();
        }
    }
}
