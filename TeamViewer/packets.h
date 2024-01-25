#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>


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

// Will add chat and screen data packets later

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

struct MessageDropRequestControlPacket : DefaultControlPacket {
    std::vector<unsigned int> lostSequenceNumbers;
};
