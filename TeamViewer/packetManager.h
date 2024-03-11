#pragma once
#include "PacketParser.h"
#include "cursor.h"
#include "keyboard.h"

class PacketManager
{
public:
    PacketManager(bool* keepAliveSwitch, bool* shutdownSwitch, std::mutex* switchesMtx);
    void handlePacket(std::unique_ptr<DefaultPacket> packet);

private:
    bool* _keepAliveSwitch;
    bool* _shutdownSwitch;
    std::mutex* _switchesMtx;
    void handleDataPacket(DefaultDataPacket* dataPacket);
    void handleControlPacket(DefaultControlPacket* controlPacket);
    void handleCursorDataPacket(CursorDataPacket* cursorPacket);
    void handleKeyboardDataPacket(KeyboardDataPacket* keyboardPacket);
    void handleKeepAliveControlPacket(DefaultControlPacket* keepAlivePacket);
    void handleShutdownControlPacket(DefaultControlPacket* shutdownPacket);
};
