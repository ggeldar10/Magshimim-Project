#pragma once
#include "packets.h"
#include "windowsHeaders.h"
#include "TeamViewerExeptions.h"

#define WAS_PRESSED 0x8001
#define NUM_OF_KEY_CODES 255

bool makeKeyboardAction(KeyboardKeysActions action, int keyCode);
KeyboardDataPacket createPacket(KeyboardKeysActions action, int ackSequenceNumber, int packetSequenceNumber, int keyCode);
void listenToKeyboard();
