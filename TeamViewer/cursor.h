#pragma once
#include "packets.h"
#include "windowsHeaders.h"
#include "TeamViewerExeptions.h"

POINT getCursorPosition();
bool setCursorPosition(POINT point);
bool makeCursorButtonAction(CursorActions action, int scrollValue);
CursorDataPacket createPacket(CursorActions action, int ackSequenceNumber, int packetSequenceNumber, POINT position, int scrollValue);