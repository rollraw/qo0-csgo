// used: random_device, mt19937, uniform_int_distribution
#include <random>

#include "misc.h"
// used: global variables
#include "../global.h"
// used: cheat variables
#include "../core/variables.h"
// used: convar interface
#include "../core/interfaces.h"
// used: angle-vector calculations
#include "../utilities/math.h"

void CMiscellaneous::Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;

	// @credits: a8pure c:
	if (C::Get<bool>(Vars.bMiscNoCrouchCooldown))
		pCmd->iButtons |= IN_BULLRUSH;

	if (C::Get<bool>(Vars.bMiscBunnyHop))
		BunnyHop(pCmd, pLocal);

	if (C::Get<bool>(Vars.bMiscAutoStrafe))
		AutoStrafe(pCmd, pLocal);

	if (C::Get<bool>(Vars.bMiscRevealRanks) && pCmd->iButtons & IN_SCORE)
		I::Client->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0U, 0, nullptr);
}

void CMiscellaneous::Event(IGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::Engine->IsInGame())
		return;
}

void CMiscellaneous::MovementCorrection(CUserCmd* pCmd, const QAngle& angOldViewPoint) const
{
	static CConVar* cl_forwardspeed = I::ConVar->FindVar(XorStr("cl_forwardspeed"));

	if (cl_forwardspeed == nullptr)
		return;

	static CConVar* cl_sidespeed = I::ConVar->FindVar(XorStr("cl_sidespeed"));

	if (cl_sidespeed == nullptr)
		return;

	static CConVar* cl_upspeed = I::ConVar->FindVar(XorStr("cl_upspeed"));

	if (cl_upspeed == nullptr)
		return;

	// get max speed limits by convars
	const float flMaxForwardSpeed = cl_forwardspeed->GetFloat();
	const float flMaxSideSpeed = cl_sidespeed->GetFloat();
	const float flMaxUpSpeed = cl_upspeed->GetFloat();

	Vector vecForward = { }, vecRight = { }, vecUp = { };
	M::AngleVectors(angOldViewPoint, &vecForward, &vecRight, &vecUp);

	// we don't attempt on forward/right roll, and on up pitch/yaw
	vecForward.z = vecRight.z = vecUp.x = vecUp.y = 0.f;

	vecForward.NormalizeInPlace();
	vecRight.NormalizeInPlace();
	vecUp.NormalizeInPlace();

	Vector vecOldForward = { }, vecOldRight = { }, vecOldUp = { };
	M::AngleVectors(pCmd->angViewPoint, &vecOldForward, &vecOldRight, &vecOldUp);

	// we don't attempt on forward/right roll, and on up pitch/yaw
	vecOldForward.z = vecOldRight.z = vecOldUp.x = vecOldUp.y = 0.f;

	vecOldForward.NormalizeInPlace();
	vecOldRight.NormalizeInPlace();
	vecOldUp.NormalizeInPlace();

	const float flPitchForward = vecForward.x * pCmd->flForwardMove;
	const float flYawForward = vecForward.y * pCmd->flForwardMove;
	const float flPitchSide = vecRight.x * pCmd->flSideMove;
	const float flYawSide = vecRight.y * pCmd->flSideMove;
	const float flRollUp = vecUp.z * pCmd->flUpMove;

	// solve corrected movement
	const float x = vecOldForward.x * flPitchSide + vecOldForward.y * flYawSide + vecOldForward.x * flPitchForward + vecOldForward.y * flYawForward + vecOldForward.z * flRollUp;
	const float y = vecOldRight.x * flPitchSide + vecOldRight.y * flYawSide + vecOldRight.x * flPitchForward + vecOldRight.y * flYawForward + vecOldRight.z * flRollUp;
	const float z = vecOldUp.x * flYawSide + vecOldUp.y * flPitchSide + vecOldUp.x * flYawForward + vecOldUp.y * flPitchForward + vecOldUp.z * flRollUp;

	// clamp and apply corrected movement
	pCmd->flForwardMove = std::clamp(x, -flMaxForwardSpeed, flMaxForwardSpeed);
	pCmd->flSideMove = std::clamp(y, -flMaxSideSpeed, flMaxSideSpeed);
	pCmd->flUpMove = std::clamp(z, -flMaxUpSpeed, flMaxUpSpeed);
}

void CMiscellaneous::AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!pLocal->IsAlive())
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;

	const short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
	const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	// check for pistol and attack
	if (pWeaponData == nullptr || pWeaponData->bFullAuto || pWeaponData->nWeaponType != WEAPONTYPE_PISTOL || !(pCmd->iButtons & IN_ATTACK))
		return;

	if (pLocal->CanShoot(static_cast<CWeaponCSBase*>(pWeapon)))
		pCmd->iButtons |= IN_ATTACK;
	else
		pCmd->iButtons &= ~IN_ATTACK;
}

void CMiscellaneous::FakeLag(CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;

	INetChannel* pNetChannel = I::ClientState->pNetChannel;

	if (pNetChannel == nullptr)
		return;

	static CConVar* sv_maxusrcmdprocessticks = I::ConVar->FindVar(XorStr("sv_maxusrcmdprocessticks"));

	if (sv_maxusrcmdprocessticks == nullptr)
		return;

	/*
	 * @note: get max available ticks to choke
	 * 2 ticks reserved for server info else player can be stacked
	 * while antiaiming and fakelag is disabled choke only 1 tick
	 */
	const int iMaxCmdProcessTicks = C::Get<bool>(Vars.bMiscFakeLag) ? sv_maxusrcmdprocessticks->GetInt() - 2 :
		C::Get<bool>(Vars.bAntiAim) ? 1 : 0;

	// choke
	bSendPacket = I::ClientState->nChokedCommands >= iMaxCmdProcessTicks;
}

void CMiscellaneous::BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocal) const
{
	static CConVar* sv_autobunnyhopping = I::ConVar->FindVar(XorStr("sv_autobunnyhopping"));

	if (sv_autobunnyhopping->GetBool())
		return;

	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_OBSERVER)
		return;

	std::random_device randomDevice;
	std::mt19937 generate(randomDevice());
	const std::uniform_int_distribution<> chance(0, 100);

	if (chance(generate) > C::Get<int>(Vars.iMiscBunnyHopChance))
		return;

	static bool bLastJumped = false, bShouldFake = false;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		pCmd->iButtons |= IN_JUMP;
	}
	else if (pCmd->iButtons & IN_JUMP)
	{
		if (pLocal->GetFlags() & FL_ONGROUND || pLocal->GetFlags() & FL_PARTIALGROUND)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			pCmd->iButtons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

void CMiscellaneous::AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	if (pLocal->GetFlags() & FL_ONGROUND)
		return;

	static CConVar* cl_sidespeed = I::ConVar->FindVar(XorStr("cl_sidespeed"));

	if (cl_sidespeed == nullptr)
		return;

	pCmd->flSideMove = pCmd->sMouseDeltaX < 0 ? -cl_sidespeed->GetFloat() : cl_sidespeed->GetFloat();
}
