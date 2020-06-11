#pragma once
// used: std::array
#include <array>
// used: std::vector
#include <vector>

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

enum EItemDefinitionIndex
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
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T = 5028,
	GLOVE_CT = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
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
	WEAPONTYPE_HEALTHSHOT = 11
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
	virtual void					Release() override = 0;
	virtual const Vector&			GetAbsOrigin() const = 0; // can be used, i have reached the correctness
	virtual const QAngle&			GetAbsAngles() const = 0; // can be used, i have reached the correctness
	virtual void*					GetMouth() = 0;
	virtual bool					GetSoundSpatialization(struct SpatializationInfo_t& info) = 0;
	virtual bool					IsBlurred() = 0;

	void SetAbsOrigin(Vector vecOrigin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector&);
		static auto oSetAbsOrigin = (SetAbsOriginFn)MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		oSetAbsOrigin(this, vecOrigin);
	}

	void SetAbsAngles(QAngle angView)
	{
		using SetAbsAngleFn = void(__thiscall*)(void*, const QAngle&);
		static auto oSetAbsAngles = (SetAbsAngleFn)MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
		oSetAbsAngles(this, angView);
	}

	DataMap_t* GetDataDescMap()
	{
		return MEM::CallVFunc<DataMap_t*>(this, 15);
	}

	DataMap_t* GetPredDescMap()
	{
		return MEM::CallVFunc<DataMap_t*>(this, 17);
	}
};

/* base viewmodel data table class */
class CBaseViewModel
{
public:
	int* GetModelIndex()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().nModelIndex);
	}

	void SetModelIndex(int nModelIndex)
	{
		MEM::CallVFunc<void>(this, 75, nModelIndex);
	}

	CBaseHandle GetOwner()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hOwner);
	}

	CBaseHandle GetWeaponHandle()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hWeapon);
	}

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

class CBaseEntity : public IClientEntity, public CBaseViewModel
{
public:
	/* DT_BasePlayer */

	int* GetButtons()
	{
		static std::uintptr_t m_nButtons = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_nButtons"));
		return (int*)((std::uintptr_t)this + m_nButtons);
	}

	int& GetButtonLast()
	{
		static std::uintptr_t m_afButtonLast = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_afButtonLast"));
		return *(int*)((std::uintptr_t)this + m_afButtonLast);
	}

	int& GetButtonPressed()
	{
		static std::uintptr_t m_afButtonPressed = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_afButtonPressed"));
		return *(int*)((std::uintptr_t)this + m_afButtonPressed);
	}

	int& GetButtonReleased()
	{
		static std::uintptr_t m_afButtonReleased = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_afButtonReleased"));
		return *(int*)((std::uintptr_t)this + m_afButtonReleased);
	}

	int GetButtonDisabled()
	{
		return *(int*)((std::uintptr_t)this + 0x3330);
	}

	int GetButtonForced()
	{
		return *(int*)((std::uintptr_t)this + 0x3334);
	}

	float* GetFallVelocity()
	{
		return (float*)((std::uintptr_t)this + CNetvarManager::Get().flFallVelocity);
	}

	QAngle& GetViewPunch()
	{
		return *(QAngle*)((std::uintptr_t)this + CNetvarManager::Get().viewPunchAngle);
	}

	QAngle& GetPunch()
	{
		return *(QAngle*)((std::uintptr_t)this + CNetvarManager::Get().aimPunchAngle);
	}

	Vector& GetViewOffset()
	{
		return *(Vector*)((std::uintptr_t)this + CNetvarManager::Get().vecViewOffset);
	}

	int* GetImpulse()
	{
		static std::uintptr_t m_nImpulse = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_nImpulse"));
		return (int*)((std::uintptr_t)this + m_nImpulse);
	}

	int& GetTickBase()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().nTickBase);
	}

	int* GetNextThinkTick()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().nNextThinkTick);
	}

	Vector GetVelocity()
	{
		return *(Vector*)((std::uintptr_t)this + CNetvarManager::Get().vecVelocity);
	}

	CUserCmd** GetCurrentCommand()
	{
		// @pattern = 89 BE ? ? ? ? E8 ? ? ? ? 85 FF + 0x2
		static std::uintptr_t m_pCurrentCommand = CNetvarManager::Get().hConstraintEntity - 0xC;
		return (CUserCmd**)((std::uintptr_t)this + m_pCurrentCommand);
	}

	CUserCmd& GetLastCommand()
	{
		return *(CUserCmd*)((std::uintptr_t)this + 0x3288);
	}

	QAngle* GetThirdPersonAngles()
	{
		return (QAngle*)((std::uintptr_t)this + CNetvarManager::Get().deadflag + 0x4);
	}

	CBaseHandle GetGroundEntity()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hGroundEntity);
	}

	int GetHealth()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iHealth);
	}

	int GetLifeState()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().lifeState);
	}

	float GetMaxSpeed()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flMaxspeed);
	}

	int GetFlags()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().fFlags);
	}

	EObserverMode* GetObserverMode()
	{
		return (EObserverMode*)((std::uintptr_t)this + CNetvarManager::Get().iObserverMode);
	}

	CBaseHandle GetObserverTarget()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hObserverTarget);
	}

	CBaseHandle GetViewModel()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hViewModel);
	}

	const char* GetLastPlace()
	{
		return (const char*)((std::uintptr_t)this + CNetvarManager::Get().szLastPlaceName);
	}

	/* DT_CSPlayer */

	int GetShotsFired()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iShotsFired);
	}

	int GetMoney()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iAccount);
	}

	int GetTotalHits()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().totalHitsOnServer);
	}

	int GetArmor()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().ArmorValue);
	}

	QAngle& GetEyeAngles()
	{
		return *(QAngle*)((std::uintptr_t)this + CNetvarManager::Get().angEyeAngles);
	}

	int GetSurvivalTeam()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().nSurvivalTeam);
	}

	float GetSpawnTime()
	{
		// @ida: 89 86 ? ? ? ? E8 ? ? ? ? 80 + 0x2
		return *(float*)((std::uintptr_t)this + 0xA370);
	}

	#pragma region baseentity_checks
	bool IsDefusing()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bIsDefusing);
	}

	bool IsScoped()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bIsScoped);
	}

	bool IsGrabbingHostage()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bIsGrabbingHostage);
	}

	bool IsRescuing()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bIsRescuing);
	}

	bool HasHelmet()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bHasHelmet);
	}

	bool HasHeavyArmor()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bHasHeavyArmor);
	}

	bool HasDefuser()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bHasDefuser);
	}

	bool HasImmunity()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bGunGameImmunity);
	}
	#pragma endregion

	float GetFriction()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flFriction);
	}

	float GetSurfaceFriction()
	{
		static std::uintptr_t m_surfaceFriction = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_surfaceFriction"));
		return *(float*)((std::uintptr_t)this + m_surfaceFriction);
	}

	int& GetTakeDamage()
	{
		return *(int*)((std::uintptr_t)this + 0x280);
	}

	float GetFlashAlpha()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flFlashMaxAlpha - 0x8);
	}

	float* GetFlashMaxAlpha()
	{
		return (float*)((std::uintptr_t)this + CNetvarManager::Get().flFlashMaxAlpha);
	}

	float GetFlashDuration()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flFlashDuration);
	}

	int GetGlowIndex()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iGlowIndex);
	}

	float GetLowerBodyYaw()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flLowerBodyYawTarget);
	}

	/* DT_BaseEntity */

	float GetAnimationTime()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flAnimTime);
	}

	float GetSimulationTime()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flSimulationTime);
	}

	float GetOldSimulationTime()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flSimulationTime + 0x4);
	}

	Vector GetOrigin()
	{
		return *(Vector*)((std::uintptr_t)this + CNetvarManager::Get().vecOrigin);
	}

	QAngle GetRotation()
	{
		return *(QAngle*)((std::uintptr_t)this + CNetvarManager::Get().angRotation);
	}

	QAngle GetAbsRotation()
	{
		static std::uintptr_t m_angAbsRotation = CNetvarManager::Get().FindInDataMap(this->GetDataDescMap(), FNV1A::HashConst("m_angAbsRotation"));
		return *(QAngle*)((std::uintptr_t)this + m_angAbsRotation);
	}

	EMoveType GetMoveType()
	{
		static std::uintptr_t m_MoveType = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_MoveType"));
		return *(EMoveType*)((std::uintptr_t)this + m_MoveType);
	}

	int GetTeam()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iTeamNum);
	}

	CBaseHandle GetOwnerEntity()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hOwnerEntity);
	}

	ICollideable* GetCollideable()
	{
		return (ICollideable*)((std::uintptr_t)this + CNetvarManager::Get().Collision);
	}

	int GetCollisionGroup()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().CollisionGroup);
	}

	const matrix3x4_t& GetCoordinateFrame()
	{
		static std::uintptr_t m_rgflCoordinateFrame = CNetvarManager::Get().FindInDataMap(this->GetDataDescMap(), FNV1A::HashConst("m_rgflCoordinateFrame"));
		return *(const matrix3x4_t*)((std::uintptr_t)this + m_rgflCoordinateFrame);
	}

	bool* GetSpotted()
	{
		return (bool*)((std::uintptr_t)this + CNetvarManager::Get().bSpotted);
	}

	/* DT_BCCLocalPlayerExclusive */

	float GetNextAttack()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flNextAttack);
	}

	/* DT_BaseCombatCharacter */

	CBaseHandle GetActiveWeapon()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hActiveWeapon);
	}

	CBaseHandle* GetWeapons()
	{
		return (CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hMyWeapons);
	}

	CBaseHandle* GetWearables()
	{
		return (CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hMyWearables);
	}

	/* DT_BaseAnimating */

	int GetSequence()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().nSequence);
	}

	int GetHitboxSet()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().nHitboxSet);
	}

	float GetCycle()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flCycle);
	}

	std::array<float, 24U>& GetPoseParameter()
	{
		return *(std::array<float, 24U>*)((std::uintptr_t)this + CNetvarManager::Get().flPoseParameter);
	}

	bool* IsClientSideAnimation()
	{
		return (bool*)((std::uintptr_t)this + CNetvarManager::Get().bClientSideAnimation);
	}

	CAnimationLayer* GetAnimationLayers()
	{
		// @ida = 8B 89 ? ? ? ? 8D 0C D1 + 0x2
		return *(CAnimationLayer**)((std::uintptr_t)this + 0x2980);
	}

	int GetAnimationOverlaysCount()
	{
		return *(int*)((std::uintptr_t)this + 0x298C);
	}

	CBasePlayerAnimState* GetAnimationState()
	{
		// @ida: 8B 8E ? ? ? ? F3 0F 10 48 ? E8 ? ? ? ? C7 + 0x2
		return (CBasePlayerAnimState*)((std::uintptr_t)this + 0x3914);
	}

	#pragma region baseentity_exports
	// DoExtraBonesProcessing
	// pattern xref: "ankle_L"
	// index xref: "SetupBones: invalid bone array size (%d - needs %d)\n"

	void Think()
	{
		MEM::CallVFunc<void>(this, 138);
	}

	bool IsPlayer()
	{
		// @xref: "effects/nightvision"
		return MEM::CallVFunc<bool>(this, 157);
	}

	Vector GetEyePosition()
	{
		Vector vecPosition = { };

		// get eye position
		MEM::CallVFunc<void, Vector&>(this, 168, vecPosition);

		// correct this like it do weapon_shootpos
		// @ida weapon_shootpos: 55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90
		if (*(int*)((std::uintptr_t)this + 0x3AB4))
		{
			CBasePlayerAnimState* pAnimState = this->GetAnimationState();

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
		// @pattern = 55 8B EC 51 56 8B F1 80 BE ? ? ? ? 00 74 ? 8B 06 FF
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
		using PhysicsRunThinkFn = bool(__thiscall*)(void*, int);
		static auto oPhysicsRunThink = (PhysicsRunThinkFn)MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87"));
		return oPhysicsRunThink(this, nThinkMethod);
	}

	int GetSequenceActivity(int iSequence);
	#pragma endregion

	#pragma region baseentity_automatics
	inline bool IsAlive()
	{
		/*
		 * @note: if u have problems with lifestate check (some servers has plugin for health restore and entity lifestate doesnt switch back to the alive)
		 * required to check for health and/or check lifestate where needed
		 * GetHealth() > 0
		 */

		return (this->GetLifeState() == LIFE_ALIVE) ? true : false;
	}

	inline void SetPoseAngles(float flYaw, float flPitch)
	{
		auto& arrPose = this->GetPoseParameter();
		arrPose.at(11U) = (flPitch + 90.f) / 180.f;
		arrPose.at(2U) = (flYaw + 180.f) / 360.f;
	}

	inline CAnimationLayer* GetAnimationLayer(int nLayer)
	{
		if (nLayer >= 0 && nLayer < MAX_LAYER_RECORDS)
			return &GetAnimationLayers()[nLayer];

		return nullptr;
	}

	CBaseCombatWeapon*	GetWeapon();
	int					GetMaxHealth();
	matrix3x4_t*		GetBoneMatrix(bool bSingleBone = false, int iSingleBone = BONE_INVALID);
	Vector				GetBonePosition(int iBone);
	int					GetBoneByHash(const FNV1A_t uBoneHash);
	Vector				GetHitboxPosition(int iHitbox);
	Vector				GetHitGroupPosition(int iHitGroup);
	void				ModifyEyePosition(CBasePlayerAnimState* pAnimState, Vector* vecPosition);
	int					PostThink();
	bool				IsEnemy(CBaseEntity* pEntity);
	bool				IsTargetingLocal(CBaseEntity* pLocal);
	bool				IsCanShoot(CBaseCombatWeapon* pWeapon);
	bool				IsVisible(CBaseEntity* pEntity, const Vector& vecSpot, bool bSmokeCheck = false);
#pragma endregion
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
	std::byte					pad0[0x4];					//0x0000
	CUtlVector<IRefCounted*>	vecCustomMaterials;			//0x0004
	std::byte					pad1[0x208];				//0x0018
	CUtlVector<IRefCounted*>	vecVisualsDataProcessors;	//0x0220
}; // Size: 0x0234

class CAttributeManager
{
public:
	std::byte		pad0[0x18];					//0x0000
	int				iReapplyProvisionParity;	//0x0018
	std::uintptr_t	hOuter;						//0x001C
	std::byte		pad1[0x4];					//0x0020
	int				iProviderType;				//0x0024
	std::byte		pad2[0x18];					//0x0028
	CEconItemView	Item;						//0x0040
}; // Size: 0x0274

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
};

class CWeaponCSBase
{
public:
	std::byte					pad0[0x9CC];				//0x0000
	CUtlVector<IRefCounted*>	vecCustomMaterials;			//0x09DC
	std::byte					pad1[0x23A0];				//0x09EE
	CAttributeManager			AttributeManager;			//0x2D80
	std::byte					pad2[0x32C];				//0x3004
	bool						bCustomMaterialInitialized; //0x3330

	int	GetZoomLevel()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().zoomLevel);
	}

	bool GetBurstMode()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bBurstMode);
	}

	int GetBurstShotsRemaining()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iBurstShotsRemaining);
	}

	float& GetAccuracyPenalty()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().fAccuracyPenalty);
	}

	float GetFireReadyTime()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flPostponeFireReadyTime);
	}
}; // Size: 0x3331

class CBaseAttributableItem
{
public:
	int* GetItemIDHigh()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().iItemIDHigh);
	}

	int* GetItemIDLow()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().iItemIDLow);
	}

	int* GetAccountID()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().iAccountID);
	}

	int* GetEntityQuality()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().iEntityQuality);
	}

	char* GetCustomName()
	{
		return (char*)((std::uintptr_t)this + CNetvarManager::Get().szCustomName);
	}

	int* GetOwnerXuidLow()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().OriginalOwnerXuidLow);
	}

	int* GetOwnerXuidHigh()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().OriginalOwnerXuidHigh);
	}

	int* GetFallbackPaintKit()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().nFallbackPaintKit);
	}

	int* GetFallbackSeed()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().nFallbackSeed);
	}

	float* GetFallbackWear()
	{
		return (float*)((std::uintptr_t)this + CNetvarManager::Get().flFallbackWear);
	}

	int* GetFallbackStatTrak()
	{
		return (int*)((std::uintptr_t)this + CNetvarManager::Get().nFallbackStatTrak);
	}
};

class CBaseCombatWeapon : public IClientEntity, public CWeaponCSBase, public CBaseViewModel, public CBaseAttributableItem
{
public:
	short* GetItemDefinitionIndex()
	{
		return (short*)((std::uintptr_t)this + CNetvarManager::Get().iItemDefinitionIndex);
	}

	float GetNextPrimaryAttack()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flNextPrimaryAttack);
	}

	float GetNextSecondaryAttack()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flNextSecondaryAttack);
	}

	int GetAmmo()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iClip1);
	}

	int GetAmmoReserve()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iPrimaryReserveAmmoCount);
	}

	int GetViewModelIndex()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iViewModelIndex);
	}

	int GetWorldModelIndex()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().iWorldModelIndex);
	}

	CBaseHandle GetWeaponWorldModel()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hWeaponWorldModel);
	}

	bool IsReloading()
	{
		static std::uintptr_t m_bInReload = CNetvarManager::Get().FindInDataMap(this->GetPredDescMap(), FNV1A::HashConst("m_bInReload"));
		return *(bool*)((std::uintptr_t)this + m_bInReload);
	}

	#pragma region baseweapon_exports
	bool IsWeapon()
	{
		return MEM::CallVFunc<bool>(this, 165);
	}

	float GetSpread()
	{
		return MEM::CallVFunc<float>(this, 452);
	}

	float GetInaccuracy()
	{
		return MEM::CallVFunc<float>(this, 482);
	}

	void UpdateAccuracyPenalty()
	{
		MEM::CallVFunc<void>(this, 483);
	}
	#pragma endregion
};

class CBaseCSGrenade
{
public:
	bool IsPinPulled()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bPinPulled);
	}

	float GetThrowTime()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().fThrowTime);
	}

	float GetThrowStrength()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flThrowStrength);
	}

	int GetEffectTickBegin()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().nExplodeEffectTickBegin);
	}
};

class CSmokeGrenade
{
public:
	int GetEffectTickBegin()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().nSmokeEffectTickBegin);
	}

	inline int GetMaxTime()
	{
		return 18.f;
	}
};

class CInferno
{
public:
	int GetEffectTickBegin()
	{
		return *(int*)((std::uintptr_t)this + CNetvarManager::Get().nFireEffectTickBegin);
	}

	inline int GetMaxTime()
	{
		return 7.f;
	}
};

class CPlantedC4
{
public:
	bool IsPlanted()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bBombTicking);
	}

	CBaseHandle GetDefuser()
	{
		return *(CBaseHandle*)((std::uintptr_t)this + CNetvarManager::Get().hBombDefuser);
	}

	bool IsDefused()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bBombDefused);
	}

	// replacement for mp_c4timer
	float GetTimerLength()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flTimerLength);
	}

	float GetDefuseLength()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flDefuseLength);
	}

	// this all here cuz msvc fucked up
	float GetTimer(float flServerTime);
	float GetDefuseTimer(float flServerTime);
};

class CEnvTonemapController
{
public:
	bool& IsUseCustomAutoExposureMin()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bUseCustomAutoExposureMin);
	}

	bool& IsUseCustomAutoExposureMax()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bUseCustomAutoExposureMax);
	}

	bool& IsUseCustomBloomScale()
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bUseCustomBloomScale);
	}

	float& GetCustomAutoExposureMin()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flCustomAutoExposureMin);
	}

	float& GetCustomAutoExposureMax()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flCustomAutoExposureMax);
	}

	float& GetCustomBloomScale()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flCustomBloomScale);
	}

	float& GetCustomBloomScaleMin()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flCustomBloomScaleMinimum);
	}

	float& GetBloomExponent()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flBloomExponent);
	}

	float& GetBloomSaturation()
	{
		return *(float*)((std::uintptr_t)this + CNetvarManager::Get().flBloomSaturation);
	}
};

class CBreakableSurface
{
public:
	bool IsBroken() // dead inside
	{
		return *(bool*)((std::uintptr_t)this + CNetvarManager::Get().bIsBroken);
	}
};
