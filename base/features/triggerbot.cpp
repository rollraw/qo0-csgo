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
	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;

	short nDefinitionIndex = *pWeapon->GetItemDefinitionIndex();
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	// check is weapon gun
	if (pWeaponData == nullptr || !pWeaponData->IsGun())
		return;

	// check is using key activation and key held
	if (C::Get<int>(Vars.iTriggerKey) > 0 && !IPT::IsKeyDown(C::Get<int>(Vars.iTriggerKey)))
	{
		timer.Reset();
		return;
	}

	// get view and add punch
	QAngle angView = pCmd->angViewPoint;
	angView += pLocal->GetPunch() * 2.f;

	Trace_t trace;
	Vector vecStart, vecEnd, vecForward;
	M::AngleVectors(angView, &vecForward);

	vecStart = pLocal->GetEyePosition();
	vecForward *= pWeaponData->flRange;
	vecEnd = vecStart + vecForward;

	if (C::Get<bool>(Vars.bTriggerAutoWall))
	{
		FireBulletData_t data;

		// check for minimal damage
		if (CAutoWall::Get().GetDamage(pLocal, vecEnd, &data) < C::Get<int>(Vars.iTriggerMinimalDamage))
			return;

		// copy trace from autowall
		trace = data.enterTrace;
	}
	else
	{
		// otherwise ray new trace
		Ray_t ray;
		ray.Init(vecStart, vecEnd);
		CTraceFilterSkipEntity filter(pLocal);
		I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
	}

	// check is valid trace player
	if (trace.pHitEntity == nullptr || !trace.pHitEntity->IsPlayer() || !trace.pHitEntity->IsAlive() || trace.pHitEntity->HasImmunity())
		return;

	// check is enemy
	if (!pLocal->IsEnemy(trace.pHitEntity))
		return;

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
		if (pLocal->IsCanShoot(pWeapon))
		{
			// check is delay elapsed
			if (C::Get<int>(Vars.iTriggerDelay) > 0)
			{
				if (timer.Elapsed() < C::Get<int>(Vars.iTriggerDelay))
					return;
			}

			pCmd->iButtons |= IN_ATTACK;
		}
	}
	else if (C::Get<int>(Vars.iTriggerDelay) > 0)
		timer.Reset();
}
