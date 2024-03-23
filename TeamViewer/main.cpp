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
    PipeManager pipeManager;
    IntCharUnion convertor = {0};
    std::vector<char> buffer = pipeManager.readDataFromPipe();

    // The gui will send the code for 1 byte
    convertor.bytes[0] = buffer[0];
    MODES mode = static_cast<MODES>(convertor.num);

    switch (mode)
    {
    case CONTROLLED:
    {
        //user = std::make_unique<Controlled>(&pipeManager);

        SrtSocket serverSocket;
        sockaddr_in serverInfo = { 0 };
        serverInfo.sin_addr.s_addr = INADDR_ANY; 
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(serverPort);
        serverSocket.srtBind(&serverInfo);
        serverSocket.listenAndAccept();
        std::cout << "found user" << std::endl;
        break;
    }
    case CONTROLLER:
    {

        Controller controller(&pipeManager);
        //controller.connectToServer();
        //controller.intilizeCursorAndKeyboard();
        controller.startImageStream();
      
        break;
    }
    default:
        std::cout << "Invalid option" << std::endl;
        break;
    };

    return 0;
}
