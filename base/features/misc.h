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
	void Event(IGameEvent* pEvent, const FNV1A_t uNameHash);
	/* correct movement while anti-aiming */
	void MovementCorrection(CUserCmd* pCmd, const QAngle& angOldViewPoint) const;

	// Extra
	/* automatic shoot when pressed attack key */
	void AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal);
	/* dont send packets for a certain number of ticks */
	void FakeLag(CBaseEntity* pLocal, bool& bSendPacket);
private:
	// Movement
	/* automatic jump when steps on the ground */
	void BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocal) const;
	/* strafes on optimal sides for maximum speed in air */
	void AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal);
};
