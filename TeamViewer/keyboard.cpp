#include "keyboard.h"

bool makeKeyboardAction(KeyboardKeysActions action, int keyCode)
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

    if (SendInput(1, &input, sizeof(INPUT)) == 0) 
    {
        std::cerr << "Failed to send keyboard input. Error code: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

KeyboardDataPacket createPacket(KeyboardKeysActions action, int ackSequenceNumber, int packetSequenceNumber, int keyCode)
{
    KeyboardDataPacket packet;
    packet.packetType = DataPacket;
    packet.dataPacketType = Keyboard;
    packet.timeStamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    packet.ackSequenceNumber = ackSequenceNumber;
    packet.packetSequenceNumber = packetSequenceNumber;
    packet.action = action;
    packet.keyCode = keyCode;
    return packet;
}

void listenToKeyboard()
{
    while (true)
    {
        for (int keyCode = 1; keyCode <= NUM_OF_KEY_CODES; keyCode++)
        {
            // Check the state of the key
            short keyState = GetAsyncKeyState(keyCode);

            // Check if the most significant bit is set, indicating the key is pressed
            if (keyState & WAS_PRESSED) {
                std::cout << "Key with code " << (char)keyCode << " is pressed." << std::endl;

            }
        }
        Sleep(100);
    }
}
