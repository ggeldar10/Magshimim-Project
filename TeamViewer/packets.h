#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#define UDP_HEADERS_SIZE 8
#define UDP_HEADER_SIZE 16
#define IP_HEADERS_SIZE 20
#define IP_SRT_PROTOCOL_NUMBER 160
#define IP_VERSION_HEADER_SIZE 4
#define MAX_IP_OPTIONS_SIZE 10
#define MAX_IP_SIZE 60
#define MIN_IP_SIZE 40


using namespace std;


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

enum DefaultPacketTypes
{
    DataPacket = 0, ControlPacket
};

struct DefaultPacket {
    DefaultPacketTypes packetType;
    unsigned int ackSequenceNumber;
    unsigned int packetSequenceNumber;
    time_t timeStamp;
    DefaultPacket* data;
};

struct DefaultDataPacket : DefaultPacket {
    DataPacketTypes dataPacketType;
    DefaultDataPacket* data;
};

struct CursorDataPacket : DefaultDataPacket {
    CursorActions action;
    int scrollValue;
    unsigned int x;
    unsigned int y;
};

struct KeyboardDataPacket : DefaultDataPacket {
    KeyboardKeysActions action;
    int keyCode;
};

typedef struct keyboardDataPacket : defaultDataPacket {
	keyboardKeysActions action;
	int keyCode;
}keyboardDataPacket;

struct DefaultControlPacket : DefaultPacket {
    ControlPacketTypes controlPacketType;
    DefaultControlPacket* data;
};

struct HandshakeControlPacket : DefaultControlPacket {
    std::string senderIP;
    bool encryption;
    unsigned int windowSize;
};

// There is no need for keep-alive, ACK, ACKACK, shutdown, congestion-warning, peer-error packet structs.
// Instead, we will send DefaultControlPacket with the right ControlPacketTypes.

struct NAKControlPacket : DefaultControlPacket {
    std::vector<unsigned int> lostSequenceNumbers;
};

typedef struct NAKControlPacket : defaultControlPacket {
	unsigned int lostSequenceNumbers[];
}NAKControlPacket;

typedef struct messageDropRequestControlPacket : defaultControlPacket {
	unsigned int lostSequenceNumbers[];
}messageDropRequestControlPacket;

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
