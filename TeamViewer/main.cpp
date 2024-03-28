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
    case MODES::CONTROLLED:
    {
        try
        {
            //user = std::make_unique<Controlled>(&pipeManager);
            Controlled controlled(&pipeManager);
            controlled.createServer(serverPort); // creates also the threads 
        }
        catch (const std::exception& error)
        {
            std::cerr << error.what() << std::endl;
        }
        break;
    }
    case MODES::CONTROLLER:
    {
        try
        {
            std::vector<char> ipBuffer = pipeManager.readDataFromPipe();
            Controller controller(&pipeManager);
            controller.connectToServer(serverPort, std::string(ipBuffer.begin(), ipBuffer.end()));
            controller.startImageStream();
        }
        catch (const std::exception& error)
        {
            std::cerr << error.what() << std::endl;
        }
        break;
    }
    default:
    {}
        std::cout << "Invalid option" << std::endl;
        break;
    };

    return 0;
}
