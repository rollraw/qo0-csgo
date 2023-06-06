#pragma once
#include "../datatypes/keyvalues.h"
#include "../datatypes/utlsymbol.h"
#include "../datatypes/utlvector.h"
#include "../datatypes/utldict.h"

// used: cthreadfastmutex
#include "../thread.h"
// used: ibaseinterface
#include "ibaseinterface.h"
// used: stringcomparei
#include "../../utilities/crt.h"

// @source: master/public/igameevents.h
// master/engine/GameEventManager.cpp
// master/engine/GameEventManager.h

#pragma region gameeventmanager_definitions
#define MAX_EVENT_NAME_LENGTH 32 // max game event name length
#define MAX_EVENT_BITS 9 // max bits needed for an event index
#define MAX_EVENT_NUMBER (1 << MAX_EVENT_BITS) // max number of events allowed
#define MAX_EVENT_BYTES 1024 // max size in bytes for a serialized event

#define EVENT_DEBUG_ID_INIT 42
#define EVENT_DEBUG_ID_SHUTDOWN 13
#pragma endregion

#pragma pack(push, 4)

class CGameEventCallback
{
public:
	void* operator new(const std::size_t nSize)
	{
		return I::MemAlloc->Alloc(nSize);
	}

	void operator delete(void* pMemory)
	{
		I::MemAlloc->Free(pMemory);
	}

	void* pCallback; // 0x00
	int nListenerType; // 0x04
};

static_assert(sizeof(CGameEventCallback) == 0x8);

class CGameEventDescriptor
{
public:
	int nEventID; // 0x00
	int nElementIndex; // 0x04
	CKeyValues* pKeys; // 0x08
	CUtlVector<CGameEventCallback*> vecListeners; // 0x0C
	bool bLocal; // 0x20
	bool bReliable; // 0x21
	int nSerialized; // 0x24
	int nUnSerialized; // 0x28
	int nTotalSerializedBits; // 0x2C
	int nTotalUnserializedBits; // 0x30
};
static_assert(sizeof(CGameEventDescriptor) == 0x34);

class IGameEvent : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	virtual ~IGameEvent() { }

	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] bool IsReliable() const
	{
		return CallVFunc<bool, 2U>(this);
	}

	[[nodiscard]] bool IsLocal() const
	{
		return CallVFunc<bool, 3U>(this);
	}

	[[nodiscard]] bool IsEmpty(const char* szKeyName = nullptr)
	{
		return CallVFunc<bool, 4U>(this, szKeyName);
	}

	[[nodiscard]] bool GetBool(const char* szKeyName = nullptr, bool bDefault = false) const
	{
		return CallVFunc<bool, 5U>(this, szKeyName, bDefault);
	}

	[[nodiscard]] int GetInt(const char* szKeyName = nullptr, int iDefault = 0) const
	{
		// @ida: engine.dll -> U8["68 ? ? ? ? FF 50 ? F3 0F 10 47" + 0x7] / sizeof(std::uintptr_t) @xref: "target2"
		return CallVFunc<int, 6U>(this, szKeyName, iDefault);
	}

	[[nodiscard]] std::uint64_t GetUint64(const char* szKeyName = nullptr, std::uint64_t ullDefault = 0ULL) const
	{
		return CallVFunc<std::uint64_t, 7U>(this, szKeyName, ullDefault);
	}

	[[nodiscard]] float GetFloat(const char* szKeyName = nullptr, float flDefault = 0.f) const
	{
		// @ida: engine.dll -> U8["8B 40 ? FF D0 F3 0F 10 47 ? D9 5F 58" + 0x2] / sizeof(std::uintptr_t) @xref: "theta"
		return CallVFunc<float, 8U>(this, szKeyName, flDefault);
	}

	[[nodiscard]] const char* GetString(const char* szKeyName = nullptr, const char* szDefault = "") const
	{
		return CallVFunc<const char*, 9U>(this, szKeyName, szDefault);
	}

	[[nodiscard]] const wchar_t* GetWString(const char* szKeyName = nullptr, const wchar_t* wszDefault = L"") const
	{
		return CallVFunc<const wchar_t*, 10U>(this, szKeyName, wszDefault);
	}

	[[nodiscard]] const void* GetPtr(const char* szKeyName = nullptr) const
	{
		return CallVFunc<const void*, 11U>(this, szKeyName);
	}

	void SetBool(const char* szKeyName, bool bValue)
	{
		CallVFunc<void, 12U>(this, szKeyName, bValue);
	}

	void SetInt(const char* szKeyName, int iValue)
	{
		CallVFunc<void, 13U>(this, szKeyName, iValue);
	}

	void SetUint64(const char* szKeyName, std::uint64_t ullValue)
	{
		CallVFunc<void, 14U>(this, szKeyName, ullValue);
	}

	void SetFloat(const char* szKeyName, float flValue)
	{
		CallVFunc<void, 15U>(this, szKeyName, flValue);
	}

	void SetString(const char* szKeyName, const char* szValue)
	{
		CallVFunc<void, 16U>(this, szKeyName, szValue);
	}

	void SetWString(const char* szKeyName, const wchar_t* wszValue)
	{
		CallVFunc<void, 17U>(this, szKeyName, wszValue);
	}

	void SetPtr(const char* szKeyName, const void* pValue)
	{
		CallVFunc<void, 18U>(this, szKeyName, pValue);
	}

	[[nodiscard]] bool ForEventData(/*IGameEventVisitor2**/void* pEventVisitor) const
	{
		return CallVFunc<bool, 19U>(this, pEventVisitor);
	}

public:
	CGameEventDescriptor* pDescriptor; // 0x00
	CKeyValues* pDataKeys; // 0x04
};
static_assert(sizeof(IGameEvent) == 0xC);

class IGameEventListener2
{
public:
	virtual ~IGameEventListener2() { }
	virtual void FireGameEvent(IGameEvent* pEvent) = 0;
	virtual int GetEventDebugID()
	{
		return nDebugID;
	}

public:
	int	nDebugID; // 0x04
};
static_assert(sizeof(IGameEventListener2) == 0x8);

class CGameEventListener : public IGameEventListener2
{
public:
	bool bRegisteredForEvents; // 0x08
};
static_assert(sizeof(CGameEventListener) == 0xC);

class CSVCMsg_GameEvent;

class IGameEventManager2 : public IBaseInterface, ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	enum
	{
		SERVERSIDE = 0, // this is a server side listener, event logger etc
		CLIENTSIDE, // this is a client side listenet, HUD element etc
		CLIENTSTUB, // this is a serverside stub for a remote client listener (used by engine only)
		SERVERSIDE_OLD, // legacy support for old server event listeners
		CLIENTSIDE_OLD // legacy support for old client event listeners
	};

	virtual ~IGameEventManager2() { }

	/// load game event descriptions from a file e.g. "resource\gameevents.res"
	/// @returns: count of loaded game events
	int LoadEventsFromFile(const char* szFileName)
	{
		return CallVFunc<int, 1U>(this, szFileName);
	}

	// remove all and anything
	void Reset()
	{
		CallVFunc<void, 2U>(this);
	}

	/// add a listener for a particular event
	/// @returns: true if listener was successfully added for given event or was already there, false otherwise
	bool AddListener(IGameEventListener2* pListener, const char* szName, bool bServerSide)
	{
		return CallVFunc<bool, 3U>(this, pListener, szName, bServerSide);
	}

	/// @returns: true if given listener is listens to given event, false otherwise
	bool FindListener(IGameEventListener2* pListener, const char* szName)
	{
		return CallVFunc<bool, 4U>(this, pListener, szName);
	}

	// remove a listener
	void RemoveListener(IGameEventListener2* pListener)
	{
		CallVFunc<void, 5U>(this, pListener);
	}

	// add a listener that listens to all events
	void AddListenerGlobal(IGameEventListener2* pListener, bool bServerSide)
	{
		CallVFunc<void, 6U>(this, pListener, bServerSide);
	}

	/// create an event by name, but doesn't fire it
	/// @param[in] bForce forces the creation even if no listener is active
	/// @returns: pointer to created event if it is know or have listener registered for it, null otherwise
	IGameEvent* CreateNewEvent(const char* szName, bool bForce = false, int* pCookie = nullptr)
	{
		return CallVFunc<IGameEvent*, 7U>(this, szName, bForce, pCookie);
	}

	/// fires a server event created earlier
	/// @param[in] bDontBroadcast when set event is not send to clients
	/// @returns: true if event was fired successfully, false otherwise
	bool FireEvent(IGameEvent* pEvent, bool bDontBroadcast = false)
	{
		return CallVFunc<bool, 8U>(this, pEvent, bDontBroadcast);
	}

	/// fires an event for the local client only, should be used only by client code
	/// @returns: true if event was fired successfully, false otherwise
	bool FireEventClientSide(IGameEvent* pEvent)
	{
		return CallVFunc<bool, 9U>(this, pEvent);
	}

	// create a new copy of given event, must be freed later
	IGameEvent* DuplicateEvent(IGameEvent* pEvent)
	{
		return CallVFunc<IGameEvent*, 10U>(this, pEvent);
	}

	// if an event was created but not fired for some reason, it has to bee freed, same 'UnserializeEvent()'
	void FreeEvent(IGameEvent* pEvent)
	{
		CallVFunc<void, 11U>(this, pEvent);
	}

	// write/read event to/from bitbuffer
	bool SerializeEvent(IGameEvent* pEvent, CSVCMsg_GameEvent* pEventMessage)
	{
		return CallVFunc<bool, 12U>(this, pEvent, pEventMessage);
	}

	IGameEvent* UnserializeEvent(const CSVCMsg_GameEvent& eventMessage)
	{
		return CallVFunc<IGameEvent*, 13U>(this, &eventMessage);
	}

	CKeyValues* GetEventDataTypes(IGameEvent* pEvent)
	{
		return CallVFunc<CKeyValues*, 14U>(this, pEvent);
	}

public:
	CGameEventDescriptor* GetEventDescriptor(const char* szEventName, int* pCookie = nullptr)
	{
		// @ida CGameEventManager::GetEventDescriptor(): engine.dll -> "55 8B EC 83 EC 08 53 8B 5D 08 56 8B F1 89"

		constexpr std::uint32_t uCookieBit = 0x80000000;
		constexpr std::uint32_t uCookieMask = ~uCookieBit;

		if (szEventName == nullptr || szEventName[0] == '\0')
			return nullptr;

		if (pCookie != nullptr && *pCookie != 0)
		{
			const int nGameEventIndex = static_cast<int>(static_cast<std::uint32_t>(*pCookie) & uCookieMask);

			if (CGameEventDescriptor* pDescriptor = &vecGameEvents[nGameEventIndex]; !CRT::StringCompareI(szEventName, mapEvents.GetElementName(pDescriptor->nElementIndex)))
				return pDescriptor;
		}

		const int nEventMapIndex = mapEvents.Find(szEventName);

		if (nEventMapIndex == mapEvents.InvalidIndex())
			return nullptr;

		const int nGameEventIndex = mapEvents[nEventMapIndex];

		if (pCookie != nullptr)
			*pCookie = static_cast<int>(static_cast<std::uint32_t>(nGameEventIndex) | uCookieBit);

		return &vecGameEvents[nGameEventIndex];
	}

	CGameEventCallback* FindEventListener(const void* pCallback)
	{
		for (int i = 0; i < vecListeners.Count(); i++)
		{
			if (CGameEventCallback* pListener = vecListeners[i]; pListener->pCallback == pCallback)
				return pListener;
		}

		return nullptr;
	}

public:
	CUtlVector<CGameEventDescriptor> vecGameEvents; // 0x04 // list of all known events
	CUtlVector<CGameEventCallback*> vecListeners; // 0x18 // list of all registered listeners // @ida: engine.dll -> U8["8B 4B ? 8B 39 8B 75 08" + 0x2]
	std::byte pad0[0x34]; // 0x2C
	CUtlVector<CUtlSymbol> vecEventFileNames; // 0x60
	CUtlDict<int, int> mapEvents; // 0x74 // @ida: engine.dll -> U8["8B 40 ? FF 74 C8 10 68 ? ? ? ? EB" + 0x2] - 0x4
	CThreadFastMutex mutex; // 0x98 // @ida: engine.dll -> ["8D B3 ? ? ? ? FF 15 ? ? ? ? 8B F8" + 0x2]
	bool bClientListenersChanged; // 0xA0 // true every time client changed listeners // @ida: engine.dll -> ["C6 87 ? ? ? ? ? B0 01 EB" + 0x2]
};
static_assert(sizeof(IGameEventManager2) == 0xA4);
#pragma pack(pop)
