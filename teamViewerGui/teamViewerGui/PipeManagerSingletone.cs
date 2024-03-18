using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace teamViewerGui
{
    public class PipeManagerSingletone
    {
        private static CommunicatorPipes pipes;
        private PipeManagerSingletone()
        {

        }

        public static CommunicatorPipes getInstance()
        {
            if (pipes == null)
            {
                pipes = new CommunicatorPipes();
            }
            return pipes;
        }       
    }
}
