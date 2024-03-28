#pragma once
#include "PacketParser.h"
#include "cursor.h"
#include "keyboard.h"
#include <memory>
#include "PipeManager.h"

class PacketManager
{
private:
    bool* _keepAliveSwitch;
    bool* _shutdownSwitch;
    std::mutex* _switchesMtx;

    PipeManager* _pipeManager;

    std::unique_ptr<POINT> _originScreenPoint;
    std::unique_ptr<POINT> _endScreenPoint;
    std::mutex _pointsMtx;
    bool _isScreenEdjusted;

    double CalculateActualWidth(double originalWidth, double originalHeight, double containerWidth, double containerHeight);
    double CalculateActualHeight(double originalWidth, double originalHeight, double containerWidth, double containerHeight);

    void handleDataPacket(std::unique_ptr<const DefaultDataPacket> dataPacket);
    void handleControlPacket(std::unique_ptr<const DefaultControlPacket> controlPacket);
    void handleCursorDataPacket(std::unique_ptr<const CursorDataPacket> cursorPacket);
    void handleKeyboardDataPacket(std::unique_ptr<const KeyboardDataPacket> keyboardPacket);
    void handleScreenDataPacket(std::unique_ptr<const ScreenDataPacket> screenPacket);
    void handleKeepAliveControlPacket(std::unique_ptr<const DefaultControlPacket> keepAlivePacket);
    void handleShutdownControlPacket(std::unique_ptr<const DefaultControlPacket> shutdownPacket);

public:
    PacketManager(bool* keepAliveSwitch, bool* shutdownSwitch, std::mutex* switchesMtx, PipeManager* pipeManager);
    void handlePacket(std::unique_ptr<const DefaultPacket> packet);
    std::unique_ptr<POINT> getOriginScreenPoint();
    std::unique_ptr<POINT> getEndScreenPoint();
    std::mutex* getPointsMtx();
};
