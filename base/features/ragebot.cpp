#include "ragebot.h"

// used: cheat variables
#include "../core/variables.h"

void CRageBot::Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;

	/* get valid entities, target sort, get best point, hitscan and aim (hf c:) */
	// @note: FYI - https://www.unknowncheats.me/forum/counterstrike-global-offensive/345397-performance-raytracer.html
}
