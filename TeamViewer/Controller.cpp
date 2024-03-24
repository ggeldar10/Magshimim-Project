#include "Controller.h"

Controller::Controller(PipeManager* pipeManager) : IUser(pipeManager, MODES::CONTROLLER)
{
}

/* 
* Connects the socket to the server
* input: 
* portNum - the port number of the server
* serverIp - the ip of the server as a string
* output:
* none
*/
void Controller::connectToServer(uint16_t portNum, const std::string& serverIp)
{
    //sockaddr_in serverInfo = { 0 };
    //serverInfo.sin_addr.s_addr = inet_addr(serverIp.c_str());
    //serverInfo.sin_family = AF_INET;
    //serverInfo.sin_port = htons(portNum);

    //this->_socket.connectToServer(&serverInfo);
    //std::cout << "connected" << std::endl;
}

