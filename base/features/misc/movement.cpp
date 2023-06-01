#include "movement.h"

// used: cheat variables
#include "../../core/variables.h"
// used: sv_autobunnyhopping, cl_sidespeed convars
#include "../../core/convar.h"
// used: getbindstate
#include "../../utilities/inputsystem.h"
// used: old player flags
#include "../prediction.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../../sdk/interfaces/imovehelper.h"

// flags of the local player before prediction began
static int nPreviousLocalFlags = 0;

using namespace F::MISC;

#pragma region movement_callbacks
void MOVEMENT::OnPreMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket)
{
	// save variables before prediction starts
	nPreviousLocalFlags = pLocal->GetFlags();

	// check if the player is alive and using the suitable movement type
	if (!pLocal->IsAlive() || pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetWaterLevel() >= WL_WAIST)
		return;

	BunnyHop(pLocal, pCmd);

	AutoStrafe(pLocal, pCmd);

	if (C::Get<bool>(Vars.bMiscNoCrouchCooldown)) // @todo: add safety check
		pCmd->nButtons |= IN_BULLRUSH;
}

void MOVEMENT::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket)
{

}

void MOVEMENT::OnPostMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket)
{
	EdgeJump(pLocal, pCmd);
}
#pragma endregion

#pragma region movement_main
void MOVEMENT::BunnyHop(CCSPlayer* pLocal, CUserCmd* pCmd)
{
	if (!C::Get<bool>(Vars.bMiscBunnyHop) || CONVAR::sv_autobunnyhopping->GetBool())
		return;

	// check if the player is able to perform jump in the water
	if (pLocal->GetWaterJumpTime() <= 0.0f)
	{
		// update the random seed
		MEM::fnRandomSeed(pCmd->iRandomSeed);
		
		// bypass of possible SMAC/VAC server anticheat detection
		if (static bool bShouldFakeJump = false; bShouldFakeJump)
		{
			pCmd->nButtons |= IN_JUMP;
			bShouldFakeJump = false;
		}
		// check is player want to jump 
		else if (pCmd->nButtons & IN_JUMP)
		{
			// check is player on the ground
			if (pLocal->GetFlags() & FL_ONGROUND)
				// note to fake jump at the next tick
				bShouldFakeJump = true;
			// check did random jump chance passed
			else if (MEM::fnRandomInt(0, 100) <= C::Get<int>(Vars.iMiscBunnyHopChance))
				pCmd->nButtons &= ~IN_JUMP;
		}
	}
}

void MOVEMENT::AutoStrafe(CCSPlayer* pLocal, CUserCmd* pCmd)
{
	if (!C::Get<bool>(Vars.bMiscAutoStrafe))
		return;

	// check if the player is in air
	if (!(pLocal->GetFlags() & FL_ONGROUND))
	{
		const float flMaxSideSpeed = CONVAR::cl_sidespeed->GetFloat();

		// determine strafe side by mouse delta that based on current and previous mouse position
		pCmd->flSideMove = pCmd->shMouseDeltaX < 0 ? -flMaxSideSpeed : flMaxSideSpeed;
	}
}

void MOVEMENT::EdgeJump(CCSPlayer* pLocal, CUserCmd* pCmd)
{
	if (!IPT::GetBindState(C::Get<KeyBind_t>(Vars.keyMiscEdgeJump)))
		return;

	// check if the player is able to perform jump in the water and are going to fall in the next tick
	if (pLocal->GetWaterJumpTime() <= 0.0f && (nPreviousLocalFlags & FL_ONGROUND) && !(pLocal->GetFlags() & FL_ONGROUND))
		pCmd->nButtons |= IN_JUMP;
}

#pragma endregion
