#pragma once
// used: iglobalvarsbase
#include "iglobalvars.h"
// used: cmovedata
#include "igamemovement.h"

// forward declarations
class CBasePlayer;
struct TypeDescription_t;

#pragma pack(push, 4)
// functions used to verify offsets:
// @ida CPrediction::Init: client.dll -> "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? 8B 40 54 FF D0 68" @xref: "CPDumpPanel"
// @ida CPrediction::Update: client.dll -> "55 8B EC 83 EC 08 53 56 8B F1 8B 0D ? ? ? ? 57 8B" @xref: "CPrediction::Update"
// @ida CPrediction::CheckError: client.dll -> "55 8B EC 83 E4 F0 83 EC 48 56 57 8B F9 8B 0D" @xref: "m_vecNetworkOrigin", "%d len(%6.3f) (%6.3f %6.3f %6.3f)"
class IPrediction : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	virtual ~IPrediction() { }

	void Update(int iStartFrame, bool bValidFrame, int nIncomingAcknowledged, int nOutgoingCommand)
	{
		CallVFunc<void, 3U>(this, iStartFrame, bValidFrame, nIncomingAcknowledged, nOutgoingCommand);
	}

	// get the view angles for the local player
	void GetLocalViewAngles(QAngle_t& angView)
	{
		CallVFunc<void, 12U>(this, &angView);
	}

	// set the view angles for the local player
	void SetLocalViewAngles(QAngle_t& angView)
	{
		CallVFunc<void, 13U>(this, &angView);
	}

	// check if the player is standing on a moving entity and adjust velocity and basevelocity appropriately
	void CheckMovingGround(CBasePlayer* pPlayer, double dbFrametime)
	{
		CallVFunc<void, 18U>(this, pPlayer, dbFrametime);
	}

	// run a movement command from the player
	void RunCommand(CBasePlayer* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
	{
		CallVFunc<void, 19U>(this, pPlayer, pCmd, pMoveHelper);
	}

	// prepare for running prediction, update move data for the given player with his and command data
	void SetupMove(CBasePlayer* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper, CMoveData* pMoveData)
	{
		CallVFunc<void, 20U>(this, pPlayer, pCmd, pMoveHelper, pMoveData);
	}

	// finish running prediction, update player data with command and move data
	void FinishMove(CBasePlayer* pPlayer, CUserCmd* pCmd, CMoveData* pMoveData)
	{
		CallVFunc<void, 21U>(this, pPlayer, pCmd, pMoveData);
	}

public:
	CBaseHandle hLastGround; // 0x04
	bool bInPrediction; // 0x08
	bool bOldCLPredictValue; // 0x09 // @ida: client.dll -> U8["88 46 ? E8 9C" + 0x2]
	bool bEnginePaused; // 0x0A // @ida: client.dll -> U8["B7 01 88 46 ? 88 7D FC" + 0x4]
	int iPreviousStartFrame; // 0x0C
	int nIncomingPacketNumber; // 0x10
	float flLastServerWorldTimeStamp; // 0x14
	bool bIsFirstTimePredicted; // 0x18
	int nCommandsPredicted; // 0x1C
	int nServerCommandsAcknowledged; // 0x20
	int bPreviousAckHadErrors; // 0x24
	float flIdealPitch; // 0x28
	int iLastCommandAcknowledged; // 0x2C
	bool bPreviousAckErrorTriggersFullLatchReset; // 0x30
	CUtlVector<CBaseHandle> vecEntitiesWithPredictionErrorsInLastAck; // 0x34
	bool bPerformedTickShift; // 0x48
	IGlobalVarsBase savedGlobalVarsBase; // 0x4C
	bool bPlayerOriginTypeDescSearched; // 0x8C // @ida: client.dll -> ["80 BF ? ? ? ? ? 75 4D" + 0x2]
	CUtlVector<const TypeDescription_t*> vecPlayerOriginTypeDescription; // 0x90 // a vector in cases where the .x, .y, and .z are separately listed
	void* pPDumpPanel; // 0xA4
};
static_assert(sizeof(IPrediction) == 0xA8); // size verify @ida: client.dll -> ["68 ? ? ? ? 56 E8 ? ? ? ? 83 C4 08 8B C6 5E 5D C2 04 00 A1" + 0x1]
#pragma pack(pop)
