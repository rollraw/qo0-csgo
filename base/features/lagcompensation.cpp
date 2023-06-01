#include "lagcompensation.h"

// used: [stl] deque
#include <deque>

// used: gamerulesproxy entity
#include "../core/entitylistener.h"
// used: cheats variables
#include "../core/variables.h"
// used: sv_maxunlag
#include "../core/convar.h"
// used: getoriginalangles
#include "../features.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/iclientstate.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/inetchannel.h"

using namespace F;

struct SequenceObject_t
{
	int nInReliableState = 0;
	int nInSequenceNr = 0;
	float flRealTime = 0.f;
};

// cache of possible sequences that we can spike
static std::deque<SequenceObject_t> vecSequenceCache = { };
// count of incoming sequences that we can spike
static int nLastIncomingSequence = 0;
// predicted tick count when server will lock our view angles
static int nLockViewAnglesTick = 0;
// locked view angles during hold aim cycle
static QAngle_t angLockViewPoint = { };

#pragma region lagcompensation_callbacks
void LAGCOMP::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, bool* pbSendPacket)
{
	// check are we alive and not during freeze period (it have less choke limit than during the game)
	if (!pLocal->IsAlive())
		return;

	// prevent client from sending packets to the server until we reach limit
	*pbSendPacket = (I::ClientState->nChokedCommands >= GetTicksToChoke());

	/* backtrack and other lag compensation abuse stuff */
}

void LAGCOMP::OnPreSendDatagram(INetChannel* pNetChannel, int* pnOldInReliableState, int* pnOldInSequenceNr)
{
	if (!C::Get<bool>(Vars.bMiscFakeLatency))
		return;

	// backup values before adding fake latency, otherwise it will cause connection loss error
	*pnOldInReliableState = pNetChannel->nInReliableState;
	*pnOldInSequenceNr = pNetChannel->nInSequenceNr;

	// calculate max available fake latency by our real ping to keep it work w/o real lags and delays
	const float flDesiredLatency = CRT::Max(0.f, CRT::Min(static_cast<float>(C::Get<int>(Vars.iMiscFakeLatencyAmount)) * 0.001f, CONVAR::sv_maxunlag->GetFloat()) - pNetChannel->GetLatency(FLOW_OUTGOING));

	for (const auto& sequence : vecSequenceCache)
	{
		// select the sequence that was received desired latency ago
		if (I::Globals->flRealTime - sequence.flRealTime >= flDesiredLatency)
		{
			pNetChannel->nInReliableState = sequence.nInReliableState;
			pNetChannel->nInSequenceNr = sequence.nInSequenceNr;
			break;
		}
	}
}

void LAGCOMP::OnPostSendDatagram(INetChannel* pNetChannel, const int nOldInReliableState, const int nOldInSequenceNr)
{
	if (!C::Get<bool>(Vars.bMiscFakeLatency))
		return;

	pNetChannel->nInReliableState = nOldInReliableState;
	pNetChannel->nInSequenceNr = nOldInSequenceNr;
}

void LAGCOMP::OnPlayerDeleted(CCSPlayer* pPlayer)
{
	if (pPlayer->GetIndex() == I::Engine->GetLocalPlayer())
	{
		ClearHoldAimCycle();
		ClearLatencySequences();
	}
}
#pragma endregion

#pragma region lagcompensation_packets
void LAGCOMP::UpdateHoldAimCycle()
{
	// @note: offsets from server.dll for debugging: int m_iLockViewanglesTickNumber = 0x1038, QAngle_t m_qangLockViewangles = 0x103C

	// store locked view angles before tick number, so it won't fail
	angLockViewPoint = GetServerAngles();

	// this will be applied when server will receive data from client, so we should take into account choked ticks
	nLockViewAnglesTick = I::Globals->nTickCount + GetTicksToChoke();
}

void LAGCOMP::ClearHoldAimCycle()
{
	nLockViewAnglesTick = 0;
	angLockViewPoint = { };
}

bool LAGCOMP::IsHoldAimCycle()
{
	// since 'sv_maxusrcmdprocessticks_holdaim' convar isn't replicated to client, determine it manually 
	const int nMaxUserCommandProcessTicksHoldAim = 1; //((ENTITY::pCSGameRulesProxy != nullptr && ENTITY::pCSGameRulesProxy->IsValveDS()) ? 5 : 1); @todo: not quite correct yet
	return (I::Globals->nTickCount - nLockViewAnglesTick < nMaxUserCommandProcessTicksHoldAim);
}

const QAngle_t& LAGCOMP::GetHoldAimCycleViewAngles()
{
	return angLockViewPoint;
}

int LAGCOMP::GetTicksToChoke()
{
	// we don't want to choke during freeze period
	if (ENTITY::pCSGameRulesProxy != nullptr && ENTITY::pCSGameRulesProxy->IsFreezePeriod())
		return 0;

	// since 'sv_maxusrcmdprocessticks' convar isn't replicated to client, determine it manually
	// it's showing how many ticks game lag compensation can handle and generally it is 8 for valve hosted servers and 16 for others, we subtract 2 due to 'CL_SendMove()' client-side limit
	const int nMaxUserCommandProcessTicks = ((ENTITY::pCSGameRulesProxy == nullptr || ENTITY::pCSGameRulesProxy->IsValveDS()) ? 6 : 14);

	if (C::Get<bool>(Vars.bMiscFakeLag))
		return CRT::Clamp(C::Get<int>(Vars.iMiscFakeLagTicks), 1, nMaxUserCommandProcessTicks); // @todo: instead of clamp here, we shouldn't even let user to set this values in GUI

	if (C::Get<bool>(Vars.bAntiAim) && C::Get<bool>(Vars.bAntiAimYawFake))
		return 2;

	return 0;
}
#pragma endregion

#pragma region lagcompensation_latency
void LAGCOMP::UpdateLatencySequences()
{
	const INetChannel* pNetChannel = I::ClientState->pNetChannel;

	if (pNetChannel == nullptr)
		return;

	// during the first update, set the last sequence to the real sequence, otherwise it will take time to get it work again
	if (nLastIncomingSequence == 0)
	{
		nLastIncomingSequence = pNetChannel->nInSequenceNr;
		return;
	}

	// check for previously received sequences
	if (pNetChannel->nInSequenceNr > nLastIncomingSequence)
	{
		// cache possible to spike sequences
		nLastIncomingSequence = pNetChannel->nInSequenceNr;
		vecSequenceCache.emplace_front(SequenceObject_t{ pNetChannel->nInReliableState, pNetChannel->nInSequenceNr, I::Globals->flRealTime });
	}

	// check is cached too much sequences
	if (vecSequenceCache.size() > 1024U)
		vecSequenceCache.pop_back();
}

void LAGCOMP::ClearLatencySequences()
{
	nLastIncomingSequence = 0;

	if (!vecSequenceCache.empty())
		vecSequenceCache.clear();
}
#pragma endregion
