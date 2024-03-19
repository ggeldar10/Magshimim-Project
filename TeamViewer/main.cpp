#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include "SrtSocket.h" 
#include "PipeManager.h"

int main()
{
    const int serverPort = 12356;
    PipeManager pipeManager;
    IntCharUnion convertor = {0};
    std::vector<char> buffer = pipeManager.readDataFromPipe();

    // The gui will send the code for 1 byte
    convertor.bytes[0] = buffer[0];
    Modes mode = static_cast<Modes>(convertor.num);

    switch (mode)
    {
    case Controled:
    {
        SrtSocket serverSocket;
        sockaddr_in serverInfo = { 0 };
        serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(serverPort);
        serverSocket.srtBind(&serverInfo);
        serverSocket.listenAndAccept();
        std::cout << "found user" << std::endl;
        break;
    }
    case Controller:
    {
        SrtSocket client;
        sockaddr_in serverInfo = { 0 };
        serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(serverPort);

        client.connectToServer(&serverInfo);
        std::cout << "connected" << std::endl;
        break;
    }
    default:
        std::cout << "Invalid option" << std::endl;
        break;
    };

    return 0;
}
