#include <iostream>
#include "windowsHeaders.h"
#include "cursor.h"


int main() {
    while (true) {
        POINT point = getCursorPosition();
        std::cout << "Cursor position - x: " << point.x << ", y: " << point.y << std::endl;

        // Check the state of the left mouse button
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            std::cout << "Left mouse button pressed." << std::endl;
        }

        // Check the state of the right mouse button
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
            std::cout << "Right mouse button pressed." << std::endl;
        }

        // Check the state of the middle mouse button
        if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
            std::cout << "Middle mouse button pressed." << std::endl;
        }

       /* MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            else if (msg.message == WM_MOUSEWHEEL) {
                int scrollAmount = GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA;
                std::cout << "Mouse wheel scrolled: " << scrollAmount << std::endl;
            }
        }*/

       
        // Sleep for a short duration to avoid high CPU usage
        Sleep(100);
    }
    
    return 0;
}
