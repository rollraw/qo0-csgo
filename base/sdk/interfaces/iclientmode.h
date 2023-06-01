#pragma once
#include "../datatypes/vector.h"
#include "../datatypes/qangle.h"

// used: hcursor
#include "isurface.h"

// forward declarations
class CBaseHudChat;
class CBaseHudWeaponSelection;

#pragma pack(push, 4)
// @source: master/public/view_shared.h
class CViewSetup
{
public:
	int iX; // 0x0000
	int iUnscaledX; // 0x0004
	int iY; // 0x0008
	int iUnscaledY; // 0x000C
	int iWidth; // 0x0010
	int iUnscaledWidth; // 0x0014
	int iHeight; // 0x0018
	int iUnscaledHeight; // 0x001C
	bool bOrtho; // 0x0020
	float flOrthoLeft; // 0x0024
	float flOrthoTop; // 0x0028
	float flOrthoRight; // 0x002C
	float flOrthoBottom; // 0x0030
	std::byte pad0[0x7C]; // 0x0034
	float flFOV; // 0x00B0
	float flViewModelFOV; // 0x00B4
	Vector_t vecOrigin; // 0x00B8
	QAngle_t angView; // 0x00C4
	float flNearZ; // 0x00D0
	float flFarZ; // 0x00D4
	float flNearViewmodelZ; // 0x00D8
	float flFarViewmodelZ; // 0x00DC
	float flAspectRatio; // 0x00E0
	float flNearBlurDepth; // 0x00E4
	float flNearFocusDepth; // 0x00E8
	float flFarFocusDepth; // 0x00EC
	float flFarBlurDepth; // 0x00F0
	float flNearBlurRadius; // 0x00F4
	float flFarBlurRadius; // 0x00F8
	float flDoFQuality; // 0x00FC
	int nMotionBlurMode; // 0x0100
	float flShutterTime; // 0x0104
	Vector_t vecShutterOpenPosition; // 0x0108
	QAngle_t vecShutterOpenAngles; // 0x0114
	Vector_t vecShutterClosePosition; // 0x0120
	QAngle_t vecShutterCloseAngles; // 0x012C
	float flOffCenterTop; // 0x0138
	float flOffCenterBottom; // 0x013C
	float flOffCenterLeft; // 0x0140
	float flOffCenterRight; // 0x0144
	bool bOffCenter : 1; // 0x0148
	bool bRenderToSubrectOfLargerScreen : 1; // 0x0148
	bool bDoBloomAndToneMapping : 1; // 0x0148
	bool bDoDepthOfField : 1; // 0x0148
	bool bHDRTarget : 1; // 0x0148
	bool bDrawWorldNormal : 1; // 0x0148
	bool bCullFontFaces : 1; // 0x0148
	bool bCacheFullSceneState : 1; // 0x0148
	bool bCSMView : 1; // 0x0149
};
static_assert(sizeof(CViewSetup) == 0x014C);
#pragma pack(pop)

// @source: master/game/client/iclientmode.h
class IClientMode
{
public:
	virtual ~IClientMode() { }
};
static_assert(sizeof(IClientMode) == 0x4);

// @source: master/game/client/clientmode_shared.h
//
// functions used to verify offsets:
// @ida ClientModeShared::Init(): client.dll -> "" @xref: "player_connect_full", "player_connect", "player_disconnect", "player_team", "server_cvar", "player_changename", "teamplay_broadcast_audio", "achievement_earned", "item_found", "items_gifted"
// @ida ClientModeShared::LevelInit(): client.dll -> "55 8B EC 83 E4 F8 83 EC 20 56 57 8B F9 8B 4F" @xref: "LevelInit", "game_newmap", "mapname"
class IClientModeShared : public IClientMode
{
public:
	virtual ~IClientModeShared() { }

public:
	std::byte pad0[0x14]; // 0x04
	void* pViewport; // 0x18 // @ida: client.dll -> U8["8B F9 8B 4F ? 8B 01 FF 90 ? ? ? ? 8B F0" + 0x5]
	CBaseHudChat* pChatElement; // 0x1C
	HCursor hCursorNone; // 0x20 // @ida: client.dll -> U8["C7 47 ? ? ? ? ? 8B C4 8D" + 0x2]
	CBaseHudWeaponSelection* pWeaponSelection; // 0x24
	int nRootSize[2]; // 0x28
};
static_assert(sizeof(IClientModeShared) == 0x30);

class IAppSystem
{
private:
	virtual void function0() = 0;
	virtual void function1() = 0;
	virtual void function2() = 0;
	virtual void function3() = 0;
	virtual void function4() = 0;
	virtual void function5() = 0;
	virtual void function6() = 0;
	virtual void function7() = 0;
	virtual void function8() = 0;
};
static_assert(sizeof(IAppSystem) == 0x4);
