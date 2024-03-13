#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include "SrtSocket.h" // Include SrtSocket.h before using SrtSocket class
#define CONTROLLER 1
#define CONTROLED 2

int main()
{
    const int serverPort = 12356;
    int option = 0;

    std::cout << "Choose connection type:\n1 - Controller\n2 - Controled" << std::endl;
    std::cin >> option;

    switch (option)
    {
    case CONTROLED:
    {
        SrtSocket serverSocket; // Create SrtSocket object for server
        sockaddr_in serverInfo = { 0 };
        serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverInfo.sin_family = AF_INET;
        serverInfo.sin_port = htons(serverPort);
        serverSocket.srtBind(&serverInfo);
        serverSocket.listenAndAccept();
        std::cout << "found user" << std::endl;
        break;
    }
    case CONTROLLER:
    {
        SrtSocket client; // Create SrtSocket object for client
        sockaddr_in serverInfo = { 0 };
        serverInfo.sin_addr.s_addr = INADDR_ANY;
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
