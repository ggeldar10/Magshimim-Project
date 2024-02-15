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

void listenToKeyboard()
{
    bool runLoop = true;
    while (runLoop)
    {
        try {
            for (int keyCode = 1; keyCode <= NUM_OF_KEY_CODES; keyCode++)
            {
                // Check the state of the key
                short keyState = GetAsyncKeyState(keyCode);

                // Check if the most significant bit is set, indicating the key is pressed
                if (keyState & WAS_PRESSED) {
                    std::cout << "Key with code " << (char)keyCode << " is pressed." << std::endl;

                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            runLoop = false;
        }
        Sleep(100);
    }
}
