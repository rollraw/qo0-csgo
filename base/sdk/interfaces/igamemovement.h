#pragma once
// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/game/shared/igamemovement.h

#pragma pack(push, 4)
class CMoveData
{
public:
	bool bFirstRunOfFunctions : 1; // 0x00
	bool bGameCodeMovedPlayer : 1; // 0x00
	bool bNoAirControl : 1; // 0x00
	std::uintptr_t hPlayerHandle; // 0x04
	int nImpulseCommand; // 0x08
	QAngle_t angViewPoint; // 0x0C
	QAngle_t angAbsViewPoint; // 0x18
	int nButtons; // 0x24
	int nOldButtons; // 0x28
	float flForwardMove; // 0x2C
	float flSideMove; // 0x30
	float flUpMove; // 0x34
	float flMaxSpeed; // 0x38
	float flClientMaxSpeed; // 0x3C
	Vector_t vecVelocity; // 0x40
	Vector_t vecTrailingVelocity; // 0x4C
	float flTrailingVelocityTime; // 0x58
	QAngle_t angEdictView; // 0x5C
	QAngle_t angOldEdictView; // 0x68
	float flOutStepHeight; // 0x74
	Vector_t vecOutWishVelocity; // 0x78
	Vector_t vecOutJumpVelocity; // 0x84
	Vector_t vecConstraintCenter; // 0x90
	float flConstraintRadius; // 0x9C
	float flConstraintWidth; // 0xA0
	float flConstraintSpeedFactor; // 0xA4
	bool bConstraintPastRadius; // 0xA8
	Vector_t vecAbsOrigin; // 0xAC
};
static_assert(sizeof(CMoveData) == 0xB8);
#pragma pack(pop)

// forward declarations
class CBasePlayer;

class IGameMovement : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	// emulate movement and update move data for given player, automatically calls 'IGameMovement::FinishMove()'
	void ProcessMovement(CBasePlayer* pPlayer, CMoveData* pMoveData)
	{
		CallVFunc<void, 1U>(this, pPlayer, pMoveData);
	}

	// reset current predictable player
	void Reset()
	{
		CallVFunc<void, 2U>(this);
	}

	// set the current predictable player
	void StartTrackPredictionErrors(CBasePlayer* pPlayer)
	{
		CallVFunc<void, 3U>(this, pPlayer);
	}

	void FinishTrackPredictionErrors(CBasePlayer* pPlayer)
	{
		CallVFunc<void, 4U>(this, pPlayer);
	}

	const Vector_t& GetPlayerMins(bool bDucked)
	{
		return CallVFunc<const Vector_t&, 6U>(this, bDucked);
	}

	const Vector_t& GetPlayerMaxs(bool bDucked)
	{
		return CallVFunc<const Vector_t&, 7U>(this, bDucked);
	}

	const Vector_t& GetPlayerViewOffset(bool bDucked)
	{
		return CallVFunc<const Vector_t&, 8U>(this, bDucked);
	}
};
