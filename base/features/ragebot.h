#pragma once
// used: winapi includes, singleton
#include "../common.h"
// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: angle
#include "../sdk/datatypes/qangle.h"
// used: baseentity
#include "../sdk/entity.h"

// @note: FYI - https://www.unknowncheats.me/forum/general-programming-and-reversing/173986-math-hack-2-predicting-future-3d-kinematics.html

class CRageBot : public CSingleton<CRageBot>
{
public:
	// Get
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket);

	// Global Values
	CBaseEntity* pBestEntity = nullptr;
private:
	// Main
	/* get entities, choose best target, aim */

	// Other
	/* 3-rd party functions */

	// Check
	/* is entity valid */

	// Values
	/* hitbox position other info */
};
