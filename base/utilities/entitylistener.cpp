#include "entitylistener.h"

// used: cliententitylist interface
#include "../core/interfaces.h"

void CEntityListener::Setup()
{
	I::ClientEntityList->AddListenerEntity(this);
}

void CEntityListener::Destroy()
{
	I::ClientEntityList->RemoveListenerEntity(this);
}

void CEntityListener::OnEntityCreated(CBaseEntity* pEntity)
{
	if (pEntity == nullptr)
		return;

	const int nIndex = pEntity->GetIndex();

	if (nIndex < 0)
		return;

	CBaseClient* pClientClass = pEntity->GetClientClass();

	if (pClientClass == nullptr)
		return;

	// entity class check
	switch (pClientClass->nClassID)
	{
	case EClassIndex::CCSPlayer:
		vecEntities.emplace_back(EntityObject_t(pEntity, nIndex));
		break;
	default:
		break;
	}
}

void CEntityListener::OnEntityDeleted(CBaseEntity* pEntity)
{
	if (pEntity == nullptr)
		return;

	const int nIndex = pEntity->GetIndex();

	if (nIndex < 0)
		return;

	// get current player entry
	const auto current = std::find_if(vecEntities.begin(), vecEntities.end(),
		// compare entry index
		[&](const EntityObject_t& entry)
		{
			return entry.nIndex == nIndex;
		});

	if (current == vecEntities.end())
		return;

	vecEntities.erase(current);
}
