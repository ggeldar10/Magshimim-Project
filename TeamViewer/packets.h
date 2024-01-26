#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#define MAX_IP_OPTIONS_SIZE 10

enum DataPacketTypes
{
    Cursor = 0, Keyboard, Screen, Chat
};

enum CursorActions
{
    CursorPosition = 0, LeftButtonDown, LeftButtonUp, RightButtonDown, RightButtonUp, MiddleButtonDown, MiddleButtonUp, WheelScroll
};

enum KeyboardKeysActions
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

enum IpPacketTypesOfServices { IPv4, IPv6, ICMPv4, ICMPv6, TCP, UDP, IGMP, IPsec, ARP, RARP };

class DefaultPacket {
private:
    DefaultPacketTypes packetType;
    uint32_t ackSequenceNumber;
    uint32_t packetSequenceNumber;
    time_t timeStamp;
    DefaultPacket* data;

public:
    DefaultPacket(DefaultPacketTypes type, uint32_t ackNum, uint32_t packetNum, time_t time);
    void setData(DefaultPacket* data);
    DefaultPacketTypes getPacketType() const;
    uint32_t getAckSequenceNumber() const;
    uint32_t getPacketSequenceNumber() const;
    time_t getTimeStamp() const;
    DefaultPacket* getData() const;
};

class DefaultDataPacket : DefaultPacket {
private:
    DataPacketTypes dataPacketType;
    DefaultDataPacket* data;
};

class CursorDataPacket : DefaultDataPacket {
    CursorActions action;
    int scrollValue;
    unsigned int x;
    unsigned int y;
};

class KeyboardDataPacket : DefaultDataPacket {
private:
    KeyboardKeysActions action;
    int keyCode;
};

// Will add chat and screen data packets later

class DefaultControlPacket : DefaultPacket {
private:
    ControlPacketTypes controlPacketType;
    DefaultControlPacket* data;
};

class HandshakeControlPacket : DefaultControlPacket {
    bool hasEncryption;
    uint16_t encryption_key;
    uint32_t windowSize;
    uint32_t initialPacketSequenceNumber;
    uint32_t maxTransmission;
    HandshakePhases phase;
};

// There is no need for keep-alive, ACK, ACKACK, shutdown, congestion-warning, peer-error packet structs.
// Instead, we will send DefaultControlPacket with the right ControlPacketTypes.

class NAKControlPacket : DefaultControlPacket {
    std::vector<unsigned int> lostSequenceNumbers;
};

class MessageDropRequestControlPacket : DefaultControlPacket {
    std::vector<unsigned int> lostSequenceNumbers;
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
struct IpPacket
{
    uint8_t version;
    uint8_t lengthOfHeaders;
    uint8_t typeOfService; // we need to look at it to know what to send
    uint16_t totalLength;
    uint16_t identification;
    uint16_t fragmentOffsetIncludingFlags; // about this one it does fragmentation to the data if sent too big
    uint8_t ttl;
    uint8_t protocol;
    uint16_t headerChecksum;
    uint32_t srcAddrs;
    uint32_t dstAddrs;
    unsigned char options[MAX_IP_OPTIONS_SIZE]; // we need to check if we have important options here
};
