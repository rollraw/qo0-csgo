#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/ienginevgui.h

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
	PAINT_UIPANELS = (1 << 0),
	PAINT_INGAMEPANELS = (1 << 1),
	PAINT_CURSOR = (1 << 2)
};

class IEngineVGui
{
public:
	virtual			~IEngineVGui() { }
	virtual VPANEL	GetPanel(EVGuiPanel type) = 0;
	virtual bool	IsGameUIVisible() = 0;
};
