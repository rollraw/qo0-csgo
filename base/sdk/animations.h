#pragma once
// used: vector
#include "datatypes/vector.h"
// used: angle
#include "datatypes/qangle.h"
// used: findpattern
#include "../utilities/memory.h"

/* max animation layers */
#define MAXOVERLAYS 15

enum ESequenceActivity : int
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
	ACT_CSGO_EXIT_LADDER_BOTTOM
};

class matrix3x4a_t;
class CBaseAnimating;
class CBoneAccessor
{
public:
	const CBaseAnimating*	pAnimating;		//0x00
	matrix3x4a_t*			matBones;		//0x04
	int						nReadableBones;	//0x08
	int						nWritableBones;	//0x0C
}; // Size: 0x10

class CAnimationLayer
{
public:
	float			flAnimationTime;		//0x00
	float			flFadeOut;				//0x04
	void*			pStudioHdr;				//0x08
	int				nDispatchedSrc;			//0x0C
	int				nDispatchedDst;			//0x10
	int				iOrder;					//0x14
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
	void Create(CBaseEntity* pEntity)
	{
		using CreateAnimationStateFn = void(__thiscall*)(void*, CBaseEntity*);
		static auto oCreateAnimationState = reinterpret_cast<CreateAnimationStateFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"))); // @xref: "ggprogressive_player_levelup"

		if (oCreateAnimationState == nullptr)
			return;

		oCreateAnimationState(this, pEntity);
	}

	void Update(QAngle angView)
	{
		using UpdateAnimationStateFn = void(__vectorcall*)(void*, void*, float, float, float, void*);
		static auto oUpdateAnimationState = reinterpret_cast<UpdateAnimationStateFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"))); // @xref: "%s_aim"

		if (oUpdateAnimationState == nullptr)
			return;

		oUpdateAnimationState(this, nullptr, 0.0f, angView.y, angView.x, nullptr);
	}

	void Reset()
	{
		using ResetAnimationStateFn = void(__thiscall*)(void*);
		static auto oResetAnimationState = reinterpret_cast<ResetAnimationStateFn>(MEM::FindPattern(CLIENT_DLL, XorStr("56 6A 01 68 ? ? ? ? 8B F1"))); // @xref: "player_spawn"

		if (oResetAnimationState == nullptr)
			return;

		oResetAnimationState(this);
	}

	std::byte	pad0[0x60]; // 0x00
	CBaseEntity* pEntity; // 0x60
	CBaseCombatWeapon* pActiveWeapon; // 0x64
	CBaseCombatWeapon* pLastActiveWeapon; // 0x68
	float		flLastUpdateTime; // 0x6C
	int			iLastUpdateFrame; // 0x70
	float		flLastUpdateIncrement; // 0x74
	float		flEyeYaw; // 0x78
	float		flEyePitch; // 0x7C
	float		flGoalFeetYaw; // 0x80
	float		flLastFeetYaw; // 0x84
	float		flMoveYaw; // 0x88
	float		flLastMoveYaw; // 0x8C // changes when moving/jumping/hitting ground
	float		flLeanAmount; // 0x90
	float		flLowerBodyYawAlignTime; // 0x94
	float		flFeetCycle; // 0x98 0 to 1
	float		flMoveWeight; // 0x9C 0 to 1
	float		flMoveWeightSmoothed; // 0xA0
	float		flDuckAmount; // 0xA4
	float		flHitGroundCycle; // 0xA8
	float		flRecrouchWeight; // 0xAC
	Vector		vecOrigin; // 0xB0
	Vector		vecLastOrigin;// 0xBC
	Vector		vecVelocity; // 0xC8
	Vector		vecVelocityNormalized; // 0xD4
	Vector		vecVelocityNormalizedNonZero; // 0xE0
	float		flVelocityLenght2D; // 0xEC
	float		flJumpFallVelocity; // 0xF0
	float		flSpeedNormalized; // 0xF4 // clamped velocity from 0 to 1 
	float		flRunningSpeed; // 0xF8
	float		flDuckingSpeed; // 0xFC
	float		flDurationMoving; // 0x100
	float		flDurationStill; // 0x104
	bool		bOnGround; // 0x108
	bool		bHitGroundAnimation; // 0x109
	std::byte	pad2[0x2]; // 0x10A
	float		flNextLowerBodyYawUpdateTime; // 0x10C
	float		flDurationInAir; // 0x110
	float		flLeftGroundHeight; // 0x114
	float		flHitGroundWeight; // 0x118 // from 0 to 1, is 1 when standing
	float		flWalkToRunTransition; // 0x11C // from 0 to 1, doesnt change when walking or crouching, only running
	std::byte	pad3[0x4]; // 0x120
	float		flAffectedFraction; // 0x124 // affected while jumping and running, or when just jumping, 0 to 1
	std::byte	pad4[0x208]; // 0x128
	float		flMinBodyYaw; // 0x330
	float		flMaxBodyYaw; // 0x334
	float		flMinPitch; //0x338
	float		flMaxPitch; // 0x33C
	int			iAnimsetVersion; // 0x340
}; // Size: 0x344
