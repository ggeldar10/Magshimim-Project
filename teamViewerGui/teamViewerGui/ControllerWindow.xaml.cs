﻿using System;
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
    /// 
    public partial class ControllerWindow : Window, IDisposable
    {
        private bool disposed = false;
        private Thread backgroundThread;
        private double imageWidth = 0;
        private double imageHeight = 0;
        private List<byte> imageBytes = new List<byte>();
        private uint lastStartSeq = 0;
        private uint lastEndSeq = 0;
        private const uint mtu = 1024; // later do it from the program itself

        public ControllerWindow()
        {
            InitializeComponent();
            this.ResizeMode = ResizeMode.NoResize;
            this.WindowState = WindowState.Maximized;
            //List<byte> bytes = new List<byte> { (byte)PIPE_CODES.START_SENDING };
            //PipeManagerSingletone.getInstance().SendMessage(bytes);
            this.backgroundThread = new Thread(setImageFromPipeData);
            this.backgroundThread.IsBackground = true;
            this.backgroundThread.Start();
            
        }

        public void displayImage(byte[] messageBytes)
        {
            uint startSeq = BitConverter.ToUInt32(messageBytes, 0);
            uint endSeq = BitConverter.ToUInt32(messageBytes, 4);
            uint currSeq = BitConverter.ToUInt32(messageBytes, 8);

            if (this.lastEndSeq != endSeq || this.lastStartSeq != startSeq)
            {
                this.lastStartSeq = startSeq;
                this.lastEndSeq = endSeq;
                this.imageBytes = new List<byte>((int)((endSeq - startSeq) * mtu));
                this.imageBytes.AddRange(new ArraySegment<byte>(messageBytes, 12, messageBytes.Length - 12));
            }
            else if (this.lastStartSeq == startSeq && this.lastEndSeq == endSeq && currSeq != endSeq)
            {
                //this.imageBytes.InsertRange((int)((currSeq - startSeq) * mtu), new ArraySegment<byte>(messageBytes, 12, messageBytes.Length - 12));
                this.imageBytes.AddRange(new ArraySegment<byte>(messageBytes, 12, messageBytes.Length - 12));
            }
            else if (currSeq == endSeq) // here i assume that it goes 1 by 1 
            {
                this.imageBytes.AddRange(new ArraySegment<byte>(messageBytes, 12, messageBytes.Length - 12));
                using (MemoryStream ms = new MemoryStream(this.imageBytes.ToArray()))
                {
                    var decoder = BitmapDecoder.Create(ms,
                BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);
                    UpdateImage(decoder.Frames[0]);
                }
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
                displayImage(PipeManagerSingletone.getInstance().ReadData());
            }
            if (PipeManagerSingletone.getInstance().isDataAvailable())
            {
                PipeManagerSingletone.getInstance().ReadData();
            }
            List<byte> bytes = new List<byte> { (byte)PIPE_CODES.STOP_SENDING_PICTURES };
            PipeManagerSingletone.getInstance().SendMessage(bytes);
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

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            this.Dispose();
        }

        private void screenDisplay_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            this.imageHeight = screenDisplay.ActualHeight;
            this.imageWidth = screenDisplay.ActualWidth;
        }
    }
}
