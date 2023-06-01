#pragma once
#include "../common.h"

#include "../sdk/datatypes/usercmd.h"

// used: cbaseplayer
#include "../sdk/entity.h"

/*
 * ENGINE PREDICTION
 * - technology used by the game to predict the player's actions and the corresponding consequences on client before receiving data from the server, in order to avoid data mismatch between them
 */
namespace F::PREDICTION
{
	/* @section: callbacks */
	// correct prediction when framerate is lower than tickrate
	void Update();
	void OnPreMove(CBasePlayer* pLocal, CUserCmd* pCmd);
	void OnPostMove(CBasePlayer* pLocal, CUserCmd* pCmd);
}
