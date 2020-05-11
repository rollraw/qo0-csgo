#pragma once
// used: winapi includes
#include "../common.h"

// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: listener event function
#include "../sdk/interfaces/igameeventmanager.h"
// used: baseentity
#include "../sdk/entity.h"

class CMiscellaneous : public CSingleton<CMiscellaneous>
{
public:
	// Get
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket);
	/* get warmup state */
	void Event(IGameEvent* pEvent);
	/* correct movement when antiaiming */
	void MovementCorrection(CUserCmd* pCmd, QAngle& angOldViewPoint);

	// Extra
	/* automatic shoot when pressed attack key */
	void AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal);
	/* dont send packets for a certain number of ticks */
	void FakeLag(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket);
private:
	// Movement
	/* automatic jump when steps on the ground */
	void BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocal);
	/* strafes on optimal sides for maximum speed in air */
	void AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal);
};
