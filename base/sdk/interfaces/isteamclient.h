#pragma once
// @credits: https://partner.steamgames.com/doc/sdk

using HSteamPipe = std::int32_t;
using HSteamUser = std::int32_t;
using HSteamAPICall = std::int64_t;

enum EAccountType
{
	EAccountTypeInvalid = 0,
	EAccountTypeIndividual = 1,		// single user account
	EAccountTypeMultiseat = 2,		// multiseat (e.g. cybercafe) account
	EAccountTypeGameServer = 3,		// game server account
	EAccountTypeAnonGameServer = 4,	// anonymous game server account
	EAccountTypePending = 5,		// pending
	EAccountTypeContentServer = 6,	// content server
	EAccountTypeClan = 7,
	EAccountTypeChat = 8,
	EAccountTypeConsoleUser = 9,	// Fake SteamID for local PSN account on PS3 or Live account on 360, etc.
	EAccountTypeAnonUser = 10,
	EAccountTypeMax
};

class ISteamClient;
class ISteamUser;
class ISteamGameServer;
class ISteamFriends;
class ISteamUtils;
class ISteamMatchmaking;
class ISteamGameSearch;
class ISteamContentServer;
class ISteamMatchmakingServers;
class ISteamUserStats;
class ISteamApps;
class ISteamNetworking;
class ISteamRemoteStorage;
class ISteamScreenshots;
class ISteamMusic;
class ISteamMusicRemote;
class ISteamGameServerStats;
class ISteamHTTP;
class ISteamUnifiedMessages;
class ISteamController;
class ISteamUGC;
class ISteamAppList;
class ISteamHTMLSurface;
class ISteamInventory;
class ISteamVideo;
class ISteamParentalSettings;
class ISteamInput;

struct SteamAPIContext_t
{
	ISteamClient*				pSteamClient;
	ISteamUser*					pSteamUser;
	ISteamFriends*				pSteamFriends;
	ISteamUtils*				pSteamUtils;
	ISteamMatchmaking*			pSteamMatchmaking;
	ISteamGameSearch*			pSteamGameSearch;
	ISteamMatchmakingServers*	pSteamMatchmakingServers;
	ISteamUserStats*			pSteamUserStats;
	ISteamApps*					pSteamApps;
	ISteamNetworking*			pSteamNetworking;
	ISteamRemoteStorage*		pSteamRemoteStorage;
	ISteamScreenshots*			pSteamScreenshots;
	ISteamHTTP*					pSteamHTTP;
	ISteamController*			pController;
	ISteamUGC*					pSteamUGC;
	ISteamAppList*				pSteamAppList;
	ISteamMusic*				pSteamMusic;
	ISteamMusicRemote*			pSteamMusicRemote;
	ISteamHTMLSurface*			pSteamHTMLSurface;
	ISteamInventory*			pSteamInventory;
	ISteamVideo*				pSteamVideo;
	ISteamParentalSettings*		pSteamParentalSettings;
	ISteamInput*				pSteamInput;
};

using SteamAPIWarningMessageHook_t = void(__cdecl*)(int, const char*);
using SteamAPI_PostAPIResultInProcess_t = void(__cdecl*)(HSteamAPICall hCall, void*, std::uint32_t unCallbackSize, int nCallbacks);
using SteamAPI_CheckCallbackRegistered_t = std::uint32_t(__cdecl*)(int nCallbacks);

class ISteamClient
{
public:
	virtual HSteamPipe CreateSteamPipe() = 0;
	virtual bool ReleaseSteamPipe(HSteamPipe hSteamPipe) = 0;
	virtual HSteamUser ConnectToGlobalUser(HSteamPipe hSteamPipe) = 0;
	virtual HSteamUser CreateLocalUser(HSteamPipe* phSteamPipe, EAccountType eAccountType) = 0;
	virtual void ReleaseUser(HSteamPipe hSteamPipe, HSteamUser hUser) = 0;
	virtual ISteamUser* GetISteamUser(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamGameServer* GetISteamGameServer(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual void SetLocalIPBinding(uint32_t unIP, uint16_t usPort) = 0;
	virtual ISteamFriends* GetISteamFriends(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamUtils* GetISteamUtils(HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamMatchmaking* GetISteamMatchmaking(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamMatchmakingServers* GetISteamMatchmakingServers(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual void* GetISteamGenericInterface(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamUserStats* GetISteamUserStats(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamGameServerStats* GetISteamGameServerStats(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamApps* GetISteamApps(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamNetworking* GetISteamNetworking(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamRemoteStorage* GetISteamRemoteStorage(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamScreenshots* GetISteamScreenshots(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual void RunFrame() = 0;
	virtual uint32_t GetIPCCallCount() = 0;
	virtual void SetWarningMessageHook(SteamAPIWarningMessageHook_t pFunction) = 0;
	virtual bool ShutdownIfAllPipesClosed() = 0;
	virtual ISteamHTTP* GetISteamHTTP(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamUnifiedMessages* GetISteamUnifiedMessages(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamController* GetISteamController(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamUGC* GetISteamUGC(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamAppList* GetISteamAppList(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamMusic* GetISteamMusic(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamMusicRemote* GetISteamMusicRemote(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamHTMLSurface* GetISteamHTMLSurface(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual void Set_SteamAPI_CPostAPIResultInProcess(SteamAPI_PostAPIResultInProcess_t func) = 0;
	virtual void Remove_SteamAPI_CPostAPIResultInProcess(SteamAPI_PostAPIResultInProcess_t func) = 0;
	virtual void Set_SteamAPI_CCheckCallbackRegisteredInProcess(SteamAPI_CheckCallbackRegistered_t func) = 0;
	virtual ISteamInventory* GetISteamInventory(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
	virtual ISteamVideo* GetISteamVideo(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;
};
