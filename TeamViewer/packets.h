#pragma once
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

enum dataPacketTypes
{
	cursorPosition = 0, cursorAction, keyboard, screen, chat
};

enum cursorButtonsActions
{
	leftButtonDown = 0, leftButtonUp, rightButtonDown, rightButtonUp, middleButtonDwon, middleButtonUp, middleButtonScroll
};

enum keyboardKeysActions
{
	keyUp = 0, keyDown
};

enum controlPacketTypes
{
	HANDSHAKE = 0, KEEPALIVE, ACK, NAK, Congestion_Warning, SHUTDOWN, ACKACK, DROPREQ, PEERERROR
};

enum defaultPacketTypes
{
	dataPacket = 0, controlPacket
};

typedef struct defaultPacket {
	defaultPacketTypes packetType;
	unsigned int ackSequenceNumber;
	unsigned int packetSequenceNumber;
	time_t timeStamp;
	defaultPacket* data;
}defaultPacket;

typedef struct defaultDataPacket : defaultPacket {
	dataPacketTypes dataPacketType;
	defaultDataPacket* data;
}defaultDataPacket;

typedef struct cursorPositionDataPacket : defaultDataPacket {
	unsigned int x;
	unsigned int y;
}cursorPositionDataPacket;

typedef struct cursorActionDataPacket : defaultDataPacket {
	cursorButtonsActions action;
	int scrollValue;
}cursorActionDataPacket;

typedef struct keyboardDataPacket : defaultDataPacket {
	keyboardKeysActions action;
	int keyCode;
}keyboardDataPacket;

//will add chat and screen data packets later

typedef struct defaultControlPacket : defaultPacket {
	controlPacketTypes controlPacketType;
	defaultControlPacket* data;
}defaultControlPacket;

typedef struct handshakeControlPacket : defaultControlPacket {
	string senderIP;
	bool encryption;
	unsigned int windowSize;
}handshakeControlPacket;

//There is no need for keep-alive, ACK, ACKACK, shutdown, congestion-warning, peer-error packet staructs.
//Instead, we will send defaultControlPacket with the right controlPacketTypes.

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
