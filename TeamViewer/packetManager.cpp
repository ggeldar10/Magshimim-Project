#include "packetManager.h"

void PacketManager::handlePacket(std::unique_ptr<DefaultPacket> packet)
{
    switch (packet->getPacketType())
    {
    case DataPacket:
        handleDataPacket(dynamic_cast<DefaultDataPacket*>(packet.get()));
        break;
    case ControlPacket:
        handleControlPacket(dynamic_cast<DefaultControlPacket*>(packet.get()));
        break;
    default:
        throw PacketParserException("Error: Invalid packet type");
        break;
    }
}

void PacketManager::handleDataPacket(DefaultDataPacket* dataPacket)
{
    switch (dataPacket->getDataType())
    {
    case Cursor:
        handleCursorDataPacket(dynamic_cast<CursorDataPacket*>(dataPacket));
        break;
    case Keyboard:
        handleKeyboardDataPacket(dynamic_cast<KeyboardDataPacket*>(dataPacket));
        break;
    case Screen:
        // To Do
        break;
    case Chat:
        // To Do
        break;
    default:
        throw PacketParserException("Error: Invalid data type");
        break;
    }
}

void PacketManager::handleControlPacket(DefaultControlPacket* controlPacket)
{
    
    switch (controlPacket->getControlType())
    {
    case HANDSHAKE:
        break;
    case KEEPALIVE:
        break;
    case ACK:
        break;
    case NAK:
        break;
    case CongestionWarning:
        break;
    case SHUTDOWN:
        break;
    case ACKACK:
        break;
    case DROPREQ:
        break;
    case PEERERROR:
        break;
    default:
        throw PacketParserException("Error: Invalid control type");
        break;
    }
}

void PacketManager::handleCursorDataPacket(CursorDataPacket* cursorPacket)
{
    if (cursorPacket->getAction() == CursorPosition)
    {
        try 
        {
            setCursorPosition(cursorPacket->getLocation());
        }
        catch (GetCursorPositionException exception)
        {
            std::cerr << exception.what() << std::endl;
        }
    }
    else 
    {
        try
        {
            makeCursorButtonAction(cursorPacket->getAction(), cursorPacket->getScrollValue());
        }
        catch (CursorButtonActionException exception)
        {
            std::cerr << exception.what() << std::endl;
        }
    }
}

void PacketManager::handleKeyboardDataPacket(KeyboardDataPacket* keyboardPacket)
{
    try
    {
        makeKeyboardAction(keyboardPacket->getAction(), keyboardPacket->getKeyCode());
    }
    catch (KeyboardButtonActionException exception)
    {
        std::cerr << exception.what() << std::endl;
    }
}
