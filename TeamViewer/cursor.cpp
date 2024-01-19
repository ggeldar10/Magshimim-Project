#include "cursor.h"


POINT getCursorPosition()
{
    POINT point;
    try {
        GetCursorPos(&point);
        return point;
    }
    catch(const std::exception& e)
    {
        throw GetCursorPositionException(e.what());
    }
}

void setCursorPosition(const POINT& point)
{
    try{
        SetCursorPos(point.x, point.y);
    }
    catch (const std::exception& e)
    {
        throw SetCursorPositionException(e.what());
    }
}

void makeCursorButtonAction(const CursorActions action, const int scrollValue)
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
        throw CursorButtonActionException_IllegalAction();
    }

    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        throw(CursorButtonActionException_ActionFailed());
    }
}

CursorDataPacket createPacket(const CursorActions action, const int ackSequenceNumber, const int packetSequenceNumber, const POINT position, const int scrollValue)
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
        packet.x = position.x;
        packet.y = position.y;
        break;

    case WheelScroll:
        packet.scrollValue = scrollValue;
        break;

    default:
        packet.x = -1;
        packet.y = -1;
        packet.scrollValue = 0;
        break;
    }
    return packet;
}

void listenToCursor()
{
    bool runLoop = true;
    while (runLoop) 
    {
        try {
            POINT point = getCursorPosition();
            std::cout << "Cursor position - x: " << point.x << ", y: " << point.y << std::endl;
        }
        catch (GetCursorPositionException& e)
        {
            std::cerr << e.what() << std::endl;
            runLoop = false;
        }

        try
        {
            // Check the state of the left mouse button
            if (GetAsyncKeyState(VK_LBUTTON) & IS_PRESSED) {
                std::cout << "Left mouse button pressed." << std::endl;
            }

            // Check the state of the right mouse button
            if (GetAsyncKeyState(VK_RBUTTON) & IS_PRESSED) {
                std::cout << "Right mouse button pressed." << std::endl;
            }

            // Check the state of the middle mouse button
            if (GetAsyncKeyState(VK_MBUTTON) & IS_PRESSED) {
                std::cout << "Middle mouse button pressed." << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            runLoop = false;
        }
        // Sleep for a short duration to avoid high CPU usage
        Sleep(100);
    }
}
