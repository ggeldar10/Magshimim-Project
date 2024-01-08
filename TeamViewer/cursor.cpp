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

bool makeCursorButtonAction(CursorButtonsActions action, int scrollValue)
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
    case MiddleButtonScroll:
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = scrollValue;
        break;
    default:
        throw MakeCursorActionException();
    }

    return SendInput(1, &input, sizeof(INPUT)) > 0;
}
