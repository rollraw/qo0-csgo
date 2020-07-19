#pragma once

class IGlobalVarsBase
{
public:
	float			flRealTime;					//0x00
	int				iFrameCount;				//0x04
	float			flAbsFrameTime;				//0x08
	float			flAbsFrameStartTime;		//0x0C
	float			flCurrentTime;				//0x10
	float			flFrameTime;				//0x14
	int				nMaxClients;				//0x18
	int				iTickCount;					//0x1C
	float			flIntervalPerTick;			//0x20
	float			flInterpolationAmount;		//0x24
	int				nFrameSimulationTicks;		//0x28
	int				iNetworkProtocol;			//0x2C
	void*			pSaveData;					//0x30
	bool			bClient;					//0x34
	bool		    bRemoteClient;				//0x35
	int				iTimestampNetworkingBase;	//0x36
	int				iTimestampRandomizeWindow;	//0x3A
}; // Size: 0x3E

class IGlobalVars : public IGlobalVarsBase
{
public:
	string_t		szMapName;					//0x3E
	string_t		szMapGroupName;				//0x42
	int				iMapVersion;				//0x46
	string_t		szStartSpot;				//0x4A
	int				nLoadType;					//0x4E
	bool			bMapLoadFailed;				//0x52
	bool			bDeathmatch;				//0x53
	bool			bCooperative;				//0x54
	bool			bTeamplay;					//0x55
	int				nMaxEntities;				//0x56
	int				nServerCount;				//0x5A
	void*			pEdicts;					//0x5E
}; // Size: 0x62
