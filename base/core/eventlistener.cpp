#include "eventlistener.h"

// used: l_print
#include "../utilities/log.h"
// used: onevent
#include "../features.h"

// used: interface handles
#include "interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/igameeventmanager.h"

class CEventListener final : public IGameEventListener2
{
public:
	void FireGameEvent(IGameEvent* pEvent) override
	{
		const FNV1A_t uEventHash = FNV1A::Hash(pEvent->GetName());

		// process all features callbacks
		F::OnEvent(uEventHash, *pEvent);
	}

	int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT;
	}
} eventListener;

bool EVENT::Setup()
{
	bool bSuccess = true;

	bSuccess &= AddToListener(Q_XOR("player_hurt"));

	return bSuccess;
}

void EVENT::Destroy()
{
	I::EventManager->RemoveListener(&eventListener);
}

#pragma region eventlistener_main
bool EVENT::AddToListener(const char* szEventName)
{
	if (!I::EventManager->AddListener(&eventListener, szEventName, false))
	{
		L_PRINT(LOG_WARNING) << Q_XOR("failed to add event: \"") << szEventName << Q_XOR("\" to the listener");
		return false;
	}

	return true;
}

void EVENT::RemoveFromListener(const char* szEventName)
{
	// check is given event exist
	if (CGameEventDescriptor* pDescriptor = I::EventManager->GetEventDescriptor(szEventName); pDescriptor != nullptr)
	{
		// check is listener registered
		if (CGameEventCallback* pListenerCallback = I::EventManager->FindEventListener(&eventListener); pListenerCallback != nullptr)
			// check is given event listened and remove reference
			pDescriptor->vecListeners.FindAndRemove(pListenerCallback);
	}
}

bool EVENT::PrioritizeCallback(const char* szEventName)
{
	CGameEventDescriptor* pDescriptor = I::EventManager->GetEventDescriptor(szEventName);

	// check is given event listened
	if (pDescriptor == nullptr)
		return false;

	// look for listener with our callback
	int nUserCallback = -1;
	for (int i = 0; i < pDescriptor->vecListeners.Count(); i++)
	{
		if (pDescriptor->vecListeners[i]->pCallback == &eventListener)
		{
			nUserCallback = i;
			break;
		}
	}

	// check that we don't have listener for given event or it doesn't have our callback
	if (nUserCallback == -1)
		return false;

	// check is our callback already topmost or there only our callback
	if (nUserCallback == 0)
		return true;

	// make copy of our callback
	//CGameEventCallback* pUserCallbackCopy = new CGameEventCallback(*pDescriptor->vecListeners[nUserCallback]);

	// remove our callback from listeners
	//pDescriptor->vecListeners.Remove(nUserCallback);

	// restore our callback but now at head, shift all other callbacks to right by 1
	//pDescriptor->vecListeners.AddToHead(pUserCallbackCopy);

	// @todo: for some reason with method above callbacks gets called twice | as far as i checked vector is fine, debug 'FireEventIntern' so lazy atm
	CRT::Swap(pDescriptor->vecListeners[0], pDescriptor->vecListeners[nUserCallback]);

	return true;
}
#pragma endregion

#pragma region eventlistener_get
IGameEventListener2* EVENT::GetListener()
{
	return &eventListener;
}
#pragma endregion
