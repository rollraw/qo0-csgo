#include "antiaim.h"

// used: global variables
#include "../global.h"
// used: cheat variables
#include "../core/variables.h"
// used: globals interface
#include "../core/interfaces.h"
// used: math definitions
#include "../utilities/math.h"
// used: keybind for desync side
#include "../utilities/inputsystem.h"
// used: get corrected tickbase
#include "prediction.h"

void CAntiAim::Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket)
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

	float flServerTime = TICKS_TO_TIME(CPrediction::Get().GetTickbase(pCmd, pLocal));

	UpdateServerAnimations(pCmd, pLocal, flServerTime);

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

	// get angles
	angSentView = pCmd->angViewPoint;

	/* edge antiaim, fakewalk, other hvhboi$tuff do here */

	// @note: fyi: https://www2.clarku.edu/faculty/djoyce/complex/polarangle.gif

	// do antiaim for pitch
	Pitch(pCmd, pLocal);
	// do antiaim for yaw
	Yaw(pCmd, pLocal, flServerTime, bSendPacket);

	if (C::Get<bool>(Vars.bAntiUntrusted))
	{
		angSentView.Normalize();
		angSentView.Clamp();
	}

	// set angles
	pCmd->angViewPoint = angSentView;
}

void CAntiAim::UpdateServerAnimations(CUserCmd* pCmd, CBaseEntity* pLocal, float flServerTime)
{
	// get values to check for change/reset
	static CBaseHandle hOldLocal = pLocal->GetRefEHandle();
	static float flOldSpawnTime = pLocal->GetSpawnTime();

	bool bAllocate = (pServerAnimState == nullptr);
	bool bChange = (!bAllocate && pLocal->GetRefEHandle() != hOldLocal);
	bool bReset = (!bAllocate && !bChange && pLocal->GetSpawnTime() != flOldSpawnTime);

	// player changed, free old animation state
	if (bChange)
		I::MemAlloc->Free(pServerAnimState);

	// check is need to reset (on respawn)
	if (bReset)
	{
		pServerAnimState->Reset();
		flOldSpawnTime = pLocal->GetSpawnTime();
	}

	// need to allocate or create new due to player change
	if (bAllocate || bChange)
	{
		// create temporary animstate
		CBasePlayerAnimState* pAnimState = (CBasePlayerAnimState*)I::MemAlloc->Alloc(sizeof(CBasePlayerAnimState));

		if (pAnimState != nullptr)
			pAnimState->Create(pLocal);

		hOldLocal = pLocal->GetRefEHandle();
		flOldSpawnTime = pLocal->GetSpawnTime();

		// note animstate for future use
		pServerAnimState = pAnimState;
	}
	else if (I::ClientState->iChokedCommands == 0)
	{
		// backup values
		std::array<CAnimationLayer, 13U> arrNetworkedLayers;
		std::copy(pLocal->GetAnimationLayers(), pLocal->GetAnimationLayers() + arrNetworkedLayers.size(), arrNetworkedLayers.data());
		const QAngle angAbsViewOld = pLocal->GetAbsAngles();
		const std::array<float, 24U> arrPosesOld = pLocal->GetPoseParameter();

		pServerAnimState->Update(angSentView);

		// restore values
		std::copy(arrNetworkedLayers.begin(), arrNetworkedLayers.end(), pLocal->GetAnimationLayers());
		pLocal->GetPoseParameter() = arrPosesOld;
		pLocal->SetAbsAngles(angAbsViewOld);

		// check is walking, delay next update by 0.22s
		if (pServerAnimState->flVelocityLenght2D > 0.1f)
			flNextLowerBodyUpdate = flServerTime + 0.22f;
		// check is standing, update every 1.1s
		else if (std::fabsf(pServerAnimState->flGoalFeetYaw - pServerAnimState->flEyeYaw) > 35.f && flServerTime > flNextLowerBodyUpdate)
			flNextLowerBodyUpdate = flServerTime + 1.1f;
	}
}

void CAntiAim::Pitch(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	switch (C::Get<int>(Vars.iAntiAimPitch))
	{
	case (int)EAntiAimPitchType::NONE:
		break;
	case (int)EAntiAimPitchType::UP:
		angSentView.x = -89.0f;
		break;
	case (int)EAntiAimPitchType::DOWN:
		angSentView.x = 89.f;
		break;
	case (int)EAntiAimPitchType::ZERO:
		// untrusted pitch example
		angSentView.x = 1080.f;
		break;
	}
}

void CAntiAim::Yaw(CUserCmd* pCmd, CBaseEntity* pLocal, float flServerTime, bool& bSendPacket)
{
	if (pServerAnimState == nullptr)
		return;

	const float flMaxDesyncDelta = GetMaxDesyncDelta(pServerAnimState);

	switch (C::Get<int>(Vars.iAntiAimYaw))
	{
	case (int)EAntiAimYawType::NONE:
		break;
	case (int)EAntiAimYawType::DESYNC:
	{
		static float flSide = 1.0f;

		/*
		 * menual change side
		 * @note: to visualy seen that - make desync chams by saving matrix or draw direction arrows
		 */
		if (C::Get<int>(Vars.iAntiAimDesyncKey) > 0 && IPT::IsKeyReleased(C::Get<int>(Vars.iAntiAimDesyncKey)))
			flSide = -flSide;

		// check is lowerbody updated and have choked command
		if (flServerTime >= flNextLowerBodyUpdate)
		{
			// check is we not choke now
			if (I::ClientState->iChokedCommands == 0)
				// choke packet to make update invisibly
				bSendPacket = false;

			angSentView.y -= 120.f * flSide;
		}

		if (bSendPacket)
			// real
			angSentView.y -= flMaxDesyncDelta * flSide;
		else
			// fake
			angSentView.y += flMaxDesyncDelta * flSide;

		break;
	}
	default:
		break;
	}
}

float CAntiAim::GetMaxDesyncDelta(CBasePlayerAnimState* pAnimState)
{
	float flDuckAmount = pAnimState->flDuckAmount;
	float flRunningSpeed = std::clamp<float>(pAnimState->flRunningSpeed, 0.0f, 1.0f);
	float flDuckingSpeed = std::clamp<float>(pAnimState->flDuckingSpeed, 0.0f, 1.0f);
	float flYawModifier = (((pAnimState->flWalkToRunTransition * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;

	if (flDuckAmount > 0.0f)
		flYawModifier += ((flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier));

	float flMaxYawModifier = flYawModifier * pAnimState->flMaxBodyYaw;
	return flMaxYawModifier;
}
