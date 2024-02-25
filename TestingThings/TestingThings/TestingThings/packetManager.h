#pragma once
#include "PacketParser.h"
#include "cursor.h"
#include "keyboard.h"

class PacketManager
{
public:
    void handlePacket(std::unique_ptr<DefaultPacket> packet);

private:
    void handleDataPacket(DefaultDataPacket* dataPacket);
    void handleControlPacket(DefaultControlPacket* controlPacket);
    void handleCursorDataPacket(CursorDataPacket* cursorPacket);
    void handleKeyboardDataPacket(KeyboardDataPacket* keyboardPacket);
};
