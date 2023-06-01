#pragma once
// @source: master/game/shared/imovehelper.h

// identifies how submerged in water a player is
enum : int
{
	WL_NOTINWATER = 0,
	WL_FEET,
	WL_WAIST,
	WL_EYES
};

// forward declarations
class CBaseEntity;

class IMoveHelper : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void SetHost(CBaseEntity* pHost)
	{
		CallVFunc<void, 1U>(this, pHost);
	}

	void ProcessImpacts()
	{
		CallVFunc<void, 4U>(this);
	}
};
