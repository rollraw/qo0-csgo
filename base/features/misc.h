#pragma once
#include "../common.h"

#include "../sdk/datatypes/usercmd.h"

// used: ccsplayer
#include "../sdk/entity.h"

/*
 * MISCELLANEOUS
 * - other assistance to the user
 */
namespace F::MISC
{
	/* @section: callbacks */
	void OnPreMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket);
	void OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket);
	void OnPostMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket);

	/* @section: automation */
	// automate non-stop shooting while attack key is pressed
	void AutoPistol(CCSPlayer* pLocal, CUserCmd* pCmd);
}
