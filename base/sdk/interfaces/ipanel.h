#pragma once
// used: ibaseinterface
#include "ibaseinterface.h"
// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/public/vgui/IPanel.h

using VPANEL = unsigned int;

class IPanel : public IBaseInterface, ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	void SetKeyBoardInputEnabled(VPANEL nVGUIPanel, bool bState)
	{
		CallVFunc<void, 31U>(this, nVGUIPanel, bState);
	}

	void SetMouseInputEnabled(VPANEL nVGUIPanel, bool bState)
	{
		CallVFunc<void, 32U>(this, nVGUIPanel, bState);
	}

	const char* GetName(VPANEL nVGUIPanel)
	{
		return CallVFunc<const char*, 36U>(this, nVGUIPanel);
	}

	const char* GetNameClass(VPANEL nVGUIPanel)
	{
		return CallVFunc<const char*, 37U>(this, nVGUIPanel);
	}
};
