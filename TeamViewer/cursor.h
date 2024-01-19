#pragma once
#include "packets.h"
#include "windowsHeaders.h"
#include <exception>

#define IS_PRESSED 0x8000

POINT getCursorPosition();
void setCursorPosition(const POINT& point);
void makeCursorButtonAction(const CursorActions action, const int scrollValue);
CursorDataPacket createPacket(const CursorActions action, const int ackSequenceNumber, const int packetSequenceNumber, const POINT position, const int scrollValue);
void listenToCursor();


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


class CursorButtonActionException_IllegalAction : public std::exception
{
public:
	virtual const char* what() const override {
		return "Error: Illegal cursor button action";
	};
};

class CursorButtonActionException_ActionFailed : public std::exception
{
public:
	virtual const char* what() const override {
		return "Error: Failed to make the cursor button action";
	};
};