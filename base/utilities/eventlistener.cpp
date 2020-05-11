#include "eventlistener.h"

// used: gameevent interface
#include "../core/interfaces.h"

/* features event */
#include "../features/resolver.h"
#include "../features/visuals.h"
#include "../features/misc.h"

void CEventListener::Setup(std::deque<const char*> arrEvents)
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
	CVisuals::Get().Event(pEvent);
	CMiscellaneous::Get().Event(pEvent);
}
