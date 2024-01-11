#include <iostream>
#include "windowsHeaders.h"
#include "cursor.h"


int main() {
    //while (true) {
    //    // Check the state of the left mouse button
    //    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
    //        std::cout << "Left mouse button pressed." << std::endl;
    //    }

    //    // Check the state of the right mouse button
    //    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
    //        std::cout << "Right mouse button pressed." << std::endl;
    //    }

    //    // Check the state of the middle mouse button
    //    if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
    //        std::cout << "Middle mouse button pressed." << std::endl;
    //    }

    //   /* MSG msg;
    //    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    //        if (msg.message == WM_QUIT) {
    //            break;
    //        }
    //        else if (msg.message == WM_MOUSEWHEEL) {
    //            int scrollAmount = GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA;
    //            std::cout << "Mouse wheel scrolled: " << scrollAmount << std::endl;
    //        }
    //    }*/

    //    POINT point = getCursorPosition();
    //    std::cout << "Cursor position - x: " << point.x << ", y: " << point.y << std::endl;

    //    // Sleep for a short duration to avoid high CPU usage
    //    Sleep(100);
    //}
    // Get the current time point
    auto currentTimePoint = std::chrono::system_clock::now();

    // Convert the time point to a time_t object
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // Convert the time_t to a string representation
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &currentTime);

    // Print the current time
    std::cout << "Current time: " << buffer;
    return 0;
}
