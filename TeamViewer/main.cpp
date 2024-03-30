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
    bool didStop = false ;
    const int serverPort = 12356;
    PipeManager pipeManager;
    while (!didStop)
    {
        IntCharUnion convertor = { 0 };
        std::vector<char> buffer = pipeManager.readDataFromPipe();

        // The gui will send the code for 1 byte
        convertor.bytes[0] = buffer[0];
        MODES mode = static_cast<MODES>(convertor.num);

        switch (mode)
        {
        case MODES::CONTROLLED:
        {
            Controlled controlled(&pipeManager);
            try
            {
                controlled.createServer(serverPort);
            }
            catch (const std::exception& error)
            {
                std::cerr << error.what() << std::endl;
            }
            while (!checkForStopCommand(&pipeManager, PIPE_CODES::CLOSE_CONNECTION))
            {
                Sleep(3000);
            }
            //send shut down to the server 
            break;
        }
        case MODES::CONTROLLER:
        {
            Controller controller(&pipeManager);
            try
            {
                std::vector<char> ipBuffer = pipeManager.readDataFromPipe();
                controller.connectToServer(serverPort, std::string(ipBuffer.begin(), ipBuffer.end()));
            }
            catch (const std::exception& error)
            {
                std::cerr << error.what() << std::endl;
            }
            while (!checkForStopCommand(&pipeManager, PIPE_CODES::CLOSE_CONNECTION))
            {
                Sleep(3000);
            }
            break;
        }
        default:
        {
            didStop = true;
        };
        }
    }

    return 0;
}
