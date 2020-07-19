#pragma once
// used: winapi includes
#include "../common.h"

// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: prediction, movehelper, movedata
#include "../sdk/interfaces/iprediction.h"
// used: baseentity
#include "../sdk/entity.h"

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/server/player_command.cpp#L315
class CPrediction : public CSingleton<CPrediction>
{
public:
	CPrediction()
	{
		iPredictionRandomSeed = *reinterpret_cast<int**>(MEM::FindPattern(CLIENT_DLL, XorStr("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")) + 0x2); // @xref: "SelectWeightedSequence"
		pSetPredictionEntity = *reinterpret_cast<CBaseEntity**>(MEM::FindPattern(CLIENT_DLL, XorStr("89 35 ? ? ? ? F3 0F 10 48 20")) + 0x2);
	}

	// Get
	void Start(CUserCmd* pCmd, CBaseEntity* pLocal);
	void End(CUserCmd* pCmd, CBaseEntity* pLocal);

	/*
	 * required cuz prediction works on frames, not ticks
	 * corrects tickbase if your framerate goes below tickrate and m_nTickBase won't update every tick
	 */
	int GetTickbase(CUserCmd* pCmd, CBaseEntity* pLocal);
private:
	// Values
	/* prediction seed */
	int* iPredictionRandomSeed = nullptr;
	/* current predictable entity */
	CBaseEntity* pSetPredictionEntity = nullptr;
	/* encapsulated input parameters to player movement */
	CMoveData moveData = { };

	/* backup */
	float flOldCurrentTime = 0.f;
	float flOldFrameTime = 0.f;
	int iOldTickCount = 0;
};
