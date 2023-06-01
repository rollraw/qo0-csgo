#pragma once
#include "datatypes/vector.h"
#include "datatypes/qangle.h"
#include "datatypes/matrix.h"
#include "datatypes/utlvector.h"
#include "datatypes/utlsymbol.h"

// used: findpattern
#include "../utilities/memory.h"

#define MAX_OVERLAYS 13 // @ida: client.dll -> U8["83 C6 04 83 F8 ? 7C D4" + 0x5]
using AnimLayerPreset_t = const int*;

#pragma region animation_enumerations
enum EAnimationStateLayer : int
{
	ANIMATION_LAYER_AIMMATRIX = 0,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
	ANIMATION_LAYER_COUNT = 13 // @ida: client.dll -> ["3D ? ? ? ? 7C AB" + 0x1] / U8["3D ? ? ? ? 7C AB" - 0x5]
};

enum EAnimationStatePoseParameters : int
{
	PLAYER_POSE_PARAM_FIRST = 0,
	PLAYER_POSE_PARAM_LEAN_YAW = PLAYER_POSE_PARAM_FIRST,
	PLAYER_POSE_PARAM_SPEED,
	PLAYER_POSE_PARAM_LADDER_SPEED,
	PLAYER_POSE_PARAM_LADDER_YAW,
	PLAYER_POSE_PARAM_MOVE_YAW,
	PLAYER_POSE_PARAM_RUN,
	PLAYER_POSE_PARAM_BODY_YAW,
	PLAYER_POSE_PARAM_BODY_PITCH,
	PLAYER_POSE_PARAM_DEATH_YAW,
	PLAYER_POSE_PARAM_STAND,
	PLAYER_POSE_PARAM_JUMP_FALL,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_IDLE,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_IDLE,
	PLAYER_POSE_PARAM_STRAFE_DIR,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_WALK,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_RUN,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_WALK,
	PLAYER_POSE_PARAM_MOVE_BLEND_WALK,
	PLAYER_POSE_PARAM_MOVE_BLEND_RUN,
	PLAYER_POSE_PARAM_MOVE_BLEND_CROUCH_WALK,
	//PLAYER_POSE_PARAM_STRAFE_CROSS,
	PLAYER_POSE_PARAM_COUNT
};

enum ESequenceActivity : int
{
	ACT_INVALID = -1,
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
#pragma endregion

// forward declarations
class CStudioHdr;
class CBaseAnimating;
class CBaseAnimatingOverlay;
class CCSPlayer;
class CWeaponCSBase;

#pragma pack(push, 4)
// @source: master/public/bone_accessor.h
class CBoneAccessor
{
public:
	const CBaseAnimating* pAnimating; // 0x00 // owner entity only used in the client module for debug verification
	Matrix3x4a_t* matBones; // 0x04 // readable/writable bones
	int nReadableBones; // 0x08	// mask of which bones can be read
	int nWritableBones; // 0x0C	// mask of which bones can be written
};
static_assert(sizeof(CBoneAccessor) == 0x10);

// @source: master/game/client/animationlayer.h
class CAnimationLayer
{
public:
	float flAnimationTime; // 0x00
	float flFadeOutTime; // 0x04
	CStudioHdr* pDispatchedStudioHdr; // 0x08
	int nDispatchedSrc; // 0x0C
	int nDispatchedDst; // 0x10
	int iOrder; // 0x14
	int nSequence; // 0x18 // in range: [-1 .. 65535]
	float flPrevCycle; // 0x1C // in range: [-2.0 .. 2.0]
	float flWeight; // 0x20 // in range: [-5.0 .. 5.0]
	float flWeightDeltaRate; // 0x24 // in range: [-5.0 .. 5.0]
	float flPlaybackRate; // 0x28
	float flCycle; // 0x2C // in range: [-2.0 .. 2.0]
	CBaseAnimatingOverlay* pOwner; // 0x30
	int nInvalidatePhysicsBits; // 0x34
};
static_assert(sizeof(CAnimationLayer) == 0x38);

// @source: master/game/shared/cstrike15/csgo_playeranimstate.cpp
// master/game/shared/cstrike15/csgo_playeranimstate.h
//
// functions used to verify offsets:
// @ida CCSGOPlayerAnimState::DoProceduralFootPlant(): client.dll -> "55 8B EC 83 E4 F0 83 EC 78 56 8B F1 57 8B 56" @xref: "rfoot_lock", "lfoot_lock"
// @ida CCSGOPlayerAnimState::SetUpVelocity(): client.dll -> "55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D"
// @ida CCSGOPlayerAnimState::SetUpAimMatrix(): client.dll -> "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D"
// @ida CCSGOPlayerAnimState::SetUpWeaponAction(): client.dll -> "55 8B EC 51 53 56 57 8B F9 8B 77 60"
// @ida CCSGOPlayerAnimState::SetUpMovement(): client.dll -> "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 57 8B 3D" @xref: "strafe", "move_%s"
// @ida CCSGOPlayerAnimState::SetUpLean(): client.dll -> "55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 20 F3" @xref: "lean"
class CCSGOPlayerAnimState
{
public:
	CCSGOPlayerAnimState(CCSPlayer* pCSPlayer);

	void* operator new(const std::size_t nSize)
	{
		return I::MemAlloc->Alloc(nSize);
	}

	void operator delete(void* pMemory)
	{
		I::MemAlloc->Free(pMemory);
	}

	void Update(const QAngle_t& angView);
	void Reset();
	void ModifyEyePosition(Vector_t& vecInputEyePosition) const;

public:
	AnimLayerPreset_t pLayerOrderPreset; // 0x0000
	bool bFirstRunSinceInit; // 0x0004
	bool bFirstFootPlantSinceInit; // 0x0005
	int nLastProceduralFootPlantUpdateFrame; // 0x0008
	float flEyePositionSmoothLerp; // 0x000C
	float flStrafeChangeWeightSmoothFalloff; // 0x0010
	std::byte pad0[0x3C]; // 0x0014
	int nCachedModelIndex; // 0x0050
	float flStepHeightLeft; // 0x0054
	float flStepHeightRight; // 0x0058
	CWeaponCSBase* pWeaponLastBoneSetup; // 0x005C
	CCSPlayer* pPlayer; // 0x0060
	CWeaponCSBase* pWeapon; // 0x0064
	CWeaponCSBase* pWeaponLast; // 0x0068
	float flLastUpdateTime; // 0x006C
	int nLastUpdateFrame; // 0x0070
	float flLastUpdateIncrement; // 0x0074
	float flEyeYaw; // 0x0078
	float flEyePitch; // 0x007C
	float flFootYaw; // 0x0080
	float flFootYawLast; // 0x0084
	float flMoveYaw; // 0x0088
	float flMoveYawIdeal; // 0x008C // changes when moving/jumping/hitting ground
	float flMoveYawCurrentToIdeal; // 0x0090
	float flTimeToAlignLowerBody; // 0x0094
	float flPrimaryCycle; // 0x0098 // in range: [0.0 .. 1.0]
	float flMoveWeight; // 0x009C // in range: [0.0 .. 1.0]
	float flMoveWeightSmoothed; // 0x00A0
	float flDuckAmount; // 0x00A4
	float flDuckAdditional; // 0x00A8 // for when we duck a bit after hitting ground from a jump
	float flRecrouchWeight; // 0x00AC
	Vector_t vecOrigin; // 0x00B0
	Vector_t vecOriginLast;// 0x00BC
	Vector_t vecVelocity; // 0x00C8
	Vector_t vecVelocityNormalized; // 0x00D4
	Vector_t vecVelocityNormalizedNonZero; // 0x00E0
	float flVelocityLength2D; // 0x00EC
	float flVelocityLengthZ; // 0x00F0
	float flRunSpeedNormalized; // 0x00F4 // @ida: client.dll -> ["F3 0F 59 8F ? ? ? ? 0F 2F" + 0x4]
	float flWalkSpeedNormalized; // 0x00F8
	float flCrouchSpeedNormalized; // 0x00FC
	float flDurationMoving; // 0x0100
	float flDurationStill; // 0x0104
	bool bOnGround; // 0x0108 // @ida: client.dll -> ABS["E8 ? ? ? ? F6 86 ? ? ? ? ? 0F 84" + 0x1] + 0x18
	bool bLanding; // 0x0109
	float flJumpToFall; // 0x010C
	float flDurationInAir; // 0x0110
	float flLeftGroundHeight; // 0x0114
	float flHitGroundWeight; // 0x0118
	float flWalkToRunTransition; // 0x011C // in range: [0.0 .. 1.0], doesn't change when walking or crouching, only running // @ida: client.dll -> ["F3 0F 10 9E ? ? ? ? 0F 57 C9" + 0x4]
	std::byte pad1[0x4]; // 0x0120
	float flInAirSmoothValue; // 0x0124 // in range: [0.0 .. 1.0], affected while jumping and running, or when just jumping
	bool bOnLadder; // 0x0128
	float flLadderWeight; // 0x012C
	float flLadderSpeed; // 0x0130
	bool bWalkToRunTransitionState; // 0x0134
	bool bDefuseStarted; // 0x0135
	bool bPlantAnimStarted; // 0x0136
	bool bTwitchAnimStarted; // 0x0137
	bool bAdjustStarted; // 0x0138
	CUtlVector<CUtlSymbol> vecActivityModifiers; // 0x013C
	float flNextTwitchTime; // 0x0150
	float flTimeOfLastKnownInjury; // 0x0154
	float flLastVelocityTestTime; // 0x0158 // @ida: client.dll -> ["F3 0F 5C A7 ? ? ? ? 0F 2F" + 0x4]
	Vector_t vecVelocityLast; // 0x015C
	Vector_t vecTargetAcceleration; // 0x0168
	Vector_t vecAcceleration; // 0x0174 // @ida: client.dll -> ["F3 0F 7E 87 ? ? ? ? 8D B7" + 0x4]
	float flAccelerationWeight; // 0x0180
	std::byte pad3[0xC]; // 0x184
	float flStrafeChangeWeight; // 0x0190 // @ida: client.dll -> ["F3 0F 10 9E ? ? ? ? 0F 28 EC" + 0x4]
	float flStrafeChangeTargetWeight; // 0x194
	float flStrafeChangeCycle; // 0x0198 // @ida: client.dll -> ["F3 0F 10 96 ? ? ? ? F3 0F 59 25" + 0x4]
	int nStrafeSequence; // 0x019C
	bool bStrafeChanging; // 0x01A0
	float flDurationStrafing; // 0x01A4
	float flFootLerp; // 0x01A8
	bool bFeetCrossed; // 0x01AC
	bool bPlayerIsAccelerating; // 0x01AD // @ida: client.dll -> ["88 86 ? ? ? ? 8B 86 ? ? ? ? 83" + 0x2]
	std::byte pad4[0x174 + 0x2]; // 0x01AE
	float flCameraSmoothHeight; // 0x0324
	bool bSmoothHeightValid; // 0x0328 @ida: client.dll -> ["C6 87 ? ? ? ? ? EB 92" + 0x2]
	std::byte pad5[0x10 + 0x3]; // 0x0329
	float flAimYawMin; // 0x033C // @ida: client.dll -> ["F3 0F 5E 86 ? ? ? ? F3 0F 59 05 ? ? ? ? 80" + 0x4]
	float flAimYawMax; // 0x0340
	int iAnimsetVersion; // 0x0344 // @xref: "animset_version"
};
static_assert(sizeof(CCSGOPlayerAnimState) == 0x348); // size verify @ida: client.dll -> ["68 ? ? ? ? 0F 45 F7" + 0x2]
#pragma pack(pop)
