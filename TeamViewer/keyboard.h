#pragma once
#include "packets.h"
#include "windowsHeaders.h"
#include "TeamViewerExeptions.h"


bool makeKeyboardAction(KeyboardKeysActions action, int keyCode);
KeyboardDataPacket createPacket(KeyboardKeysActions action, int ackSequenceNumber, int packetSequenceNumber, int keyCode);