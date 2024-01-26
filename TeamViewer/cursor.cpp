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
