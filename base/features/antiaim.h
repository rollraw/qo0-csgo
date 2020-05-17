#pragma once
// used: winapi includes, singleton
#include "../common.h"
// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: angle
#include "../sdk/datatypes/qangle.h"
// used: baseentity
#include "../sdk/entity.h"

class CAntiAim : public CSingleton<CAntiAim>
{
public:
	// Get
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal, QAngle angles, bool& bSendPacket);
private:
	// Main
	void Pitch(CBaseEntity* pLocal, QAngle& angView);
	void Yaw(CUserCmd* pCmd, CBaseEntity* pLocal, QAngle& angView, bool& bSendPacket);

	// Extra
	/* returns max server desynchronization angle delta */
	float GetMaxDesyncDelta(CBasePlayerAnimState* pAnimState);
};
