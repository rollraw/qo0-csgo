#pragma once
#include "../datatypes/keyvalues.h"

// used: max_player_name_length
#include "../const.h"
// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/public/matchmaking/iplayer.h
// master/matchmaking/player.cpp
// master/matchmaking/player.h
#pragma pack(push, 4)
class IPlayer : protected ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	std::uint64_t GetXUID()
	{
	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		return ullXUID;
	#else
		return CallVFunc<std::uint64_t, 0U>(this);
	#endif
	}

	int GetIndex()
	{
	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		return iController;
	#else
		return CallVFunc<int, 1U>(this);
	#endif
	}

	const char* GetName()
	{
	#ifdef Q_ALLOW_VIRTUAL_REBUILD
		return szName;
	#else
		return CallVFunc<const char*, 2U>(this);
	#endif
	}

private:
	void* pVTableIPlayer; // 0x00
	void* pVTablePlayer; // 0x04
public:
	std::uint64_t ullXUID; // 0x08
	int iController; // 0x10
	int nOnlineState; // 0x14
	char szName[MAX_PLAYER_NAME_LENGTH]; // 0x18
};
static_assert(sizeof(IPlayer) == 0x98);
#pragma pack(pop)

class IPlayerLocal : public IPlayer
{
public:

};

// @source: master/public/matchmaking/iplayermanager.h
class IPlayerManager : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	// returns a local player interface for a given controller index
	IPlayerLocal* GetLocalPlayer(int nIndex)
	{
		return CallVFunc<IPlayerLocal*, 1U>(this, nIndex);
	}
};

// @source: master/public/matchmaking/imatchframework.h
class IMatchExtensions;
class IMatchEventsSubscription;
class IMatchTitle;
class IMatchNetworkMsgController;
class IMatchSession
{
public:
	// @todo: virtual calls if those are checked
	virtual CKeyValues* GetSessionSystemData() = 0; // get an internal pointer to session system-specific data
	virtual CKeyValues* GetSessionSettings() = 0; // get an internal pointer to session settings
	virtual void UpdateSessionSettings(CKeyValues* pSettings) = 0; // update session settings, only changing keys and values need to be passed and they will be updated
	virtual void Command(CKeyValues* pCommand) = 0; // issue a session command
};

// @source: master/public/matchmaking/imatchsystem.h
class IMatchSystem : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	// returns a local player interface for a given controller index
	IPlayerManager* GetPlayerManager()
	{
		return CallVFunc<IPlayerManager*, 0U>(this);
	}
};

class IMatchFramework : public IAppSystem
{
public:
	// @todo: virtual calls if those are checked
	virtual void RunFrame() = 0; // run frame of the matchmaking framework
	virtual IMatchExtensions* GetMatchExtensions() = 0; // get matchmaking extensions
	virtual IMatchEventsSubscription* GetEventsSubscription() = 0; // get events container
	virtual IMatchTitle* GetMatchTitle() = 0; // get the matchmaking title interface
	virtual IMatchSession* GetMatchSession() = 0; // get the match session interface of the current match framework type
	virtual IMatchNetworkMsgController* GetMatchNetworkMsgController() = 0; // get the network msg encode/decode factory
	virtual IMatchSystem* GetMatchSystem() = 0; // get match system interface
	virtual void CreateSession(CKeyValues* pSettings) = 0; // entry point to create session
	virtual void MatchSession(CKeyValues* pSettings) = 0; // entry point to match into a session
	virtual void AcceptInvite(int iController) = 0; // accept invite
	virtual void CloseSession() = 0; // close the session
};
