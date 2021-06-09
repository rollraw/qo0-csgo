#pragma once
// used: std::array
#include <array>
// used: std::vector
#include <vector>
// used: std::optional
#include <optional>

// used: animation state
#include "animations.h"
// used: baseentity lifestate
#include "definitions.h"
// used: bf_read
#include "bitbuf.h"
// used: usercmd
#include "datatypes/usercmd.h"
// used: netvars
#include "../core/netvar.h"
// used: basehandle
#include "interfaces/icliententitylist.h"
// used: model
#include "interfaces/ivmodelinfo.h"

// @note: all clasees values arranged in order similar to the tables! keep it like that

#pragma region entities_definitions
#define INVALID_EHANDLE_INDEX		0xFFFFFFFF
#define NUM_ENT_ENTRY_BITS			(11 + 2)
#define NUM_ENT_ENTRIES				(1 << NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_BITS			16
#define NUM_SERIAL_NUM_SHIFT_BITS	(32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK				((1 << NUM_SERIAL_NUM_BITS) - 1)
#pragma endregion

#pragma region entities_enumerations
enum EDataUpdateType : int
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

enum ETeamID : int
{
	TEAM_UNASSIGNED = 0,
	TEAM_SPECTATOR,
	TEAM_TT,
	TEAM_CT
};

enum EThinkMethods : int
{
	THINK_FIRE_ALL_FUNCTIONS = 0,
	THINK_FIRE_BASE_ONLY,
	THINK_FIRE_ALL_BUT_BASE,
};

enum EItemDefinitionIndex : short
{
	WEAPON_NONE = 0,
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_ZONE_REPULSOR = 20,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SHIELD = 37,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE_GG = 41,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS = 69,
	WEAPON_BREACHCHARGE = 70,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE = 75,
	WEAPON_HAMMER = 76,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB = 81,
	WEAPON_DIVERSION = 82,
	WEAPON_FRAG_GRENADE = 83,
	WEAPON_SNOWBALL = 84,
	WEAPON_BUMPMINE = 85,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	WEAPON_KNIFE_CORD = 517,
	WEAPON_KNIFE_CANIS = 518,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
	WEAPON_KNIFE_OUTDOOR = 521,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER = 523,
	WEAPON_KNIFE_SKELETON = 525,
	GLOVE_STUDDED_BROKENFANG = 4725,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T = 5028,
	GLOVE_CT = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_HANDWRAPS = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_STUDDED_HYDRA = 5035,
	SPECIAL_AGENT_BLUEBERRIES_BUCKSHOT = 4619,
	SPECIAL_AGENT_TWO_TIMES_MCCOY_TACP = 4680,
	SPECIAL_AGENT_COMMANDOR_MAE_JAMISON = 4711,
	SPECIAL_AGENT_1ST_LIEUTENANT_FARLOW,
	SPECIAL_AGENT_JOHN_KASK,
	SPECIAL_AGENT_BIO_HAZ_SPECIALIST,
	SPECIAL_AGENT_SERGEANT_BOMBSON,
	SPECIAL_AGENT_CHEM_HAZ_SPECIALIST,
	SPECIAL_AGENT_REZAN_THE_REDSHIRT = 4718,
	SPECIAL_AGENT_SIR_BLOODY_MIAMI_DARRYL = 4726,
	SPECIAL_AGENT_SAFECRACKER_VOLTZMANN,
	SPECIAL_AGENT_LITTLE_KEV,
	SPECIAL_AGENT_GETAWAY_SALLY = 4730,
	SPECIAL_AGENT_NUMBER_K = 4732,
	SPECIAL_AGENT_SIR_BLOODY_SILENT_DARRYL = 4733,
	SPECIAL_AGENT_SIR_BLOODY_SKULLHEAD_DARRYL,
	SPECIAL_AGENT_SIR_BLOODY_DARRYL_ROYALE,
	SPECIAL_AGENT_SIR_BLOODY_LOUDMOUTH_DARRYL,
	SPECIAL_AGENT_T = 5036,
	SPECIAL_AGENT_CT = 5037,
	SPECIAL_AGENT_GROUND_REBEL = 5105,
	SPECIAL_AGENT_OSIRIS,
	SPECIAL_AGENT_SHAHMAT,
	SPECIAL_AGENT_MUHLIK,
	SPECIAL_AGENT_SOLDIER = 5205,
	SPECIAL_AGENT_ENFORCER,
	SPECIAL_AGENT_SLINGSHOT,
	SPECIAL_AGENT_STREET_SOLDIER,
	SPECIAL_AGENT_OPERATOR = 5305,
	SPECIAL_AGENT_MARKUS_DELROW,
	SPECIAL_AGENT_MICHAEL_SYFERS,
	SPECIAL_AGENT_AVA,
	SPECIAL_AGENT_3RD_COMMANDO_COMPANY = 5400,
	SPECIAL_AGENT_SEAL_TEAM_6_SOLDIER,
	SPECIAL_AGENT_BUCKSHOT,
	SPECIAL_AGENT_TWO_TIMES_MCCOY_USAF,
	SPECIAL_AGENT_RICKSAW,
	SPECIAL_AGENT_DRAGOMIR = 5500,
	SPECIAL_AGENT_MAXIMUS,
	SPECIAL_AGENT_REZAN_THE_READY,
	SPECIAL_AGENT_BLACKWOLF = 5503,
	SPECIAL_AGENT_THE_DOCTOR,
	SPECIAL_AGENT_DRAGOMIR_FOOTSOLDIERS,
	SPECIAL_AGENT_B_SQUADRON_OFFICER = 5601
};

enum EWeaponType : int
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL = 1,
	WEAPONTYPE_SUBMACHINEGUN = 2,
	WEAPONTYPE_RIFLE = 3,
	WEAPONTYPE_SHOTGUN = 4,
	WEAPONTYPE_SNIPER = 5,
	WEAPONTYPE_MACHINEGUN = 6,
	WEAPONTYPE_C4 = 7,
	WEAPONTYPE_PLACEHOLDER = 8,
	WEAPONTYPE_GRENADE = 9,
	WEAPONTYPE_HEALTHSHOT = 11,
	WEAPONTYPE_FISTS = 12,
	WEAPONTYPE_BREACHCHARGE = 13,
	WEAPONTYPE_BUMPMINE = 14,
	WEAPONTYPE_TABLET = 15,
	WEAPONTYPE_MELEE = 16
};
#pragma endregion

class IHandleEntity
{
public:
	virtual							~IHandleEntity() { }
	virtual void					SetRefEHandle(const CBaseHandle& hRef) = 0;
	virtual const CBaseHandle&		GetRefEHandle() const = 0;
};

class IClientUnknown;
class ICollideable
{
public:
	virtual IHandleEntity*			GetEntityHandle() = 0;
	virtual const Vector&			OBBMins() const = 0;
	virtual const Vector&			OBBMaxs() const = 0;
	virtual void					WorldSpaceTriggerBounds(Vector* pVecWorldMins, Vector* pVecWorldMaxs) const = 0;
	virtual bool					TestCollision(const Ray_t& ray, unsigned int fContentsMask, Trace_t& tr) = 0;
	virtual bool					TestHitboxes(const Ray_t& ray, unsigned int fContentsMask, Trace_t& tr) = 0;
	virtual int						GetCollisionModelIndex() = 0;
	virtual const Model_t*			GetCollisionModel() = 0;
	virtual Vector&					GetCollisionOrigin() const = 0;
	virtual QAngle&					GetCollisionAngles() const = 0;
	virtual const matrix3x4_t&		CollisionToWorldTransform() const = 0;
	virtual ESolidType				GetSolid() const = 0;
	virtual int						GetSolidFlags() const = 0;
	virtual IClientUnknown*			GetIClientUnknown() = 0;
	virtual int						GetCollisionGroup() const = 0;

	/*
	 * @note: should be equivalent to C_BaseAnimating::ComputeHitboxSurroundingBox
	 * DOESNT NEEDED TRANSORMATION!
	 * and bugged when trying to get non-player entity box
	 */
	virtual void					WorldSpaceSurroundingBounds(Vector* pVecMins, Vector* pVecMaxs) = 0;
	virtual unsigned int			GetRequiredTriggerFlags() const = 0;
	virtual const matrix3x4_t*		GetRootParentToWorldTransform() const = 0;
	virtual void*					GetVPhysicsObject() const = 0;
};

class IClientAlphaProperty
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void SetAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual void SetRenderFX(int nRenderFx, int nRenderMode, float flStartTime = FLT_MAX, float flDuration = 0.0f) = 0;
	virtual void SetFade(float flGlobalFadeScale, float flDistFadeStart, float flDistFadeEnd) = 0;
	virtual void SetDesyncOffset(int nOffset) = 0;
	virtual void EnableAlphaModulationOverride(bool bEnable) = 0;
	virtual void EnableShadowAlphaModulationOverride(bool bEnable) = 0;
	virtual void SetDistanceFadeMode(int nFadeMode) = 0;
};

class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class CBaseEntity;
class IClientThinkable;
class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable*			GetCollideable() = 0;
	virtual IClientNetworkable*		GetClientNetworkable() = 0;
	virtual IClientRenderable*		GetClientRenderable() = 0;
	virtual IClientEntity*			GetIClientEntity() = 0;
	virtual CBaseEntity*			GetBaseEntity() = 0;
	virtual IClientThinkable*		GetClientThinkable() = 0;
	virtual IClientAlphaProperty*	GetClientAlphaProperty() = 0;
};

struct RenderableInstance_t
{
	std::uint8_t uAlpha;
};

using ClientShadowHandle_t = std::uint16_t;
using ClientRenderHandle_t = std::uint16_t;
using ModelInstanceHandle_t = std::uint16_t;
class IClientRenderable
{
public:
	virtual IClientUnknown*			GetIClientUnknown() = 0;
	virtual Vector&					GetRenderOrigin() = 0;
	virtual QAngle&					GetRenderAngles() = 0;
	virtual bool					ShouldDraw() = 0;
	virtual int						GetRenderFlags() = 0;
	virtual bool					IsTransparent() = 0;
	virtual ClientShadowHandle_t	GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t&	RenderHandle() = 0;
	virtual const Model_t*			GetModel() const = 0;
	virtual int						DrawModel(int nFlags, const RenderableInstance_t& uInstance) = 0;
	virtual int						GetBody() = 0;
	virtual void					GetColorModulation(float* pColor) = 0;
	virtual bool					LODTest() = 0;
	virtual bool					SetupBones(matrix3x4_t* matBoneToWorldOut, int nMaxBones, int fBoneMask, float flCurrentTime) = 0;
	virtual void					SetupWeights(const matrix3x4_t* matBoneToWorld, int nFlexWeightCount, float* flFlexWeights, float* flFlexDelayedWeights) = 0;
	virtual void					DoAnimationEvents() = 0;
	virtual void*					GetPVSNotifyInterface() = 0;
	virtual void					GetRenderBounds(Vector& vecMins, Vector& vecMaxs) = 0;
	virtual void					GetRenderBoundsWorldspace(Vector& vecMins, Vector& vecMaxs) = 0;
	virtual void					GetShadowRenderBounds(Vector& vecMins, Vector& vecMaxs, int iShadowType) = 0;
	virtual bool					ShouldReceiveProjectedTextures(int nFlags) = 0;
	virtual bool					GetShadowCastDistance(float* pDistance, int iShadowType) const = 0;
	virtual bool					GetShadowCastDirection(Vector* vecDirection, int iShadowType) const = 0;
	virtual bool					IsShadowDirty() = 0;
	virtual void					MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable*		GetShadowParent() = 0;
	virtual IClientRenderable*		FirstShadowChild() = 0;
	virtual IClientRenderable*		NextShadowPeer() = 0;
	virtual int						ShadowCastType() = 0;
	virtual void					unused2() {}
	virtual void					CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t	GetModelInstance() = 0;
	virtual const matrix3x4_t&		RenderableToWorldTransform() = 0;
	virtual int						LookupAttachment(const char* pAttachmentName) = 0;
	virtual bool					GetAttachment(int nIndex, Vector& vecOrigin, QAngle& angView) = 0;
	virtual bool					GetAttachment(int nIndex, matrix3x4_t& matAttachment) = 0;
	virtual bool					ComputeLightingOrigin(int nAttachmentIndex, Vector vecModelLightingCenter, const matrix3x4_t& matrix, Vector& vecTransformedLightingCenter) = 0;
	virtual float*					GetRenderClipPlane() = 0;
	virtual int						GetSkin() = 0;
	virtual void					OnThreadedDrawSetup() = 0;
	virtual bool					UsesFlexDelayedWeights() = 0;
	virtual void					RecordToolMessage() = 0;
	virtual bool					ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual std::uint8_t			OverrideAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual std::uint8_t			OverrideShadowAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual void*					GetClientModelRenderable() = 0;
};

class CBaseClient;
class IClientNetworkable
{
public:
	virtual IClientUnknown*			GetIClientUnknown() = 0;
	virtual void					Release() = 0;
	virtual CBaseClient*			GetClientClass() = 0;
	virtual void					NotifyShouldTransmit(int iState) = 0;
	virtual void					OnPreDataChanged(EDataUpdateType updateType) = 0;
	virtual void					OnDataChanged(EDataUpdateType updateType) = 0;
	virtual void					PreDataUpdate(EDataUpdateType updateType) = 0;
	virtual void					PostDataUpdate(EDataUpdateType updateType) = 0;
	virtual void					OnDataUnchangedInPVS() = 0;
	virtual bool					IsDormant() const = 0;
	virtual int						GetIndex() const = 0;
	virtual void					ReceiveMessage(EClassIndex classIndex, bf_read& msg) = 0;
	virtual void*					GetDataTableBasePtr() = 0;
	virtual void					SetDestroyedOnRecreateEntities() = 0;
};

class CClientThinkHandle;
using ClientThinkHandle_t = CClientThinkHandle*;
class IClientThinkable
{
public:
	virtual IClientUnknown*			GetIClientUnknown() = 0;
	virtual void					ClientThink() = 0;
	virtual ClientThinkHandle_t		GetThinkHandle() = 0;
	virtual void					SetThinkHandle(ClientThinkHandle_t hThink) = 0;
	virtual void					Release() = 0;
};

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual const Vector&			GetAbsOrigin() const = 0;
	virtual const QAngle&			GetAbsAngles() const = 0;
	virtual void*					GetMouth() = 0;
	virtual bool					GetSoundSpatialization(struct SpatializationInfo_t& info) = 0;
	virtual bool					IsBlurred() = 0;

	void SetAbsOrigin(Vector vecOrigin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector&);
		static auto oSetAbsOrigin = reinterpret_cast<SetAbsOriginFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8")));
		oSetAbsOrigin(this, vecOrigin);
	}

	void SetAbsAngles(QAngle angView)
	{
		using SetAbsAngleFn = void(__thiscall*)(void*, const QAngle&);
		static auto oSetAbsAngles = reinterpret_cast<SetAbsAngleFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8")));
		oSetAbsAngles(this, angView);
	}

	DataMap_t* GetDataDescMap()
	{
		return MEM::CallVFunc<DataMap_t*>(this, 15);
	}

	DataMap_t* GetPredictionDescMap()
	{
		return MEM::CallVFunc<DataMap_t*>(this, 17);
	}
};

class CWeaponCSBase;
class CBaseEntity : public IClientEntity
{
public:
	#pragma region DT_BasePlayer
	N_ADD_PVARIABLE(float, GetFallVelocity, "CBasePlayer->m_flFallVelocity");
	N_ADD_VARIABLE(QAngle, GetViewPunch, "CBasePlayer->m_viewPunchAngle");
	N_ADD_VARIABLE(QAngle, GetPunch, "CBasePlayer->m_aimPunchAngle");
	N_ADD_VARIABLE(Vector, GetViewOffset, "CBasePlayer->m_vecViewOffset[0]");
	N_ADD_VARIABLE(float, GetFriction, "CBasePlayer->m_flFriction");
	N_ADD_VARIABLE(int, GetTickBase, "CBasePlayer->m_nTickBase");
	N_ADD_PVARIABLE(int, GetNextThinkTick, "CBasePlayer->m_nNextThinkTick");
	N_ADD_VARIABLE(Vector, GetVelocity, "CBasePlayer->m_vecVelocity[0]");
	N_ADD_PVARIABLE_OFFSET(CUserCmd*, GetCurrentCommand, "CBasePlayer->m_hConstraintEntity", -0xC); // @pattern = 89 BE ? ? ? ? E8 ? ? ? ? 85 FF + 0x2
	N_ADD_PVARIABLE_OFFSET(QAngle, GetViewAngles, "CBasePlayer->deadflag", 0x4);
	N_ADD_VARIABLE(CBaseHandle, GetGroundEntityHandle, "CBasePlayer->m_hGroundEntity");
	N_ADD_VARIABLE(int, GetHealth, "CBasePlayer->m_iHealth");
	N_ADD_VARIABLE(int, GetLifeState, "CBasePlayer->m_lifeState");
	N_ADD_VARIABLE(float, GetMaxSpeed, "CBasePlayer->m_flMaxspeed");
	N_ADD_VARIABLE(int, GetFlags, "CBasePlayer->m_fFlags");
	N_ADD_PVARIABLE(int, GetObserverMode, "CBasePlayer->m_iObserverMode");
	N_ADD_VARIABLE(CBaseHandle, GetObserverTargetHandle, "CBasePlayer->m_hObserverTarget");
	N_ADD_VARIABLE(CBaseHandle, GetViewModelHandle, "CBasePlayer->m_hViewModel[0]");
	N_ADD_PVARIABLE(const char, GetLastPlace, "CBasePlayer->m_szLastPlaceName");

	N_ADD_DATAFIELD(int, GetEFlags, this->GetPredictionDescMap(), "m_iEFlags");
	N_ADD_PDATAFIELD(int, GetButtons, this->GetPredictionDescMap(), "m_nButtons");
	N_ADD_DATAFIELD(int, GetButtonLast, this->GetPredictionDescMap(), "m_afButtonLast");
	N_ADD_DATAFIELD(int, GetButtonPressed, this->GetPredictionDescMap(), "m_afButtonPressed");
	N_ADD_DATAFIELD(int, GetButtonReleased, this->GetPredictionDescMap(), "m_afButtonReleased");
	N_ADD_PDATAFIELD(int, GetImpulse, this->GetPredictionDescMap(), "m_nImpulse");
	N_ADD_DATAFIELD(float, GetSurfaceFriction, this->GetPredictionDescMap(), "m_surfaceFriction");

	N_ADD_OFFSET(int, GetTakeDamage, 0x280); // @ida: 80 BE ? ? ? ? ? 75 46 8B 86 + 0x2
	N_ADD_OFFSET(CUserCmd, GetLastCommand, 0x3288);
	N_ADD_OFFSET(int, GetButtonDisabled, 0x3330);
	N_ADD_OFFSET(int, GetButtonForced, 0x3334);

	inline bool IsAlive()
	{
		/*
		 * @note: if u have problems with lifestate check (some servers has plugin for health restore and entity lifestate doesnt switch back to the alive)
		 * required to check for health and/or check lifestate where needed
		 * GetHealth() > 0
		 */

		return (this->GetLifeState() == LIFE_ALIVE) ? true : false;
	}
	#pragma endregion

	#pragma region DT_CSPlayer
	N_ADD_VARIABLE(int, GetShotsFired, "CCSPlayer->m_iShotsFired");
	N_ADD_VARIABLE(int, GetMoney, "CCSPlayer->m_iAccount");
	N_ADD_VARIABLE(int, GetTotalHits, "CCSPlayer->m_totalHitsOnServer");
	N_ADD_VARIABLE(int, GetArmor, "CCSPlayer->m_ArmorValue");
	N_ADD_VARIABLE(QAngle, GetEyeAngles, "CCSPlayer->m_angEyeAngles");
	N_ADD_VARIABLE(bool, IsDefusing, "CCSPlayer->m_bIsDefusing");
	N_ADD_VARIABLE(bool, IsScoped, "CCSPlayer->m_bIsScoped");
	N_ADD_VARIABLE(bool, IsGrabbingHostage, "CCSPlayer->m_bIsGrabbingHostage");
	N_ADD_VARIABLE(bool, IsRescuing, "CCSPlayer->m_bIsRescuing");
	N_ADD_VARIABLE(bool, HasHelmet, "CCSPlayer->m_bHasHelmet");
	N_ADD_VARIABLE(bool, HasHeavyArmor, "CCSPlayer->m_bHasHeavyArmor");
	N_ADD_VARIABLE(bool, HasDefuser, "CCSPlayer->m_bHasDefuser");
	N_ADD_VARIABLE(bool, HasImmunity, "CCSPlayer->m_bGunGameImmunity");
	N_ADD_VARIABLE(bool, IsInBuyZone, "CCSPlayer->m_bInBuyZone");
	N_ADD_PVARIABLE(float, GetFlashMaxAlpha, "CCSPlayer->m_flFlashMaxAlpha");
	N_ADD_VARIABLE_OFFSET(float, GetFlashAlpha, "CCSPlayer->m_flFlashMaxAlpha", -0x8);
	N_ADD_VARIABLE(float, GetFlashDuration, "CCSPlayer->m_flFlashDuration");
	N_ADD_VARIABLE_OFFSET(int, GetGlowIndex, "CCSPlayer->m_flFlashDuration", 0x18);
	N_ADD_VARIABLE(float, GetLowerBodyYaw, "CCSPlayer->m_flLowerBodyYawTarget");
	N_ADD_VARIABLE(int, GetSurvivalTeam, "CCSPlayer->m_nSurvivalTeam");
	N_ADD_VARIABLE_OFFSET(int, IsUsedNewAnimState, "CCSPlayer->m_flLastExoJumpTime", 0x8);
	#pragma endregion

	#pragma region DT_BaseEntity
	N_ADD_VARIABLE(float, GetAnimationTime, "CBaseEntity->m_flAnimTime");
	N_ADD_VARIABLE(float, GetSimulationTime, "CBaseEntity->m_flSimulationTime");
	N_ADD_VARIABLE_OFFSET(float, GetOldSimulationTime, "CBaseEntity->m_flSimulationTime", 0x4);
	N_ADD_VARIABLE(Vector, GetOrigin, "CBaseEntity->m_vecOrigin");
	N_ADD_VARIABLE(QAngle, GetRotation, "CBaseEntity->m_angRotation");
	N_ADD_VARIABLE(int, GetEffects, "CBaseEntity->m_fEffects");
	N_ADD_VARIABLE(int, GetTeam, "CBaseEntity->m_iTeamNum");
	N_ADD_VARIABLE(CBaseHandle, GetOwnerEntityHandle, "CBaseEntity->m_hOwnerEntity");
	N_ADD_PVARIABLE(ICollideable, GetCollision, "CBaseEntity->m_Collision");
	N_ADD_VARIABLE(int, GetCollisionGroup, "CBaseEntity->m_CollisionGroup");
	N_ADD_PVARIABLE(bool, IsSpotted, "CBaseEntity->m_bSpotted");

	N_ADD_DATAFIELD(QAngle, GetAbsRotation, this->GetDataDescMap(), "m_angAbsRotation");
	N_ADD_DATAFIELD(const matrix3x4_t, GetCoordinateFrame, this->GetDataDescMap(), "m_rgflCoordinateFrame");
	N_ADD_DATAFIELD(int, GetMoveType, this->GetPredictionDescMap(), "m_MoveType");

	N_ADD_OFFSET(float, GetSpawnTime, 0xA370); // @ida: 89 86 ? ? ? ? E8 ? ? ? ? 80 + 0x2
	#pragma endregion

	#pragma region DT_BaseCombatCharacter
	N_ADD_VARIABLE(float, GetNextAttack, "CBaseCombatCharacter->m_flNextAttack");
	N_ADD_VARIABLE(CBaseHandle, GetActiveWeaponHandle, "CBaseCombatCharacter->m_hActiveWeapon");
	N_ADD_PVARIABLE(CBaseHandle, GetWeaponsHandle, "CBaseCombatCharacter->m_hMyWeapons");
	N_ADD_PVARIABLE(CBaseHandle, GetWearablesHandle, "CBaseCombatCharacter->m_hMyWearables");
	#pragma endregion

	#pragma region DT_BaseAnimating
	N_ADD_VARIABLE(int, GetSequence, "CBaseAnimating->m_nSequence");
	N_ADD_PVARIABLE_OFFSET(CBoneAccessor, GetBoneAccessor, "CBaseAnimating->m_nForceBone", 0x1C);
	N_ADD_VARIABLE(int, GetHitboxSet, "CBaseAnimating->m_nHitboxSet");
	N_ADD_VARIABLE(bool, IsClientSideAnimation, "CBaseAnimating->m_bClientSideAnimation");
	N_ADD_VARIABLE(float, GetCycle, "CBaseAnimating->m_flCycle");

	N_ADD_OFFSET(int, GetAnimationOverlaysCount, 0x298C);

	[[nodiscard]] std::array<float, MAXSTUDIOPOSEPARAM>& GetPoseParameter()
	{
		static std::uintptr_t m_flPoseParameter = CNetvarManager::Get().mapProps[FNV1A::HashConst("CBaseAnimating->m_flPoseParameter")].uOffset;
		return *reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(reinterpret_cast<std::uintptr_t>(this) + m_flPoseParameter);
	}

	inline void SetPoseAngles(float flYaw, float flPitch)
	{
		auto& arrPose = this->GetPoseParameter();
		arrPose.at(11U) = (flPitch + 90.f) / 180.f;
		arrPose.at(2U) = (flYaw + 180.f) / 360.f;
	}

	[[nodiscard]] CAnimationLayer* GetAnimationOverlays()
	{
		// @ida = 8B 89 ? ? ? ? 8D 0C D1 + 0x2
		return *reinterpret_cast<CAnimationLayer**>(reinterpret_cast<std::uintptr_t>(this) + 0x2980);
	}

	[[nodiscard]] inline CAnimationLayer* GetAnimationLayer(int nLayer)
	{
		if (nLayer >= 0 && nLayer < MAXOVERLAYS)
			return &GetAnimationOverlays()[nLayer];

		return nullptr;
	}

	[[nodiscard]] CCSGOPlayerAnimState* GetAnimationState()
	{
		// @ida: 8B 8E ? ? ? ? F3 0F 10 48 ? E8 ? ? ? ? C7 + 0x2
		return *reinterpret_cast<CCSGOPlayerAnimState**>(reinterpret_cast<std::uintptr_t>(this) + 0x3914);
	}
	#pragma endregion

	// DoExtraBonesProcessing
	// pattern @xref: "ankle_L"
	// index @xref: "SetupBones: invalid bone array size (%d - needs %d)\n"

	const char* GetClassname()
	{
		// @ida: 8B 01 FF 90 ? ? ? ? 90 + 0x4
		return MEM::CallVFunc<const char*>(this, 59);
	}

	int IsMaxHealth()
	{
		// @ida: FF 90 ? ? ? ? 85 C0 0F 8F ? ? ? ? 80 + 0x2
		return MEM::CallVFunc<int>(this, 122);
	}

	void Think()
	{
		MEM::CallVFunc<void>(this, 138);
	}

	unsigned int PhysicsSolidMaskForEntity()
	{
		// @xref: "func_breakable", "func_breakable_surf"
		return MEM::CallVFunc<unsigned int>(this, 151);
	}

	bool IsPlayer()
	{
		// @xref: "effects/nightvision"
		return MEM::CallVFunc<bool>(this, 157);
	}

	[[nodiscard]] Vector GetEyePosition(bool bShouldCorrect = true)
	{
		Vector vecPosition = { };

		// get eye position
		MEM::CallVFunc<void, Vector&>(this, 168, vecPosition);

		// correct this like it do weapon_shootpos
		// @ida weapon_shootpos: 55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90
		if (IsUsedNewAnimState() && bShouldCorrect)
		{
			CCSGOPlayerAnimState* pAnimState = this->GetAnimationState();

			if (pAnimState != nullptr)
				ModifyEyePosition(pAnimState, &vecPosition);
		}

		// return corrected position
		return vecPosition;
	}

	void SetSequence(int iSequence)
	{
		MEM::CallVFunc<void>(this, 218, iSequence);
	}

	void StudioFrameAdvance()
	{
		MEM::CallVFunc<void>(this, 219);
	}

	void UpdateClientSideAnimations()
	{
		// @xref: "UpdateClientSideAnimations"
		// @ida updateclientsideanimations: 55 8B EC 51 56 8B F1 80 BE ? ? ? ? 00 74 ? 8B 06 FF
		MEM::CallVFunc<void>(this, 223);
	}

	void PreThink()
	{
		MEM::CallVFunc<void>(this, 317);
	}

	void UpdateCollisionBounds()
	{
		MEM::CallVFunc<void>(this, 339);
	}

	bool PhysicsRunThink(int nThinkMethod)
	{
		// @xref: from sub with "CLIENT:  %s(%s) thinking for %.02f ms!!!\n"
		using PhysicsRunThinkFn = bool(__thiscall*)(void*, int);
		static auto oPhysicsRunThink = reinterpret_cast<PhysicsRunThinkFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87")));
		return oPhysicsRunThink(this, nThinkMethod);
	}

	static CBaseEntity*		GetLocalPlayer();
	int						GetSequenceActivity(int iSequence);
	CBaseCombatWeapon*		GetWeapon();
	int						GetMaxHealth();
	std::optional<Vector>	GetBonePosition(int iBone);
	int						GetBoneByHash(const FNV1A_t uBoneHash) const;
	std::optional<Vector>	GetHitboxPosition(int iHitbox);
	std::optional<Vector>	GetHitGroupPosition(int iHitGroup);
	void					ModifyEyePosition(CCSGOPlayerAnimState* pAnimState, Vector* vecPosition) const;
	void					PostThink();
	bool					IsEnemy(CBaseEntity* pEntity);
	bool					IsTargetingLocal(CBaseEntity* pLocal);
	bool					CanShoot(CWeaponCSBase* pBaseWeapon);
	bool					IsVisible(CBaseEntity* pEntity, const Vector& vecSpot, bool bSmokeCheck = false);
};

class CCSWeaponData
{
public:
	std::byte	pad0[0x14];				// 0x0000
	int			iMaxClip1;				// 0x0014
	int			iMaxClip2;				// 0x0018
	int			iDefaultClip1;			// 0x001C
	int			iDefaultClip2;			// 0x0020
	int			iPrimaryMaxReserveAmmo; // 0x0024
	int			iSecondaryMaxReserveAmmo; // 0x0028
	char*		szWorldModel;			// 0x002C
	char*		szViewModel;			// 0x0030
	char*		szDroppedModel;			// 0x0034
	std::byte	pad1[0x50];				// 0x0038
	char*		szHudName;				// 0x0088
	char*		szWeaponName;			// 0x008C
	std::byte	pad2[0x2];				// 0x0090
	bool		bIsMeleeWeapon;			// 0x0092
	std::byte	pad3[0x9];				// 0x0093
	float		flWeaponWeight;			// 0x009C
	std::byte	pad4[0x28];				// 0x00A0
	int			nWeaponType;			// 0x00C8
	std::byte	pad5[0x4];				// 0x00CC
	int			iWeaponPrice;			// 0x00D0
	int			iKillAward;				// 0x00D4
	std::byte	pad6[0x4];				// 0x00D8
	float		flCycleTime;			// 0x00DC
	float		flCycleTimeAlt;			// 0x00E0
	std::byte	pad7[0x8];				// 0x00E4
	bool		bFullAuto;				// 0x00EC
	std::byte	pad8[0x3];				// 0x00ED
	int			iDamage;				// 0x00F0
	float		flArmorRatio;			// 0x00F4
	int			iBullets;				// 0x00F8
	float		flPenetration;			// 0x00FC
	std::byte	pad9[0x8];				// 0x0100
	float		flRange;				// 0x0108
	float		flRangeModifier;		// 0x010C
	float		flThrowVelocity;		// 0x0110
	std::byte	pad10[0xC];				// 0x0114
	bool		bHasSilencer;			// 0x0120
	std::byte	pad11[0xB];				// 0x0121
	char*		szBulletType;			// 0x012C
	float		flMaxSpeed;				// 0x0130
	float		flMaxSpeedAlt;			// 0x0134
	float		flSpread;				// 0x0138
	float		flSpreadAlt;			// 0x013C
	std::byte	pad12[0x44];			// 0x0180
	int			iRecoilSeed;			// 0x0184
	std::byte	pad13[0x20];			// 0x0188

	bool IsGun()
	{
		switch (this->nWeaponType)
		{
		case WEAPONTYPE_PISTOL:
		case WEAPONTYPE_SUBMACHINEGUN:
		case WEAPONTYPE_RIFLE:
		case WEAPONTYPE_SHOTGUN:
		case WEAPONTYPE_SNIPER:
		case WEAPONTYPE_MACHINEGUN:
			return true;
		}

		return false;
	}
};

class IRefCounted;
class CEconItemView
{
public:
	N_ADD_OFFSET(CUtlVector<IRefCounted*>, GetCustomMaterials, 0x14);
	// @ida: "8B 7C 24 34 81 C7 ? ? ? ?" + 0x2
	// @xref: "Original material not found! Name: %s"
	N_ADD_OFFSET(CUtlVector<IRefCounted*>, GetVisualsDataProcessors, 0x230);
};

class CBaseCombatWeapon : public IClientEntity
{
public:
	#pragma region DT_BaseCombatWeapon
	N_ADD_VARIABLE(float, GetNextPrimaryAttack, "CBaseCombatWeapon->m_flNextPrimaryAttack");
	N_ADD_VARIABLE(float, GetNextSecondaryAttack, "CBaseCombatWeapon->m_flNextSecondaryAttack");
	N_ADD_VARIABLE(int, GetAmmo, "CBaseCombatWeapon->m_iClip1");
	N_ADD_VARIABLE(int, GetAmmoReserve, "CBaseCombatWeapon->m_iPrimaryReserveAmmoCount");
	N_ADD_VARIABLE(int, GetViewModelIndex, "CBaseCombatWeapon->m_iViewModelIndex");
	N_ADD_VARIABLE(int, GetWorldModelIndex, "CBaseCombatWeapon->m_iWorldModelIndex");
	N_ADD_VARIABLE(CBaseHandle, GetWorldModelHandle, "CBaseCombatWeapon->m_hWeaponWorldModel");

	N_ADD_DATAFIELD(bool, IsReloading, this->GetPredictionDescMap(), "m_bInReload");
	#pragma endregion

	#pragma region DT_BaseAttributableItem
	N_ADD_VARIABLE(short, GetItemDefinitionIndex, "CBaseAttributableItem->m_iItemDefinitionIndex");
	N_ADD_VARIABLE(int, GetItemIDHigh, "CBaseAttributableItem->m_iItemIDHigh");
	N_ADD_VARIABLE(int, GetItemIDLow, "CBaseAttributableItem->m_iItemIDLow");
	N_ADD_VARIABLE(int, GetAccountID, "CBaseAttributableItem->m_iAccountID");
	N_ADD_VARIABLE(int, GetEntityQuality, "CBaseAttributableItem->m_iEntityQuality");
	N_ADD_PVARIABLE(char, GetCustomName, "CBaseAttributableItem->m_szCustomName");
	N_ADD_VARIABLE(int, GetOwnerXuidLow, "CBaseAttributableItem->m_OriginalOwnerXuidLow");
	N_ADD_VARIABLE(int, GetOwnerXuidHigh, "CBaseAttributableItem->m_OriginalOwnerXuidHigh");
	N_ADD_VARIABLE(int, GetFallbackPaintKit, "CBaseAttributableItem->m_nFallbackPaintKit");
	N_ADD_VARIABLE(int, GetFallbackSeed, "CBaseAttributableItem->m_nFallbackSeed");
	N_ADD_VARIABLE(float, GetFallbackWear, "CBaseAttributableItem->m_flFallbackWear");
	N_ADD_VARIABLE(int, GetFallbackStatTrak, "CBaseAttributableItem->m_nFallbackStatTrak");
	N_ADD_PVARIABLE(CEconItemView, GetEconItemView, "CBaseAttributableItem->m_Item");
	#pragma endregion

	void SetModelIndex(int nModelIndex)
	{
		MEM::CallVFunc<void>(this, 75, nModelIndex);
	}

	bool IsWeapon()
	{
		return MEM::CallVFunc<bool>(this, 165);
	}

	[[nodiscard]] float GetSpread()
	{
		return MEM::CallVFunc<float>(this, 452);
	}

	[[nodiscard]] float GetInaccuracy()
	{
		return MEM::CallVFunc<float>(this, 482);
	}

	void UpdateAccuracyPenalty()
	{
		MEM::CallVFunc<void>(this, 483);
	}
};

class CTEFireBullets
{
public:
	std::byte	pad0[0x10];				//0x0000
	int			nPlayer;				//0x0010
	int			nItemDefinitionIndex;	//0x0014
	Vector		vecOrigin;				//0x0018
	Vector		vecAngles;				//0x0024
	int			iWeapon;				//0x0030
	int			nWeaponID;				//0x0034
	int			iMode;					//0x0038
	int			nSeed;					//0x003C
	float		flInaccuracy;			//0x0040
	float		flRecoilIndex;			//0x0044
	float		flSpread;				//0x0048
	int			nSoundType;				//0x004C
}; // Size: 0x0050

class CWeaponCSBase : public CBaseCombatWeapon
{
public:
	#pragma region DT_WeaponCSBaseGun
	N_ADD_VARIABLE(int, GetZoomLevel, "CWeaponCSBaseGun->m_zoomLevel");
	N_ADD_VARIABLE(int, GetBurstShotsRemaining, "CWeaponCSBaseGun->m_iBurstShotsRemaining");
	#pragma endregion

	#pragma region DT_WeaponCSBase
	N_ADD_VARIABLE(bool, IsBurstMode, "CWeaponCSBase->m_bBurstMode");
	N_ADD_VARIABLE(float, GetAccuracyPenalty, "CWeaponCSBase->m_fAccuracyPenalty");
	N_ADD_VARIABLE(float, GetFireReadyTime, "CWeaponCSBase->m_flPostponeFireReadyTime");
	#pragma endregion

	CUtlVector<IRefCounted*>& GetCustomMaterials()
	{
		static auto uAddress = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(CLIENT_DLL, XorStr("83 BE ? ? ? ? ? 7F 67")) + 0x2) - 0xC;
		return *reinterpret_cast<CUtlVector<IRefCounted*>*>(reinterpret_cast<std::uintptr_t>(this) + uAddress);
	}

	bool& IsCustomMaterialInitialized()
	{
		static auto uAddress = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(CLIENT_DLL, XorStr("C6 86 ? ? ? ? ? FF 50 04")) + 0x2);
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + uAddress);
	}
};

class CBaseCSGrenade : public CWeaponCSBase
{
public:
	#pragma region DT_BaseCSGrenade
	N_ADD_VARIABLE(bool, IsPinPulled, "CBaseCSGrenade->m_bPinPulled");
	N_ADD_VARIABLE(float, GetThrowTime, "CBaseCSGrenade->m_fThrowTime");
	N_ADD_VARIABLE(float, GetThrowStrength, "CBaseCSGrenade->m_flThrowStrength");
	#pragma endregion

	#pragma region DT_BaseCSGrenadeProjectile
	N_ADD_VARIABLE(int, GetEffectTickBegin, "CBaseCSGrenadeProjectile->m_nExplodeEffectTickBegin");
	#pragma endregion
};

class CSmokeGrenade
{
public:
	#pragma region DT_SmokeGrenadeProjectile
	N_ADD_VARIABLE(int, GetEffectTickBegin, "CSmokeGrenadeProjectile->m_nSmokeEffectTickBegin");
	#pragma endregion

	inline float GetMaxTime()
	{
		return 18.f;
	}
};

class CInferno
{
public:
	#pragma region DT_Inferno
	N_ADD_VARIABLE(int, GetEffectTickBegin, "CInferno->m_nFireEffectTickBegin");
	#pragma endregion

	inline float GetMaxTime()
	{
		// @todo: get with inferno_flame_lifetime convar
		return 7.f;
	}
};

class CPlantedC4
{
public:
	#pragma region DT_PlantedC4
	N_ADD_VARIABLE(float, GetTimerLength, "CPlantedC4->m_flTimerLength");
	N_ADD_VARIABLE(float, GetDefuseLength, "CPlantedC4->m_flDefuseLength");
	N_ADD_VARIABLE(bool, IsPlanted, "CPlantedC4->m_bBombTicking");
	N_ADD_VARIABLE(CBaseHandle, GetDefuserHandle, "CPlantedC4->m_hBombDefuser");
	N_ADD_VARIABLE(bool, IsDefused, "CPlantedC4->m_bBombDefused");

	inline float GetTimer(float flServerTime)
	{
		static std::uintptr_t m_flC4Blow = CNetvarManager::Get().mapProps[FNV1A::HashConst("CPlantedC4->m_flC4Blow")].uOffset;
		const float flTimer = *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flC4Blow) - flServerTime;
		return std::max(0.f, flTimer);
	}

	inline float GetDefuseTimer(float flServerTime)
	{
		static std::uintptr_t m_flDefuseCountDown = CNetvarManager::Get().mapProps[FNV1A::HashConst("CPlantedC4->m_flDefuseCountDown")].uOffset;
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flDefuseCountDown) - flServerTime;
	}
	#pragma endregion
};

class CBaseViewModel
{
public:
	#pragma region DT_BaseViewModel
	N_ADD_VARIABLE(int, GetModelIndex, "CBaseViewModel->m_nModelIndex");
	N_ADD_VARIABLE(CBaseHandle, GetOwnerHandle, "CBaseViewModel->m_hOwner");
	N_ADD_VARIABLE(CBaseHandle, GetWeaponHandle, "CBaseViewModel->m_hWeapon");
	#pragma endregion

	void SendViewModelMatchingSequence(int nSequence)
	{
		MEM::CallVFunc<void>(this, 246, nSequence);
	}

	void SetWeaponModel(const char* szFileName, CBaseCombatWeapon* pWeapon)
	{
		// @ida setweaponmodel: 57 8B F9 8B 97 ? ? ? ? 83 FA FF 74 6A
		MEM::CallVFunc<void>(this, 247, szFileName, pWeapon);
	}
};

class CEnvTonemapController
{
public:
	#pragma region DT_EnvTonemapController
	N_ADD_VARIABLE(bool, IsUseCustomAutoExposureMin, "CEnvTonemapController->m_bUseCustomAutoExposureMin");
	N_ADD_VARIABLE(bool, IsUseCustomAutoExposureMax, "CEnvTonemapController->m_bUseCustomAutoExposureMax");
	N_ADD_VARIABLE(bool, IsUseCustomBloomScale, "CEnvTonemapController->m_bUseCustomBloomScale");
	N_ADD_VARIABLE(float, GetCustomAutoExposureMin, "CEnvTonemapController->m_flCustomAutoExposureMin");
	N_ADD_VARIABLE(float, GetCustomAutoExposureMax, "CEnvTonemapController->m_flCustomAutoExposureMax");
	N_ADD_VARIABLE(float, GetCustomBloomScale, "CEnvTonemapController->m_flCustomBloomScale");
	N_ADD_VARIABLE(float, GetCustomBloomScaleMin, "CEnvTonemapController->m_flCustomBloomScaleMinimum");
	N_ADD_VARIABLE(float, GetBloomExponent, "CEnvTonemapController->m_flBloomExponent");
	N_ADD_VARIABLE(float, GetBloomSaturation, "CEnvTonemapController->m_flBloomSaturation");
	#pragma endregion
};

class CBreakableSurface : public CBaseEntity, public IBreakableWithPropData
{
public:
	#pragma region DT_BreakableSurface
	N_ADD_VARIABLE(bool, IsBroken, "CBreakableSurface->m_bIsBroken");
	#pragma endregion
};
