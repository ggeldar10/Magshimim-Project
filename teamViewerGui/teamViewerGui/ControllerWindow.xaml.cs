using System;
using System.Collections.Generic;
using System.IO;
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

namespace teamViewerGui
{
    /// <summary>
    /// Interaction logic for ControllerWindow.xaml
    /// </summary>
    public partial class ControllerWindow : Window
    {
        public ControllerWindow()
        {
            InitializeComponent();
            setImageFromPipeData();
        }

        public void displayImage(byte[] imageBytes)
        {
            using (MemoryStream ms = new MemoryStream(imageBytes))
            {
                var decoder = BitmapDecoder.Create(ms,
            BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);
                screenDisplay.Source = decoder.Frames[0];
            }
        }

        public void setImageFromPipeData()
        {
            // maybe add mutex if i decide to do it in a thread
            displayImage(PipeManagerSingletone.getInstance().ReadData());
            
        }


    }
}
