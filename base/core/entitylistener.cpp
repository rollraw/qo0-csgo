#include "entitylistener.h"

// used: onplayercreate
#include "../features.h"
// used: mapentity_skiptonextentity, mapentity_parsetoken, centitymapdata
#include "../sdk/entitymapdata.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/icliententitylist.h"
#include "../sdk/interfaces/ibaseclientdll.h"
#include "../sdk/interfaces/iengineclient.h"

class CEntityListener final : public IClientEntityListener
{
public:
	void OnEntityCreated(CBaseEntity* pEntity) override
	{
		const CClientClass* pClientClass = pEntity->GetClientClass();

		if (pClientClass == nullptr)
			return;

		switch (pClientClass->nClassID)
		{
		case EClassIndex::CCSPlayer:
		{
			CCSPlayer* pPlayer = static_cast<CCSPlayer*>(pEntity);

			if (pPlayer->GetIndex() == I::Engine->GetLocalPlayer())
				ENTITY::ParseMapBreakableEntities();

			F::OnPlayerCreated(pPlayer);
			break;
		}
		case EClassIndex::CCSGameRulesProxy:
			static std::uint8_t* pCSGameRulesAddress = MEM::FindPattern(CLIENT_DLL, Q_XOR("8B 35 ? ? ? ? 57 8B F9 85 F6 74 20")) + 0x2;
			ENTITY::pCSGameRulesProxy = *reinterpret_cast<CCSGameRulesProxy**>(pCSGameRulesAddress);
			break;
		case EClassIndex::CCSPlayerResource:
			ENTITY::pCSPlayerResource = reinterpret_cast<CCSPlayerResource*>(pEntity);
			break;
		default:
			break;
		}
	}

	void OnEntityDeleted(CBaseEntity* pEntity) override
	{
		const CClientClass* pClientClass = pEntity->GetClientClass();

		if (pClientClass == nullptr)
			return;

		switch (pClientClass->nClassID)
		{
		case EClassIndex::CCSPlayer:
		{
			CCSPlayer* pPlayer = static_cast<CCSPlayer*>(pEntity);

			F::OnPlayerDeleted(pPlayer);
			break;
		}
		case EClassIndex::CCSGameRulesProxy:
			ENTITY::pCSGameRulesProxy = nullptr;
			break;
		case EClassIndex::CCSPlayerResource:
			ENTITY::pCSPlayerResource = nullptr;
			break;
		default:
			break;
		}

	}
} entityListener;

void ENTITY::Setup()
{
	I::ClientEntityList->AddListenerEntity(&entityListener);

	// manually invoke callbacks, if the injection happened while the user was already on the map
	if (I::Engine->IsInGame())
	{
		for (int i = 1; i <= I::ClientEntityList->GetHighestEntityIndex(); i++)
		{
			if (CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i); pEntity != nullptr)
				entityListener.OnEntityCreated(pEntity);
		}
	}
}

void ENTITY::Destroy()
{
	// manually invoke callbacks, if the unload happened while the user was already on the map
	if (I::Engine->IsInGame())
	{
		for (int i = 1; i <= I::ClientEntityList->GetHighestEntityIndex(); i++)
		{
			if (CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i); pEntity != nullptr)
				entityListener.OnEntityDeleted(pEntity);
		}
	}

	I::ClientEntityList->RemoveListenerEntity(&entityListener);
}

void ENTITY::ParseMapBreakableEntities()
{
	vecBreakableEntities.clear();

	// get BSP data of the map entities
	if (const char* szMapData = I::Engine->GetMapEntitiesString(); szMapData != nullptr)
	{
		char szWorkBuffer[MAPKEY_MAXLENGTH];

		// loop through all entities in the map data
		for (; true; szMapData = MapEntity_SkipToNextEntity(szMapData, szWorkBuffer))
		{
			char szToken[MAPKEY_MAXLENGTH];
			szMapData = MapEntity_ParseToken(szMapData, szToken);

			// check did we've finished parsing
			if (szMapData == nullptr)
				break;

			if (*szToken != '{')
			{
				Q_ASSERT(false); // expected opening brace
				continue;
			}

			// lightweight version of 'MapEntity_ParseEntity' just for data we need
			CEntityMapData entityMapData(const_cast<char*>(szMapData));

			char szKeyName[MAPKEY_MAXLENGTH];
			char szValue[MAPKEY_MAXLENGTH];

			if (entityMapData.GetFirstKey(szKeyName, szValue))
			{
				int iHealth = 0;
				int nSpawnFlags = 0;

				BreakableObject_t breakableObject;

				do
				{
					if (CRT::StringCompare(szKeyName, Q_XOR("health")) == 0)
						iHealth = CRT::StringToInteger<int>(szValue, nullptr, 10);
					else if (CRT::StringCompare(szKeyName, Q_XOR("spawnflags")) == 0)
						nSpawnFlags = CRT::StringToInteger<int>(szValue, nullptr, 10);
					else if (CRT::StringCompare(szKeyName, Q_XOR("model")) == 0)
						CRT::StringCopy(breakableObject.szModelPath, szValue);
					else if (CRT::StringCompare(szKeyName, Q_XOR("classname")) == 0)
						CRT::StringCopy(breakableObject.szClassName, szValue);
				} while (entityMapData.GetNextKey(szKeyName, szValue));

				// check is entity breakable and all values were successfully retrieved
				if (iHealth > 0 && !(nSpawnFlags & 1) && *breakableObject.szModelPath != '\0' && *breakableObject.szClassName != '\0')
					vecBreakableEntities.emplace_back(breakableObject);
			}

			szMapData = entityMapData.CurrentBufferPosition();
		}
	}
}
