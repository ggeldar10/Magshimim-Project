#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "UdpSocket.h"
#include <algorithm>
#include <iostream>
#include <exception>

std::string last_message = ""; // used in recieve

/*
this function puts the number inside the char array it is given in big endian order
input: the number to convert from, the char array to put to (must have at least two cells)
output: puts the number in the char array in the big endian order
*/
void UdpSocket::convertShortToChar(short num, char* str)
{
	str[0] = num >> EIGHT_BITS;
	str[1] = num & 0xFF;
}

UdpPacket UdpSocket::createPacket(std::string message, sockaddr_in* addrsInfo)
{
	return UdpPacket(message, htons(this->_port), addrsInfo->sin_port);
}

UdpSocket::UdpSocket() : _socket(NULL), _port(0)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "Error starting WSA" << std::endl;
		throw std::invalid_argument("Error starting WSA");
	}
	this->_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (this->_socket == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket" << std::endl;
		throw std::invalid_argument("Error creating socket"); // change later
	}
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bindUdpSocket(0, &addr);
}

UdpSocket::~UdpSocket()
{
	try
	{
		closesocket(this->_socket);
		this->_socket = NULL;
		WSACleanup();
	}
	catch (...) {}
}

/*
this function will send the msg to the given address
input: the message to send, the address to send to
output: the error code of sendto
*/
void UdpSocket::sendMsg(std::string msg, sockaddr_in* to) // maybe change later to a real exception insted of retruning the error code
{
	if (this->_socket == INVALID_SOCKET)
	{
		std::cerr << "Invalid socket" << std::endl;
		throw "Error socket is null"; //todo add an excpetion
	}
	UdpPacket udpPacket = createPacket(msg, to);
	short length = msg.size() + DATAGRAM_SIZE;
	short srcPort = this->_port;
	char* packet = new char[length + 1];
	memset(packet, 0, length);
	convertShortToChar(htons(srcPort), packet);
	convertShortToChar(htons(static_cast<short>(to->sin_port)), packet + 2); // puts the port in the 2 and 3 positions
	convertShortToChar(length, packet + 4); // puts the length in the 4 and 5 postions
	strcpy_s(packet + DATAGRAM_SIZE, (length + 1) - DATAGRAM_SIZE, msg.c_str());
	int bytes = sendto(this->_socket, packet, length + 1, 0, (struct sockaddr*)to, sizeof(sockaddr_in));
	if (bytes < 0)
	{
		std::cerr << "Error sending data: " << std::endl;
		perror("send error");
		delete[] packet;
		throw "Error while sending";
	}
	else
	{
		delete[] packet;
	}
}

int UdpSocket::recvieve(char* buffer, int len)
{
	if (this->_port == 0)
	{
		throw "Error trying to recive without a port";
	}
	if (last_message.empty())
	{
		// todo do a check on checksum
		bool found = false;
		while (!found)
		{
			char tempBuffer[DATAGRAM_SIZE] = "";
			int length = 0;
			int port = 0;
			if (recv(this->_socket, tempBuffer, DATAGRAM_SIZE, 0) < 0)
			{
				throw "Error while recieving";
			}
			// convert from big endian byte representation to an integer
			port = (tempBuffer[DST_PORT_POSITION] << EIGHT_BITS) | tempBuffer[DST_PORT_POSITION + 1];
			length = (tempBuffer[LENGTH_POSITION] << EIGHT_BITS) | tempBuffer[LENGTH_POSITION + 1];
			char* fullMsg = new char[length];
			if (recv(this->_socket, fullMsg, length - DATAGRAM_SIZE, 0))
			{
				throw "Error while recieving";
			}
			if (port == this->_port)
			{
				found = true;
				last_message = fullMsg;
			}

			delete[] fullMsg;
		}
	}
	if (len <= last_message.size())
	{
		strcpy_s(buffer, len, last_message.c_str());
		last_message = "";
	}
	else
	{
		strcpy_s(buffer, len, last_message.substr(0, len).c_str());
		last_message = last_message.substr(len);
	}
	return len;
}
/*
binds the the socket to listen to incoming messages 
using a certain port
input: the port to listen to, the local address
output: the socket is listening to incoming messages
*/
void UdpSocket::bindUdpSocket(int port, std::string addrs)
{
	if (this->_socket == INVALID_SOCKET)
	{
		throw "Error: socket is null";
	}
	sockaddr_in addrsInfo;
	memset(&addrsInfo, 0, sizeof(addrsInfo));
	addrsInfo.sin_family = AF_INET;
	addrsInfo.sin_port = htons(port);
	addrsInfo.sin_addr.s_addr = inet_addr(addrs.c_str());
	bind(this->_socket, reinterpret_cast<sockaddr*>(&addrsInfo), sizeof(addrsInfo));
	if (port == 0)
	{
		int sockLen = sizeof(sockaddr);
		sockaddr help;
		getsockname(this->_socket, &help, &sockLen);
		sockaddr_in* name = reinterpret_cast<sockaddr_in*>(&help);
		this->_port = name->sin_port;
	}
	else 
	{
		this->_port = port;
	}
}

void UdpSocket::bindUdpSocket(int port, sockaddr_in* addr)
{
	bind(this->_socket, reinterpret_cast<sockaddr*>(addr), sizeof(addr));
	if (port == 0)
	{
		int sockLen = sizeof(sockaddr);
		sockaddr help;
		getsockname(this->_socket, &help, &sockLen);
		sockaddr_in* name = reinterpret_cast<sockaddr_in*>(&help);
		this->_port = name->sin_port;
	}
	else
	{
		this->_port = port;
	}
}
