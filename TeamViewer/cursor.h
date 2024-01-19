#pragma once
#include "packets.h"
#include "windowsHeaders.h"
#include "TeamViewerExeptions.h"

#define IS_PRESSED 0x8000

POINT getCursorPosition();
bool setCursorPosition(const POINT point);
bool makeCursorButtonAction(const CursorActions action, const int scrollValue);
CursorDataPacket createPacket(const CursorActions action, const int ackSequenceNumber, const int packetSequenceNumber, const POINT position, const int scrollValue);
void listenToCursor();