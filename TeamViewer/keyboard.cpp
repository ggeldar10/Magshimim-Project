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

void listenToKeyboard(bool* shutdownSwitch, std::mutex* switchesMtx, std::queue<std::vector<char>>& packetSendQueue, std::mutex* packetSendQueueMtx)
{
    bool runLoop = true;

    HookParameters hookParams;
    hookParams.packetSendQueue = &packetSendQueue;
    hookParams.packetSendQueueMtx = packetSendQueueMtx;

    std::unique_lock<std::mutex> switchLock(*switchesMtx);
    switchLock.unlock();

    try {
        SetHook(hookParams);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error setting hook: " << e.what() << std::endl;
    }
    MSG msg;

    while (runLoop) 
    {
        // PeekMessage checks the message queue and returns immediately
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(500);
        switchLock.lock();
        runLoop = !*shutdownSwitch;
        switchLock.unlock();
    }
    Unhook();
}

HHOOK hHook = nullptr;

void SetHook(HookParameters& hookParams)
{
    // Set the low-level keyboard hook with parameters
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(nullptr), reinterpret_cast<DWORD_PTR>(&hookParams));
    if (hHook == nullptr) {
        throw std::runtime_error("Failed to set keyboard hook.");
    }
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
    HookParameters* hookParams = reinterpret_cast<HookParameters*>(lParam);

    std::unique_lock<std::mutex> sendLock(*hookParams->packetSendQueueMtx);

    std::chrono::system_clock::time_point now;
    std::time_t currentTime;

    if (nCode >= 0)
    {
        // Extract the key code from the event
        KBDLLHOOKSTRUCT* pKeyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        int keyCode = pKeyStruct->vkCode;

        // Check if the key is pressed or released
        bool keyPressed = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool keyReleased = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        // Check if Shift key is pressed or released
        /*if (keyCode == VK_SHIFT)
        {
            shiftPressed = keyPressed;
        }*/

        // Print the key information
        if (keyPressed)
        {
            std::cout << "Key pressed: " << keyCode << std::endl;

            // Check if Shift key is pressed
            /*if (shiftPressed) {
                std::cout << " Shift key is pressed.";
            }*/
            now = std::chrono::system_clock::now();
            currentTime = std::chrono::system_clock::to_time_t(now);
            std::unique_ptr<KeyboardDataPacket> packetPtr = std::make_unique<KeyboardDataPacket>(-1, -1, currentTime, KeyUp, keyCode);
            hookParams->packetSendQueue->push(packetPtr->toBuffer());
        }
        else if (keyReleased)
        {
            std::cout << "Key released: " << keyCode << std::endl;
            now = std::chrono::system_clock::now();
            currentTime = std::chrono::system_clock::to_time_t(now);
            std::unique_ptr<KeyboardDataPacket> packetPtr = std::make_unique<KeyboardDataPacket>(-1, -1, currentTime, KeyDown, keyCode);
            hookParams->packetSendQueue->push(packetPtr->toBuffer());
        }
    }

    // Call the next hook in the chain
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

