#include "proxies.h"

// used: ccsplayer, cbasecombatweapon
#include "../sdk/entity.h"

// used: interface definitions
#include "../sdk/interfaces/iweaponsystem.h"

bool P::Setup()
{
	// @note: as example
#if 0
	if (!hkBaseViewModelSequence.Create(NETVAR::GetProperty(FNV1A::HashConst("DT_BaseViewModel::m_nSequence")), &BaseViewModelSequence))
		return false;
#endif

	return true;
}

void P::Destroy()
{
	// @note: as example
#if 0
	hkBaseViewModelSequence.Restore();
#endif
}

#pragma region proxies_handlers
void P::BaseViewModelSequence(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	const auto oSequence = hkBaseViewModelSequence.GetOriginal();

	if (CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer(); pLocal != nullptr && pLocal->IsAlive())
	{
		if (CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon(); pWeapon != nullptr)
		{
			// check is deagle and playing inspect animation
			if (pWeapon->GetEconItemView()->GetItemDefinitionIndex() == WEAPON_DEAGLE && pData->Value.lValue == 7)
				// force spinning animation
				const_cast<CRecvProxyData*>(pData)->Value.lValue = 8;
		}
	}

	oSequence(pData, pStruct, pOut);
}
#pragma endregion
