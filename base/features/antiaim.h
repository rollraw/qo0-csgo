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
	// Values
	/* current lowerbody yaw update state */
	bool bLowerBodyUpdate = false;
private:
	// Main
	void Pitch(CBaseEntity* pLocal, QAngle& angle);
	void Yaw(CBaseEntity* pLocal, QAngle& angle, bool& bSendPacket);
};
