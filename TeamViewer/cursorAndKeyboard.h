#pragma once
#include "packets.h"
#include "windowsHeaders.h"
#include "TeamViewerExeptions.h"

POINT getCursorPosition()
{
	POINT point;
	if (GetCursorPos(&point))
	{
		return point;
	}
	else
	{
		throw GetCursorPositionException();
	}
}

bool setCursorPosition(POINT point)
{
	if (SetCursorPos(point.x, point.y))
	{
		return true;
	}
	else
	{
		throw SetCursorPositionException();
	}
}

