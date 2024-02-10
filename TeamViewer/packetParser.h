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

class PacketParser
{
public:
	template<typename nthSize>
	inline static nthSize networkToHost(const std::string& buffer, int index);
	static std::vector<char> packetToBytes(const IpPacket& ipHeaders, const UdpPacket& udpHeaders, const DefaultPacket& strHeaders, const std::vector<char>* data);
	template<typename htnSize>
	static inline void hostToNetworkIntoVector(std::vector<char>* addVector, htnSize value);
	static IpPacket createIpPacketFromString(const std::string& ipPacketBuffer);
	static DefaultPacket createDefaultPacketFromString(const std::string& defaultPacketBuffer, int& index);
	static DefaultDataPacket createDefaultDataPacketFromString(const std::string& defaultDataPacketBuffer, int& index);
	static CursorDataPacket createCursorDataPacketFromString(const std::string& cursorDataPacketBuffer);
	static KeyboardDataPacket createKeyboardDataPacketFromString(const std::string& keyboardDataPacketBuffer);
	static DefaultControlPacket createDefaultControlPacketFromString(const std::string& defaultControlPacketBuffer, int& index);
	static HandshakeControlPacket createHandshakeControlPacketFromString(const std::string& handshakeControlPacketBuffer);
	static NAKControlPacket createNAKControlPacketFromString(const std::string& nakControlPacketBuffer);
	static MessageDropRequestControlPacket createMessageDropRequestControlPacketFromString(const std::string& messageDropRequestControlPacketBuffer);
	static UdpPacket createUdpPacketFromString(const std::string& udpPacketBuffer);

	static DefaultPacket* createPacketGlobal(const std::string& globalPacketBuffer);
};

