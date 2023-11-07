#include "UdpSocket.h"

UdpSocket::UdpSocket() : _socket(NULL), _port(0)
{
	this->_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	this->_port = 0;
}

UdpSocket::~UdpSocket()
{
	closesocket(this->_socket);
	this->_socket = NULL;
}

void UdpSocket::sendTo(std::string msg, sockaddr* to)
{
	if (this->_socket == NULL)
	{
		throw "Error socket is null"; //todo add an excpetion
	}
	send(this->_socket, msg.c_str(), msg.size(), 0);
}

void UdpSocket::sendTo(char* msg, sockaddr* to)
{
	if (this->_socket == NULL)
	{
		throw "Error socket is null"; //todo add an excpetion
	}
	send(this->_socket, msg, sizeof(msg), 0);
}

int UdpSocket::recv(std::string buffer, int len)
{
	return 0;
}

void UdpSocket::bind(int port, sockaddr* addrs)
{

}
