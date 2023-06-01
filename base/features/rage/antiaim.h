#pragma once
#include "../../common.h"

#include "../../sdk/datatypes/usercmd.h"

// used: ccsplayer
#include "../../sdk/entity.h"

/*
 * ANTI-AIM
 * - desynchronize server and client angles to prevent the enemy from hitting the user
 */
namespace F::RAGE::ANTIAIM
{
	/* @section: callbacks */
	void OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket);

	/* @section: get */
	/// @returns: max difference between eye and foot yaw delta on server
	float GetMaxDesyncDelta(const CCSGOPlayerAnimState* pAnimationState);
}
