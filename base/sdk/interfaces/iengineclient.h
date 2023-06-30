#pragma once
#include "../datatypes/matrix.h"
#include "../datatypes/qangle.h"
#include "../hash/crc32.h"

// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/public/cdll_int.h

#pragma region engineclient_enumerations
enum EClientFrameStage : int
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	// a network packet is being received
	FRAME_NET_UPDATE_START,
	// data has been received and we are going to start calling postdataupdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// data has been received and called postdataupdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// received all packets, we can now do interpolation, prediction, etc
	FRAME_NET_UPDATE_END,
	// start rendering the scene
	FRAME_RENDER_START,
	// finished rendering the scene
	FRAME_RENDER_END,
	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE
};

enum ERenderViewInfo : int
{
	RENDERVIEW_UNSPECIFIED =				0,
	RENDERVIEW_DRAWVIEWMODEL =				(1 << 0),
	RENDERVIEW_DRAWHUD =					(1 << 1),
	RENDERVIEW_SUPPRESSMONITORRENDERING =	(1 << 2)
};
#pragma endregion

#pragma pack(push, 8) // @todo: why it fits perfectly with 8 bytes pack instead of 4?
// functions to verify offsets:
// @ida CBaseClient::FillUserInfo(): engine.dll -> "55 8B EC 83 EC 0C 56 57 8B 7D 08 8B F1 68"
struct PlayerInfo_t
{
	std::uint64_t ullVersion; // 0x0000 // version for future compatibility
	std::uint64_t ullXuid; // 0x0008 // network xuid
	char szName[128]; // 0x0010 // scoreboard information
	int nUserID; // 0x0090 // local server user ID, unique while server is running
	char szSteamID[33]; // 0x0094 // global unique player identifier
	std::uint32_t nFriendsID; // 0x00B8 // friends identification number
	char szFriendsName[128]; // 0x00BC
	bool bFakePlayer; // 0x013C // true if player is a bot
	bool bIsHLTV; // 0x013D // true if player is the HLTV proxy
	CRC32_t uCustomFiles[4]; // 0x0140 // custom files CRC for this player
	std::uint8_t uFilesDownloaded; // 0x0150 // this counter increases each time the server downloaded a new file
};
static_assert(sizeof(PlayerInfo_t) == 0x158); // size verify @ida: engine.dll -> ["68 ? ? ? ? 85 C0 74 26" + 0x1] @xref: "userinfo"
#pragma pack(pop)

// forward declarations
class INetChannelInfo;
struct SteamAPIContext_t;

class IEngineClient : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void GetScreenSize(int& iWidth, int& iHeight)
	{
		CallVFunc<void, 5U>(this, &iWidth, &iHeight);
	}

	[[nodiscard]] bool GetPlayerInfo(int nEntityIndex, PlayerInfo_t* pInfo)
	{
		return CallVFunc<bool, 8U>(this, nEntityIndex, pInfo);
	}

	[[nodiscard]] int GetPlayerForUserID(int nUserID)
	{
		return CallVFunc<int, 9U>(this, nUserID);
	}

	[[nodiscard]] bool IsConsoleVisible()
	{
		return CallVFunc<bool, 11U>(this);
	}

	[[nodiscard]] int GetLocalPlayer()
	{
		return CallVFunc<int, 12U>(this);
	}

	[[nodiscard]] float GetLastTimeStamp()
	{
		return CallVFunc<float, 14U>(this);
	}

	void GetViewAngles(QAngle_t& angView)
	{
		CallVFunc<void, 18U>(this, &angView);
	}

	void SetViewAngles(QAngle_t& angView)
	{
		CallVFunc<void, 19U>(this, &angView);
	}

	[[nodiscard]] int GetMaxClients()
	{
		return CallVFunc<int, 20U>(this);
	}

	/// @returns: true if the player is fully connected and active in game (i.e, not still loading)
	[[nodiscard]] bool IsInGame()
	{
		return CallVFunc<bool, 26U>(this);
	}

	/// @returns: true if the player is connected, but not necessarily active in game (could still be loading)
	[[nodiscard]] bool IsConnected()
	{
		return CallVFunc<bool, 27U>(this);
	}

	[[nodiscard]] bool IsDrawingLoadingImage()
	{
		return CallVFunc<bool, 28U>(this);
	}

	[[nodiscard]] const ViewMatrix_t& WorldToScreenMatrix()
	{
		return CallVFunc<const ViewMatrix_t&, 37U>(this);
	}

	[[nodiscard]] void* GetBSPTreeQuery()
	{
		return CallVFunc<void*, 43U>(this);
	}

	[[nodiscard]] const char* GetLevelName()
	{
		return CallVFunc<const char*, 52U>(this);
	}

	[[nodiscard]] const char* GetLevelNameShort()
	{
		return CallVFunc<const char*, 53U>(this);
	}

	void FireEvents()
	{
		CallVFunc<void, 59U>(this);
	}

	[[nodiscard]] INetChannelInfo* GetNetChannelInfo()
	{
		return CallVFunc<INetChannelInfo*, 78U>(this);
	}

	[[nodiscard]] bool IsPlayingDemo()
	{
		return CallVFunc<bool, 82U>(this);
	}

	[[nodiscard]] bool IsRecordingDemo()
	{
		return CallVFunc<bool, 83U>(this);
	}

	[[nodiscard]] bool IsPlayingTimeDemo()
	{
		return CallVFunc<bool, 84U>(this);
	}

	[[nodiscard]] bool IsPaused()
	{
		return CallVFunc<bool, 90U>(this);
	}

	[[nodiscard]] bool IsTakingScreenshot()
	{
		return CallVFunc<bool, 92U>(this);
	}

	[[nodiscard]] bool IsHLTV()
	{
		return CallVFunc<bool, 93U>(this);
	}

	[[nodiscard]] const char* GetMapEntitiesString()
	{
		return CallVFunc<const char*, 99U>(this);
	}

	[[nodiscard]] unsigned int GetEngineBuildNumber()
	{
		return CallVFunc<unsigned int, 104U>(this);
	}

	[[nodiscard]] const char* GetProductVersionString()
	{
		return CallVFunc<const char*, 105U>(this);
	}

	void ExecuteClientCmd(const char* szCmdString)
	{
		CallVFunc<void, 108U>(this, szCmdString);
	}

	void ClientCmdUnrestricted(const char* szCmdString, bool bFromConsoleOrKeybind = false)
	{
		CallVFunc<void, 114U>(this, szCmdString, bFromConsoleOrKeybind);
	}

	[[nodiscard]] SteamAPIContext_t* GetSteamAPIContext()
	{
		return CallVFunc<SteamAPIContext_t*, 185U>(this);
	}

	[[nodiscard]] bool IsVoiceRecording() const
	{
		return CallVFunc<bool, 224U>(this);
	}
};
