﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace teamViewerGui
{
    public enum PIPE_CODES
    { 
        CLOSE_PIPE, STOP_SENDING_PICTURES, START_SENDING
    }

    public enum MODES
    { 
        CONTROLLER, CONTROLLED
    }
    public class CommunicatorPipes : IDisposable
    {
        private const string serverPipeName = "srtGuiPipe";
        private NamedPipeServerStream pipeServer;
        private bool disposedValue;

        public CommunicatorPipes()
        {
            this.pipeServer = new NamedPipeServerStream(serverPipeName, PipeDirection.InOut, 1, PipeTransmissionMode.Byte);
            disposedValue = false;
        }

        public void Listen()
        {
            this.pipeServer.WaitForConnection();
        }

        public void SendMessage(List<byte> data)
        {
            int dataLength = data.Count();
            byte[] buffer = new byte[dataLength + 4];
            BitConverter.GetBytes(dataLength).CopyTo(buffer, 0);
            data.CopyTo(buffer, 4);
            pipeServer.Write(buffer, 0, data.Count + 4);
        }

        public byte[] ReadData()
        {
            byte[] lengthInBytes = new byte[4];
            this.pipeServer.Read(lengthInBytes, 0, 4);
            int length = BitConverter.ToInt32(lengthInBytes.ToArray(), 0); 
            byte[] data = new byte[length];
            this.pipeServer.Read(data, 0, length);
            return data;
        }



        protected virtual void Dispose(bool disposing)
        {
            if (pipeServer == null)
            {
                return;
            }
            if (!disposedValue)
            {
                if (disposing)
                { 
                    pipeServer.Close();
                    
                }
                disposedValue = true;
            }
        }

        // // TODO: override finalizer only if 'Dispose(bool disposing)' has code to free unmanaged resources
        ~CommunicatorPipes()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: false);
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool PeekNamedPipe(SafeHandle handle,
    byte[] buffer, uint nBufferSize, ref uint bytesRead,
    ref uint bytesAvail, ref uint BytesLeftThisMessage);

        public bool isDataAvailable()
        { 
            uint bytesRead = 0;
            uint bytesAvail = 0;
            uint bytesLeft = 0;
            if (!PeekNamedPipe(this.pipeServer.SafePipeHandle, null, 0, ref bytesRead, ref bytesAvail, ref bytesLeft))
            {
                return false;
            }
            return bytesAvail > 0;
        }

    }
}
