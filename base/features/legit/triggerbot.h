#pragma once
#include "../../common.h"

#include "../../sdk/datatypes/usercmd.h"

// used: ccsplayer
#include "../../sdk/entity.h"

/*
 * TRIGGER-BOT
 * - automatically fires at an enemy when it is on the user's crosshair
 */
namespace F::LEGIT::TRIGGER
{
	/* @section: callbacks */
	void OnMove(CCSPlayer* pLocal, CUserCmd* pCmd);
};
