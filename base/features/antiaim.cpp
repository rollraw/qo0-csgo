#include "antiaim.h"

// used: global variables
#include "../global.h"
// used: cheat variables
#include "../core/variables.h"
// used: globals interface
#include "../core/interfaces.h"
// used: math definitions
#include "../utilities/math.h"

void CAntiAim::Run(CUserCmd* pCmd, CBaseEntity* pLocal, QAngle angViewPoint, bool& bSendPacket)
{
	// check is not frozen and alive
	if (pLocal->GetFlags() & FL_FROZEN || !pLocal->IsAlive())
		return;

	// is not on a ladder or use noclip (to skip that needs more proper movefix)
	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	// is not uses anything (open doors, defuse, etc)
	if (pCmd->iButtons & IN_USE)
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;

	short nDefinitionIndex = *pWeapon->GetItemDefinitionIndex();
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	if (pWeaponData == nullptr)
		return;

	float flServerTime = TICKS_TO_TIME(pLocal->GetTickBase());

	// weapon shoot check
	if (pWeaponData->IsGun() && pLocal->IsCanShoot(pWeapon) && (pCmd->iButtons & IN_ATTACK || (nDefinitionIndex == WEAPON_REVOLVER && pCmd->iButtons & IN_SECOND_ATTACK)))
		return;
	// knife attack check
	else if (pWeaponData->nWeaponType == WEAPONTYPE_KNIFE)
	{
		// slash
		if ((pCmd->iButtons & IN_ATTACK && pWeapon->GetNextPrimaryAttack() <= flServerTime) ||
			// stab
			(pCmd->iButtons & IN_SECOND_ATTACK && pWeapon->GetNextSecondaryAttack() <= flServerTime))
			return;
	}
	// grenade throw check
	else if (auto pGrenade = (CBaseCSGrenade*)pWeapon; pGrenade != nullptr && pWeaponData->nWeaponType == WEAPONTYPE_GRENADE)
	{
		// check is being thrown a grenade
		if (!pGrenade->IsPinPulled() || pCmd->iButtons & (IN_ATTACK | IN_SECOND_ATTACK))
		{
			if (pGrenade->GetThrowTime() > 0.f)
				return;
		}
	}

	/* edge antiaim, fakewalk, other hvhboi$tuff do here */

	// @note: fyi: https://www2.clarku.edu/faculty/djoyce/complex/polarangle.gif

	// do antiaim for pitch
	Pitch(pLocal, angViewPoint);
	// do antiaim for yaw
	Yaw(pCmd, pLocal, angViewPoint, bSendPacket);

	if (C::Get<bool>(Vars.bAntiUntrusted))
	{
		angViewPoint.Normalize();
		angViewPoint.Clamp();
	}

	// set angles
	pCmd->angViewPoint = angViewPoint;
}

void CAntiAim::Pitch(CBaseEntity* pLocal, QAngle& angView)
{
	switch (C::Get<int>(Vars.iAntiAimPitch))
	{
	case (int)EAntiAimPitchType::NONE:
		break;
	case (int)EAntiAimPitchType::UP:
		angView.x = -89.0f;
		break;
	case (int)EAntiAimPitchType::DOWN:
		angView.x = 89.f;
		break;
	case (int)EAntiAimPitchType::ZERO:
		// untrusted pitch example
		angView.x = 1080.f;
		break;
	}
}

void CAntiAim::Yaw(CUserCmd* pCmd, CBaseEntity* pLocal, QAngle& angView, bool& bSendPacket)
{
	CBasePlayerAnimState* pAnimState = pLocal->GetAnimationState();

	if (pAnimState == nullptr)
		return;

	const auto flMaxDesyncDelta = GetMaxDesyncDelta(pAnimState);

	switch (C::Get<int>(Vars.iAntiAimYaw))
	{
	case (int)EAntiAimYawType::NONE:
		break;
	case (int)EAntiAimYawType::DESYNC:
	{
		// check is not moving now
		if (std::fabsf(pCmd->flSideMove) < 5.0f)
		{
			// force server to update our lby by making micromovements (also u can use breaker instead)
			if (pCmd->iTickCount % 2)
				pCmd->flSideMove = (pCmd->iButtons & IN_DUCK) ? -3.25f : -1.1f;
			else
				pCmd->flSideMove = (pCmd->iButtons & IN_DUCK) ? 3.25f : 1.1f;
		}

		/*
		 * @note: needed jitter/manual switch
		 * to visualy seen that - make desync chams by saving matrix or draw direction arrows
		 */
		if (bSendPacket)
			angView.y -= pLocal->GetLowerBodyYaw() + flMaxDesyncDelta;
		else
			angView.y += pLocal->GetLowerBodyYaw() + flMaxDesyncDelta;
		break;
	}
	default:
		break;
	}
}

float CAntiAim::GetMaxDesyncDelta(CBasePlayerAnimState* pAnimState)
{
	// @credits: sharklaser1's reversed setupvelocity
	float flDuckAmount = pAnimState->flDuckAmount;
	float flRunningSpeed = std::clamp<float>(pAnimState->flRunningSpeed, 0.0f, 1.0f);
	float flDuckingSpeed = std::clamp<float>(pAnimState->flDuckingSpeed, 0.0f, 1.0f);
	float flYawModifier = (((pAnimState->flWalkToRunTransition * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;

	if (flDuckAmount > 0.0f)
		flYawModifier += ((flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier));

	float flMaxYawModifier = flYawModifier * pAnimState->flMaxBodyYaw;
	return flMaxYawModifier;
}
