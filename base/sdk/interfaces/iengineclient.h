#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/cdll_int.h

// used: viewmatrix_t, matrix3x4_t
#include "../datatypes/matrix.h"
// used: angle
#include "../datatypes/qangle.h"

#pragma region engineclient_enumerations
enum EClientFrameStage : int
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	// a network packet is being recieved
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

struct PlayerInfo_t
{
	std::uint64_t	ullVersion = 0ULL;
	int				nXuidLow;
	int				nXuidHigh;
	char			szName[128];
	int				nUserID;
	char			szSteamID[33];
	std::uint32_t	nFriendsID;
	char			szFriendsName[128];
	bool			bFakePlayer;
	bool			bIsHLTV;
	CRC32_t			uCustomFiles[4];
	std::uint8_t	dFilesDownloaded;
};

struct AudioState_t
{
	Vector			vecOrigin;
	QAngle			angAngles;
	bool			bIsUnderwater;
};

typedef struct InputContextHandle__* InputContextHandle_t;

struct Model_t;
struct SteamAPIContext_t;
struct ClientTextMessage_t;
struct SurfInfo_t;
struct Frustum_t;
class CSentence;
class CAudioSource;
class CPhysCollide;
class CGamestatsData;
class ICollideable;
class ISpatialQuery;
class ISPSharedMemory;
class IAchievementMgr;
class IMaterial;
class IMaterialSystem;

class IEngineClient
{
public:
	void GetScreenSize(int& iWidth, int& iHeight)
	{
		MEM::CallVFunc<void, int&, int&>(this, 5, iWidth, iHeight);
	}

	bool GetPlayerInfo(int nEntityIndex, PlayerInfo_t* pInfo)
	{
		return MEM::CallVFunc<bool>(this, 8, nEntityIndex, pInfo);
	}

	int GetPlayerForUserID(int nUserID)
	{
		return MEM::CallVFunc<int>(this, 9, nUserID);
	}

	bool IsConsoleVisible()
	{
		return MEM::CallVFunc<bool>(this, 11);
	}

	int GetLocalPlayer()
	{
		return MEM::CallVFunc<int>(this, 12);
	}

	float GetLastTimeStamp()
	{
		return MEM::CallVFunc<float>(this, 14);
	}

	void GetViewAngles(QAngle& angView)
	{
		MEM::CallVFunc<void, QAngle&>(this, 18, angView);
	}

	void SetViewAngles(QAngle& angView)
	{
		MEM::CallVFunc<void, QAngle&>(this, 19, angView);
	}

	int GetMaxClients()
	{
		return MEM::CallVFunc<int>(this, 20);
	}

	// returns true if the player is fully connected and active in game (i.e, not still loading) and for check doesnt need isconnected
	bool IsInGame()
	{
		return MEM::CallVFunc<bool>(this, 26);
	}

	// returns true if the player is connected, but not necessarily active in game (could still be loading)
	bool IsConnected()
	{
		return MEM::CallVFunc<bool>(this, 27);
	}

	bool IsDrawingLoadingImage()
	{
		return MEM::CallVFunc<bool>(this, 28);
	}

	const ViewMatrix_t& WorldToScreenMatrix()
	{
		return MEM::CallVFunc<const ViewMatrix_t&>(this, 37);
	}

	void* GetBSPTreeQuery()
	{
		return MEM::CallVFunc<void*>(this, 43);
	}

	const char* GetLevelName()
	{
		return MEM::CallVFunc<const char*>(this, 52);
	}

	const char* GetLevelNameShort()
	{
		return MEM::CallVFunc<const char*>(this, 53);
	}

	INetChannelInfo* GetNetChannelInfo()
	{
		return MEM::CallVFunc<INetChannelInfo*>(this, 78);
	}

	bool IsPlayingDemo()
	{
		return MEM::CallVFunc<bool>(this, 82);
	}

	bool IsRecordingDemo()
	{
		return MEM::CallVFunc<bool>(this, 83);
	}

	bool IsPlayingTimeDemo()
	{
		return MEM::CallVFunc<bool>(this, 84);
	}

	bool IsTakingScreenshot()
	{
		return MEM::CallVFunc<bool>(this, 92);
	}

	bool IsHLTV()
	{
		return MEM::CallVFunc<bool>(this, 93);
	}

	unsigned int GetEngineBuildNumber()
	{
		return MEM::CallVFunc<unsigned int>(this, 104);
	}

	const char* GetProductVersionString()
	{
		return MEM::CallVFunc<const char*>(this, 105);
	}

	void ExecuteClientCmd(const char* szCmdString)
	{
		MEM::CallVFunc<void>(this, 108, szCmdString);
	}

	void ClientCmd_Unrestricted(const char* szCmdString, const char* szFlag = nullptr)
	{
		MEM::CallVFunc<void>(this, 114, szCmdString, szFlag);
	}

	SteamAPIContext_t* GetSteamAPIContext()
	{
		return MEM::CallVFunc<SteamAPIContext_t*>(this, 185);
	}

	bool IsVoiceRecording()
	{
		return MEM::CallVFunc<bool>(this, 224);
	}
};
