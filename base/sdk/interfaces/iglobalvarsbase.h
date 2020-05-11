#pragma once

class IGlobalVarsBase
{
public:
	float			flRealTime;					//0x0000
	std::uint32_t	iFrameCount;				//0x0004
	float			flAbsoluteFrameTime;		//0x0008
	float			flFrameStartTime;			//0x000C
	float			flCurrentTime;				//0x0010
	float			flFrameTime;				//0x0014
	std::uint32_t	nMaxClients;				//0x0018
	std::uint32_t	iTickCount;					//0x001C
	float			flIntervalPerTick;			//0x0020
	float			flInterpolationAmount;		//0x0024
	std::uint32_t	nSimTicksThisFrame;			//0x0028
	std::uint32_t	iNetworkProtocol;			//0x002C
	void*			pSaveData;					//0x0030
	std::byte		pad0[0x4];					//0x0034
	std::uint32_t	iTimestampNetworkingBase;	//0x0038 
	std::uint32_t	iTimestampRandomizeWindow;	//0x003C
	char*			szMapName;					//0x0040
	std::uint32_t	iMapVersion;				//0x0044
	char*			szStartSpot;				//0x0048
	std::uint32_t	uLoadType;					//0x004C
	bool			bMapLoadFailed;				//0x0050
	bool			bDeathmatch;				//0x0051
	bool			bCooperative;				//0x0052
	bool			bTeamplay;					//0x0053
	std::uint32_t	nMaxEntities;				//0x0054
	std::uint32_t	iServerCount;				//0x0058
}; //Size: 0x005C
