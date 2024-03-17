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
        public static CommunicatorPipes manager;
        public MainWindow()
        {
            InitializeComponent();
            PipeManagerSingletone.getInstance().Listen();
            //manager.Listen();
            //List<byte> bytes = new List<byte>();
            //string message = "Hello";
            //foreach (var c in message)
            //{
            //    bytes.Add((byte)c);
            //}
            //manager.SendMessage(bytes);
            ////testing recv

            //foreach(char c in manager.ReadData())
            //{ 
            //    Console.Write(c);
            //}
            //Console.WriteLine();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            ControllerWindow window = new ControllerWindow();
            window.Show();
        }
    }
}
