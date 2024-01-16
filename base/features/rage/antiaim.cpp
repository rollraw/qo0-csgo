#include "antiaim.h"

// used: cheat variables
#include "../../core/variables.h"
// used: getbindstate
#include "../../utilities/inputsystem.h"
// used: visual view angles
#include "../../features.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../../sdk/interfaces/iglobalvars.h"
#include "../../sdk/interfaces/iweaponsystem.h"

using namespace F::RAGE;

#pragma region antiaim_callbacks
void ANTIAIM::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket)
{
	if (!C::Get<bool>(Vars.bAntiAim))
		return;

	// check that we aren't interacting with anything (doors, defuse, etc)
	if (pCmd->nButtons & IN_USE)
		return;

	// check is not frozen and alive
	if (!pLocal->IsAlive() || pLocal->HasImmunity() || (pLocal->GetFlags() & FL_FROZEN))
		return;

	// check is not on a ladder or using noclip (to continue on those we need more complex movement correction)
	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	const CCSGOPlayerAnimState* pAnimationState = pLocal->GetAnimationState();
	if (pAnimationState == nullptr)
		return;

	const float flServerTime = I::Globals->flCurrentTime;

	if (CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon(); pWeapon != nullptr)
	{
		const ItemDefinitionIndex_t nDefinitionIndex = pWeapon->GetEconItemView()->GetItemDefinitionIndex();

		if (const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex); pWeaponData != nullptr)
		{
			// avoid changing angles when conditions of different actions were passed to prevent them from being missed
			if (pLocal->CanAttack(flServerTime) &&
				// check are we going to attack with knife/gun
				(((pCmd->nButtons & IN_ATTACK) && static_cast<CWeaponCSBaseGun*>(pWeapon)->CanPrimaryAttack(pWeaponData->nWeaponType, flServerTime)) || ((pCmd->nButtons & IN_SECOND_ATTACK) && static_cast<CWeaponCSBaseGun*>(pWeapon)->CanSecondaryAttack(pWeaponData->nWeaponType, flServerTime)) ||
				// check are we going to throw grenade (short and/or long distance)
				(pWeaponData->nWeaponType == WEAPONTYPE_GRENADE && (!static_cast<CBaseCSGrenade*>(pWeapon)->IsPinPulled() || (pCmd->nButtons & (IN_ATTACK | IN_SECOND_ATTACK))) && static_cast<CBaseCSGrenade*>(pWeapon)->GetThrowTime() > 0.0f)))
				return;
		}
	}

	/*
	 * angles in game engine are polar:
	 *
	 *   135      90     45
	 *      \     |     /
	 *        \   |   /
	 * -180,180 \ | /  0,360
	 *   ------------------>
	 *          / | \
	 *        /   |   \
	 *      /     |     \
	 *  -135     -90    -45
	 */

	QAngle_t angModifiedView = pCmd->angViewPoint;

	// get inverter bind state
	const bool bInverter = IPT::GetBindState(C::Get<KeyBind_t>(Vars.keyAntiAimYawFakeInverter));

	// do antiaim for pitch
	if (C::Get<bool>(Vars.bAntiAimPitch))
		angModifiedView.x = static_cast<float>(C::Get<int>(Vars.iAntiAimPitch));

	// do antiaim for yaw
	// real
	if (C::Get<bool>(Vars.bAntiAimYawReal))
		angModifiedView.y += static_cast<float>(C::Get<int>(Vars.iAntiAimYawRealOffset));

	angModifiedView.Normalize();

	SetClientAngles(angModifiedView);

	// fake
	if (C::Get<bool>(Vars.bAntiAimYawFake))
	{
		// determine max possible delta value of desynchronization
		// it doesn't necessary to use exact value because server will always clamp higher values itself
		const float flMaxDesyncDelta = GetMaxDesyncDelta(pAnimationState);

		/*
		 * force game to always update lower body yaw when standing
		 * to do this we need to keep player velocity greater than (CS_PLAYER_SPEED_STOPPED = 1.0f)
		 *
		 * @ida: server.dll -> "F3 0F 10 05 ? ? ? ? 0F 2F 86 ? ? ? ? 72"
		 * @source: master/game/shared/cstrike15/csgo_playeranimstate.cpp#L2380
		 */
		if (pAnimationState->bOnGround && !pAnimationState->bOnLadder && pLocal->GetVelocity().Length2DSqr() < 25.f)
		{
			/*
			 * compute speed we need to force to reach desired velocity
			 * since duck have its own modifier on velocity (CS_PLAYER_SPEED_DUCK_MODIFIER = 0.34f)
			 * we should also take this into account, reciprocal of it is 2.94f, 1.01 * 2.94 = 2.97
			 */
			const float flWishSpeed = ((pCmd->nButtons & IN_DUCK) ? 2.97f : 1.01f);

			// switch direction every tick so we're "sliding" at same place
			pCmd->flForwardMove = ((pCmd->iCommandNumber & 1) ? +flWishSpeed : -flWishSpeed);
		}

		if (!*pbSendPacket)
			angModifiedView.y -= (bInverter ? +flMaxDesyncDelta : -flMaxDesyncDelta);

		angModifiedView.Normalize();
	}

	SetServerAngles(angModifiedView);
}
#pragma endregion

#pragma region antiaim_get
float ANTIAIM::GetMaxDesyncDelta(const CCSGOPlayerAnimState* pAnimationState)
{
	// @ida: server.dll -> "0F 2F C3 76 05 0F 28 D8 EB 08 F3 0F 5D 1D ? ? ? ? F3"

	float flYawRange = (((pAnimationState->flWalkToRunTransition * -0.30f) - 0.20f) * pAnimationState->flRunSpeedNormalized) + 1.0f;

	if (pAnimationState->flDuckAmount > 0.0f)
		flYawRange += (pAnimationState->flDuckAmount * pAnimationState->flCrouchSpeedNormalized) * (0.5f - flYawRange);

	return pAnimationState->flAimYawMax * flYawRange;
}
#pragma endregion
