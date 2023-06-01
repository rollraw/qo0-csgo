#pragma once
#include "../common.h"

#include "../sdk/datatypes/usercmd.h"

// used: ccsplayer
#include "../sdk/entity.h"

/*
 * LEGIT
 * - light and non-suspicious assistance to the user
 */
namespace F::LEGIT
{
	/* @section: callbacks */
	void OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, bool* pbSendPacket);
}
