using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace teamViewerGui
{
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
            // todo a check if the connection worked
        }

        public void SendMessage(List<byte> data)
        {
            //check exactly how does it send
            int dataLength = data.Count();
            byte[] buffer = new byte[dataLength + 4];
            BitConverter.GetBytes(dataLength).CopyTo(buffer, 0);
            data.CopyTo(buffer, 4);
            pipeServer.Write(buffer, 0, data.Count + 4);
        }

        public byte[] ReadData()
        {
            //todo change from string to get also the bytes with null
            byte[] lengthInBytes = new byte[4];
            this.pipeServer.Read(lengthInBytes, 0, 4);
            int length = BitConverter.ToInt32(lengthInBytes.ToArray(), 0); // might have a problem
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
                    pipeServer.Dispose();
                    pipeServer = null;
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
    }
}
