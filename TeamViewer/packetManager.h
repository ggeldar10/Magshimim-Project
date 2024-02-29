#pragma once
#include "PacketParser.h"
#include "cursor.h"
#include "keyboard.h"

class PacketManager
{
public:
    PacketManager(bool* keepAliveSwitch);
    void handlePacket(std::unique_ptr<DefaultPacket> packet);

private:
    bool* _keepAliveSwitch;
    void handleDataPacket(DefaultDataPacket* dataPacket);
    void handleControlPacket(DefaultControlPacket* controlPacket);
    void handleCursorDataPacket(CursorDataPacket* cursorPacket);
    void handleKeyboardDataPacket(KeyboardDataPacket* keyboardPacket);
    void handleKeepAliveControlPacket(DefaultControlPacket* keepAlivePacket);
};
