#pragma once

#pragma pack(push, 4)
// @source: master/public/globalvars_base.h
class IGlobalVarsBase
{
public:
	float flRealTime; // 0x00
	int nFrameCount; // 0x04
	float flAbsFrameTime; // 0x08
	float flAbsFrameStartTime; // 0x0C
	float flCurrentTime; // 0x10
	float flFrameTime; // 0x14
	int nMaxClients; // 0x18
	int nTickCount; // 0x1C
	float flIntervalPerTick; // 0x20
	float flInterpolationAmount; // 0x24
	int nFrameSimulationTicks; // 0x28
	int iNetworkProtocol; // 0x2C
	void* pSaveData; // 0x30
	bool bClient;// 0x34
	bool bRemoteClient; // 0x35
	int iTimestampNetworkingBase; // 0x38
	int iTimestampRandomizeWindow; // 0x3C
};
static_assert(sizeof(IGlobalVarsBase) == 0x40);

// @source: master/public/edict.h
class IGlobalVars : public IGlobalVarsBase
{
public:
	const char* szMapName; // 0x40
	const char* szMapGroupName; // 0x44
	int iMapVersion; // 0x48
	const char* szStartSpot; // 0x4C
	int nLoadType; // 0x50
	bool bMapLoadFailed; // 0x54
	bool bDeathmatch; // 0x55
	bool bCooperative; // 0x56
	bool bTeamplay; // 0x57
	int nMaxEntities; // 0x58
	int nServerCount; // 0x5C
	void* pEdicts; // 0x60
};
static_assert(sizeof(IGlobalVars) == 0x64);
#pragma pack(pop)
