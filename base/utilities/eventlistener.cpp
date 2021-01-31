#include "eventlistener.h"

// used: gameevent interface
#include "../core/interfaces.h"

/* features event */
#include "../features/resolver.h"
#include "../features/visuals.h"

void CEventListener::Setup(const std::deque<const char*>& arrEvents)
{
	if (arrEvents.empty())
		return;

	for (auto szEvent : arrEvents)
	{
		I::GameEvent->AddListener(this, szEvent, false);

		if (!I::GameEvent->FindListener(this, szEvent))
			throw std::runtime_error(XorStr("failed add listener"));
	}
}

void CEventListener::Destroy()
{
	I::GameEvent->RemoveListener(this);
}

void CEventListener::FireGameEvent(IGameEvent* pEvent)
{
	if (pEvent == nullptr)
		return;

	// get hash of event name
	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());

	#pragma region eventlistener_handlers
	CVisuals::Get().Event(pEvent, uNameHash);
	#pragma endregion
}
