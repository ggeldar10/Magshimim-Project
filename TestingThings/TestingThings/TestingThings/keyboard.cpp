#include "keyboard.h"

void makeKeyboardAction(KeyboardActions action, int keyCode)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(keyCode);
    input.ki.wScan = 0;
    if (action == KeyDown) 
    {
        input.ki.dwFlags = 0;
    }
    else 
    {
        input.ki.dwFlags = KEYEVENTF_KEYUP;
    }
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    if (SendInput(1, &input, sizeof(INPUT)) != 1) 
    {
        throw(KeyboardButtonActionException("Failed to send keyboard input. Error code: " + GetLastError()));
       
    }
}
bool shiftPressed = false;

void listenToKeyboard()
{
    SetHook();  // Set the low-level keyboard hook
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Unhook();  // Unhook when the message loop exits
}

HHOOK hHook = nullptr;

void SetHook()
{
    // Set the low-level keyboard hook
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(nullptr), 0);
}

void Unhook()
{
    // Unhook the low-level keyboard hook
    if (hHook != nullptr) {
        UnhookWindowsHookEx(hHook);
        hHook = nullptr;
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        // Extract the key code from the event
        KBDLLHOOKSTRUCT* pKeyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        int keyCode = pKeyStruct->vkCode;

        // Check if the key is pressed or released
        bool keyPressed = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool keyReleased = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        // Check if Shift key is pressed or released
        if (keyCode == VK_SHIFT)
        {
            shiftPressed = keyPressed;
        }

        // Print the key information
        if (keyPressed || keyReleased)
        {
            std::cout << (keyPressed ? "Key pressed: " : "Key released: ") << keyCode;

            // Check if Shift key is pressed
            if (shiftPressed) {
                std::cout << " Shift key is pressed.";
            }

            std::cout << std::endl;

            // You can use shiftPressed and keyCode in your logic as needed
        }
    }

    // Call the next hook in the chain
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
