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
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket);
	/* align local animation state to server */
	void UpdateServerAnimations(CUserCmd* pCmd, CBaseEntity* pLocal); // @credits: pazzo

	// Values
	/* angles modified by antiaim and being sent */
	QAngle angSentView = { };
private:
	// Main
	void Pitch(CUserCmd* pCmd, CBaseEntity* pLocal);
	void Yaw(CUserCmd* pCmd, CBaseEntity* pLocal, float flServerTime, bool& bSendPacket);

	// Extra
	/* returns max server desynchronization angle delta */
	float GetMaxDesyncDelta(CCSGOPlayerAnimState* pAnimState); // @credits: sharklaser1's reversed setupvelocity

	// Values
	/* updated by server animstate */
	CCSGOPlayerAnimState* pServerAnimState = nullptr;
	/* next lower body yaw update time from server */
	float flNextLowerBodyUpdate = 0.f;
};
