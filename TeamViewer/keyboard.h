#pragma once
#include "packets.h"
#include "mutex"
#include "windowsHeaders.h"

#define WAS_PRESSED 0x8001
#define NUM_OF_KEY_CODES 255

void makeKeyboardAction(KeyboardActions action, int keyCode);
void listenToKeyboard();
void SetHook();
void Unhook();
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
extern bool shiftPressed;

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
