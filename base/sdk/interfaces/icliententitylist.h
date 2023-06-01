#pragma once
#include "../datatypes/utlvector.h"
#include "../datatypes/utllinkedlist.h"
#include "../datatypes/utlmap.h"
#include "../datatypes/basehandle.h"

// used: callvfunc
#include "../../utilities/memory.h"

// forward declarations
class IHandleEntity;
class IClientUnknown;
class IClientNetworkable;
class IClientEntity;
class CBaseEntity;

// @source: master/public/icliententitylist.h
// master/game/client/cliententitylist.cpp
// master/game/client/cliententitylist.h

class IClientEntityListener
{
public:
	virtual void OnEntityCreated(CBaseEntity* pEntity) { }
	virtual void OnEntityDeleted(CBaseEntity* pEntity) { }
};

#pragma pack(push, 4)
struct EntityCacheInfo_t
{
	IClientNetworkable* pNetworkable; // 0x00
	unsigned short nBaseEntitiesIndex; // 0x04
	unsigned short bDormant; // 0x06
};
static_assert(sizeof(EntityCacheInfo_t) == 0x8);

class IClientEntityList : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	[[nodiscard]] IClientNetworkable* GetClientNetworkable(int nIndex)
	{
#ifdef Q_ALLOW_VIRTUAL_REBUILD
		Q_ASSERT(nIndex >= 0 && nIndex < MAX_EDICTS);
		return arrEntityCacheInfo[nIndex].pNetworkable;

#else
		return CallVFunc<IClientNetworkable*, 0U>(this, nIndex);
#endif
	}

	[[nodiscard]] IClientNetworkable* GetClientNetworkableFromHandle(CBaseHandle hNetworkable)
	{
		return CallVFunc<IClientNetworkable*, 1U>(this, hNetworkable);
	}

	[[nodiscard]] IClientUnknown* GetClientUnknownFromHandle(CBaseHandle hUnknown)
	{
		return CallVFunc<IClientUnknown*, 2U>(this, hUnknown);
	}

	[[nodiscard]] IClientEntity* GetClientEntity(int nIndex)
	{
		return CallVFunc<IClientEntity*, 3U>(this, nIndex);
	}

	[[nodiscard]] IClientEntity* GetClientEntityFromHandle(CBaseHandle hEntity)
	{
		return CallVFunc<IClientEntity*, 4U>(this, hEntity);
	}

	[[nodiscard]] int GetNumberOfEntities(bool bIncludeNonNetworkable)
	{
	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		return (bIncludeNonNetworkable ? nServerEntitiesCount + nClientNonNetworkableCount : nServerEntitiesCount);
	#else
		return CallVFunc<int, 5U>(this, bIncludeNonNetworkable);
	#endif
	}

	[[nodiscard]] int GetHighestEntityIndex()
	{
	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		return nMaxUsedServerIndex;
	#else
		return CallVFunc<int, 6U>(this);
	#endif
	}

	[[nodiscard]] int GetMaxEntities()
	{
	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		return nMaxServerEntitiesCount;
	#else
		return CallVFunc<int, 8U>(this);
	#endif
	}

	[[nodiscard]] EntityCacheInfo_t* GetClientNetworkableArray()
	{
	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		return arrEntityCacheInfo;
	#else
		return CallVFunc<EntityCacheInfo_t*, 9U>(this);
	#endif
	}

	Q_INLINE void AddListenerEntity(IClientEntityListener* pListener)
	{
		if (vecEntityListeners.Find(pListener) >= 0)
		{
			Q_ASSERT(false); // can't add listeners multiple times
			return;
		}

		vecEntityListeners.AddToTail(pListener);
	}

	Q_INLINE void RemoveListenerEntity(IClientEntityListener* pListener)
	{
		vecEntityListeners.FindAndRemove(pListener);
	}

	template <class T = IClientEntity>
	[[nodiscard]] Q_INLINE T* Get(const int nIndex)
	{
		return static_cast<T*>(GetClientEntity(nIndex));
	}

	template <>
	[[nodiscard]] Q_INLINE IClientEntity* Get(const int nIndex)
	{
		return GetClientEntity(nIndex);
	}

	template <>
	[[nodiscard]] Q_INLINE IClientNetworkable* Get(const int nIndex)
	{
		return GetClientNetworkable(nIndex);
	}

	template <class T = IClientEntity>
	[[nodiscard]] Q_INLINE T* Get(const CBaseHandle& hEntity)
	{
		return static_cast<T*>(GetClientEntityFromHandle(hEntity));
	}

	template <>
	[[nodiscard]] Q_INLINE IClientEntity* Get(const CBaseHandle& hEntity)
	{
		return GetClientEntityFromHandle(hEntity);
	}

	template <>
	[[nodiscard]] Q_INLINE IClientNetworkable* Get(const CBaseHandle& hEntity)
	{
		return GetClientNetworkableFromHandle(hEntity);
	}

	template <>
	[[nodiscard]] Q_INLINE IClientUnknown* Get(const CBaseHandle& hEntity)
	{
		return GetClientUnknownFromHandle(hEntity);
	}

private:
	void* pVTable; // 0x000000
public:
	CUtlVector<IClientEntityListener*> vecEntityListeners; // 0x000004 // @ida: client.dll -> ["8B 86 ? ? ? ? 53 8B 0C B8 8B 01 FF 10" + 0x2] - 0x20014
	int nServerEntitiesCount; // 0x000018 // @ida: client.dll -> ["FF 86 ? ? ? ? 3B" + 0x2]
	int nMaxServerEntitiesCount; // 0x00001C
	int nClientNonNetworkableCount; // 0x000020 // @ida: client.dll -> ["FF 86 ? ? ? ? 8B BE" + 0x2] - 0x20014
	int nMaxUsedServerIndex; // 0x000024
	EntityCacheInfo_t arrEntityCacheInfo[NUM_ENT_ENTRIES]; // 0x000028 // @ida: client.dll -> ["8D 9E ? ? ? ? 8D 1C" + 0x2] - 0x20014
	CUtlLinkedList<CBaseEntity*, unsigned short> vecBaseEntities; // 0x010028 // @ida: client.dll -> ["8D 8E ? ? ? ? E8 ? ? ? ? 0F B7 F8" + 0x2] - 0x20014
	CUtlLinkedList<void*, unsigned short> vecPVSNotifyInfos; // 0x010044 // @ida: client.dll -> ["8D 8E ? ? ? ? E8 ? ? ? ? 0F B7 D0" + 0x2] - 0x20014
	CUtlMap<IClientUnknown*, unsigned short, unsigned short> mapPVSNotifier; // 0x010060 // @ida: client.dll -> ["81 C1 ? ? ? ? E8 ? ? ? ? 5B" + 0x1] - 0x20014
};
static_assert(sizeof(IClientEntityList) == 0x1007C);
#pragma pack(pop)
