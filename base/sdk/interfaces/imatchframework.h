#pragma once

class IPlayerLocal
{
public:
	int GetXUIDLow()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x8);
	}

	int GetXUIDHigh()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0xC);
	}

	const char* GetName()
	{
		return MEM::CallVFunc<const char*>(this, 2);
	}
};

// @note: not a full class, full here: https://github.com/pmrowla/hl2sdk-csgo/blob/master/public/matchmaking/iplayermanager.h
class IPlayerManager
{
public:
	IPlayerLocal* GetLocalPlayer(int nIndex) //	returns a local player interface for a given controller index
	{
		return MEM::CallVFunc<IPlayerLocal*>(this, 1, nIndex);
	}
};

// @credits: https://github.com/pmrowla/hl2sdk-csgo/blob/master/public/matchmaking/imatchframework.h
class IMatchExtensions;
class IMatchEventsSubscription;
class IMatchTitle;
class IMatchNetworkMsgController;

class IMatchSession
{
public:
	virtual CKeyValues* GetSessionSystemData() = 0; // get an internal pointer to session system-specific data
	virtual CKeyValues* GetSessionSettings() = 0; // get an internal pointer to session settings
	virtual void		UpdateSessionSettings(CKeyValues* pSettings) = 0; // update session settings, only changing keys and values need to be passed and they will be updated
	virtual void		Command(CKeyValues* pCommand) = 0; // issue a session command
};

// @note: not a full class, full here: https://github.com/pmrowla/hl2sdk-csgo/blob/master/public/matchmaking/imatchsystem.h
class IMatchSystem
{
public:
	virtual IPlayerManager* GetPlayerManager() = 0;
};

class IMatchFramework : public IAppSystem
{
public:
	virtual void						RunFrame() = 0; // run frame of the matchmaking framework
	virtual IMatchExtensions*			GetMatchExtensions() = 0; // get matchmaking extensions
	virtual IMatchEventsSubscription*	GetEventsSubscription() = 0; // get events container
	virtual IMatchTitle*				GetMatchTitle() = 0; // get the matchmaking title interface
	virtual IMatchSession*				GetMatchSession() = 0; // get the match session interface of the current match framework type
	virtual IMatchNetworkMsgController* GetMatchNetworkMsgController() = 0; // get the network msg encode/decode factory
	virtual IMatchSystem*				GetMatchSystem() = 0; // get match system interface
	virtual void						CreateSession(CKeyValues* pSettings) = 0; // entry point to create session
	virtual void						MatchSession(CKeyValues* pSettings) = 0; // entry point to match into a session
	virtual void						AcceptInvite(int iController) = 0; // accept invite
	virtual void						CloseSession() = 0; // close the session
};
