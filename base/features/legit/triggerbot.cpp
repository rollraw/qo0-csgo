#include "triggerbot.h"

// used: getbindstate
#include "../../utilities/inputsystem.h"
// used: cheat variables
#include "../../core/variables.h"
// used: weapon_recoil_scale
#include "../../core/convar.h"
// used: getdamage
#include "../autowall.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface definitions
#include "../../sdk/interfaces/iglobalvars.h"

using namespace F::LEGIT;

// last time when crosshair was on target for delay before shot
static float flLastTargetTime = 0.0f;

#pragma region triggerbot_callbacks
void TRIGGER::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd)
{
	if (!C::Get<bool>(Vars.bTrigger) || !pLocal->IsAlive())
		return;

	CWeaponCSBaseGun* pWeaponCSBaseGun = static_cast<CWeaponCSBaseGun*>(pLocal->GetActiveWeapon());

	if (pWeaponCSBaseGun == nullptr)
		return;

	const ItemDefinitionIndex_t nDefinitionIndex = pWeaponCSBaseGun->GetEconItemView()->GetItemDefinitionIndex();
	const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	if (pWeaponData == nullptr || !pWeaponData->IsGun())
		return;

	// check for keybind
	if (auto& keyBind = C::Get<KeyBind_t>(Vars.keyTrigger); keyBind.uKey > 0U && !IPT::GetBindState(keyBind))
	{
		flLastTargetTime = I::Globals->flRealTime;
		return;
	}

	// get view and take punch into account
	const QAngle_t angView = pCmd->angViewPoint + pLocal->GetLocalData()->GetAimPunch() * CONVAR::weapon_recoil_scale->GetFloat();

	Vector_t vecForward = { };
	angView.ToDirections(&vecForward);
	vecForward *= pWeaponData->flRange;

	const Vector_t vecStart = pLocal->GetWeaponShootPosition();
	const Vector_t vecEnd = vecStart + vecForward;

	Trace_t trace = { };
	if (C::Get<bool>(Vars.bTriggerAutoWall))
	{
		SimulateBulletObject_t data = { };

		// check for minimal damage
		if (AUTOWALL::GetDamage(pLocal, vecEnd, &data) < static_cast<float>(C::Get<int>(Vars.iTriggerMinimalDamage)))
			return;

		// copy trace from autowall
		trace = data.enterTrace;
	}
	else
	{
		// otherwise ray new trace
		Ray_t ray(vecStart, vecEnd);
		CTraceFilterSimple filter(pLocal);
		I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
	}

	// check is trace player valid
	if (auto pEntity = static_cast<CCSPlayer*>(trace.pHitEntity); pEntity == nullptr || !pEntity->IsAlive() || pEntity->IsDormant() || !pEntity->IsPlayer() || pEntity->HasImmunity() || !pLocal->IsOtherEnemy(pEntity))
	{
		flLastTargetTime = I::Globals->flRealTime;
		return;
	}

	const float flServerTime = I::Globals->flCurrentTime;

	// check do we can shoot
	if (!pLocal->CanAttack(flServerTime) || !pWeaponCSBaseGun->CanPrimaryAttack(pWeaponData->nWeaponType, flServerTime))
		// don't reset timer
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
		// check is delay elapsed
		if (C::Get<int>(Vars.iTriggerDelay) > 0 && I::Globals->flRealTime - flLastTargetTime < (static_cast<float>(C::Get<int>(Vars.iTriggerDelay)) / 1000.f))
			return;

		pCmd->nButtons |= IN_ATTACK;
	}
}
#pragma endregion
