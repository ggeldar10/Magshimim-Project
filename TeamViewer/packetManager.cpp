#include "packetManager.h"

PacketManager::PacketManager(bool* keepAliveSwitch, bool* shutDownSwitch, std::mutex* switchesMtx, PipeManager* pipeManager)
    : _pipeManager(pipeManager), _keepAliveSwitch(keepAliveSwitch), _shutdownSwitch(shutDownSwitch), _switchesMtx(switchesMtx), _originScreenPoint(nullptr), _endScreenPoint(nullptr), _isScreenEdjusted(false)
{
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

double PacketManager::CalculateActualWidth(double originalWidth, double originalHeight, double containerWidth, double containerHeight)
{
    double ratioWidth = containerWidth / originalWidth;
    double ratioHeight = containerHeight / originalHeight;
    double ratio = (ratioWidth < ratioHeight) ? ratioWidth : ratioHeight;
    return originalWidth * ratio;
}

double PacketManager::CalculateActualHeight(double originalWidth, double originalHeight, double containerWidth, double containerHeight)
{
    double ratioWidth = containerWidth / originalWidth;
    double ratioHeight = containerHeight / originalHeight;
    double ratio = (ratioWidth < ratioHeight) ? ratioWidth : ratioHeight;
    return originalHeight * ratio;
}

POINT* PacketManager::getOriginScreenPoint() const
{
    return this->_originScreenPoint;
}

POINT* PacketManager::getEndScreenPoint() const
{
    return this->_endScreenPoint;
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
        auto screenPacket = dynamic_cast<const ScreenDataPacket*>(dataPacket.get());
        handleScreenDataPacket(std::unique_ptr<const ScreenDataPacket>(screenPacket));
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

void PacketManager::handleScreenDataPacket(std::unique_ptr<const ScreenDataPacket> screenPacket)
{
    _pipeManager->sendToPipe(screenPacket->getImageBytes());
    /*int controlledScreenHeight = screenPacket->getHeight();
    int controlledScreenWidth = screenPacket->getWidth();*/
    if (!this->_isScreenEdjusted)
    {
        std::lock_guard<std::mutex> pointsLock(this->_pointsMtx);
        int myScreenHeight = GetSystemMetrics(SM_CXSCREEN);
        int myScreenWidth = GetSystemMetrics(SM_CYSCREEN);

        int guiScreenHeight = CalculateActualHeight(myScreenWidth, myScreenHeight, myScreenWidth - 200, myScreenHeight);
        int guiScreenWidth = CalculateActualWidth(myScreenWidth, myScreenHeight, myScreenWidth - 200, myScreenHeight);

        this->_originScreenPoint = new POINT{ 200, myScreenHeight / 2 - guiScreenHeight / 2 };
        this->_endScreenPoint = new POINT{ 200 + guiScreenWidth, myScreenHeight / 2 - guiScreenHeight / 2 + guiScreenHeight };

        this->_isScreenEdjusted = true;
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
