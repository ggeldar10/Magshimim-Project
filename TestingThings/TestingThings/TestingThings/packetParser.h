#pragma once
#include "packets.h"
#include <vector>

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
    * Insert the value to the vector in big endian order
    * input:
    * addVector - a reference to the vector to insert to
    * value - the value to insert to the vector
    * output: none
    */
    template<typename htnSize>
    static void hostToNetworkIntoVector(std::vector<char>* addVector, htnSize value)
    {
        for (int i = 0; i < sizeof(htnSize); i++)
        {
            addVector->push_back(value & 0xFF);
            value >>= BYTE_IN_BITS;
        }
    }

    template<typename nthSize>
    static inline nthSize networkToHost(const std::vector<char>& buffer, int index)
    {
        if (index + sizeof(nthSize) > buffer.size())
        {
            std::cerr << "Error: buffer size is not big enough" << std::endl;
            throw "Error: buffer size is not big enough";
        }

        nthSize networkToHostNum = 0;

        for (int i = sizeof(nthSize) - 1; i >= 0; i--)
        {
            networkToHostNum <<= BYTE_IN_BITS;
            networkToHostNum |= buffer[index + i];
        }

        return networkToHostNum;
    }

    static std::vector<char> packetToBytes(const UdpPacket& udpHeaders, const DefaultPacket& strHeaders, const std::vector<char>* data);

    static IpPacket createIpPacketFromVector(const std::vector<char>& ipPacketBuffer);
    static std::unique_ptr<const DefaultPacket> createDefaultPacketFromVector(const std::vector<char>& defaultPacketBuffer, int& index);
    static std::unique_ptr<const DefaultDataPacket> createDefaultDataPacketFromVector(const std::vector<char>& defaultDataPacketBuffer, int& index);
    static std::unique_ptr<const CursorDataPacket> createCursorDataPacketFromVector(const std::vector<char>& cursorDataPacketBuffer);
    static std::unique_ptr<const KeyboardDataPacket> createKeyboardDataPacketFromVector(const std::vector<char>& keyboardDataPacketBuffer);
    static std::unique_ptr<const DefaultControlPacket> createDefaultControlPacketFromVector(const std::vector<char>& defaultControlPacketBuffer, int& index);
    static std::unique_ptr<const HandshakeControlPacket> createHandshakeControlPacketFromVector(const std::vector<char>& handshakeControlPacketBuffer);
    static std::unique_ptr<const NAKControlPacket> createNAKControlPacketFromVector(const std::vector<char>& nakControlPacketBuffer);
    static std::unique_ptr<const MessageDropRequestControlPacket> createMessageDropRequestControlPacketFromVector(const std::vector<char>& messageDropRequestControlPacketBuffer);
    static UdpPacket createUdpPacketFromVector(const std::vector<char>& udpPacketBuffer);
    static std::unique_ptr<const DefaultPacket> createPacketFromVectorGlobal(const std::vector<char>& globalPacketBuffer);
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
