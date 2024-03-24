#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include "SrtSocket.h" 
#include "PipeManager.h"
#include "IUser.h"
#include "Controlled.h"
#include "Controller.h"


int main()
{
    const int serverPort = 12356;
    //PipeManager pipeManager;
    //IntCharUnion convertor = {0};
    //std::vector<char> buffer = pipeManager.readDataFromPipe();
    //std::unique_ptr<IUser> user;

    //// The gui will send the code for 1 byte
    //convertor.bytes[0] = buffer[0];
    //MODES mode = static_cast<MODES>(convertor.num);
    
    MODES mode = MODES::CONTROLLED;
    switch (mode)
    {
    case MODES::CONTROLLED:
    {
        //user = std::make_unique<Controlled>(&pipeManager);

        SrtSocket serverSocket;
        sockaddr_in serverInfo = { 0 };
        serverInfo.sin_addr.s_addr = INADDR_ANY /*inet_addr("10.0.0.18")*/;
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(serverPort);
        serverSocket.srtBind(&serverInfo);
        serverSocket.listenAndAccept();
        std::cout << "found user" << std::endl;
        while(true){}
        break;
    }
    case MODES::CONTROLLER:
    {
        //todo get the data from the user
        SrtSocket socket;
        sockaddr_in serverInfo = { 0 };
        serverInfo.sin_addr.s_addr = inet_addr("10.0.0.27");
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(serverPort);
        socket.connectToServer(&serverInfo);
       /* this->_socket.connectToServer(&serverInfo);*/
        std::cout << "connected" << std::endl;
      
        break;
    }
    default:
    {}
        std::cout << "Invalid option" << std::endl;
        break;
    };

    return 0;
}
