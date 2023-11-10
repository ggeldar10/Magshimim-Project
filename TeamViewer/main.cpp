#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include "winapi.h"
#include "UdpSocket.h"


int main()
{
	UdpSocket socket;
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	socket.sendMsg("hello", &addr);

}