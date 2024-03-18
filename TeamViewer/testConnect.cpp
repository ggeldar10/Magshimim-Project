#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include "SrtSocket.h"

int main()
{
	const int serverPort = 12356;
	SrtSocket client;

	sockaddr_in serverInfo = { 0 };
	serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(serverPort);

	client.connectToServer(&serverInfo);
	
}