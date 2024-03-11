#pragma once
#include "packets.h"
#include "mutex"
#include "windowsHeaders.h"
#include <queue>

#define WAS_PRESSED 0x8001
#define NUM_OF_KEY_CODES 255

struct HookParameters
{
    std::queue<std::vector<char>>* packetSendQueue;
    std::mutex* packetSendQueueMtx;
};


void makeKeyboardAction(KeyboardActions action, int keyCode);
void listenToKeyboard(bool* shutdownSwitch, std::mutex* switchesMtx, std::queue<std::vector<char>>& packetSendQueue, std::mutex* packetSendQueueMtx);
void SetHook(HookParameters& hookParams);
void Unhook();
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
//extern bool shiftPressed;


class KeyboardButtonActionException : public std::exception
{
private:
    std::string message;

public:
    explicit KeyboardButtonActionException(const char* msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};
