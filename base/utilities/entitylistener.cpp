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
		vecEntities.emplace_back(pEntity, nIndex);
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

	if (const auto it = std::ranges::find(vecEntities, nIndex, &EntityObject_t::nIndex); it != vecEntities.end())
		vecEntities.erase(it);
}
