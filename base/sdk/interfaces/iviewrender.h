#pragma once
// used: viewplane_t
#include "ivrenderview.h"

// @source: master/game/client/iviewrender.h
// master/game/client/viewrender.cpp
// master/game/client/viewrender.h
class IViewRender : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void RenderView(const CViewSetup& viewSetup, const CViewSetup& viewHUD, int nClearFlags, int nWhatToDraw)
	{
		CallVFunc<void, 6U>(this, &viewSetup, &viewHUD, nClearFlags, nWhatToDraw);
	}

	[[nodiscard]] int GetDrawFlags()
	{
		return CallVFunc<int, 7U>(this);
	}

	[[nodiscard]] ViewPlane_t* GetFrustum()
	{
		return CallVFunc<ViewPlane_t*, 10U>(this);
	}

	[[nodiscard]] const CViewSetup* GetPlayerViewSetup(int iSlot = -1)
	{
		return CallVFunc<const CViewSetup*, 12U>(this, iSlot);
	}

	[[nodiscard]] const CViewSetup* GetViewSetup()
	{
		return CallVFunc<const CViewSetup*, 13U>(this);
	}

	[[nodiscard]] float GetZNear()
	{
		return CallVFunc<float, 25U>(this);
	}

	[[nodiscard]] float GetZFar()
	{
		return CallVFunc<float, 26U>(this);
	}

public:
	std::byte pad0[0x588]; // 0x0000
	float flSmokeOverlayAmount; // 0x0588 // @ida: client.dll -> ["0F 2F 82 ? ? ? ? 73" + 0x3] @xref: "effects/overlaysmoke"
};
