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
	char key;
}cursorActionDataPacket;











typedef struct defaultControlPacket : defaultPacket {
	controlPacketTypes controlPacketType;

}defaultControlPacket;