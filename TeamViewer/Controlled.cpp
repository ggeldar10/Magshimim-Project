#include "Controlled.h"
#include <fstream>

Controlled::Controlled(PipeManager* pipeManager) : IUser(pipeManager, MODES::CONTROLLED), _screenCapture(NULL)
{
}

void Controlled::createServer(unsigned int serverPort)
{
    sockaddr_in serverInfo = { 0 };
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(serverPort);
    this->_socket.srtBind(&serverInfo);
    this->_socket.listenAndAccept();
}

