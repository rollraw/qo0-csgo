#pragma once

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/server_class.h
class CBaseServer
{
public:
	char*			szNetworkName;
	void*			pTable;
	CBaseServer*	pNext;
	int				iClassID;
	int				nInstanceBaselineIndex;
};

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/eiface.h
class CSaveRestoreData;
class IServerGameDLL
{
public:
	virtual bool			DLLInit(CreateInterfaceFn fnEngineFactory, CreateInterfaceFn fnPhysicsFactory, CreateInterfaceFn fnFileSystemFactory, IGlobalVars* pGlobals) = 0; // initialize the game (one-time call when the DLL is first loaded). return false if there is an error during startup.
	virtual void			DLLShutdown() = 0; // called once during DLL shutdown
	virtual bool			ReplayInit(CreateInterfaceFn fnReplayFactory) = 0; // setup replay interfaces on the server
	virtual float			GetTickInterval() const = 0; // get the simulation interval (must be compiled with identical values into both client and game .dll for mod!). right now this is only requested at server startup time so it can't be changed on the fly, etc.
	virtual bool			GameInit() = 0; // this is called when a new game is started. (restart, map)
	virtual void			GameShutdown() = 0; // this is called when a game ends (server disconnect, death, restart, load) NOT on level transitions within a game
	virtual bool			LevelInit(char const* szMapName, char const* szMapEntities, char const* szOldLevel, char const* szLandmarkName, bool bLoadGame, bool bBackground) = 0; // called any time a new level is started (after GameInit() also on level transitions within a game)
	virtual void			ServerActivate(void* pEdictList, int nEdictCount, int nClientMax) = 0; // the server is about to activate
	virtual void			LevelShutdown() = 0; // called when a level is shutdown (including changing levels)
	virtual void			GameFrame(bool bSimulating) = 0; // the server should run physics/think on all edicts
	virtual void			PreClientUpdate(bool bSimulating) = 0; // called once per simulation frame on the final tick
	virtual CBaseServer*	GetAllClasses() = 0; // give the list of datatable classes to the engine.  The engine matches class names from here with edict_t::classname to figure out how to encode a class's data for networking
	virtual const char*		GetGameDescription() = 0; // returns string describing current .dll e.g. teamfortress 2, half-life 2. it's more descriptive than just the name of the game directory
	virtual void			CreateNetworkStringTables() = 0; // let the game .dll allocate it's own network/shared string tables
	virtual CSaveRestoreData* SaveInit(int nSize) = 0;
	virtual void			SaveWriteFields(CSaveRestoreData*, const char*, void*, DataMap_t*, TypeDescription_t*, int) = 0;
	virtual void			SaveReadFields(CSaveRestoreData*, const char*, void*, DataMap_t*, TypeDescription_t*, int) = 0;
	virtual void			SaveGlobalState(CSaveRestoreData*) = 0;
	virtual void			RestoreGlobalState(CSaveRestoreData*) = 0;
	virtual void			PreSave(CSaveRestoreData*) = 0;
	virtual void			Save(CSaveRestoreData*) = 0;
	virtual void			GetSaveComment(char* szComment, int nMaxLength, float flMinutes, float flSeconds, bool bNoTime = false) = 0;
	virtual void			WriteSaveHeaders(CSaveRestoreData*) = 0;
	virtual void			ReadRestoreHeaders(CSaveRestoreData*) = 0;
	virtual void			Restore(CSaveRestoreData*, bool) = 0;
	virtual bool			IsRestoring() = 0;
	virtual int				CreateEntityTransitionList(CSaveRestoreData*, int) = 0; // returns the number of entities moved across the transition
	virtual void			BuildAdjacentMapList() = 0; // build the list of maps adjacent to the current map
	virtual bool			GetUserMessageInfo(int iMsgType, char* szName, int nMaxNameLength, int& iSize) = 0; // retrieve info needed for parsing the specified user message
	virtual void*			GetStandardSendProxies() = 0; // hand over the StandardSendProxies in the game dll's module.
	virtual void			PostInit() = 0; // called once during startup, after the game .dll has been loaded and after the client .dll has also been loaded
	virtual void			Think(bool finalTick) = 0; // called once per frame even when no level is loaded
	virtual void			PreSaveGameLoaded(char const* szSaveName, bool bCurrentlyInGame) = 0;
	virtual bool			ShouldHideServer() = 0; // returns true if the game dll wants the server not to be made public
	virtual void			InvalidateMdlCache() = 0;
	virtual void			OnQueryCvarValueFinished(void* iCookie, void* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) = 0;
	virtual void			GameServerSteamAPIActivated() = 0;
	virtual void			GameServerSteamAPIShutdown() = 0; // called after the steam API has been shutdown post-level startup
	virtual void			SetServerHibernation(bool bHibernating) = 0;
	virtual void*			GetServerGCLobby() = 0; // interface to the new GC based lobby system
	virtual const char*		GetServerBrowserMapOverride() = 0; // return override string to show in the server browser
	virtual const char*		GetServerBrowserGameData() = 0; // get gamedata string to send to the master serer updater.
};
