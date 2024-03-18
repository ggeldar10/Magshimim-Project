#pragma once
#include "PacketParser.h"
#include "cursor.h"
#include "keyboard.h"
#include <memory>
class PacketManager
{
private:
    bool* _keepAliveSwitch;
    bool* _shutdownSwitch;
    std::mutex* _switchesMtx;

    void handleDataPacket(std::unique_ptr<const DefaultDataPacket> dataPacket);
    void handleControlPacket(std::unique_ptr<const DefaultControlPacket> controlPacket);
    void handleCursorDataPacket(std::unique_ptr<const CursorDataPacket> cursorPacket);
    void handleKeyboardDataPacket(std::unique_ptr<const KeyboardDataPacket> keyboardPacket);
    void handleScreenDataPacket(std::unique_ptr<const KeyboardDataPacket> screenPacket);
    void handleKeepAliveControlPacket(std::unique_ptr<const DefaultControlPacket> keepAlivePacket);
    void handleShutdownControlPacket(std::unique_ptr<const DefaultControlPacket> shutdownPacket);

public:
    PacketManager(bool* keepAliveSwitch, bool* shutdownSwitch, std::mutex* switchesMtx);
    void handlePacket(std::unique_ptr<const DefaultPacket> packet);
};
