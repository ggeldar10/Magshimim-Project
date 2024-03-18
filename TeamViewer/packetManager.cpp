#include "packetManager.h"

PacketManager::PacketManager(bool* keepAliveSwitch, bool* shutDownSwitch, std::mutex* switchesMtx)
{
    this->_keepAliveSwitch = keepAliveSwitch;
    this->_shutdownSwitch = shutDownSwitch;
    this->_switchesMtx = switchesMtx;
}

void PacketManager::handlePacket(std::unique_ptr<const DefaultPacket> packet)
{
    switch (packet->getPacketType())
    {
    case DataPacket:
    {
        auto dataPacket = dynamic_cast<const DefaultDataPacket*>(packet.get());
        handleDataPacket(std::unique_ptr<const DefaultDataPacket>(dataPacket));
        break;
    }
    case ControlPacket:
    {
        auto controlPacket = dynamic_cast<const DefaultControlPacket*>(packet.get());
        handleControlPacket(std::unique_ptr<const DefaultControlPacket>(controlPacket));
        break;
    }
    default:
    {
        throw PacketParserException("Error: Invalid packet type");
        break;
    }
    }
}

void PacketManager::handleDataPacket(std::unique_ptr<const DefaultDataPacket> dataPacket)
{
    switch (dataPacket->getDataType())
    {
    case Cursor:
    {
        auto cursorPacket = dynamic_cast<const CursorDataPacket*>(dataPacket.get());
        handleCursorDataPacket(std::unique_ptr<const CursorDataPacket>(cursorPacket));
        break;
    }
    case Keyboard:
    {
        auto keyboardPacket = dynamic_cast<const KeyboardDataPacket*>(dataPacket.get());
        handleKeyboardDataPacket(std::unique_ptr<const KeyboardDataPacket>(keyboardPacket));
        break;
    }
    case Screen:
    {
        // To Do
        break;
    }
    case Chat:
    {
        // To Do
        break;
    }
    default:
    {
        throw PacketParserException("Error: Invalid data type");
        break;
    }
    }
}

void PacketManager::handleControlPacket(std::unique_ptr<const DefaultControlPacket> controlPacket)
{
    
    switch (controlPacket->getControlType())
    {
    case HANDSHAKE:
    {
        break;
    }
    case KEEPALIVE:
    {
        handleKeepAliveControlPacket(std::move(controlPacket));
        break;
    }
    case ACK:
    {
        break;
    }
    case NAK:
    {
        break;
    }
    case CongestionWarning:
    {
        break;
    }
    case SHUTDOWN:
    {
        handleShutdownControlPacket(std::move(controlPacket));
        break;
    }
    case ACKACK:
    {
        break;
    }
    case DROPREQ:
    {
        break;
    }
    case PEERERROR:
    {
        break;
    }
    default:
    {
        throw PacketParserException("Error: Invalid control type");
        break;
    }
    }
}

void PacketManager::handleCursorDataPacket(std::unique_ptr<const CursorDataPacket> cursorPacket)
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

void PacketManager::handleKeyboardDataPacket(std::unique_ptr<const KeyboardDataPacket> keyboardPacket)
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

void PacketManager::handleKeepAliveControlPacket(std::unique_ptr<const DefaultControlPacket> keepAlivePacket)
{
    std::lock_guard<std::mutex> lock(*this->_switchesMtx);
    *this->_keepAliveSwitch = true;
}

void PacketManager::handleShutdownControlPacket(std::unique_ptr<const DefaultControlPacket> shutdownPacket)
{
    std::lock_guard<std::mutex> lock(*this->_switchesMtx);
    *this->_shutdownSwitch = true;
}
