#pragma once
// used: vector
#include "datatypes/vector.h"

/* max animation overlays */
#define MAX_LAYER_RECORDS	15

/* knife sequences */
#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12
#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15
#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13
#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12
#define SEQUENCE_BOWIE_IDLE1 1

enum EAnimationActivity : int
{
	ACT_CSGO_NULL = 957,
	ACT_CSGO_DEFUSE,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM,
};

class CAnimationLayer
{
public:
	bool			bClientBlend;			//0x00
	float			flBlendIn;				//0x04
	void*			pStudioHdr;				//0x08
	int				nDispatchedSrc;			//0x0C
	int				nDispatchedDst;			//0x10
	std::uintptr_t  iOrder;					//0x14
	std::uintptr_t  nSequence;				//0x18
	float			flPrevCycle;			//0x1C
	float			flWeight;				//0x20
	float			flWeightDeltaRate;		//0x24
	float			flPlaybackRate;			//0x28
	float			flCycle;				//0x2C
	void*			pOwner;					//0x30
	int				nInvalidatePhysicsBits;	//0x34
}; // Size: 0x38

class CBaseEntity;
class CBaseCombatWeapon;
class CCSGOPlayerAnimState
{
public:
	std::byte	pad0[0x18]; //0x00
	float		flAnimUpdateTimer; //0x18
	std::byte	pad1[0xC]; //0x1C
	float		flStartedMovingTime; //0x28
	float		flLastMoveTime; //0x2C
	std::byte	pad2[0xC]; //0x30
	float		flSpeed; //0x3C
	float		flLastLbyTime; //0x40
	std::byte	pad3[0x8]; //0x44
	float		flRunAmount; //0x4C
	std::byte	pad4[0x10]; //0x50
	CBaseEntity* pEntity; //0x60
	CBaseCombatWeapon* pActiveWeapon; //0x64
	CBaseCombatWeapon* pLastActiveWeapon; //0x68
	float		flLastClientSideAnimationUpdateTime; //0x6C
	int			iLastClientSideAnimationUpdateFramecount; //0x70
	float		flEyeTimer; //0x74
	float		flEyeYaw; //0x78
	float		flEyePitch; //0x7C
	float		flGoalFeetYaw; //0x80
	float		flCurrentFeetYaw; //0x84
	float		flCurrentTorsoYaw; //0x88
	float		flLastMoveYaw; //0x8C // changes when moving/jumping/hitting ground
	float		flLeanAmount; //0x90
	std::byte	pad5[0x4]; //0x94
	float		flFeetCycle; //0x98 0 to 1
	float		flFeetYawRate; //0x9C 0 to 1
	std::byte	pad6[0x4]; //0xA0
	float		flDuckAmount; //0xA4
	float		flLandingDuckAdditiveSomething; //0xA8
	std::byte	pad7[0x4]; //0xAC
	Vector		vecOrigin; //0xB8
	Vector		vecLastOrigin;//0xC4
	Vector		vecVelocity; //0xC8
	float		flAffectedMoveDirection; //0xD4 // affected by movement and direction
	std::byte	pad8[0x8]; //0xDC
	float		flAffectedDirection; //0xE0 // from -1 to 1 when moving and affected by direction
	float		flAffectedDirection2; //0xE4 // from -1 to 1 when moving and affected by direction
	std::byte	pad9[0x4]; //0xE8
	float		flVelocity; //0xEC
	float		flJumpFallVelocity; //0xF0
	float		flSpeedNormalized; //0xF4 // clamped velocity from 0 to 1 
	float		flFeetSpeedForwardsOrSideways; //0xF8 // from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float		flFeetSpeedForwardOrSideways2; //0xFC // from 0 to 3. something
	float		flTimeSinceStartedMoving; //0x100
	float		flTimeSinceStoppedMoving; //0x104
	bool		bOnGround; //0x108
	bool		bInHitGroundAnimation; //0x109
	std::byte	pad10[0x4]; //0x10A
	float		flLastOriginZ; //0x10E
	float		flHeadHeightOrOffsetFromHittingGroundAnimation; //0x112 // from 0 to 1, is 1 when standing
	float		flStopToFullRunningFraction; //0x116 // from 0 to 1, doesnt change when walking or crouching, only running
	std::byte	pad11[0x4]; //0x11A
	float		flAffectedFraction; //0x11E // affected while jumping and running, or when just jumping, 0 to 1
	std::byte	pad12[0x218]; //0x122
}; // Size: 0x33A
