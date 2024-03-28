#include "cursor.h"


POINT getCursorPosition()
{
    POINT point;
    if (!GetCursorPos(&point))
    {
        throw GetCursorPositionException("Error: Failed to get cursor position");
    }
    return point;
}

void setCursorPosition(const POINT& point)
{
    if (!SetCursorPos(point.x, point.y))
    {
        throw SetCursorPositionException("Error: Failed to set cursor position");
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
        throw CursorButtonActionException("Error: Illegal cursor button action");
    }

    if (SendInput(1, &input, sizeof(INPUT)) != 1)
    {
        throw(CursorButtonActionException("Error: Failed to make the cursor button action"));
    }
}

void listenToCursor(bool* shutdownSwitch, std::mutex* switchesMtx, std::queue<std::vector<char>>& packetSendQueue, std::mutex* packetSendQueueMtx, std::unique_ptr<POINT> originPoint, std::unique_ptr<POINT> endPoint, std::mutex* pointsMtx)
{
    bool runLoop = true;

    std::chrono::system_clock::time_point now;
    std::time_t currentTime;

    std::unique_lock<std::mutex> switchLock(*switchesMtx);
    switchLock.unlock();
    std::unique_lock<std::mutex> sendLock(*packetSendQueueMtx);
    sendLock.unlock();
    std::unique_lock<std::mutex> pointsLock(*pointsMtx);
    pointsLock.unlock();

    CursorActions previousAction = CursorPosition;

    while (runLoop)
    {
        try {
            POINT position = getCursorPosition();
            std::cout << "Cursor position - x: " << position.x << ", y: " << position.y << std::endl;
            pointsLock.lock();
            if (originPoint != nullptr && endPoint != nullptr)
            {
                if (position.x >= originPoint->x && position.y >= originPoint->y && position.x <= endPoint->x && position.y <= endPoint->y)
                {
                    now = std::chrono::system_clock::now();
                    currentTime = std::chrono::system_clock::to_time_t(now);
                    std::unique_ptr<CursorDataPacket> packetPtr = std::make_unique<CursorDataPacket>(-1, -1, currentTime, CursorPosition, 0, position.x, position.y);
                    sendLock.lock();
                    packetSendQueue.push(packetPtr->toBuffer());
                    sendLock.unlock();
                }
            }
            pointsLock.unlock();
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

                now = std::chrono::system_clock::now();
                currentTime = std::chrono::system_clock::to_time_t(now);
                if (previousAction != LeftButtonDown)
                {
                    std::unique_ptr<CursorDataPacket> packetPtr = std::make_unique<CursorDataPacket>(-1, -1, currentTime, LeftButtonDown, 0, 0, 0);
                    sendLock.lock();
                    packetSendQueue.push(packetPtr->toBuffer());
                    sendLock.unlock();
                    previousAction = LeftButtonDown;
                }
               
            }
            else if (previousAction == LeftButtonDown)
            {
                // Left mouse button released
                now = std::chrono::system_clock::now();
                currentTime = std::chrono::system_clock::to_time_t(now);
                std::unique_ptr<CursorDataPacket> packetPtr = std::make_unique<CursorDataPacket>(-1, -1, currentTime, LeftButtonUp, 0, 0, 0);
                sendLock.lock();
                packetSendQueue.push(packetPtr->toBuffer());
                sendLock.unlock();
                previousAction = LeftButtonUp;
            }

            // Check the state of the right mouse button
            if (GetAsyncKeyState(VK_RBUTTON) & IS_PRESSED) {
                std::cout << "Left mouse button pressed." << std::endl;

                now = std::chrono::system_clock::now();
                currentTime = std::chrono::system_clock::to_time_t(now);
                if (previousAction != RightButtonDown)
                {
                    std::unique_ptr<CursorDataPacket> packetPtr = std::make_unique<CursorDataPacket>(-1, -1, currentTime, RightButtonDown, 0, 0, 0);
                    sendLock.lock();
                    packetSendQueue.push(packetPtr->toBuffer());
                    sendLock.unlock();
                    previousAction = RightButtonDown;
                }

            }
            else if (previousAction == RightButtonDown)
            {
                // Right mouse button released
                now = std::chrono::system_clock::now();
                currentTime = std::chrono::system_clock::to_time_t(now);
                std::unique_ptr<CursorDataPacket> packetPtr = std::make_unique<CursorDataPacket>(-1, -1, currentTime, RightButtonUp, 0, 0, 0);
                sendLock.lock();
                packetSendQueue.push(packetPtr->toBuffer());
                sendLock.unlock();
                previousAction = RightButtonUp;
            }

            // Check the state of the middle mouse button
            if (GetAsyncKeyState(VK_MBUTTON) & IS_PRESSED) {
                std::cout << "Left mouse button pressed." << std::endl;

                now = std::chrono::system_clock::now();
                currentTime = std::chrono::system_clock::to_time_t(now);
                if (previousAction != MiddleButtonDown)
                {
                    std::unique_ptr<CursorDataPacket> packetPtr = std::make_unique<CursorDataPacket>(-1, -1, currentTime, MiddleButtonDown, 0, 0, 0);
                    sendLock.lock();
                    packetSendQueue.push(packetPtr->toBuffer());
                    sendLock.unlock();
                    previousAction = MiddleButtonDown;
                }

            }
            else if (previousAction == MiddleButtonDown)
            {
                // Right mouse button released
                now = std::chrono::system_clock::now();
                currentTime = std::chrono::system_clock::to_time_t(now);
                std::unique_ptr<CursorDataPacket> packetPtr = std::make_unique<CursorDataPacket>(-1, -1, currentTime, MiddleButtonUp, 0, 0, 0);
                sendLock.lock();
                packetSendQueue.push(packetPtr->toBuffer());
                sendLock.unlock();
                previousAction = MiddleButtonUp;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            runLoop = false;
        }
        // Sleep for a short duration to avoid high CPU usage
        Sleep(200);
        switchLock.lock();
        runLoop = !*shutdownSwitch;
        switchLock.unlock();
    }
}
