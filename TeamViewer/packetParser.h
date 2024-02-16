#pragma once
#include "packets.h"
#include "SrtSocket.h"

#define UDP_HEADERS_SIZE 8
#define UDP_HEADER_SIZE 16
#define IP_HEADERS_SIZE 20
#define IP_SRT_PROTOCOL_NUMBER 160
#define IP_VERSION_HEADER_SIZE 4
#define MAX_IP_SIZE 40
#define MIN_IP_SIZE 20
#define FOUR_BITS 4
#define TURN_WORD_TO_BYTES_FACTOR 4
#define BYTE_IN_BITS 8
#define IPV4 4
#define DEFAULT_TTL 64
#define UDP_PROTOCOL_CODE 17
#define DEFUALT_MTU_SIZE 1024
#define DEFUALT_MAX_TRANSMISSION 10
#define HANDSHAKE_PACKET_SIZE 19
#define IP_FIRST_HEADER_ROW 4

class PacketParser
{
public:
	/*
	* insert the value to the vector in a big endian order
	* input:
	* addVector - a pointer to the vector to insert to
	* value - the value to insert to the vector
	* output: none
	*/
	template<typename htnSize>
	static void hostToNetworkIntoVector(std::vector<char>* addVector, htnSize value)
	{
		htnSize andFactor = 0xFF << ((sizeof(htnSize) - 1) * BYTE_IN_BITS);
		for (int i = 0; i < sizeof(htnSize); i++)
		{
			addVector->push_back(value & andFactor);
			andFactor >>= BYTE_IN_BITS;
		}
	}
	template<typename nthSize>
	static inline nthSize networkToHost(const std::string& buffer, int index)
	{
		if (index + sizeof(nthSize) > buffer.size())
		{
			std::cerr << "Error buffer size is not big enough" << std::endl;
			throw "Error buffer size is not big enough";
		}
		nthSize networkToHostNum = 0;
		for (int i = 0; i < sizeof(nthSize); i++)
		{
			networkToHostNum = networkToHostNum << BYTE_IN_BITS;
			networkToHostNum = networkToHostNum | static_cast<nthSize>(buffer[index + i]);
		}
		if (sizeof(nthSize) >= sizeof(uint32_t))
		{
			return ntohl(networkToHostNum);
		}
		return ntohs(networkToHostNum);
	}

	static std::vector<char> packetToBytes(const UdpPacket& udpHeaders, const DefaultPacket& strHeaders, const std::vector<char>* data);

	static IpPacket createIpPacketFromString(const std::string& ipPacketBuffer);
	static DefaultPacket* createDefaultPacketFromString(const std::string& defaultPacketBuffer, int& index);
	static DefaultDataPacket* createDefaultDataPacketFromString(const std::string& defaultDataPacketBuffer, int& index);
	static CursorDataPacket* createCursorDataPacketFromString(const std::string& cursorDataPacketBuffer);
	static KeyboardDataPacket* createKeyboardDataPacketFromString(const std::string& keyboardDataPacketBuffer);
	static DefaultControlPacket* createDefaultControlPacketFromString(const std::string& defaultControlPacketBuffer, int& index);
	static HandshakeControlPacket* createHandshakeControlPacketFromString(const std::string& handshakeControlPacketBuffer);
	static NAKControlPacket* createNAKControlPacketFromString(const std::string& nakControlPacketBuffer);
	static MessageDropRequestControlPacket* createMessageDropRequestControlPacketFromString(const std::string& messageDropRequestControlPacketBuffer);
	static UdpPacket createUdpPacketFromString(const std::string& udpPacketBuffer);
	static DefaultPacket* createPacketFromStringGlobal(const std::string& globalPacketBuffer);
};

class PacketParserException : public std::exception
{
private:
	std::string message;

public:
	explicit PacketParserException(const char* msg) : message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};