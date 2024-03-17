using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
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
    public partial class ControllerWindow : Window, IDisposable
    {
        private bool disposed = false;
        private Thread backgroundThread; 
        public ControllerWindow()
        {
            InitializeComponent();
            this.backgroundThread = new Thread(setImageFromPipeData);
            this.backgroundThread.Start();
        }

        public void displayImage(byte[] imageBytes)
        {
            using (MemoryStream ms = new MemoryStream(imageBytes))
            {
                var decoder = BitmapDecoder.Create(ms,
            BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);
                UpdateImage(decoder.Frames[0]);
            }
        }
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (disposing)
                {
                    
                }

                this.disposed = true;
            }
        }
        public void Dispose()
        {
            // send a message to the backend to stop the pictures
            Dispose(true);
        }

        public void setImageFromPipeData()
        {
            while (!this.disposed)
            {
                // maybe add mutex if i decide to do it in a thread
                displayImage(PipeManagerSingletone.getInstance().ReadData());
                Thread.Sleep(100);
            }
        }

        private void UpdateImage(BitmapFrame frame)
        {
            if (Application.Current.Dispatcher.CheckAccess())
            {
                screenDisplay.Source = frame;
            }
            else
            {
                Application.Current.Dispatcher.Invoke(() => screenDisplay.Source = frame);
            }
        }
    }
}
