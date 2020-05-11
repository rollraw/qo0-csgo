#include "antiaim.h"

// used: global variables
#include "../global.h"
// used: cheat variables
#include "../core/variables.h"
// used: globals interface
#include "../core/interfaces.h"
// used: math definitions
#include "../utilities/math.h"

void CAntiAim::Run(CUserCmd* pCmd, CBaseEntity* pLocal, QAngle angles, bool& bSendPacket)
{
	// check is not frozen and alive
	if (pLocal->GetFlags() & FL_FROZEN || !pLocal->IsAlive())
		return;

	// is not on a ladder or use noclip (to except this need more proper movefix)
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

	// weapon shoot check
	if (pWeaponData->IsGun() && pLocal->IsCanShoot(pWeapon) &&
		(pCmd->iButtons & IN_ATTACK || (nDefinitionIndex == WEAPON_REVOLVER && pCmd->iButtons & (IN_ATTACK | IN_SECOND_ATTACK)))) // @todo: fix burst
		return;
	// knife attack check
	else if (pWeaponData->nWeaponType == WEAPONTYPE_KNIFE)
	{
		float flNextPrimaryAttack = pWeapon->GetNextPrimaryAttack() - I::Globals->flCurrentTime;
		float flNextSecondaryAttack = pWeapon->GetNextSecondaryAttack() - I::Globals->flCurrentTime;

			// slash
		if ((pCmd->iButtons & IN_ATTACK && flNextPrimaryAttack <= 0.f) ||
			// stab
			(pCmd->iButtons & IN_SECOND_ATTACK && flNextSecondaryAttack <= 0.f))
			return;
	}
	// grenade throw check
	else if (CBaseCSGrenade* pGrenade = (CBaseCSGrenade*)pWeapon; pGrenade != nullptr && pWeaponData->nWeaponType == WEAPONTYPE_GRENADE)
	{
		// check is being thrown a grenade
		if (!pGrenade->IsPinPulled() || pCmd->iButtons & (IN_ATTACK | IN_SECOND_ATTACK))
		{
			if (pGrenade->GetThrowTime() > 0.f)
				return;
		}
	}

	/* edge antiaim, fakewalk, other hvhboi$tuff do here */

	// do antiaim for pitch
	Pitch(pLocal, angles);
	// do antiaim for yaw
	Yaw(pLocal, angles, bSendPacket);

	if (C::Get<bool>(Vars.bAntiUntrusted))
	{
		angles.Normalize();
		angles.Clamp();
	}

	// set angles
	pCmd->angViewPoint = angles;
}

void CAntiAim::Pitch(CBaseEntity* pLocal, QAngle& angle)
{
	switch (C::Get<int>(Vars.iAntiAimPitch))
	{
	case (int)EAntiAimPitchType::NONE:
		break;
	case (int)EAntiAimPitchType::UP:
		angle.x = -89.0f;
		break;
	case (int)EAntiAimPitchType::DOWN:
		angle.x = 89.f;
		break;
	case (int)EAntiAimPitchType::ZERO:
		angle.x = 1080.f; // @note: fyi: https://www2.clarku.edu/faculty/djoyce/complex/polarangle.gif
		break;
	}
}

void CAntiAim::Yaw(CBaseEntity* pLocal, QAngle& angle, bool& bSendPacket)
{
	float flServerTime = TICKS_TO_TIME(pLocal->GetTickBase());

	// @note: simply pseudocode example for different AA's at move/stand/jump/crouch
	/*static int iCurrentReal, iCurrentFake;

	// is moving
	if (pLocal->GetVelocity().Length2D() > 0.1f)
	{
		iCurrentReal = 1;
		iCurrentFake = 1;
	}
	// is jumping
	else if (!(pLocal->GetFlags() & FL_ONGROUND))
	{
		iCurrentReal = 2;
		iCurrentFake = 2;
	}
	// is crouching
	else if (pLocal->GetFlags & FL_DUCKING)
	{
		iCurrentReal = 3;
		iCurrentFake = 3;
	}
	// is standing
	else
	{
		iCurrentReal = 0;
		iCurrentFake = 0;
	}*/

	if (bSendPacket)
	{
		/* at target do here */

		switch (C::Get<int>(Vars.iAntiAimYaw))
		{
		case (int)EAntiAimYawType::NONE:
			break;
		case (int)EAntiAimYawType::SIDEWAYS:
			angle.y += 90;
			break;
		}

		/* additional yaw modifier here */
	}
	else
	{
		/* fake at target do here */

		// @note: fake angles fixed now for ~half-year and i leave this here only for example
		switch (C::Get<int>(Vars.iAntiAimYaw))
		{
		case (int)EAntiAimYawType::NONE:
			break;
		case (int)EAntiAimYawType::SIDEWAYS:
			angle.y -= 90;
			break;
		}

		/* additional fake yaw modifier here */
	}
}
