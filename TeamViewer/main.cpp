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
    std::unique_ptr<IUser> user;

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
        serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1"); // maybe INADDR_ANY
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(serverPort);
        serverSocket.srtBind(&serverInfo);
        serverSocket.listenAndAccept();
        std::cout << "found user" << std::endl;
        break;
    }
    case CONTROLLER:
    {
        //todo get the data from the user
        user = std::make_unique<Controller>(&pipeManager);
        ((Controller*)user.get())->connectToServer();
        // todo get the ip to connect to and connect from the user in the gui
      
        break;
    }
    default:
        std::cout << "Invalid option" << std::endl;
        break;
    };

    return 0;
}
