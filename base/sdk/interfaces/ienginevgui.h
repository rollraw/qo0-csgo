#pragma once
// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/public/ienginevgui.h

using VPANEL = unsigned int;

#define INVALID_PANEL 0xFFFFFFFF

enum EVGuiPanel
{
	PANEL_ROOT = 0,
	PANEL_GAMEUIDLL,
	PANEL_CLIENTDLL,
	PANEL_TOOLS,
	PANEL_INGAMESCREENS,
	PANEL_GAMEDLL,
	PANEL_CLIENTDLL_TOOLS
};

enum EPaintMode
{
	PAINT_UIPANELS =		(1 << 0),
	PAINT_INGAMEPANELS =	(1 << 1),
	PAINT_CURSOR =			(1 << 2)
};

class IEngineVGui : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	VPANEL GetPanel(EVGuiPanel nType)
	{
		return CallVFunc<VPANEL, 1U>(this, nType);
	}
};
