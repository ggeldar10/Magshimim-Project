#include "UdpPacket.h"
#include "UdpSocket.h"
#include <iostream>

/*
this function puts the number inside the char array it is given in big endian order
input: the number to convert from, the char array to put to (must have at least two cells)
output: puts the number in the char array in the big endian order
*/
void UdpPacket::convertShortToChar(const unsigned short num, unsigned char* str) const
{
	str[0] = num >> EIGHT_BITS;
	str[1] = num & 0xFF;
}

UdpPacket::UdpPacket(const std::string& payload, const short srcPort, const short dstPort) :
	_payload(nullptr), _srcPort(srcPort), _dstPort(dstPort)
{
	this->_payload = new unsigned char[payload.size() + 1];
	memcpy_s(this->_payload, payload.size() + 1, payload.c_str(), payload.size());
	this->_payload[payload.size()] = '\0';
	this->_length = payload.size() + 1 + DATAGRAM_SIZE;
}

UdpPacket::~UdpPacket()
{
	delete[] _payload;
}

/*
this function convert a packet structure to a bytearray
input: the packet to convert from , the array to insert to
output: the byteArray with the packet
*/
void UdpPacket::convertPacketToChar(unsigned char* byteArr, int byteArrLen) const
{
	if (this->_length - DATAGRAM_SIZE > byteArrLen)
	{
		throw std::invalid_argument("the actual length is smaller than the byteArrayLength given");
	}
	convertShortToChar(this->_srcPort, byteArr);
	convertShortToChar(this->_dstPort, byteArr + DST_PORT_POSITION);
	convertShortToChar(this->_length, byteArr + LENGTH_POSITION);
	memcpy_s(byteArr, byteArrLen, this->_payload, this->_length - DATAGRAM_SIZE);
	byteArr[byteArrLen - 1] = '\0';
}

unsigned short UdpPacket::getLength() const
{
	return _length;
}

unsigned short UdpPacket::getDstPort() const
{
	return _dstPort;
}

unsigned short UdpPacket::getSrcPort() const
{
	return _srcPort;
}

unsigned short UdpPacket::getChecksum() const
{
	return _checksum;
}

const unsigned char* UdpPacket::getPayload() const
{
	return _payload;
}
