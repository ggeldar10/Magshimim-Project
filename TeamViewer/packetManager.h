#pragma once
#include "PacketParser.h"
#include "cursor.h"
#include "keyboard.h"
#include "mutex"
class PacketManager
{
public:
    PacketManager(bool* keepAliveSwitch, std::mutex* switchesMtx);
    void handlePacket(std::unique_ptr<DefaultPacket> packet);

private:
    bool* _keepAliveSwitch;
    std::mutex* _keepAliveMtx;
    void handleDataPacket(DefaultDataPacket* dataPacket);
    void handleControlPacket(DefaultControlPacket* controlPacket);
    void handleCursorDataPacket(CursorDataPacket* cursorPacket);
    void handleKeyboardDataPacket(KeyboardDataPacket* keyboardPacket);
    void handleKeepAliveControlPacket(DefaultControlPacket* keepAlivePacket);
};
