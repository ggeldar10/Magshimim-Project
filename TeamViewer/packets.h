#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum DataPacketTypes
{
    CursorPosition = 0, CursorAction, Keyboard, Screen, Chat
};

enum CursorButtonsActions
{
    LeftButtonDown = 0, LeftButtonUp, RightButtonDown, RightButtonUp, MiddleButtonDown, MiddleButtonUp, MiddleButtonScroll
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

struct CursorPositionDataPacket : DefaultDataPacket {
    unsigned int x;
    unsigned int y;
};

struct CursorActionDataPacket : DefaultDataPacket {
    CursorButtonsActions action;
    int scrollValue;
};

struct KeyboardDataPacket : DefaultDataPacket {
    KeyboardKeysActions action;
    int keyCode;
};

// Will add chat and screen data packets later

struct DefaultControlPacket : DefaultPacket {
    ControlPacketTypes controlPacketType;
    DefaultControlPacket* data;
};

struct HandshakeControlPacket : DefaultControlPacket {
    string senderIP;
    bool encryption;
    unsigned int windowSize;
};

// There is no need for keep-alive, ACK, ACKACK, shutdown, congestion-warning, peer-error packet structs.
// Instead, we will send DefaultControlPacket with the right ControlPacketTypes.

struct NAKControlPacket : DefaultControlPacket {
    vector<unsigned int> lostSequenceNumbers;
};

struct MessageDropRequestControlPacket : DefaultControlPacket {
    vector<unsigned int> lostSequenceNumbers;
};
