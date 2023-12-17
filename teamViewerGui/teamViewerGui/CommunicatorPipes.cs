using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace teamViewerGui
{
    internal class CommunicatorPipes : IDisposable
    {
        private string nameOfPipe;
        private string serverPipeName = "serverPipe";
        private NamedPipeClientStream pipeClient;
        private bool disposedValue;

        public CommunicatorPipes(string nameOfPipe)
        { 
            this.nameOfPipe = nameOfPipe;
        }
        public void CreatePipe()
        {
            this.pipeClient = new NamedPipeClientStream(serverPipeName, nameOfPipe, PipeDirection.InOut);
            this.pipeClient.Connect();
        }

        public void SendMessage(string message)
        {
            using (StreamWriter writer = new StreamWriter(pipeClient))
            {
                writer.WriteLine(message);
            }
        }

        public string ReadMessage()
        {
            string message = "";
            using (StreamReader reader = new StreamReader(pipeClient))
            {
                message = reader.ReadToEnd();
            }
            return message;
        }



        protected virtual void Dispose(bool disposing)
        {
            if (pipeClient == null)
            {
                return;
            }
            if (!disposedValue)
            {
                if (disposing)
                {
                    pipeClient.Dispose();
                    pipeClient = null;
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
