#pragma once
#include <ctime>
#include <iostream>
#include <string>
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
}cursorActionDataPacket;

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