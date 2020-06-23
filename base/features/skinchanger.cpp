#include "skinchanger.h"

// used: get localplayer
#include "../utilities.h"
// used: client, engine interfaces
#include "../core/interfaces.h"

IClientNetworkable* CL_CreateDLLEntity(int iEntity, EClassIndex classIndex, int nSerial)
{
	CBaseClient* pClient = I::Client->GetAllClasses();

	while (pClient)
	{
		if (pClient->nClassID == classIndex)
			return pClient->pCreateFn(iEntity, nSerial);
		pClient = pClient->pNext;
	}

	return false;
}

void CSkinChanger::Run()
{
	CBaseEntity* pLocal = U::GetLocalPlayer();

	if (pLocal == nullptr || !pLocal->IsAlive())
		return;

	PlayerInfo_t pInfo;
	if (!I::Engine->GetPlayerInfo(I::Engine->GetLocalPlayer(), &pInfo))
		return;

	// it will help you here my sweetest candy <3
	// https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Skin_Changer
}

void CSkinChanger::Event(IGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (pEvent == nullptr || !I::Engine->IsInGame())
		return;
}
