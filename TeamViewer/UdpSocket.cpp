#include "UdpSocket.h"

UdpSocket::UdpSocket() : _socket(NULL), _port(0)
{
	this->_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (this->_socket < 0)
	{
		throw "Error creating the socket";
	}
	this->_port = 0;
}

UdpSocket::~UdpSocket()
{
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

int UdpSocket::recv(std::string buffer, int len)
{
	return 0;
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
		throw "Error: socket is null"
	}
	bind(this->_socket, addrs, sizeof(*addrs));
	this->_port = port;
}
