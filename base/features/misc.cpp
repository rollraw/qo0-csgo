#include "misc.h"

// used: cheat variables
#include "../core/variables.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/ibaseclientdll.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/iweaponsystem.h"

// used: sub-features
#include "misc/movement.h"

using namespace F;

#pragma region misc_callbacks
void MISC::OnPreMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket)
{
	if (C::Get<bool>(Vars.bMiscRevealRanks) && (pCmd->nButtons & IN_SCORE))
		// @ida __MsgFunc_ServerRankRevealAll(): client.dll -> "55 8B EC 51 A1 ? ? ? ? 85 C0 75"
		I::Client->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0U, 0, nullptr);

	MOVEMENT::OnPreMove(pLocal, pCmd, pbSendPacket);
}

void MISC::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket)
{
	AutoPistol(pLocal, pCmd);

	MOVEMENT::OnMove(pLocal, pCmd, pbSendPacket);
}

void MISC::OnPostMove(CCSPlayer* pLocal, CUserCmd* pCmd, const bool* pbSendPacket)
{
	MOVEMENT::OnPostMove(pLocal, pCmd, pbSendPacket);
}
#pragma endregion

#pragma region misc_automation
void MISC::AutoPistol(CCSPlayer* pLocal, CUserCmd* pCmd)
{
	if (!C::Get<bool>(Vars.bMiscAutoPistol) || !pLocal->IsAlive())
		return;

	CWeaponCSBaseGun* pWeaponCSBaseGun = static_cast<CWeaponCSBaseGun*>(pLocal->GetActiveWeapon());

	if (pWeaponCSBaseGun == nullptr)
		return;

	const float flServerTime = TICKS_TO_TIME(pLocal->GetTickBase());

	// check do we can attack
	if (!(pCmd->nButtons & IN_ATTACK) || !pLocal->CanAttack(flServerTime))
		return;

	const ItemDefinitionIndex_t nDefinitionIndex = pWeaponCSBaseGun->GetEconItemView()->GetItemDefinitionIndex();
	const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	// check is non-automatic pistol
	if (pWeaponData == nullptr || pWeaponData->bFullAuto || pWeaponData->nWeaponType != WEAPONTYPE_PISTOL)
		return;

	if (pWeaponCSBaseGun->CanPrimaryAttack(pWeaponData->nWeaponType, flServerTime))
		pCmd->nButtons |= IN_ATTACK;
	else
		pCmd->nButtons &= ~IN_ATTACK;
}
#pragma endregion
