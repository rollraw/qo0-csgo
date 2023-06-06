#pragma once
#include "../common.h"
// used: ccsplayer
#include "../sdk/entity.h"
#include "../sdk/interfaces/inetchannel.h"

// forward declarations
class IGameEvent;
class INetChannel;

/*
 * LAG COMPENSATION
 * - game technology to normalize server-side state of the world for each player as that player's user commands are executed
 */
namespace F::LAGCOMP
{
	/* @section: callbacks */
	void OnPreMove(CCSPlayer* pLocal, CUserCmd* pCmd, bool* pbSendPacket);
	void OnPreSendDatagram(INetChannel* pNetChannel, int* pnOldInReliableState, int* pnOldInSequenceNr);
	void OnPostSendDatagram(INetChannel* pNetChannel, const int nOldInReliableState, const int nOldInSequenceNr);
	void OnPlayerDeleted(CCSPlayer* pPlayer);

	/* @section: packets */
	// predict and update next tick of server "hold aim" cycle
	void UpdateHoldAimCycle();
	/// @returns: true if server locked view angles during 'sv_maxusrcmdprocessticks_holdaim' cycle, false otherwise
	bool IsHoldAimCycle();
	/// @returns: last view angles that should be locked on during 'sv_maxusrcmdprocessticks_holdaim' cycle
	const QAngle_t& GetHoldAimCycleViewAngles();
	// reset lock view angles tick number
	void ClearHoldAimCycle();
	/// @returns: count of future ticks that would be choked
	int GetTicksToChoke();

	/* @section: latency */
	// collect and cache incoming sequences
	void UpdateLatencySequences();
	// clear incoming sequences cache
	void ClearLatencySequences();
	// go through sequences cache and simulate that we're still on sequence that was received desired latency ago
}
