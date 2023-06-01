#pragma once
#include "../datatypes/utlvector.h"

// used: iappsystem
#include "iclientmode.h"

// @source: master/public/tier1/iconvar.h

#pragma region convar_enumerations
// command to convars and concommands
enum EConVarFlag : int
{
	// convar systems
	FCVAR_NONE = 0,
	FCVAR_UNREGISTERED =			(1 << 0),	// if this is set, don't add to linked list, etc
	FCVAR_DEVELOPMENTONLY =			(1 << 1),	// hidden in released products. flag is removed automatically if allow_development_cvars is defined
	FCVAR_GAMEDLL =					(1 << 2),	// defined by the game dll
	FCVAR_CLIENTDLL =				(1 << 3),	// defined by the client dll
	FCVAR_HIDDEN =					(1 << 4),	// hidden. doesn't appear in find or autocomplete. like developmentonly, but can't be compiled out

	// convar only
	FCVAR_PROTECTED =				(1 << 5),	// it's a server cvar, but we don't send the data since it's a password, etc. sends 1 if it's not bland/zero, 0 otherwise as value
	FCVAR_SPONLY =					(1 << 6),	// this cvar cannot be changed by clients connected to a multiplayer server
	FCVAR_ARCHIVE =					(1 << 7),	// set to cause it to be saved to vars.rc
	FCVAR_NOTIFY =					(1 << 8),	// notifies players when changed
	FCVAR_USERINFO =				(1 << 9),	// changes the client's info string
	FCVAR_CHEAT =					(1 << 14),	// only useable in singleplayer/debug/multiplayer & sv_cheats
	FCVAR_PRINTABLEONLY =			(1 << 10),	// this cvar's string cannot contain unprintable characters (e.g., used for player name etc)
	FCVAR_UNLOGGED =				(1 << 11),	// if this is a fcvar_server, don't log changes to the log file / console if we are creating a log
	FCVAR_NEVER_AS_STRING =			(1 << 12),	// never try to print that cvar

	// it's a convar that's shared between the client and the server.
	// at signon, the values of all such convars are sent from the server to the client (skipped for local client, ofc)
	// if a change is requested it must come from the console (i.e., no remote client changes)
	// if a value is changed while a server is active, it's replicated to all connected clients
	FCVAR_REPLICATED =				(1 << 13),	// server setting enforced on clients, replicated
	// @todo: (1 << 14) used by the game, probably used as modification detection
	FCVAR_DEMO =					(1 << 16),	// record this cvar when starting a demo file
	FCVAR_DONTRECORD =				(1 << 17),	// don't record these command in demofiles
	FCVAR_RELOAD_MATERIALS =		(1 << 20),	// if this cvar changes, it forces a material reload
	FCVAR_RELOAD_TEXTURES =			(1 << 21),	// if this cvar changes, if forces a texture reload
	FCVAR_NOT_CONNECTED =			(1 << 22),	// cvar cannot be changed by a client that is connected to a server
	FCVAR_MATERIAL_SYSTEM_THREAD =	(1 << 23),	// indicates this cvar is read from the material system thread
	FCVAR_ARCHIVE_XBOX =			(1 << 24),	// cvar written to config.cfg on the xbox
	FCVAR_ACCESSIBLE_FROM_THREADS = (1 << 25),	// used as a debugging tool necessary to check material system thread convars
	FCVAR_SERVER_CAN_EXECUTE =		(1 << 28),	// the server is allowed to execute this command on clients via clientcommand/net_stringcmd/cbaseclientstate::processstringcmd
	FCVAR_SERVER_CANNOT_QUERY =		(1 << 29),	// if this is set, then the server is not allowed to query this cvar's value (via iserverpluginhelpers::startquerycvarvalue)
	FCVAR_CLIENTCMD_CAN_EXECUTE =	(1 << 30),	// ivengineclient::clientcmd is allowed to execute this command
	FCVAR_MATERIAL_THREAD_MASK =	(FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD)
};
#pragma endregion

using CVarDLLIdentifier_t = int;

using FnCommandCallbackV1_t = void(Q_CDECL*)();
using FnChangeCallback_t = void(Q_CDECL*)(void*, const char*, float);

class CConCommandBase : protected ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	virtual ~CConCommandBase() { }

	[[nodiscard]] bool IsCommand() const
	{
		return CallVFunc<bool, 1U>(this);
	}

	[[nodiscard]] bool IsFlagSet(int nFlag) const
	{
		return CallVFunc<bool, 2U>(this, nFlag);
	}

	void AddFlags(int nFlag)
	{
		CallVFunc<void, 3U>(this, nFlag);
	}

	void RemoveFlags(int nFlag)
	{
		CallVFunc<void, 4U>(this, nFlag);
	}

	[[nodiscard]] int GetFlags() const
	{
		return CallVFunc<int, 5U>(this);
	}

	[[nodiscard]] const char* GetName() const
	{
		// @ida ConVar::GetName(): client.dll -> "8B 41 1C 8B 40 0C"
		return CallVFunc<const char*, 6U>(this);
	}

	[[nodiscard]] const char* GetHelpText() const
	{
		return CallVFunc<const char*, 7U>(this);
	}

	[[nodiscard]] bool IsRegistered() const
	{
		return CallVFunc<bool, 8U>(this);
	}

public:
	CConCommandBase* pNext; // 0x04
	std::uint32_t bRegistered : 1; // 0x08 // @ida: engine.dll -> U8["8B 40 ? 83 E0 01 C3" + 0x2] | vstdlib.dll -> U8["83 4E ? 01 FF 50 18 85 C0 0F" + 0x2]
	std::uint32_t nFlagsBackup : 31; // 0x08 // @note: used to detect convars modification, added since 21.10.2022 (version 1.38.4.5, build 1524)
	const char* szName; // 0x0C
	const char* szHelpText; // 0x10
	int nFlags; // 0x14
	FnCommandCallbackV1_t pCallback; // 0x18
};
static_assert(sizeof(CConCommandBase) == 0x1C);

class CConCommand : public CConCommandBase
{
public:
	virtual ~CConCommand() { }

public:
	union
	{
		FnCommandCallbackV1_t fnCommandCallbackV1;
		void* fnCommandCallback;
		void* pCommandCallback;
	}; // 0x1C

	union
	{
		void* fnCompletionCallback;
		void* fnCommandCompletionCallback;
	}; // 0x20

	bool bHasCompletionCallback : 1; // 0x24
	bool bUsingNewCommandCallback : 1; // 0x24
	bool bUsingCommandCallbackInterface : 1; // 0x24
};
static_assert(sizeof(CConCommand) == 0x28);

class CConVar : public CConCommandBase
{
public:
	virtual ~CConVar() { }

	[[nodiscard]] float GetFloat() const
	{
		// @ida ConVar::GetFloat(): client.dll -> "55 8B EC 51 56 8B F1 8B 4E 1C 3B CE 75 4C"

	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		// @test: even not modified convars can trigger this ~valve
		//Q_ASSERT(value.flValue == valueBackup.flValue && value.iValue == valueBackup.iValue && (nFlags ^ nFlagsBackup) == 0); // you've triggered possible detection vector; use virtual calls to set values instead of direct access
		const std::uint32_t uObscure = std::bit_cast<std::uint32_t>(pParent->value.flValue) ^ reinterpret_cast<std::intptr_t>(this);
		return std::bit_cast<float>(uObscure);
	#else
		return CallVFunc<float, 12U>(this);
	#endif
	}

	[[nodiscard]] int GetInt() const
	{
		// @ida ConVar::GetInt(): client.dll -> "56 8B F1 8B 4E 1C 3B CE 75 3E"

	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		// @test: even not modified convars can trigger this ~valve
		//Q_ASSERT(value.flValue == valueBackup.flValue && value.iValue == valueBackup.iValue && (nFlags ^ nFlagsBackup) == 0); // you've triggered possible detection vector; use virtual calls to set values instead of direct access
		return (pParent->value.iValue ^ reinterpret_cast<std::intptr_t>(this));
	#else
		return CallVFunc<int, 13U>(this);
	#endif
	}

	[[nodiscard]] Q_INLINE bool GetBool() const
	{
		return !!GetInt();
	}

	[[nodiscard]] const char* GetString() const
	{
		// @xref: "FCVAR_NEVER_AS_STRING"

		if (nFlags & FCVAR_NEVER_AS_STRING)
			return "FCVAR_NEVER_AS_STRING";

		const char* szValue = pParent->value.szString;
		return szValue != nullptr ? szValue : "";
	}

	void SetValue(const char* szNewValue)
	{
		return CallVFunc<void, 14U>(this, szNewValue);
	}

	void SetValue(float flNewValue)
	{
		return CallVFunc<void, 15U>(this, flNewValue);
	}

	void SetValue(int iNewValue)
	{
		return CallVFunc<void, 16U>(this, iNewValue);
	}

	void SetValue(Color_t colNewValue)
	{
		return CallVFunc<void, 17U>(this, colNewValue);
	}

public:
	struct CVValue_t
	{
		char* szString; // 0x00
		int nStringLength; // 0x04
		float flValue; // 0x08
		int iValue; // 0x0C
	};

	CConVar* pParent; // 0x1C
	const char* szDefaultValue; // 0x20
	CVValue_t value; // 0x24
	CVValue_t valueBackup; // 0x34 // @note: used to detect convars modification, added since 21.10.2022 (version 1.38.4.5, build 1524)
	bool bHasMin; // 0x44
	float flMinValue; // 0x48
	bool bHasMax; // 0x4C
	float flMaxValue; // 0x50
	CUtlVector<FnChangeCallback_t> vecChangeCallbacks; // 0x54 // @ida: engine.dll -> U8["8B 4B ? 39 31 74 40 40" + 0x2]
};
static_assert(sizeof(CConVar) == 0x68); // size verify @ida: engine.dll -> U8["FF 50 04 85 C0 74 1B 68" - 0x5] @xref: "Custom user info value"

class IConVar : public IAppSystem, ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	class ICVarIteratorInternal
	{
	public:
		virtual void SetFirst() = 0;
		virtual void Next() = 0;
		virtual	bool IsValid() = 0;
		virtual CConCommandBase* Get() = 0;
	};

	void RegisterConCommand(CConVar* pCommandBase, int iDefaultValue = 1)
	{
		CallVFunc<void, 10U>(this, pCommandBase, iDefaultValue);
	}

	void UnregisterConCommand(CConVar* pCommandBase)
	{
		CallVFunc<void, 11U>(this, pCommandBase);
	}

	const char* GetCommandLineValue(const char* szVariableName)
	{
		return CallVFunc<const char*, 13U>(this, szVariableName);
	}

	CConCommandBase* FindCommandBase(const char* szName)
	{
		return CallVFunc<CConCommandBase*, 14U>(this, szName);
	}

	const CConCommandBase* FindCommandBase(const char* szName) const
	{
		return CallVFunc<const CConCommandBase*, 15U>(this, szName);
	}

	CConVar* FindVar(const char* szName)
	{
		return CallVFunc<CConVar*, 16U>(this, szName);
	}

	const CConVar* FindVar(const char* szName) const
	{
		return CallVFunc<const CConVar*, 17U>(this, szName);
	}

	CConCommand* FindCommand(const char* szName)
	{
		return CallVFunc<CConCommand*, 18U>(this, szName);
	}

	const CConCommand* FindCommand(const char* szName) const
	{
		return CallVFunc<const CConCommand*, 19U>(this, szName);
	}

	template <typename... Args_t>
	void ConsoleColorPrintf(const Color_t& colText, const char* szFormat, Args_t... argList) const
	{
		// @note: don't use 'CallVFunc()' due to variadic args forwarding
		MEM::GetVFunc<void(Q_CDECL*)(const IConVar*, const Color_t&, const char*, ...)>(this, 25U)(this, colText, szFormat, argList...);
	}

	template <typename... Args_t>
	void ConsolePrintf(const char* szFormat, Args_t... argList) const
	{
		// @note: don't use 'CallVFunc()' due to variadic args forwarding
		MEM::GetVFunc<void(Q_CDECL*)(const IConVar*, const char*, ...)>(this, 26U)(this, szFormat, argList...);
	}

	template <typename... Args_t>
	void ConsoleDPrintf(const char* szFormat, Args_t... argList) const
	{
		// @note: don't use 'CallVFunc()' due to variadic args forwarding
		MEM::GetVFunc<void(Q_CDECL*)(const IConVar*, const char*, ...)>(this, 27U)(this, szFormat, argList...);
	}

	ICVarIteratorInternal* FactoryInternalIterator()
	{
		return CallVFunc<ICVarIteratorInternal*, 45U>(this);
	}
};
