#include "triggerbot.h"

// used: cheat variables
#include "../core/variables.h"
// used: max distance, classindex, itemdefinitionindex
#include "../sdk/definitions.h"
// used: angle/vector calculations
#include "../utilities/math.h"
// used: is key pressed for bind
#include "../utilities/inputsystem.h"
// used: convar class, globals interface
#include "../core/interfaces.h"
// used: firebullet data
#include "autowall.h"

void CTriggerBot::Run(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static CConVar* weapon_recoil_scale = I::ConVar->FindVar(XorStr("weapon_recoil_scale"));

	if (weapon_recoil_scale == nullptr)
		return;

	if (!pLocal->IsAlive())
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;

	const short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
	const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	if (pWeaponData == nullptr || !pWeaponData->IsGun())
		return;

	// check is using key activation and key is held
	if (C::Get<int>(Vars.iTriggerKey) > 0 && !IPT::IsKeyDown(C::Get<int>(Vars.iTriggerKey)))
	{
		timerDelay.Reset();
		return;
	}

	// get view and take punch into account
	const QAngle angView = pCmd->angViewPoint + pLocal->GetPunch() * weapon_recoil_scale->GetFloat();

	Vector vecForward = { };
	M::AngleVectors(angView, &vecForward);
	vecForward *= pWeaponData->flRange;

	const Vector vecStart = pLocal->GetEyePosition();
	const Vector vecEnd = vecStart + vecForward;

	Trace_t trace = { };
	if (C::Get<bool>(Vars.bTriggerAutoWall))
	{
		FireBulletData_t data = { };

		// check for minimal damage
		if (CAutoWall::GetDamage(pLocal, vecEnd, &data) < C::Get<int>(Vars.iTriggerMinimalDamage))
			return;

		// copy trace from autowall
		trace = data.enterTrace;
	}
	else
	{
		// otherwise ray new trace
		Ray_t ray(vecStart, vecEnd);
		CTraceFilter filter(pLocal);
		I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
	}

	// check is trace player valid and enemy
	if (CBaseEntity* pEntity = trace.pHitEntity; pEntity == nullptr || !pEntity->IsAlive() || pEntity->IsDormant() || !pEntity->IsPlayer() || pEntity->HasImmunity() || !pLocal->IsEnemy(pEntity))
	{
		timerDelay.Reset();
		return;
	}

	// hitgroup filters check
		// head
	if ((C::Get<bool>(Vars.bTriggerHead) && trace.iHitGroup == HITGROUP_HEAD) ||
		// chest
		(C::Get<bool>(Vars.bTriggerChest) && trace.iHitGroup == HITGROUP_CHEST) ||
		// stomach
		(C::Get<bool>(Vars.bTriggerStomach) && trace.iHitGroup == HITGROUP_STOMACH) ||
		// arms
		(C::Get<bool>(Vars.bTriggerArms) && (trace.iHitGroup == HITGROUP_LEFTARM || trace.iHitGroup == HITGROUP_RIGHTARM)) ||
		// legs
		(C::Get<bool>(Vars.bTriggerLegs) && (trace.iHitGroup == HITGROUP_LEFTLEG || trace.iHitGroup == HITGROUP_RIGHTLEG)))
	{
		if (pLocal->CanShoot(static_cast<CWeaponCSBase*>(pWeapon)))
		{
			// check is delay elapsed
			if (C::Get<int>(Vars.iTriggerDelay) > 0 && timerDelay.Elapsed() < C::Get<int>(Vars.iTriggerDelay))
				return;

			pCmd->iButtons |= IN_ATTACK;
		}
	}
}
