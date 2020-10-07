#pragma once
// used: sdt::deque
#include <deque>

// used: gameevent interface classes
#include "../sdk/interfaces/igameeventmanager.h"

class CEventListener : public IGameEventListener2
{
public:
	void Setup(const std::deque<const char*>& arrEvents);
	void Destroy();

	virtual void FireGameEvent(IGameEvent* pEvent) override;
	virtual int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT;
	}
};
