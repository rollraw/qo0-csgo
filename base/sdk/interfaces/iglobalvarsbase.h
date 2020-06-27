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
	char*			szMapName;					//0x3E
	int				iMapVersion;				//0x42
	char*			szStartSpot;				//0x46
	int				nLoadType;					//0x4A
	bool			bMapLoadFailed;				//0x4E
	bool			bDeathmatch;				//0x4F
	bool			bCooperative;				//0x50
	bool			bTeamplay;					//0x51
	int				nMaxEntities;				//0x52
	int				nServerCount;				//0x56
}; //Size: 0x5A
