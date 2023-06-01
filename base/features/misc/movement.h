#pragma once
#include "../../sdk/datatypes/usercmd.h"

// used: ccsplayer
#include "../../sdk/entity.h"

namespace F::MISC::MOVEMENT
{
	/* @section: callbacks */
	void OnPreMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket);
	void OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket);
	void OnPostMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket);

	/* @section: main */
	// non-stop jumping while hitting the ground
	void BunnyHop(CCSPlayer* pLocal, CUserCmd* pCmd);
	// strafe to optimal sides in the air to maximize speed
	void AutoStrafe(CCSPlayer* pLocal, CUserCmd* pCmd);
	// jump at last edge of the land to maximize jump distance
	void EdgeJump(CCSPlayer* pLocal, CUserCmd* pCmd);
}
