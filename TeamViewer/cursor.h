#pragma once
#include "packets.h"
#include "windowsHeaders.h"
#include "TeamViewerExeptions.h"

POINT getCursorPosition();
bool setCursorPosition(POINT point);
bool makeCursorButtonAction(CursorButtonsActions action, int scrollValue);