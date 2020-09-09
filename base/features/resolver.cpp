#include "resolver.h"

// used: engine, globals, cliententitylist interfaces
#include "../core/interfaces.h"

void CResolver::Run()
{
	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr)
			continue;

		/* if u doing this u minimal need to know basis of c++ and how game works and here i can't be helpful cuz ways of resolve always updates and changes */
	}
}

void CResolver::Event(IGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::Engine->IsInGame())
		return;

	/* check if we hit somebody in which hitbox (head or body) and log her angles to resolve */
}
