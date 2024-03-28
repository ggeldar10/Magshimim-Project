#pragma once
#include "packets.h"
#include "mutex"
#include <exception>
#include <queue>
#define IS_PRESSED 0x8000

POINT getCursorPosition();
void setCursorPosition(const POINT& point);
void makeCursorButtonAction(const CursorActions action, const int scrollValue);
void listenToCursor(bool* shutdownSwitch, std::mutex* switchesMtx, std::queue<std::vector<char>>& packetSendQueue, std::mutex* packetSendQueueMtx, POINT* originPoint, POINT* endPoint, std::mutex* pointsMtx);


class GetCursorPositionException : public std::exception
{
private:
    std::string message;

public:
    explicit GetCursorPositionException(const char* msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class SetCursorPositionException : public std::exception
{
private:
    std::string message;

public:
    explicit SetCursorPositionException(const char* msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class CursorButtonActionException : public std::exception
{
private:
    std::string message;

public:
    explicit CursorButtonActionException(const char* msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};
