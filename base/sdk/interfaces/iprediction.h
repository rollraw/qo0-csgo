#pragma once
// used: vector
#include "../datatypes/vector.h"
// used: angle
#include "../datatypes/qangle.h"
// used: usercmd
#include "../datatypes/usercmd.h"
// used: cbasehandle
#include "icliententitylist.h"

// @credits: https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/shared/igamemovement.h
class CMoveData
{
public:
	bool			bFirstRunOfFunctions : 1;
	bool			bGameCodeMovedPlayer : 1;
	bool			bNoAirControl : 1;
	std::uintptr_t	hPlayerHandle;		// edict index on server, client entity handle on client=
	int				nImpulseCommand;	// impulse command issued.
	QAngle			angViewAngles;		// command view angles (local space)
	QAngle			angAbsViewAngles;	// command view angles (world space)
	int				nButtons;			// attack buttons.
	int				nOldButtons;		// from host_client->oldbuttons;
	float			flForwardMove;
	float			flSideMove;
	float			flUpMove;
	float			flMaxSpeed;
	float			flClientMaxSpeed;
	Vector			vecVelocity;		// edict::velocity	// current movement direction.
	Vector			vecTrailingVelocity;
	float			flTrailingVelocityTime;
	Vector			vecAngles;			// edict::angles
	Vector			vecOldAngles;
	float			flOutStepHeight;	// how much you climbed this move
	Vector			vecOutWishVel;		// this is where you tried 
	Vector			vecOutJumpVel;		// this is your jump velocity
	Vector			vecConstraintCenter;
	float			flConstraintRadius;
	float			flConstraintWidth;
	float			flConstraintSpeedFactor;
	bool			bConstraintPastRadius;
	Vector			vecAbsOrigin;
};

// @credits: https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/shared/imovehelper.h
class CBaseEntity;
class IPhysicsSurfaceProps;
class CGameTrace;
enum ESoundLevel;
class IMoveHelper
{
public:
	virtual	const char*			GetName(void* hEntity) const = 0;
	virtual void				SetHost(CBaseEntity* pHost) = 0;
	virtual void				ResetTouchList() = 0;
	virtual bool				AddToTouched(const CGameTrace& trace, const Vector& vecImpactVelocity) = 0;
	virtual void				ProcessImpacts() = 0;
	virtual void				Con_NPrintf(int nIndex, char const* fmt, ...) = 0;
	virtual void				StartSound(const Vector& vecOrigin, int iChannel, char const* szSample, float flVolume, ESoundLevel soundlevel, int fFlags, int iPitch) = 0;
	virtual void				StartSound(const Vector& vecOrigin, const char* soundname) = 0;
	virtual void				PlaybackEventFull(int fFlags, int nClientIndex, unsigned short uEventIndex, float flDelay, Vector& vecOrigin, Vector& vecAngles, float flParam1, float flParam2, int iParam1, int iParam2, int bParam1, int bParam2) = 0;
	virtual bool				PlayerFallingDamage() = 0;
	virtual void				PlayerSetAnimation(int playerAnimation) = 0;
	virtual IPhysicsSurfaceProps* GetSurfaceProps() = 0;
	virtual bool				IsWorldEntity(const unsigned long& hEntity) = 0;
};

class IGameMovement
{
public:
	virtual						~IGameMovement() { }
	virtual void				ProcessMovement(CBaseEntity* pEntity, CMoveData* pMove) = 0;
	virtual void				Reset() = 0;
	virtual void				StartTrackPredictionErrors(CBaseEntity* pEntity) = 0;
	virtual void				FinishTrackPredictionErrors(CBaseEntity* pEntity) = 0;
	virtual void				DiffPrint(char const* fmt, ...) = 0;
	virtual Vector const&		GetPlayerMins(bool bDucked) const = 0;
	virtual Vector const&		GetPlayerMaxs(bool bDucked) const = 0;
	virtual Vector const&		GetPlayerViewOffset(bool bDucked) const = 0;
	virtual bool				IsMovingPlayerStuck() const = 0;
	virtual CBaseEntity*		GetMovingPlayer() const = 0;
	virtual void				UnblockPusher(CBaseEntity* pEntity, CBaseEntity* pPusher) = 0;
	virtual void				SetupMovementBounds(CMoveData* pMove) = 0;
};

class IPrediction
{
public:
	std::byte		pad0[0x4];						// 0x0000
	std::uintptr_t	hLastGround;					// 0x0004
	bool			bInPrediction;					// 0x0008
	bool			bIsFirstTimePredicted;			// 0x0009
	bool			bEnginePaused;					// 0x000A
	bool			bOldCLPredictValue;				// 0x000B
	int				iPreviousStartFrame;			// 0x000C
	int				nIncomingPacketNumber;			// 0x0010
	float			flLastServerWorldTimeStamp;		// 0x0014

	struct Split_t
	{
		bool		bIsFirstTimePredicted;			// 0x0018
		std::byte	pad0[0x3];						// 0x0019
		int			nCommandsPredicted;				// 0x001C
		int			nServerCommandsAcknowledged;	// 0x0020
		int			iPreviousAckHadErrors;			// 0x0024
		float		flIdealPitch;					// 0x0028
		int			iLastCommandAcknowledged;		// 0x002C
		bool		bPreviousAckErrorTriggersFullLatchReset; // 0x0030
		CUtlVector<CBaseHandle> vecEntitiesWithPredictionErrorsInLastAck; // 0x0031
		bool		bPerformedTickShift;			// 0x0045
	};

	Split_t			Split[1];						// 0x0018

public:
	void Update(int iStartFrame, bool bValidFrame, int nIncomingAcknowledged, int nOutgoingCommand)
	{
		MEM::CallVFunc<void>(this, 3, iStartFrame, bValidFrame, nIncomingAcknowledged, nOutgoingCommand);
	}

	void GetLocalViewAngles(QAngle& angView)
	{
		MEM::CallVFunc<void, QAngle&>(this, 12, angView);
	}

	void SetLocalViewAngles(QAngle& angView)
	{
		MEM::CallVFunc<void, QAngle&>(this, 13, angView);
	}

	void CheckMovingGround(CBaseEntity* pEntity, double dbFrametime)
	{
		MEM::CallVFunc<void>(this, 18, pEntity, dbFrametime);
	}

	void SetupMove(CBaseEntity* pEntity, CUserCmd* pCmd, IMoveHelper* pHelper, CMoveData* pMoveData)
	{
		MEM::CallVFunc<void>(this, 20, pEntity, pCmd, pHelper, pMoveData);
	}

	void FinishMove(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveData* pMoveData)
	{
		MEM::CallVFunc<void>(this, 21, pEntity, pCmd, pMoveData);
	}
};
