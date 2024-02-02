#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "windowsHeaders.h"

#define MAX_IP_OPTIONS_SIZE 10

enum DataPacketTypes
{
    Cursor = 0, Keyboard, Screen, Chat
};

enum CursorActions
{
    CursorPosition = 0, LeftButtonDown, LeftButtonUp, RightButtonDown, RightButtonUp, MiddleButtonDown, MiddleButtonUp, WheelScroll
};

enum KeyboardActions
{
    KeyUp = 0, KeyDown
};

enum ControlPacketTypes
{
    HANDSHAKE = 0, KEEPALIVE, ACK, NAK, CongestionWarning, SHUTDOWN, ACKACK, DROPREQ, PEERERROR
};

enum HandshakePhases : uint8_t
{
    INDUCTION_1 = 0, INDUCTION_2, SUMMARY_1, SUMMARY_2
};

enum DefaultPacketTypes
{
    DataPacket = 0, ControlPacket
};

enum IpPacketTypesOfServices { IPv4 = 0, IPv6, ICMPv4, ICMPv6, TCP, UDP, IGMP, IPsec, ARP, RARP };

class DefaultPacket {
protected:
    DefaultPacketTypes packetType;
    uint32_t ackSequenceNumber;
    uint32_t packetSequenceNumber;
    time_t timeStamp;
public:
    DefaultPacket(DefaultPacketTypes type, uint32_t ackNum, uint32_t packetNum, time_t time);
    DefaultPacketTypes getPacketType() const;
    uint32_t getAckSequenceNumber() const;
    uint32_t getPacketSequenceNumber() const;
    time_t getTimeStamp() const;
};

class DefaultDataPacket : public DefaultPacket {
protected:
    DataPacketTypes dataPacketType;
public:
    DefaultDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, DataPacketTypes dataPacketType);
    DataPacketTypes getDataType() const;
};

class CursorDataPacket : public DefaultDataPacket {
private:
    CursorActions action;
    int scrollValue;
    POINT location;
public:
    CursorDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, CursorActions action, int scrollValue, unsigned int x, unsigned int y);
    CursorActions getAction(CursorActions action);
    POINT getLocation() const;
    int getScrollValue() const;
};

class KeyboardDataPacket : public DefaultDataPacket {
private:
    KeyboardActions action;
    unsigned int keyCode;
public:
    KeyboardDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, KeyboardActions action, int keyCode);
    KeyboardActions getAction() const;
    unsigned int getKeyCode() const;
};

// Will add chat and screen data packets later

class DefaultControlPacket : public DefaultPacket {
protected:
    ControlPacketTypes controlPacketType;
public:
    DefaultControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, ControlPacketTypes controlPacketType);
    ControlPacketTypes getControlType() const;
};

class HandshakeControlPacket : public DefaultControlPacket {
private:
    bool isEncrypted;
    uint16_t encryption_key;
    uint32_t windowSize;
    uint32_t initialPacketSequenceNumber;
    uint32_t maxTransmission;
    HandshakePhases phase;
public:
    HandshakeControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, bool hasEncryption, uint16_t encryption_key, uint32_t windowSize, uint32_t initialPacketSequenceNumber, uint32_t maxTransmission, HandshakePhases phase);
    bool hasEncryption() const;
    uint16_t getEncryptionKey() const;
    uint32_t getWindowSize() const;
    uint32_t getInitialPacketSequenceNumber() const;
    uint32_t getMaxTransmission() const;
    HandshakePhases getPhase() const;
};

// There is no need for keep-alive, ACK, ACKACK, shutdown, congestion-warning, peer-error packet structs.
// Instead, we will send DefaultControlPacket with the right ControlPacketTypes.

class NAKControlPacket : public DefaultControlPacket {
private:
    std::vector<unsigned int> lostSequenceNumbers;
public:
    NAKControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums);
    const std::vector<unsigned int>& getLostSequenceNumbers() const;
};

class MessageDropRequestControlPacket : public DefaultControlPacket {
    std::vector<unsigned int> lostSequenceNumbers;
public:
    MessageDropRequestControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums);
    const std::vector<unsigned int>& getLostSequenceNumbers() const;

};


/*
 0                   1
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Flags|     Fragment Offset     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Header Checksum        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+         Source Address        +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+      Destination Address      +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            Options            |
+               +-+-+-+-+-+-+-+-+
|               |    Padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
class IpPacket {
private:
    uint8_t version;
    uint8_t lengthOfHeaders;
    uint8_t typeOfService;
    uint16_t totalLength;
    uint16_t identification;
    uint16_t fragmentOffsetIncludingFlags;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t headerChecksum;
    uint32_t srcAddrs;
    uint32_t dstAddrs;
    unsigned char options[MAX_IP_OPTIONS_SIZE];
public:
    IpPacket(uint8_t version, uint8_t lengthOfHeaders, uint8_t typeOfService,
        uint16_t totalLength, uint16_t identification, uint16_t fragmentOffsetIncludingFlags,
        uint8_t ttl, uint8_t protocol, uint16_t headerChecksum, uint32_t srcAddrs, uint32_t dstAddrs,
        const unsigned char* options);
    uint8_t getVersion() const;
    uint8_t getLengthOfHeaders() const;
    uint8_t getTypeOfService() const;
    uint16_t getTotalLength() const;
    uint16_t getIdentification() const;
    uint16_t getFragmentOffsetIncludingFlags() const;
    uint8_t getTtl() const;
    uint8_t getProtocol() const;
    uint16_t getHeaderChecksum() const;
    uint32_t getSrcAddrs() const;
    uint32_t getDstAddrs() const;
    const unsigned char* getOptions() const;
};

