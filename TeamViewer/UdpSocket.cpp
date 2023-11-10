#define _CRT_SECURE_NO_WARNINGS

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
	short length = msg.size() + DATAGRAM_SIZE;
	char* packet = new char[length + 1];
	memset(packet, 0, length);
	convertShortToChar(static_cast<short>(to->sin_port), packet + 2); // puts the port in the 2 and 3 positions
	convertShortToChar(length, packet + 4); // puts the length in the 4 and 5 postions
	strcpy_s(packet + DATAGRAM_SIZE, (length + 1) - DATAGRAM_SIZE, msg.c_str());
	errno = 0;
	int bytes = sendto(this->_socket, packet, length + 1, 0, (struct sockaddr*)to, sizeof(sockaddr_in));
	if (bytes < 0)
	{
		int errorCode = WSAGetLastError();
		std::cerr << "Error sending data: " << errorCode << std::endl;
		perror("send error");
		delete[] packet;
		throw "Error while sending";
	}
	else
	{
		delete[] packet;
	}
}

/*
this function will send the msg to the given address
input: the message to send, the address to send to
output: the error code of sendto
*/
int UdpSocket::sendMsg(char* msg, sockaddr* to)
{
	if (this->_socket == NULL)
	{
		throw "Error socket is null"; //todo add an excpetion
	}
	return sendto(this->_socket, msg, sizeof(msg), 0, to, sizeof(*to));
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

			delete fullMsg;
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
void UdpSocket::bindUdpSocket(int port, sockaddr* addrs)
{
	if (this->_socket == NULL)
	{
		throw "Error: socket is null";
	}
	bind(this->_socket, addrs, sizeof(*addrs));
	this->_port = port;
}
