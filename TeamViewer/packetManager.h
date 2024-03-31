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

    void handleDataPacket(const DefaultDataPacket* dataPacket);
    void handleControlPacket(const DefaultControlPacket* controlPacket);
    void handleCursorDataPacket(const CursorDataPacket* cursorPacket);
    void handleKeyboardDataPacket(const KeyboardDataPacket* keyboardPacket);
    void handleScreenDataPacket(const ScreenDataPacket* screenPacket);
    void handleKeepAliveControlPacket(const DefaultControlPacket* keepAlivePacket);
    void handleShutdownControlPacket(const DefaultControlPacket* shutdownPacket);

public:
    PacketManager(bool* keepAliveSwitch, bool* shutdownSwitch, std::mutex* switchesMtx, PipeManager* pipeManager);
    void handlePacket(std::unique_ptr<const DefaultPacket> packet);
    std::unique_ptr<POINT> getOriginScreenPoint();
    std::unique_ptr<POINT> getEndScreenPoint();
    std::mutex* getPointsMtx();
};
