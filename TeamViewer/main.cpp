#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include "SrtSocket.h" 
#include "PipeManager.h"
#include "IUser.h"
#include "Controlled.h"
#include "Controller.h"

bool checkForStopCommand(PipeManager* pipeManager, PIPE_CODES codeToCheckFor)
{
    if (!pipeManager->isDataAvail())
    {
        return false;
    }
    IntCharUnion convertor = { 0 };
    std::vector<char> command = pipeManager->readDataFromPipe();
    convertor.bytes[0] = command[0];
    if (command.size() == 1 && (PIPE_CODES)convertor.num == codeToCheckFor)
    {
        return true;
    }
    return false;
}

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
        while (!checkForStopCommand(&pipeManager, PIPE_CODES::CLOSE_CONNECTION))
        {
            try
            {
                Controlled controlled(&pipeManager);
                controlled.createServer(serverPort);
            }
            catch (const std::exception& error)
            {
                std::cerr << error.what() << std::endl;
            }
        }
        //send shut down to the server 
        break;
    }
    case MODES::CONTROLLER:
    {
        while (!checkForStopCommand(&pipeManager, PIPE_CODES::CLOSE_CONNECTION))
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
