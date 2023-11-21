#pragma once
#include <string>

class UdpPacket
{
protected:
	unsigned short _srcPort;
	unsigned short _dstPort;
	unsigned short _length;
	unsigned short _checksum;
	unsigned char* _payload;


	void convertShortToChar(const unsigned short num, unsigned char* str) const;

public:
	UdpPacket(const std::string& payload, const short srcPort, const short dstPort);
	~UdpPacket();
	virtual void convertPacketToChar(unsigned char* byteArr, const int byteArrLen) const;
	unsigned short getLength() const;
	unsigned short getDstPort() const;
	unsigned short getSrcPort() const;
	unsigned short getChecksum() const;
	const unsigned char* getPayload() const;
};



