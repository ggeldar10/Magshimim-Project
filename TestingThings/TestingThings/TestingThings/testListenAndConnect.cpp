#include <iostream>
#include "SrtSocket.h"



int main()
{
	const int serverPort = 12356;

	SrtSocket serverSocket;

	sockaddr_in serverInfo = { 0 };
	serverInfo.sin_addr.s_addr = INADDR_ANY;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(serverPort);
	serverSocket.srtBind(&serverInfo);
	serverSocket.listenAndAccept();



}
