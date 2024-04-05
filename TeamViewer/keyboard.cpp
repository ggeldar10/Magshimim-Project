#include "keyboard.h"

void makeKeyboardAction(KeyboardActions action, int keyCode)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(keyCode);
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    if (action == KeyDown)
    {
        input.ki.dwFlags = 0;
    }
    else
    {
        input.ki.dwFlags = KEYEVENTF_KEYUP;
    }

    if (SendInput(1, &input, sizeof(INPUT)) != 1)
    {
        DWORD errorCode = GetLastError();
        throw KeyboardButtonActionException("Failed to send keyboard input.");
        std::cerr << "Error code: " << std::to_string(errorCode) << std::endl;
    }
}

void listenToKeyboard(bool* shutdownSwitch, std::mutex* switchesMtx, std::queue<std::vector<char>>& packetSendQueue, std::mutex* packetSendQueueMtx)
{
    HookParameters hookParams;
    hookParams.packetSendQueue = &packetSendQueue;
    hookParams.packetSendQueueMtx = packetSendQueueMtx;

    try {
        SetHook(hookParams);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error setting hook: " << e.what() << std::endl;
        return; // Exit the function if setting hook fails
    }

    MSG msg;
    while (!*shutdownSwitch)
    {
        // PeekMessage checks the message queue and returns immediately
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(100); // Adjust sleep time as needed
    }

    Unhook();
}

HHOOK hHook = nullptr;

thread_local HookParameters* g_HookParams = nullptr;

void SetHook(HookParameters& hookParams)
{
    g_HookParams = &hookParams;

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    if (hInstance == nullptr) {
        DWORD errorCode = GetLastError();
        throw std::runtime_error("Failed to get module handle. Error code: " + std::to_string(errorCode));
    }

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
    if (hHook == nullptr) {
        DWORD errorCode = GetLastError();
        throw std::runtime_error("Failed to set keyboard hook. Error code: " + std::to_string(errorCode));
    }
}

void Unhook()
{
    if (hHook != nullptr) {
        UnhookWindowsHookEx(hHook);
        hHook = nullptr;
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (g_HookParams != nullptr) {
        HookParameters* hookParams = g_HookParams;
        std::unique_lock<std::mutex> sendLock(*hookParams->packetSendQueueMtx);

        if (nCode >= 0)
        {
            KBDLLHOOKSTRUCT* pKeyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            int keyCode = pKeyStruct->vkCode;

            bool keyPressed = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            bool keyReleased = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

            if (keyPressed)
            {
                std::cout << "Key pressed: " << keyCode << std::endl;
                std::unique_ptr<KeyboardDataPacket> packetPtr = std::make_unique<KeyboardDataPacket>(-1, -1, currentTime, KeyUp, keyCode);
                hookParams->packetSendQueue->push(packetPtr->toBuffer());
            }
            else if (keyReleased)
            {
                std::cout << "Key released: " << keyCode << std::endl;
                std::unique_ptr<KeyboardDataPacket> packetPtr = std::make_unique<KeyboardDataPacket>(-1, -1, currentTime, KeyDown, keyCode);
                hookParams->packetSendQueue->push(packetPtr->toBuffer());
            }
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
