#include "cursor.h"


POINT getCursorPosition()
{
    POINT point;
    if (GetCursorPos(&point))
    {
        return point;
    }
    else
    {
        throw GetCursorPositionException();
    }
}

bool setCursorPosition(POINT point)
{
    if (SetCursorPos(point.x, point.y))
    {
        return true;
    }
    else
    {
        throw SetCursorPositionException();
    }
}

bool makeCursorButtonAction(CursorActions action, int scrollValue)
{
    INPUT input = { 0 };

    input.type = INPUT_MOUSE;

    switch (action)
    {
    case LeftButtonDown:
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case LeftButtonUp:
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        break;
    case RightButtonDown:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    case RightButtonUp:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        break;
    case MiddleButtonDown:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    case MiddleButtonUp:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        break;
    case WheelScroll:
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = scrollValue;
        break;
    default:
        throw MakeCursorActionException();
    }

    return SendInput(1, &input, sizeof(INPUT)) > 0;
}

DefaultDataPacket* createPacket(CursorActions action, int ackSequenceNumber, int packetSequenceNumber)
{
    CursorDataPacket packet;
    packet.packetType = DataPacket;
    packet.dataPacketType = Cursor;
    packet.timeStamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    packet.ackSequenceNumber = ackSequenceNumber;
    packet.packetSequenceNumber = packetSequenceNumber;
    packet.action = action;

    switch (action)
    {
    case CursorPosition:
        POINT point;
        point = getCursorPosition();
        packet.x = point.x;
        packet.y = point.y;
        break;

    /*case WheelScroll:
        packet.scrollValue = scrollValue;
        break;*/

    default:
        packet.x = -1;
        packet.y = -1;
        packet.scrollValue = 0;
        break;
    }
    return &packet;
}
