#include "packetManager.h"

PacketManager::PacketManager(bool* keepAliveSwitch, bool* shutDownSwitch, std::mutex* switchesMtx, PipeManager* pipeManager)
    : _pipeManager(pipeManager), _keepAliveSwitch(keepAliveSwitch), _shutdownSwitch(shutDownSwitch), _switchesMtx(switchesMtx), _originScreenPoint(nullptr), _endScreenPoint(nullptr), _isScreenEdjusted(false)
{
    this->_originScreenPoint = std::make_unique<POINT>();
    this->_originScreenPoint->x = 0;
    this->_originScreenPoint->y = 0;
    this->_endScreenPoint = std::make_unique<POINT>();
    this->_endScreenPoint->x = 1980;
    this->_endScreenPoint->y = 1020;

    this->_isScreenEdjusted = true;
}

void PacketManager::handlePacket(std::unique_ptr<const DefaultPacket> packet)
{
    switch (packet->getPacketType())
    {
    case DataPacket:
    {
        handleDataPacket(reinterpret_cast<const DefaultDataPacket*>(packet.get()));
        break;
    }
    case ControlPacket:
    {
        handleControlPacket(reinterpret_cast<const DefaultControlPacket*>(packet.get()));
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

std::unique_ptr<POINT> PacketManager::getOriginScreenPoint() 
{
    return std::move(this->_originScreenPoint);
}

std::unique_ptr<POINT> PacketManager::getEndScreenPoint() 
{
    return std::move(this->_endScreenPoint);
}

std::mutex* PacketManager::getPointsMtx()
{
    return &this->_pointsMtx;
}


void PacketManager::handleDataPacket(const DefaultDataPacket* dataPacket)
{
    switch (dataPacket->getDataType())
    {
    case Cursor:
    {
        handleCursorDataPacket(reinterpret_cast<const CursorDataPacket*>(dataPacket));
        break;
    }
    case Keyboard:
    {
        
        handleKeyboardDataPacket(reinterpret_cast<const KeyboardDataPacket*>(dataPacket));
        break;
    }
    case Screen:
    {
        handleScreenDataPacket(reinterpret_cast<const ScreenDataPacket*>(dataPacket));
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

void PacketManager::handleControlPacket(const DefaultControlPacket* controlPacket)
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

void PacketManager::handleCursorDataPacket(const CursorDataPacket* cursorPacket)
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

void PacketManager::handleKeyboardDataPacket(const KeyboardDataPacket* keyboardPacket)
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

void PacketManager::handleScreenDataPacket(const ScreenDataPacket* screenPacket)
{
    // inserts the data to a buffer to send to the gui
    std::vector<char> buffer;
    IntCharUnion convertor = {0};
    convertor.num = screenPacket->getStartSequenceNumber();
    buffer.insert(buffer.end(), convertor.bytes, convertor.bytes + sizeof(uint32_t));
    convertor.num = screenPacket->getEndSequenceNumber();
    buffer.insert(buffer.end(), convertor.bytes, convertor.bytes + sizeof(uint32_t));
    convertor.num = screenPacket->getPacketSequenceNumber();
    buffer.insert(buffer.end(), convertor.bytes, convertor.bytes + sizeof(uint32_t));
    buffer.insert(buffer.end(), screenPacket->getImageBytes().begin(), screenPacket->getImageBytes().end());
    _pipeManager->sendToPipe(buffer);

    
    int controlledScreenHeight = screenPacket->getHeight();
    int controlledScreenWidth = screenPacket->getWidth();
    if (!this->_isScreenEdjusted)
    {
        std::lock_guard<std::mutex> pointsLock(this->_pointsMtx);
        /*int myScreenHeight = GetSystemMetrics(SM_CXSCREEN);
        int myScreenWidth = GetSystemMetrics(SM_CYSCREEN);

        int guiScreenHeight = CalculateActualHeight(controlledScreenWidth, controlledScreenHeight, myScreenWidth - 200, myScreenHeight);
        int guiScreenWidth = CalculateActualWidth(controlledScreenWidth, controlledScreenHeight, myScreenWidth - 200, myScreenHeight);

        this->_originScreenPoint = std::make_unique<POINT>();
        this->_originScreenPoint->x = 200;
        this->_originScreenPoint->y = myScreenHeight / 2 - guiScreenHeight / 2;
        this->_endScreenPoint = std::make_unique<POINT>();
        this->_endScreenPoint->x = 200 + guiScreenWidth;
        this->_endScreenPoint->y = myScreenHeight / 2 - guiScreenHeight / 2 + guiScreenHeight;*/
        /*this->_isScreenEdjusted = true;*/
    }
}

void PacketManager::handleKeepAliveControlPacket(const DefaultControlPacket* keepAlivePacket)
{
    std::lock_guard<std::mutex> lock(*this->_switchesMtx);
    *this->_keepAliveSwitch = true;
}

void PacketManager::handleShutdownControlPacket(const DefaultControlPacket* shutdownPacket)
{
    std::lock_guard<std::mutex> lock(*this->_switchesMtx);
    *this->_shutdownSwitch = true;
}
