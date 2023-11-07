#include "UdpSocket.h"
#include <algorithm>

std::string last_message = ""; // used in recieve

UdpSocket::UdpSocket() : _socket(NULL), _port(0)
{
	this->_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (this->_socket < 0)
	{
		throw "Error creating the socket";
	}
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		throw "Error WSAStartup failed";
	}
	this->_port = 0;
}

UdpSocket::~UdpSocket()
{
	WSACleanup();
	closesocket(this->_socket);
	this->_socket = NULL;
}

/*
this function will send the msg to the given address
input: the message to send, the address to send to
output: the error code of sendto
*/
int UdpSocket::sendTo(std::string msg, sockaddr* to) // maybe change later to a real exception insted of retruning the error code
{
	if (this->_socket == NULL)
	{
		throw "Error socket is null"; //todo add an excpetion
	}
	return sendto(this->_socket, msg.c_str(), msg.size(), 0, to, sizeof(*to));
}

/*
this function will send the msg to the given address
input: the message to send, the address to send to
output: the error code of sendto
*/
int UdpSocket::sendTo(char* msg, sockaddr* to)
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
			port = (tempBuffer[DST_PORT_POSITION] << EIGHT_BYTES) | tempBuffer[DST_PORT_POSITION + 1];
			length = (tempBuffer[LENGTH_POSITION] << EIGHT_BYTES) | tempBuffer[LENGTH_POSITION + 1];
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
