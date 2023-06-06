#pragma once
#include "../common.h"

#include "datatypes/basehandle.h"
#include "datatypes/usercmd.h"

// used: n_add_offset, n_add_poffset, n_add_variable, n_add_pvariable, n_add_datafield, n_add_pdatafield
#include "../core/netvar.h"
// used: m_lerp
#include "../utilities/math.h"
// used: ccsgoplayeranimstate
#include "animation.h"
// used: ehitgroup, elifestate
#include "const.h"
// used: bf_read_t
#include "bitbuf.h"
// used: studiohdr_t, maxstudioposeparam
#include "studio.h"
// used: bonevector_t, bonequaternionaligned_t
#include "bonesetup.h"
// used: max_item_custom_name_database_size
#include "econitem.h"
// used: ray_t, trace_t
#include "interfaces/ienginetrace.h"
// used: model_t, mdlhandle_t
#include "interfaces/ivmodelinfo.h"
// used: ccustommaterialowner, ivisualsdataprocessor
#include "interfaces/imaterialsystem.h"

#pragma region entity_enumerations
enum EDataUpdateType : int
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

enum EThinkMethod : int
{
	THINK_FIRE_ALL_FUNCTIONS = 0,
	THINK_FIRE_BASE_ONLY,
	THINK_FIRE_ALL_BUT_BASE,
};

// @credits: https://tf2b.com/itemlist.php?gid=730
using ItemDefinitionIndex_t = std::uint16_t;
enum EItemDefinitionIndex : ItemDefinitionIndex_t
{
	ITEM_MUSIC_KIT = 58U,

	ITEM_KNIFE_BAYONET = 500U,
	ITEM_KNIFE_CLASSIC = 503U,
	ITEM_KNIFE_FLIP = 505U,
	ITEM_KNIFE_GUT = 506U,
	ITEM_KNIFE_KARAMBIT = 507U,
	ITEM_KNIFE_M9_BAYONET = 508U,
	ITEM_KNIFE_TACTICAL = 509U,
	ITEM_KNIFE_FALCHION = 512U,
	ITEM_KNIFE_SURVIVAL_BOWIE = 514U,
	ITEM_KNIFE_BUTTERFLY = 515U,
	ITEM_KNIFE_PUSH = 516U,
	ITEM_KNIFE_CORD = 517U,
	ITEM_KNIFE_CANIS = 518U,
	ITEM_KNIFE_URSUS = 519U,
	ITEM_KNIFE_GYPSY_JACKKNIFE = 520U,
	ITEM_KNIFE_OUTDOOR = 521U,
	ITEM_KNIFE_STILETTO = 522U,
	ITEM_KNIFE_WIDOWMAKER = 523U,
	ITEM_KNIFE_SKELETON = 525U,

	ITEM_STICKER = 1209U,

	ITEM_PATCH = 4609U,

	ITEM_GLOVE_STUDDED_BROKENFANG = 4725U,
	ITEM_GLOVE_STUDDED_BLOODHOUND = 5027U,
	ITEM_GLOVE_T = 5028U,
	ITEM_GLOVE_CT = 5029U,
	ITEM_GLOVE_SPORTY = 5030U,
	ITEM_GLOVE_SLICK = 5031U,
	ITEM_GLOVE_LEATHER_HANDWRAPS = 5032U,
	ITEM_GLOVE_MOTORCYCLE = 5033U,
	ITEM_GLOVE_SPECIALIST = 5034U,
	ITEM_GLOVE_STUDDED_HYDRA = 5035U,

	ITEM_AGENT_T = 5036U,
	ITEM_AGENT_CT = 5037U
};

enum EWeaponHoldsPlayerAnimationCapability : int
{
	WEAPON_PLAYER_ANIMS_UNKNOWN = 0,
	WEAPON_PLAYER_ANIMS_AVAILABLE,
	WEAPON_PLAYER_ANIMS_NOT_AVAILABLE
};
#pragma endregion

// forward declarations
struct SpatializationInfo_t; // not implemented
class IClientUnknown;
class IClientRenderable;
class IClientNetworkable;
class IClientThinkable;
class IClientEntity;
class CClientClass;
class CIKContext;
class CBoneMergeCache;
class CBaseCombatWeapon;
class CWeaponCSBaseGun;
class CEconItemView;

using ClientShadowHandle_t = std::uint16_t;
using ClientRenderHandle_t = std::uint16_t;
using ModelInstanceHandle_t = std::uint16_t;

// @source: master/public/ihandleentity.h
class IHandleEntity
{
public:
	virtual ~IHandleEntity() { }
	virtual void SetRefEHandle(const CBaseHandle& hRef) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

// @source: master/public/engine/ICollideable.h
class ICollideable
{
public:
	virtual IHandleEntity* GetEntityHandle() = 0;
	virtual const Vector_t& OBBMins() const = 0;
	virtual const Vector_t& OBBMaxs() const = 0;
	virtual void WorldSpaceTriggerBounds(Vector_t* pvecWorldMins, Vector_t* pvecWorldMaxs) const = 0;
	virtual bool TestCollision(const Ray_t& ray, unsigned int nContentsMask, Trace_t& trace) = 0;
	virtual bool TestHitboxes(const Ray_t& ray, unsigned int nContentsMask, Trace_t& trace) = 0;
	virtual int GetCollisionModelIndex() = 0;
	virtual const Model_t* GetCollisionModel() = 0;
	virtual Vector_t& GetCollisionOrigin() const = 0;
	virtual QAngle_t& GetCollisionAngles() const = 0;
	virtual const Matrix3x4_t& CollisionToWorldTransform() const = 0;
	virtual ESolidType GetSolid() const = 0;
	virtual int GetSolidFlags() const = 0;
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual int GetCollisionGroup() const = 0;
	virtual void WorldSpaceSurroundingBounds(Vector_t* pvecMins, Vector_t* pvecMaxs) = 0;
	virtual unsigned int GetRequiredTriggerFlags() const = 0;
	virtual const Matrix3x4_t* GetRootParentToWorldTransform() const = 0;
	virtual void* GetVPhysicsObject() const = 0;
};

// @source: master/public/iclientalphaproperty.h
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

// @source: master/public/iclientunknown.h
class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual CBaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty() = 0;
};

struct RenderableInstance_t
{
	std::uint8_t uAlpha;
};

class IClientModelRenderable
{
public:
	virtual bool GetRenderData(void* pData, int nModelDataCategory) = 0;
};

// @source: master/public/iclientrenderable.h
class IClientRenderable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual Vector_t& GetRenderOrigin() = 0;
	virtual QAngle_t& GetRenderAngles() = 0;
	virtual bool ShouldDraw() = 0;
	virtual int GetRenderFlags() = 0;
	virtual bool IsTransparent() = 0;
	virtual ClientShadowHandle_t GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t& RenderHandle() = 0;
	virtual const Model_t* GetModel() const = 0;
	virtual int DrawModel(int nFlags, const RenderableInstance_t& instance) = 0;
	virtual int GetBody() = 0;
	virtual void GetColorModulation(float* arrColor) = 0;
	virtual bool LODTest() = 0;
	virtual bool SetupBones(Matrix3x4a_t* arrBonesToWorld, int iMaxBones, int nBoneMask, float flCurrentTime) = 0;
	virtual void SetupWeights(const Matrix3x4_t* arrBonesToWorld, int nFlexWeightCount, float* arrFlexWeight, float* arrFlexDelayedWeight) = 0;
	virtual void DoAnimationEvents() = 0;
	virtual void* GetPVSNotifyInterface() = 0;
	virtual void GetRenderBounds(Vector_t& vecMins, Vector_t& vecMaxs) = 0;
	virtual void GetRenderBoundsWorldspace(Vector_t& vecMins, Vector_t& vecMaxs) = 0;
	virtual void GetShadowRenderBounds(Vector_t& vecMins, Vector_t& vecMaxs, int nShadowType) = 0;
	virtual bool ShouldReceiveProjectedTextures(int nFlags) = 0;
	virtual bool GetShadowCastDistance(float* pflDistance, int iShadowType) const = 0;
	virtual bool GetShadowCastDirection(Vector_t* vecDirection, int iShadowType) const = 0;
	virtual bool IsShadowDirty() = 0;
	virtual void MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual int ShadowCastType() = 0;
	virtual void unused2() { }
	virtual void CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t GetModelInstance() = 0;
	virtual const Matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual int LookupAttachment(const char* szAttachmentName) = 0;
	virtual bool GetAttachment(int nIndex, Vector_t& vecOrigin, QAngle_t& angView) = 0;
	virtual bool GetAttachment(int nIndex, Matrix3x4_t& matAttachment) = 0;
	virtual bool ComputeLightingOrigin(int nAttachmentIndex, Vector_t vecModelLightingCenter, const Matrix3x4_t& matrix, Vector_t& vecTransformedLightingCenter) = 0;
	virtual float* GetRenderClipPlane() = 0;
	virtual int GetSkin() = 0;
	virtual void OnThreadedDrawSetup() = 0;
	virtual bool UsesFlexDelayedWeights() = 0;
	virtual void RecordToolMessage() = 0;
	virtual bool ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual std::uint8_t OverrideAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual std::uint8_t OverrideShadowAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual IClientModelRenderable* GetClientModelRenderable() = 0;
};

// @source: master/public/iclientnetworkable.h
class IClientNetworkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void Release() = 0;
	virtual CClientClass* GetClientClass() = 0;
	virtual void NotifyShouldTransmit(int iState) = 0;
	virtual void OnPreDataChanged(EDataUpdateType nUpdateType) = 0;
	virtual void OnDataChanged(EDataUpdateType nUpdateType) = 0;
	virtual void PreDataUpdate(EDataUpdateType nUpdateType) = 0;
	virtual void PostDataUpdate(EDataUpdateType nUpdateType) = 0;
	virtual void OnDataUnchangedInPVS() = 0;
	virtual bool IsDormant() const = 0;
	virtual int GetIndex() const = 0;
	virtual void ReceiveMessage(EClassIndex nClassIndex, bf_read& msg) = 0;
	virtual void* GetDataTableBasePtr() = 0;
	virtual void SetDestroyedOnRecreateEntities() = 0;
};

// @source: master/public/iclientthinkable.h
class IClientThinkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void ClientThink() = 0;
	virtual void* GetThinkHandle() = 0;
	virtual void SetThinkHandle(void* hThink) = 0;
	virtual void Release() = 0;
};

// @source: master/public/icliententity.h
class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual const Vector_t& GetAbsOrigin() const = 0;
	virtual const QAngle_t& GetAbsAngles() const = 0;
	virtual void* GetMouth() = 0;
	virtual bool GetSoundSpatialization(SpatializationInfo_t& info) = 0;
	virtual bool IsBlurred() = 0;
};

class CDefaultClientRenderable : public IClientUnknown, public IClientRenderable
{
public:
	ClientRenderHandle_t hRenderHandle; // 0x08
};
static_assert(sizeof(CDefaultClientRenderable) == 0xC);

// @source: master/game/shared/collisionproperty.h
class CCollisionProperty : public ICollideable
{
public:
	Q_CLASS_NO_INITIALIZER(CCollisionProperty);

	N_ADD_VARIABLE(Vector_t, GetPropertyMins, "DT_CollisionProperty::m_vecMins");
	N_ADD_VARIABLE(Vector_t, GetPropertyMaxs, "DT_CollisionProperty::m_vecMaxs");
	N_ADD_VARIABLE(unsigned short, GetPropertySolidFlags, "DT_CollisionProperty::m_usSolidFlags"); // ESolidFlags
	N_ADD_VARIABLE(unsigned char, GetPropertySolidType, "DT_CollisionProperty::m_nSolidType"); // ESolidType
	N_ADD_VARIABLE(unsigned char, GetPropertyTriggerBloat, "DT_CollisionProperty::m_triggerBloat");
	N_ADD_VARIABLE(unsigned char, GetPropertySurroundType, "DT_CollisionProperty::m_nSurroundType");
	N_ADD_VARIABLE(Vector_t, GetPropertySpecifiedSurroundingMins, "DT_CollisionProperty::m_vecSpecifiedSurroundingMins");
	N_ADD_VARIABLE(Vector_t, GetPropertySpecifiedSurroundingMaxs, "DT_CollisionProperty::m_vecSpecifiedSurroundingMaxs");

	[[nodiscard]] Q_INLINE Vector_t OBBCenter()
	{
		return M_LERP(this->GetPropertyMins(), this->GetPropertyMaxs(), 0.5f);
	}
};

// @source: master/game/client/c_baseentity.h
class CBaseEntity : public IClientEntity, public IClientModelRenderable, protected ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseEntity);

	N_ADD_VARIABLE(int, GetEffects, "DT_BaseEntity::m_fEffects");
	N_ADD_VARIABLE(int, GetTeam, "DT_BaseEntity::m_iTeamNum");
	N_ADD_VARIABLE(QAngle_t, GetRotation, "DT_BaseEntity::m_angRotation");
	N_ADD_VARIABLE(Vector_t, GetOrigin, "DT_BaseEntity::m_vecOrigin");
	N_ADD_VARIABLE(CBaseHandle, GetOwnerEntityHandle, "DT_BaseEntity::m_hOwnerEntity");
	N_ADD_VARIABLE(short, GetModelIndex, "DT_BaseEntity::m_nModelIndex");
	N_ADD_VARIABLE(float, GetAnimationTime, "DT_BaseEntity::m_flAnimTime");
	N_ADD_VARIABLE_OFFSET(float, GetOldAnimationTime, "DT_BaseEntity::m_flAnimTime", 0x4);
	N_ADD_VARIABLE(float, GetSimulationTime, "DT_BaseEntity::m_flSimulationTime");
	N_ADD_VARIABLE_OFFSET(float, GetOldSimulationTime, "DT_BaseEntity::m_flSimulationTime", 0x4);
	N_ADD_VARIABLE_OFFSET(float, GetSimulationTick, "DT_BaseEntity::m_flUseLookAtAngle", -0x20); // @ida C_BaseEntity::m_nSimulationTick: (C_BasePlayer::PhysicsSimulate) client.dll -> ["8B 40 1C 39 86" + 0x5]
	N_ADD_VARIABLE_OFFSET(float, GetSpawnTime, "DT_BaseEntity::m_flUseLookAtAngle", 0xC); // @ida C_BaseEntity::m_flSpawnTime: (C_BaseEntity::GetTextureAnimationStartTime) client.dll -> ["D9 81 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC 55 8B EC 83 EC 1C" + 0x2]
	N_ADD_VARIABLE_OFFSET(CBaseHandle, GetMoveParentHandle, "DT_BaseEntity::m_flFadeScale", 0xC); // @ida C_BaseEntity::m_pMoveParent: (C_BaseEntity::UnlinkFromHierarchy) client.dll -> ["83 BF ? ? ? ? ? 74 2F 8B" + 0x2] @xref: "C_BaseEntity::UnlinkFromHierarchy(): Entity has a child with the wrong parent!\n"
	N_ADD_PVARIABLE(CCollisionProperty, GetCollisionProperty, "DT_BaseEntity::m_Collision");
	N_ADD_VARIABLE(int, GetCollisionGroup, "DT_BaseEntity::m_CollisionGroup");
	N_ADD_VARIABLE(bool, IsSpotted, "DT_BaseEntity::m_bSpotted");

	N_ADD_DATAFIELD(unsigned char, GetMoveType, this->GetPredictionDescMap(), "m_MoveType");
	N_ADD_DATAFIELD(int, GetEFlags, this->GetPredictionDescMap(), "m_iEFlags");
	N_ADD_DATAFIELD(QAngle_t, GetAbsRotation, this->GetPredictionDescMap(), "m_angAbsRotation");
	N_ADD_DATAFIELD(Vector_t, GetAbsVelocity, this->GetPredictionDescMap(), "m_vecAbsVelocity");
	N_ADD_DATAFIELD(const Matrix3x4_t&, GetCoordinateFrame, this->GetDataDescMap(), "m_rgflCoordinateFrame");

	[[nodiscard]] char& GetTakeDamage()
	{
		static const auto uTakeDamageOffset = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(CLIENT_DLL, Q_XOR("80 BE ? ? ? ? ? 75 46 8B 86")) + 0x2);
		return *reinterpret_cast<char*>(reinterpret_cast<std::uint8_t*>(this) + uTakeDamageOffset);
	}

	[[nodiscard]] Q_INLINE DataMap_t* GetDataDescMap()
	{
		return CallVFunc<DataMap_t*, 15U>(this);
	}

	[[nodiscard]] Q_INLINE DataMap_t* GetPredictionDescMap()
	{
		// @ida: U8["FF 50 ? 50 8B 43 08 8D 4B 08 FF" + 0x2] / sizeof(std::uintptr_t) @xref: "C_BaseEntity::RestoreData", "C_BaseEntity::SaveData"
		return CallVFunc<DataMap_t*, 17U>(this);
	}

	Q_INLINE CBaseAnimating* GetBaseAnimating()
	{
		// @ida: (C_BaseViewModel::UpdateParticles) client.dll -> ["FF 90 ? ? ? ? 68 ? ? ? ? 8D 48" + 0x2] / sizeof(std::uintptr_t) @xref: "Wick"
		return CallVFunc<CBaseAnimating*, 44U>(this);
	}

	Q_INLINE CBaseAnimatingOverlay* GetBaseAnimatingOverlay()
	{
		// @ida: client.dll -> ["8B 80 ? ? ? ? FF D0 F3 0F 10 0D ? ? ? ? 8B 88" + 0x2] / sizeof(std::uintptr_t) @xref: "death_yaw"
		return CallVFunc<CBaseAnimatingOverlay*, 45U>(this);
	}

	Q_INLINE void SetModelIndex(int nModelIndex)
	{
		CallVFunc<void, 75U>(this, nModelIndex);
	}

	[[nodiscard]] Q_INLINE const Vector_t& WorldSpaceCenter()
	{
		// @ida CCollisionProperty::WorldSpaceCenter(): client.dll -> "56 57 8D B9 20 03 00 00 E8 ? ? ? ? 8B F0"

		// @ida: client.dll -> ["8B 80 ? ? ? ? FF D0 8B 4D E8" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<const Vector_t&, 79U>(this);
	}

	[[nodiscard]] Q_INLINE int GetMaxHealth()
	{
		// @ida: client.dll -> ["8B 80 ? ? ? ? FF D0 66 0F 6E C0 0F 5B C0 0F" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<int, 123U>(this);
	}

	Q_INLINE void Think()
	{
		// @ida: client.dll -> ["FF 90 ? ? ? ? FF 35 ? ? ? ? 8B 4C" + 0x2] / sizeof(std::uintptr_t)
		CallVFunc<void, 139U>(this);
	}

	[[nodiscard]] Q_INLINE const char* GetClassname()
	{
		// @ida C_BaseEntity::GetClassname(): client.dll -> "56 8B F1 C6 05 ? ? ? ? ? 8B 46"

		// @ida: client.dll -> ["8B 01 FF 90 ? ? ? ? 90" + 0x4] / sizeof(std::uintptr_t)
		return CallVFunc<const char*, 143U>(this);
	}

	[[nodiscard]] Q_INLINE unsigned int PhysicsSolidMaskForEntity()
	{
		// @xref: "func_breakable", "func_breakable_surf"
		return CallVFunc<unsigned int, 152U>(this);
	}

	[[nodiscard]] Q_INLINE bool IsAlive()
	{
		// @ida: client.dll -> ["8B 80 ? ? ? ? FF D0 84 C0 75 08 38 87 ? ? ? ? 74 72" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<bool, 156U>(this);
	}

	[[nodiscard]] Q_INLINE bool IsPlayer() const
	{
		// @ida: client.dll -> ["8B 80 ? ? ? ? FF D0 84 C0 0F 84 ? ? ? ? 8B 44" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<bool, 158U>(this);
	}

	[[nodiscard]] Q_INLINE bool IsWeaponWorldModel() const
	{
		return CallVFunc<bool, 159U>(this);
	}

	[[nodiscard]] Q_INLINE bool IsWeapon() const
	{
		// C_BaseEntity::IsBaseCombatWeapon
		return CallVFunc<bool, 166U>(this);
	}

	[[nodiscard]] Q_INLINE Vector_t GetEyePosition()
	{
		Vector_t vecPosition = { };
		CallVFunc<void, 169U>(this, &vecPosition);
		return vecPosition;
	}

	void SetAbsOrigin(const Vector_t& vecAbsOrigin)
	{
		static auto fnSetAbsOrigin = reinterpret_cast<void(Q_THISCALL*)(CBaseEntity*, const Vector_t&)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ? ? ? 8B 7D")));
		fnSetAbsOrigin(this, vecAbsOrigin);
	}

	void SetAbsAngles(const QAngle_t& angAbsView)
	{
		static auto fnSetAbsAngles = reinterpret_cast<void(Q_THISCALL*)(CBaseEntity*, const QAngle_t&)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1")));
		fnSetAbsAngles(this, angAbsView);
	}

	void SetAbsVelocity(const Vector_t& vecAbsVelocity)
	{
		static auto fnSetAbsVelocity = reinterpret_cast<void(Q_THISCALL*)(CBaseEntity*, const Vector_t&)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1 F3")));
		fnSetAbsVelocity(this, vecAbsVelocity);
	}

	[[nodiscard]] bool PhysicsRunThink(EThinkMethod nThinkMethod = THINK_FIRE_ALL_FUNCTIONS)
	{
		static auto fnPhysicsRunThink = reinterpret_cast<bool(Q_THISCALL*)(CBaseEntity*, EThinkMethod)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87")));
		return fnPhysicsRunThink(this, nThinkMethod);
	}

	[[nodiscard]] bool IsBreakable();
};

// @source: master/game/client/c_baseanimating.h
class CBaseAnimating : public CBaseEntity
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseAnimating);

	N_ADD_VARIABLE(int, IsRagdoll, "DT_BaseAnimating::m_bClientSideRagdoll");
	N_ADD_VARIABLE(int, GetHitboxSet, "DT_BaseAnimating::m_nHitboxSet");
	N_ADD_VARIABLE(float, GetCycle, "DT_BaseAnimating::m_flCycle");
	N_ADD_VARIABLE_OFFSET(int, GetCustomBlendingRuleMask, "DT_BaseAnimating::m_nBody", 0x4);
	N_ADD_VARIABLE_OFFSET(unsigned int, GetAnimationLODFlags, "DT_BaseAnimating::m_nBody", 0x8);
	N_ADD_VARIABLE_OFFSET(unsigned int, GetOldAnimationLODFlags, "DT_BaseAnimating::m_nBody", 0xC);
	N_ADD_VARIABLE_OFFSET(int, GetComputedAnimationLODFrame, "DT_BaseAnimating::m_nBody", 0x10); // @ida C_BaseAnimating::m_nComputedLODframe: client.dll -> ["8B B7 ? ? ? ? 89 75 F8" + 0x2]
	N_ADD_VARIABLE(float[MAXSTUDIOBONECTRLS], GetEncodedControllerArray, "DT_BaseAnimating::m_flEncodedController");
	N_ADD_VARIABLE_OFFSET(BoneVector_t[MAXSTUDIOBONES], GetCachedBonesPosition, "DT_BaseAnimating::m_nMuzzleFlashParity", 0x4); // @ida C_BaseAnimating::m_pos_cached: client.dll -> ["8D 87 ? ? ? ? 50 E8 ? ? ? ? 8B 06 83 C4" + 0x2]
	N_ADD_VARIABLE_OFFSET(BoneQuaternionAligned_t[MAXSTUDIOBONES], GetCachedBonesRotation, "DT_BaseAnimating::m_nMuzzleFlashParity", 0x4 + sizeof(BoneVector_t[MAXSTUDIOBONES])); // @ida C_BaseAnimating::m_q_cached: client.dll -> ["8D 87 ? ? ? ? 50 E8 ? ? ? ? 83 C4 0C F7" + 0x2]
	N_ADD_VARIABLE_OFFSET(CIKContext*, GetIKContext, "DT_BaseAnimating::m_vecForce", -0x14); // @ida C_BaseAnimating::m_pIk: client.dll -> ["89 87 ? ? ? ? 8D 47 FC 8B" + 0x2]
	N_ADD_VARIABLE(Vector_t, GetForce, "DT_BaseAnimating::m_vecForce");
	N_ADD_VARIABLE(int, GetForceBone, "DT_BaseAnimating::m_nForceBone");
	N_ADD_VARIABLE_OFFSET(unsigned long, GetMostRecentModelBoneCounter, "DT_BaseAnimating::m_nForceBone", 0x4); // @ida C_BaseAnimating::m_iMostRecentModelBoneCounter: client.dll -> ["89 87 ? ? ? ? 8B 8F ? ? ? ? 85 C9 74 10" + 0x2] @xref: "Model '%s' has skin but thinks it can render fastpath\n"
	N_ADD_VARIABLE_OFFSET(int, GetPreviousBoneMask, "DT_BaseAnimating::m_nForceBone", 0x10); // @ida C_BaseAnimating::m_iPrevBoneMask: (C_BaseAnimating::SetupBones) client.dll -> ["8B 87 ? ? ? ? 89 87 ? ? ? ? 8D 47" + 0x2]
	N_ADD_VARIABLE_OFFSET(int, GetAccumulatedBoneMask, "DT_BaseAnimating::m_nForceBone", 0x14); // @ida C_BaseAnimating::m_iAccumulatedBoneMask: (C_BaseAnimating::SetupBones) client.dll -> ["89 87 ? ? ? ? 8D 47 FC C7" + 0x2]
	N_ADD_VARIABLE_OFFSET(CBoneAccessor, GetBoneAccessor, "DT_BaseAnimating::m_nForceBone", 0x18); // @ida C_BaseAnimating::m_BoneAccessor: client.dll -> ["89 BF ? ? ? ? 89 87" + 0x2]
	N_ADD_VARIABLE_OFFSET(CUtlVector<CBaseHandle>, GetBoneAttachments, "DT_BaseAnimating::m_nForceBone", 0x3C); // @ida C_BaseAnimating::m_BoneAttachments: client.dll -> ["8D 8F ? ? ? ? 8B 00" + 0x2]
	N_ADD_VARIABLE(float, GetModelScale, "DT_BaseAnimating::m_flModelScale");
	N_ADD_VARIABLE(int, GetModelScaleType, "DT_BaseAnimating::m_ScaleType");
	N_ADD_VARIABLE(float[MAXSTUDIOPOSEPARAM], GetPoseParameterArray, "DT_BaseAnimating::m_flPoseParameter");
	N_ADD_VARIABLE(bool, IsClientSideAnimation, "DT_BaseAnimating::m_bClientSideAnimation");
	N_ADD_VARIABLE(int, GetSequence, "DT_BaseAnimating::m_nSequence");
	N_ADD_VARIABLE_OFFSET(CBoneMergeCache*, GetBoneMergeCache, "DT_BaseAnimating::m_hLightingOrigin", -0x38); // @ida C_BaseAnimating::m_pBoneMergeCache: (C_BaseAnimating::CalcBoneMerge) client.dll -> ["89 86 ? ? ? ? E8 ? ? ? ? FF 75 08" + 0x2]
	N_ADD_VARIABLE_OFFSET(CUtlVectorAligned<Matrix3x4a_t>, GetCachedBonesData, "DT_BaseAnimating::m_hLightingOrigin", -0x34); // @ida C_BaseAnimating::m_CachedBoneData: client.dll -> ["8B 87 ? ? ? ? 89 BF" + 0x2]
	N_ADD_VARIABLE_OFFSET(float, GetLastBoneSetupTime, "DT_BaseAnimating::m_hLightingOrigin", -0x20); // @ida C_BaseAnimating::m_flLastBoneSetupTime: client.dll -> ["C7 87 ? ? ? ? ? ? ? ? 89 87 ? ? ? ? 8B 8F" + 0x2] @xref: "Model '%s' has skin but thinks it can render fastpath\n"
	N_ADD_VARIABLE_OFFSET(bool, IsJiggleBonesAllowed, "DT_BaseAnimating::m_hLightingOrigin", -0x18); // @ida C_BaseAnimating::m_isJiggleBonesEnabled: client.dll -> ["80 BF ? ? ? ? ? 0F 84 ? ? ? ? 8B 74 24 14" + 0x2] @xref: r_jiggle_bones
	N_ADD_VARIABLE_OFFSET(CStudioHdr*, GetStudioHdr, "DT_BaseAnimating::m_hLightingOrigin", 0x8); // @ida C_BaseAnimating::m_pStudioHdr: client.dll -> ["8B 8E ? ? ? ? 85 C9 0F 84 ? ? ? ? 83 39" + 0x2] @xref: "Bip01_Head", "head_0", "L_Hand", "hand_L", "R_Hand", "hand_R", "weapon_bone"
	N_ADD_VARIABLE_OFFSET(MDLHandle_t, GetStudioHdrHandle, "DT_BaseAnimating::m_hLightingOrigin", 0xC); // @ida C_BaseAnimating::m_hStudioHdr: (C_BaseAnimating::LockStudioHdr) client.dll -> ["66 89 83 ? ? ? ? EB 2F" + 0x3]

	// incremented each frame in 'InvalidateModelBones()'. models compare this value to what it was last time they setup their bones to determine if they need to re-setup their bones
	[[nodiscard]] static unsigned long& GetModelBoneCounter()
	{
		static std::uint8_t* uModelBoneCounterOffset = MEM::FindPattern(CLIENT_DLL, Q_XOR("3B 05 ? ? ? ? 0F 84 ? ? ? ? 8B 47")) + 0x2;
		return *reinterpret_cast<unsigned long*>(uModelBoneCounterOffset);
	}

	// update latched IK contacts if they're in a moving reference frame
	Q_INLINE void UpdateIKLocks(float flCurrentTime)
	{
		// @ida C_BaseAnimating::UpdateIKLocks(): client.dll -> "55 8B EC 83 EC 08 8B C1 53 89"
		CallVFunc<void, 192U>(this, flCurrentTime);
	}

	// find the ground or external attachment points needed by IK rules
	Q_INLINE void CalculateIKLocks(float flCurrentTime)
	{
		/*
		 * the only meaningful difference of client to server version is handling of attachments
		 *
		 * @ida C_BaseAnimating::CalculateIKLocks(): client.dll -> "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 57 8B F9 89 7C 24 18"
		 * @ida CBaseAnimating::CalculateIKLocks(): server.dll -> "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 83"
		 */
		CallVFunc<void, 193U>(this, flCurrentTime);
	}

	Q_INLINE void SetSequence(int iSequence)
	{
		// @ida: client.dll -> ["FF 90 ? ? ? ? 8B 07 8B CF FF 90 ? ? ? ? 8B CF" + 0x2] / sizeof(std::uintptr_t)
		CallVFunc<void, 219U>(this, iSequence);
	}

	Q_INLINE void StudioFrameAdvance()
	{
		// @ida: client.dll -> ["FF 90 ? ? ? ? 8B 07 8B CF FF 90 ? ? ? ? 8B CF" + 0xC] / sizeof(std::uintptr_t)
		CallVFunc<void, 220U>(this);
	}

	Q_INLINE void UpdateClientSideAnimation()
	{
		// @ida C_CSPlayer::UpdateClientSideAnimation(): client.dll -> "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"
		// @ida: client.dll -> ["FF 90 ? ? ? ? 46 3B F7 7C E4 8B" + 0x2] / sizeof(std::uintptr_t) @xref: "UpdateClientSideAnimations"
		CallVFunc<void, 224U>(this);
	}

	[[nodiscard]] Q_INLINE bool IsViewModel() const
	{
		return CallVFunc<bool, 234U>(this);
	}

	void SetupBones_AttachmentHelper(CStudioHdr* pStudioHdr)
	{
		static auto fnSetupBones_AttachmentHelper = reinterpret_cast<void(Q_THISCALL*)(CBaseAnimating*, CStudioHdr*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 48 53 8B 5D")));
		fnSetupBones_AttachmentHelper(this, pStudioHdr);
	}

	void LockStudioHdr()
	{
		static auto fnLockStudioHdr = reinterpret_cast<void(Q_THISCALL*)(CBaseAnimating*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 51 53 8B D9 56 57 8D B3 ? ? ? ? FF")));
		fnLockStudioHdr(this);
	}

	void UnlockStudioHdr()
	{
		static auto fnUnlockStudioHdr = reinterpret_cast<void(Q_THISCALL*)(CBaseAnimating*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("56 57 8B F9 8B B7 ? ? ? ? 85 F6 0F 84 ? ? ? ? 8B 4E")));
		fnUnlockStudioHdr(this);
	}

	[[nodiscard]] Q_INLINE CStudioHdr* GetModelPtr()
	{
		// @ida C_BaseAnimating::GetModelPtr(): client.dll -> ABS["E8 ? ? ? ? 8B 55 38" + 0x1]

		if (CStudioHdr* pStudioHdr = this->GetStudioHdr(); pStudioHdr == nullptr)
		{
			if (this->GetModel() != nullptr)
				this->LockStudioHdr();
			else
				return nullptr;
		}
		else if (pStudioHdr->IsValid())
			return pStudioHdr;

		return nullptr;
	}

	[[nodiscard]] int GetSequenceActivity(const int iSequence)
	{
		if (iSequence == -1)
			return ACT_INVALID;

		CStudioHdr* pStudioHdr = this->GetModelPtr();

		if (pStudioHdr == nullptr)
			return ACT_INVALID;

		static auto fnGetSequenceActivity = reinterpret_cast<int(Q_FASTCALL*)(CBaseAnimating*, CStudioHdr*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 53 8B 5D 08 56 8B F1 83"))); // @xref: "Need to handle the activity %d\n"
		return fnGetSequenceActivity(this, pStudioHdr, iSequence);
	}

	[[nodiscard]] Q_INLINE float GetModelHierarchyScale()
	{
		if (this->GetModelScaleType() == HIERARCHICAL_MODEL_SCALE)
			return this->GetModelScale();

		const CStudioHdr* pStudioHdr = this->GetModelPtr();
		return (pStudioHdr != nullptr && pStudioHdr->GetBoneCount() == 1) ? this->GetModelScale() : 1.0f;
	}

	Q_INLINE void InvalidateBoneCache()
	{
		// @ida C_BaseAnimating::InvalidateBoneCache() [inlined]: client.dll -> "48 C7 87 ? ? ? ? ? ? ? ? 89 87 ? ? ? ? 8B 8F" - 0x14 @xref: "Model '%s' has skin but thinks it can render fastpath\n"

		this->GetMostRecentModelBoneCounter() = GetModelBoneCounter() - 1;
		this->GetLastBoneSetupTime() = -FLT_MAX;
	}

	[[nodiscard]] Q_INLINE bool IsBoneCacheValid()
	{
		return (this->GetMostRecentModelBoneCounter() == GetModelBoneCounter());
	}

	[[nodiscard]] float GetPoseParameter(const CStudioHdr* pStudioHdr, int iParameter);
	float SetPoseParameter(const CStudioHdr* pStudioHdr, int iParameter, float flValue);
	[[nodiscard]] int GetBoneByHash(const FNV1A_t uBoneHash);
	[[nodiscard]] Vector_t GetBonePosition(int nBoneIndex);
	[[nodiscard]] Vector_t GetHitboxPosition(const int nHitboxIndex);
	[[nodiscard]] Vector_t GetHitGroupPosition(const int iHitGroup);
	void GetSkeleton(CStudioHdr* pStudioHdr, BoneVector_t* arrBonesPosition, BoneQuaternionAligned_t* arrBonesRotation, int nBoneMask);
};

// @source: master/game/client/c_baseanimatingoverlay.h
class CBaseAnimatingOverlay : public CBaseAnimating
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseAnimatingOverlay);

	[[nodiscard]] CUtlVector<CAnimationLayer>& GetAnimationOverlays()
	{
		// @ida C_BaseAnimatingOverlay::GetAnimOverlay(): client.dll -> "55 8B EC 51 53 8B 5D 08 33 C0"
		static const std::uintptr_t uAnimationOverlaysOffset = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(CLIENT_DLL, Q_XOR("8B 89 ? ? ? ? 8D 0C D1")) + 0x2);
		return *reinterpret_cast<CUtlVector<CAnimationLayer>*>(reinterpret_cast<std::uint8_t*>(this) + uAnimationOverlaysOffset);
	}

	Q_INLINE bool UpdateDispatchLayer(CAnimationLayer* pLayer, CStudioHdr* pWeaponStudioHdr, int iSequence)
	{
		// @ida C_BaseAnimatingOverlay::UpdateDispatchLayer(): client.dll | server.dll -> "55 8B EC 56 57 8B 7D 0C 8B D1"
		return CallVFunc<bool, 247U>(this, pLayer, pWeaponStudioHdr, iSequence);
	}

	void GetSkeleton(CStudioHdr* pStudioHdr, BoneVector_t* arrBonesPosition, BoneQuaternionAligned_t* arrBonesRotation, int nBoneMask);
};

// @source: master/game/client/c_baseflex.h
class CBaseFlex : public CBaseAnimatingOverlay
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseFlex);
};

// @source: master/game/client/c_basecombatcharacter.h
class CBaseCombatCharacter : public CBaseFlex
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseCombatCharacter);

	N_ADD_VARIABLE(float, GetNextAttack, "DT_BaseCombatCharacter::m_flNextAttack");
	N_ADD_VARIABLE(CBaseHandle, GetActiveWeaponHandle, "DT_BaseCombatCharacter::m_hActiveWeapon");
	N_ADD_VARIABLE(CBaseHandle[MAX_WEAPONS], GetWeaponsHandle, "DT_BaseCombatCharacter::m_hMyWeapons");
	N_ADD_VARIABLE(CBaseHandle[MAX_WEARABLES], GetWearablesHandle, "DT_BaseCombatCharacter::m_hMyWearables");

	[[nodiscard]] CBaseCombatWeapon* GetActiveWeapon();
};

// @source: master/public/PlayerState.h
class CPlayerState
{
public:
	Q_CLASS_NO_INITIALIZER(CPlayerState);

	N_ADD_VARIABLE_OFFSET(QAngle_t, GetViewAngles, "DT_PlayerState::deadflag", 0x4);
};

// @source: master/game/client/c_playerlocaldata.h
class CPlayerLocalData
{
public:
	Q_CLASS_NO_INITIALIZER(CPlayerLocalData);

	N_ADD_VARIABLE(float, GetFallVelocity, "DT_Local::m_flFallVelocity");
	N_ADD_VARIABLE(QAngle_t, GetViewPunch, "DT_Local::m_viewPunchAngle");
	N_ADD_VARIABLE(QAngle_t, GetAimPunch, "DT_Local::m_aimPunchAngle");
};

// @source: master/game/client/c_baseplayer.h
class CCommandContext
{
public:
	bool bNeedsProcessing; // 0x00
	CUserCmd command; // 0x04
	int nCommandNumber; // 0x68
};
static_assert(sizeof(CCommandContext) == 0x6C);

class CBasePlayer : public CBaseCombatCharacter
{
public:
	Q_CLASS_NO_INITIALIZER(CBasePlayer);

	N_ADD_VARIABLE(int, GetNextThinkTick, "DT_BasePlayer::m_nNextThinkTick");
	N_ADD_VARIABLE(int, GetHealth, "DT_BasePlayer::m_iHealth");
	N_ADD_VARIABLE(int, GetFlags, "DT_BasePlayer::m_fFlags");
	N_ADD_VARIABLE(Vector_t, GetViewOffset, "DT_BasePlayer::m_vecViewOffset[0]");
	N_ADD_VARIABLE(Vector_t, GetBaseVelocity, "DT_BasePlayer::m_vecBaseVelocity");
	N_ADD_VARIABLE(float, GetFriction, "DT_BasePlayer::m_flFriction");
	N_ADD_VARIABLE(CBaseHandle, GetGroundEntityHandle, "DT_BasePlayer::m_hGroundEntity");
	N_ADD_VARIABLE(std::uint8_t, GetWaterLevel, "DT_BasePlayer::m_nWaterLevel");
	N_ADD_VARIABLE(std::int8_t, GetLifeState, "DT_BasePlayer::m_lifeState");
	N_ADD_VARIABLE(int, GetCoachingTeam, "DT_BasePlayer::m_iCoachingTeam");
	N_ADD_VARIABLE(float, GetDuckAmount, "DT_BasePlayer::m_flDuckAmount");
	N_ADD_VARIABLE(float, GetDuckSpeed, "DT_BasePlayer::m_flDuckSpeed");
	N_ADD_PVARIABLE(CPlayerLocalData, GetLocalData, "DT_BasePlayer::m_Local");
	N_ADD_VARIABLE(Vector_t, GetVelocity, "DT_BasePlayer::m_vecVelocity[0]");
	N_ADD_PVARIABLE(CPlayerState, GetPlayerState, "DT_BasePlayer::pl");
	N_ADD_VARIABLE(float, GetMaxSpeed, "DT_BasePlayer::m_flMaxspeed");
	N_ADD_VARIABLE(CBaseHandle, GetVehicleHandle, "DT_BasePlayer::m_hVehicle");
	N_ADD_VARIABLE(CBaseHandle, GetViewModelHandle, "DT_BasePlayer::m_hViewModel[0]");
	N_ADD_VARIABLE_OFFSET(int, GetButtonDisabled, "DT_BasePlayer::m_hViewEntity", -0xC);
	N_ADD_VARIABLE_OFFSET(int, GetButtonForced, "DT_BasePlayer::m_hViewEntity", -0x8);
	N_ADD_VARIABLE_OFFSET(CUserCmd*, GetCurrentCommand, "DT_BasePlayer::m_hViewEntity", -0x4); // @ida: client.dll -> ["89 BE ? ? ? ? E8 ? ? ? ? 85 FF" + 0x2]
	N_ADD_VARIABLE(int, GetObserverMode, "DT_BasePlayer::m_iObserverMode");
	N_ADD_VARIABLE(CBaseHandle, GetObserverTargetHandle, "DT_BasePlayer::m_hObserverTarget");
	N_ADD_VARIABLE(char[MAX_PLACE_NAME_LENGTH], GetLastPlaceName, "DT_BasePlayer::m_szLastPlaceName");
	N_ADD_VARIABLE(int, GetTickBase, "DT_BasePlayer::m_nTickBase");
	N_ADD_VARIABLE_OFFSET(int, GetFinalPredictedTick, "DT_BasePlayer::m_nTickBase", 0x4);
	N_ADD_VARIABLE_OFFSET(CCommandContext, GetCommandContext, "DT_BasePlayer::m_flLaggedMovementValue", -0x98); // @ida: client.dll -> ["C6 86 ? ? ? ? ? F6 86" + 0x2]

	N_ADD_DATAFIELD(int, GetButtons, this->GetPredictionDescMap(), "m_nButtons");
	N_ADD_PDATAFIELD(int, GetImpulse, this->GetPredictionDescMap(), "m_nImpulse");
	N_ADD_DATAFIELD(int, GetButtonLast, this->GetPredictionDescMap(), "m_afButtonLast");
	N_ADD_DATAFIELD(int, GetButtonPressed, this->GetPredictionDescMap(), "m_afButtonPressed");
	N_ADD_DATAFIELD(int, GetButtonReleased, this->GetPredictionDescMap(), "m_afButtonReleased");
	N_ADD_DATAFIELD(float, GetWaterJumpTime, this->GetPredictionDescMap(), "m_flWaterJumpTime");
	N_ADD_DATAFIELD(float, GetSurfaceFriction, this->GetPredictionDescMap(), "m_surfaceFriction");

	[[nodiscard]] CUserCmd& GetLastCommand()
	{
		static const std::uintptr_t uLastCommandOffset = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(CLIENT_DLL, Q_XOR("8D 8E ? ? ? ? 89 5C 24 3C")) + 0x2);
		return *reinterpret_cast<CUserCmd*>(reinterpret_cast<std::uint8_t*>(this) + uLastCommandOffset);
	}

	Q_INLINE void PreThink()
	{
		CallVFunc<void, 318U>(this);
	}

	Q_INLINE void UpdateCollisionBounds()
	{
		// @ida: client.dll -> ["FF 90 ? ? ? ? 83 3D ? ? ? ? ? 7E" + 0x2] / sizeof(std::uintptr_t)
		CallVFunc<void, 340U>(this);
	}

	void PostThink();
	/// @returns: the player's team or, if coach, coaching team
	[[nodiscard]] int GetAssociatedTeam();
	/// @param[in] bSmokeCheck if true, also check if the trace goes through smoke
	/// @returns: true if @a'pOtherPlayer' is visible to this player at @a'vecEnd' point, false otherwise
	[[nodiscard]] bool IsOtherVisible(const CBasePlayer* pOtherPlayer, const Vector_t& vecEnd, const bool bSmokeCheck = false);
};

// @source: master/game/client/cstrike15/c_cs_player.h
class CCSPlayer : public CBasePlayer
{
public:
	Q_CLASS_NO_INITIALIZER(CCSPlayer);

	// @todo: rem? @ida customplayer offset: client.dll -> ["80 BF ? ? ? ? ? 0F 84 ? ? ? ? 83 BF ? ? ? ? ? 74 7A" + 0x2]
	N_ADD_VARIABLE_OFFSET(CCSGOPlayerAnimState*, GetAnimationState, "DT_CSPlayer::m_bIsScoped", -0x14); // @ida: client.dll -> ["8B 8E ? ? ? ? F3 0F 10 48 ? E8 ? ? ? ? C7" + 0x2]
	N_ADD_VARIABLE(bool, IsScoped, "DT_CSPlayer::m_bIsScoped");
	N_ADD_VARIABLE(bool, IsDefusing, "DT_CSPlayer::m_bIsDefusing");
	N_ADD_VARIABLE(bool, IsGrabbingHostage, "DT_CSPlayer::m_bIsGrabbingHostage");
	N_ADD_VARIABLE(bool, IsRescuing, "DT_CSPlayer::m_bIsRescuing");
	N_ADD_VARIABLE(bool, HasImmunity, "DT_CSPlayer::m_bGunGameImmunity");
	N_ADD_VARIABLE(bool, IsInBuyZone, "DT_CSPlayer::m_bInBuyZone");
	N_ADD_VARIABLE(bool, IsWaitForNoAttack, "DT_CSPlayer::m_bWaitForNoAttack");
	N_ADD_VARIABLE(float, GetLowerBodyYaw, "DT_CSPlayer::m_flLowerBodyYawTarget");
	N_ADD_VARIABLE(bool, IsGhost, "DT_CSPlayer::m_bIsPlayerGhost");
	N_ADD_VARIABLE_OFFSET(bool, IsUsingNewAnimState, "DT_CSPlayer::m_flLastExoJumpTime", 0x8); // @xref: "custom_player"
	N_ADD_VARIABLE_OFFSET(float, GetLastSpawnTimeIndex, "DT_CSPlayer::m_iAddonBits", -0x4); // @ida C_CSPlayer::m_flLastSpawnTimeIndex: (C_CSPlayer::Spawn) client.dll -> ["89 86 ? ? ? ? E8 ? ? ? ? 80 BE" + 0x2]
	N_ADD_VARIABLE(int, GetShotsFired, "DT_CSPlayer::m_iShotsFired");
	N_ADD_VARIABLE(int, GetTotalHits, "DT_CSPlayer::m_totalHitsOnServer");
	N_ADD_VARIABLE(int, GetSurvivalTeam, "DT_CSPlayer::m_nSurvivalTeam");
	N_ADD_VARIABLE(CBaseHandle, GetRagdollHandle, "DT_CSPlayer::m_hRagdoll");
	N_ADD_VARIABLE_OFFSET(float, GetFlashAlpha, "DT_CSPlayer::m_flFlashMaxAlpha", -0x8);
	N_ADD_VARIABLE(float, GetFlashMaxAlpha, "DT_CSPlayer::m_flFlashMaxAlpha");
	N_ADD_VARIABLE(float, GetFlashDuration, "DT_CSPlayer::m_flFlashDuration");
	N_ADD_VARIABLE_OFFSET(int, GetGlowIndex, "DT_CSPlayer::m_flFlashDuration", 0x18);
	N_ADD_VARIABLE(int, GetMoney, "DT_CSPlayer::m_iAccount");
	N_ADD_VARIABLE(bool, HasHelmet, "DT_CSPlayer::m_bHasHelmet");
	N_ADD_VARIABLE(bool, HasHeavyArmor, "DT_CSPlayer::m_bHasHeavyArmor");
	N_ADD_VARIABLE(int, GetArmor, "DT_CSPlayer::m_ArmorValue");
	N_ADD_VARIABLE(QAngle_t, GetEyeAngles, "DT_CSPlayer::m_angEyeAngles");
	N_ADD_VARIABLE(bool, HasDefuser, "DT_CSPlayer::m_bHasDefuser");

	// adjust bones outside of bounding box @note: pseudo name, added since 22.09.2021 (version 1.38.0.2, build 1345)
	void AdjustBonesToBBox(Matrix3x4_t* arrBonesToWorld, int nBoneMask)
	{
		/*
		 * mostly no difference between client-server, uses not networked variables
		 *
		 * @ida C_CSPlayer::AdjustBonesToBBox(): client.dll -> "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38"
		 * @ida CCSPlayer::AdjustBonesToBBox(): server.dll -> "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89"
		 */

		static auto fnAdjustBonesToBBox = reinterpret_cast<void(Q_THISCALL*)(CCSPlayer*, Matrix3x4_t*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38")));
		fnAdjustBonesToBBox(this, arrBonesToWorld, nBoneMask);
	}

	[[nodiscard]] Vector_t GetWeaponShootPosition();
	[[nodiscard]] static CCSPlayer* GetLocalPlayer();
	[[nodiscard]] int GetMaxHealth(); // @todo: find a game way doing this
	/// @returns: true if given @a'iHitGroup' is armored, false otherwise
	[[nodiscard]] bool IsArmored(const int iHitGroup);
	/// @returns: true if @a'pOtherPlayer' is enemy to this player, false otherwise
	[[nodiscard]] bool IsOtherEnemy(CCSPlayer* pOtherPlayer);
	/// @returns: true if player is ready to attack, false otherwise
	[[nodiscard]] bool CanAttack(const float flServerTime);
};

class CCSRagdoll : public CBaseAnimatingOverlay
{
public:
	Q_CLASS_NO_INITIALIZER(CCSRagdoll);

	N_ADD_VARIABLE(CBaseHandle, GetOwnerHandle, "DT_CSRagdoll::m_hPlayer");
};

// @source: master/game/shared/econ/attribute_manager.h
class CAttributeManager
{
public:
	Q_CLASS_NO_INITIALIZER(CAttributeManager);

	N_ADD_PVARIABLE(CEconItemView, GetItem, "DT_AttributeContainer::m_Item");
};

// @source: master/game/shared/econ/econ_entity.h
class CEconEntity : public CBaseFlex
{
public:
	Q_CLASS_NO_INITIALIZER(CEconEntity);

	N_ADD_PVARIABLE(CAttributeManager, GetAttributeManager, "DT_EconEntity::m_AttributeManager");
	N_ADD_VARIABLE(std::uint32_t, GetOriginalOwnerXuidLow, "DT_EconEntity::m_OriginalOwnerXuidLow");
	N_ADD_VARIABLE(std::uint32_t, GetOriginalOwnerXuidHigh, "DT_EconEntity::m_OriginalOwnerXuidHigh");
	N_ADD_VARIABLE(int, GetFallbackPaintKit, "DT_EconEntity::m_nFallbackPaintKit");
	N_ADD_VARIABLE(int, GetFallbackSeed, "DT_EconEntity::m_nFallbackSeed");
	N_ADD_VARIABLE(float, GetFallbackWear, "DT_EconEntity::m_flFallbackWear");
	N_ADD_VARIABLE(int, GetFallbackStatTrak, "DT_EconEntity::m_nFallbackStatTrak");

	[[nodiscard]] std::uint64_t GetOriginalOwnerXuid()
	{
		return (static_cast<std::uint64_t>(this->GetOriginalOwnerXuidHigh()) << 32ULL) | this->GetOriginalOwnerXuidLow();
	}
};

class CBaseAttributableItem : public CEconEntity
{
	Q_CLASS_NO_INITIALIZER(CBaseAttributableItem);
};

// @source: master/game/shared/basecombatweapon_shared.h
class CBaseCombatWeapon : public CBaseAnimating
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseCombatWeapon);

	N_ADD_VARIABLE(float, GetNextPrimaryAttack, "DT_BaseCombatWeapon::m_flNextPrimaryAttack");
	N_ADD_VARIABLE(float, GetNextSecondaryAttack, "DT_BaseCombatWeapon::m_flNextSecondaryAttack");
	N_ADD_VARIABLE(int, GetAmmo, "DT_BaseCombatWeapon::m_iClip1");
	N_ADD_VARIABLE(int, GetAmmoReserve, "DT_BaseCombatWeapon::m_iPrimaryReserveAmmoCount");
	N_ADD_VARIABLE(int, GetViewModelIndex, "DT_BaseCombatWeapon::m_iViewModelIndex");
	N_ADD_VARIABLE(int, GetWorldModelIndex, "DT_BaseCombatWeapon::m_iWorldModelIndex");
	N_ADD_VARIABLE(CBaseHandle, GetWorldModelHandle, "DT_BaseCombatWeapon::m_hWeaponWorldModel");

	N_ADD_DATAFIELD(bool, IsReloading, this->GetPredictionDescMap(), "m_bInReload");
	N_ADD_DATAFIELD(int, GetSubType, this->GetPredictionDescMap(), "m_iSubType");

	CEconItemView* GetEconItemView()
	{
		return reinterpret_cast<CEconEntity*>(this)->GetAttributeManager()->GetItem();
	}
};

// @source: master/game/shared/cstrike15/weapon_csbase.h
class CWeaponCSBase : public CBaseCombatWeapon
{
public:
	Q_CLASS_NO_INITIALIZER(CWeaponCSBase);

	N_ADD_VARIABLE(int, GetWeaponMode, "DT_WeaponCSBase::m_weaponMode");
	N_ADD_VARIABLE(float, GetAccuracyPenalty, "DT_WeaponCSBase::m_fAccuracyPenalty");
	N_ADD_VARIABLE(float, GetRecoilIndex, "DT_WeaponCSBase::m_flRecoilIndex");
	N_ADD_VARIABLE(bool, IsBurstMode, "DT_WeaponCSBase::m_bBurstMode");
	N_ADD_VARIABLE(float, GetPostponeFireReadyTime, "DT_WeaponCSBase::m_flPostponeFireReadyTime");
	N_ADD_VARIABLE(float, GetLastShotTime, "DT_WeaponCSBase::m_fLastShotTime");

	[[nodiscard]] CUtlVector<IRefCounted*>& GetCustomMaterials()
	{
		static const std::uintptr_t uAddress = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(CLIENT_DLL, Q_XOR("83 BE ? ? ? ? ? 7F 67")) + 0x2) - 0xC;
		return *reinterpret_cast<CUtlVector<IRefCounted*>*>(reinterpret_cast<std::uint8_t*>(this) + uAddress);
	}

	[[nodiscard]] bool& IsCustomMaterialInitialized()
	{
		static const std::uintptr_t uAddress = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(CLIENT_DLL, Q_XOR("C6 86 ? ? ? ? ? FF 50 04")) + 0x2);
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uint8_t*>(this) + uAddress);
	}

	[[nodiscard]] Q_INLINE float GetInaccuracy() const
	{
		// @ida C_WeaponCSBase::GetInaccuracy(): client.dll -> "55 8B EC 83 E4 F8 83 EC 18 56 8B F1 57" @xref: 'weapon_accuracy_forcespread', 'weapon_accuracy_nospread'
		return CallVFunc<float, 483U>(this);
	}

	Q_INLINE void UpdateAccuracyPenalty()
	{
		// @ida C_WeaponCSBase::UpdateAccuracyPenalty(): client.dll -> "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 8B 8F" @xref: 'weapon_recoil_decay_coefficient'
		return CallVFunc<void, 484U>(this);
	}
};

// @source: master/game/shared/cstrike15/weapon_csbasegun.h
class CWeaponCSBaseGun : public CWeaponCSBase
{
public:
	Q_CLASS_NO_INITIALIZER(CWeaponCSBaseGun);

	N_ADD_VARIABLE(int, GetZoomLevel, "DT_WeaponCSBaseGun::m_zoomLevel");
	N_ADD_VARIABLE(int, GetBurstShotsRemaining, "DT_WeaponCSBaseGun::m_iBurstShotsRemaining");

	N_ADD_DATAFIELD(float, GetNextBurstShotTime, GetPredictionDescMap(), "m_fNextBurstShot");

	/// @returns: true if weapon can primary attack this tick, false otherwise
	[[nodiscard]] bool CanPrimaryAttack(const int nWeaponType, const float flServerTime);
	/// @returns: true if weapon can alternative attack this tick, false otherwise
	[[nodiscard]] bool CanSecondaryAttack(const int nWeaponType, const float flServerTime);
};

class CTEFireBullets
{
public:
	Q_CLASS_NO_INITIALIZER(CTEFireBullets);

	N_ADD_VARIABLE(int, GetPlayer, "DT_TEFireBullets::m_iPlayer");
	N_ADD_VARIABLE(std::uint16_t, GetItemDefinitionIndex, "DT_TEFireBullets::m_nItemDefIndex");
	N_ADD_VARIABLE(Vector_t, GetOrigin, "DT_TEFireBullets::m_vecOrigin");
	N_ADD_VARIABLE(QAngle_t, GetAngles, "DT_TEFireBullets::m_vecAngles[0]");
	N_ADD_VARIABLE(int, GetWeapon, "DT_TEFireBullets::m_weapon");
	N_ADD_VARIABLE(int, GetWeaponID, "DT_TEFireBullets::m_iWeaponID");
	N_ADD_VARIABLE(int, GetMode, "DT_TEFireBullets::m_iMode");
	N_ADD_VARIABLE(int, GetSeed, "DT_TEFireBullets::m_iSeed");
	N_ADD_VARIABLE(float, GetInaccuracy, "DT_TEFireBullets::m_fInaccuracy");
	N_ADD_VARIABLE(float, GetRecoilIndex, "DT_TEFireBullets::m_flRecoilIndex");
	N_ADD_VARIABLE(float, GetSpread, "DT_TEFireBullets::m_fSpread");
	N_ADD_VARIABLE(int, GetSoundType, "DT_TEFireBullets::m_iSoundType");
};

// @source: master/game/shared/basegrenade_shared.h
class CBaseGrenade : public CBaseAnimating
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseGrenade);

	N_ADD_VARIABLE(int, GetFlags, "DT_BaseGrenade::m_fFlags");
	N_ADD_VARIABLE(Vector_t, GetVelocity, "DT_BaseGrenade::m_vecVelocity");
	N_ADD_VARIABLE(bool, IsLive, "DT_BaseGrenade::m_bIsLive");
	N_ADD_VARIABLE(float, GetDamageRadius, "DT_BaseGrenade::m_DmgRadius");
	N_ADD_VARIABLE(float, GetDamage, "DT_BaseGrenade::m_flDamage");
	N_ADD_VARIABLE(CBaseHandle, GetThrowerHandle, "DT_BaseGrenade::m_hThrower");
};

class CBaseCSGrenade : public CWeaponCSBase
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseCSGrenade);

	N_ADD_VARIABLE(bool, IsPinPulled, "DT_BaseCSGrenade::m_bPinPulled");
	N_ADD_VARIABLE(float, GetThrowTime, "DT_BaseCSGrenade::m_fThrowTime");
	N_ADD_VARIABLE(float, GetThrowStrength, "DT_BaseCSGrenade::m_flThrowStrength");
};

// @source: master/game/shared/cstrike15/basecsgrenade_projectile.h
class CBaseCSGrenadeProjectile : public CBaseGrenade
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseCSGrenadeProjectile);

	N_ADD_VARIABLE(Vector_t, GetInitialVelocity, "DT_BaseCSGrenadeProjectile::m_vInitialVelocity");
	N_ADD_VARIABLE(int, GetBounces, "DT_BaseCSGrenadeProjectile::m_nBounces");
	N_ADD_VARIABLE(int, GetExplodeEffectIndex, "DT_BaseCSGrenadeProjectile::m_nExplodeEffectIndex");
	N_ADD_VARIABLE(int, GetExplodeEffectTickBegin, "DT_BaseCSGrenadeProjectile::m_nExplodeEffectTickBegin");
	N_ADD_VARIABLE(Vector_t, GetExplodeEffectOrigin, "DT_BaseCSGrenadeProjectile::m_vecExplodeEffectOrigin");
};

// @source: master/game/shared/cstrike15/smokegrenade_projectile.h
class CSmokeGrenadeProjectile : public CBaseCSGrenadeProjectile
{
public:
	Q_CLASS_NO_INITIALIZER(CSmokeGrenadeProjectile);

	N_ADD_VARIABLE(int, GetSmokeEffectTickBegin, "DT_SmokeGrenadeProjectile::m_nSmokeEffectTickBegin");
	N_ADD_VARIABLE(bool, DidSmokeEffect, "DT_SmokeGrenadeProjectile::m_bDidSmokeEffect");

	[[nodiscard]] static Q_INLINE float GetMaxTime()
	{
		return 18.f;
	}

	[[nodiscard]] static Q_INLINE float GetRadius()
	{
		/*
		 * smoke grenade radius constant is actually tuned for the bots and not for gameplay.
		 * visualizing smoke will show that it goes up from the emitter by 128 units (fuzzy top),
		 * nothing goes down, and it makes a wide xy-donut with a radius of *128* units (fuzzy edges)
		 */
		return 166.f;
	}
};

// @source: master/game/shared/cstrike15/molotov_projectile.h
class CMolotovProjectile : public CBaseCSGrenadeProjectile
{
public:
	Q_CLASS_NO_INITIALIZER(CMolotovProjectile);

	N_ADD_VARIABLE(int, IsIncendiaryGrenade, "DT_MolotovProjectile::m_bIsIncGrenade");
};

// @source: master/game/client/cstrike15/Effects/clientinferno.h
class CInferno : public CBaseEntity
{
public:
	Q_CLASS_NO_INITIALIZER(CInferno);

	enum
	{
		MAX_INFERNO_FIRES = 64
	};

	N_ADD_VARIABLE(int[MAX_INFERNO_FIRES], GetFireXDelta, "DT_Inferno::m_fireXDelta");
	N_ADD_VARIABLE(int[MAX_INFERNO_FIRES], GetFireYDelta, "DT_Inferno::m_fireYDelta");
	N_ADD_VARIABLE(int[MAX_INFERNO_FIRES], GetFireZDelta, "DT_Inferno::m_fireZDelta");
	N_ADD_VARIABLE(bool[MAX_INFERNO_FIRES], IsFireBurning, "DT_Inferno::m_bFireIsBurning");
	N_ADD_VARIABLE(int, GetFireCount, "DT_Inferno::m_fireCount");
	N_ADD_VARIABLE(int, GetEffectTickBegin, "DT_Inferno::m_nFireEffectTickBegin");

	[[nodiscard]] static float GetMaxTime();
};

// @source: master/game/shared/cstrike15/weapon_c4.h
class CC4 : public CWeaponCSBase
{
public:
	Q_CLASS_NO_INITIALIZER(CC4);

	N_ADD_VARIABLE(bool, IsPlantingViaUse, "DT_WeaponC4::m_bIsPlantingViaUse");
};

// @source: master/game/client/cstrike15/c_plantedc4.h
class CPlantedC4 : public CBaseAnimating
{
public:
	Q_CLASS_NO_INITIALIZER(CPlantedC4);

	N_ADD_VARIABLE(bool, IsBombActive, "DT_PlantedC4::m_bBombTicking");
	N_ADD_VARIABLE(float, GetBlowCountDown, "DT_PlantedC4::m_flC4Blow");
	N_ADD_VARIABLE(float, GetBlowLength, "DT_PlantedC4::m_flTimerLength");
	N_ADD_VARIABLE(float, GetDefuseLength, "DT_PlantedC4::m_flDefuseLength");
	N_ADD_VARIABLE(float, GetDefuseCountDown, "DT_PlantedC4::m_flDefuseCountDown");
	N_ADD_VARIABLE(bool, IsPlanted, "DT_PlantedC4::m_bBombTicking");
	N_ADD_VARIABLE(CBaseHandle, GetDefuserHandle, "DT_PlantedC4::m_hBombDefuser");
	N_ADD_VARIABLE(bool, IsDefused, "DT_PlantedC4::m_bBombDefused");
};

// @source: master/game/shared/econ/econ_item_view.h
class CEconItemView : public CDefaultClientRenderable, public IEconItemInterface, public CCustomMaterialOwner
{
public:
	Q_CLASS_NO_INITIALIZER(CEconItemView);

	N_ADD_VARIABLE(ItemDefinitionIndex_t, GetItemDefinitionIndex, "DT_ScriptCreatedItem::m_iItemDefinitionIndex");
	N_ADD_VARIABLE(std::uint32_t, GetEntityLevel, "DT_ScriptCreatedItem::m_iEntityLevel");
	N_ADD_VARIABLE(std::uint32_t, GetItemIDHigh, "DT_ScriptCreatedItem::m_iItemIDHigh");
	N_ADD_VARIABLE(std::uint32_t, GetItemIDLow, "DT_ScriptCreatedItem::m_iItemIDLow");
	N_ADD_VARIABLE(std::uint32_t, GetAccountID, "DT_ScriptCreatedItem::m_iAccountID");
	N_ADD_VARIABLE(int, GetEntityQuality, "DT_ScriptCreatedItem::m_iEntityQuality");
	N_ADD_VARIABLE(int, IsInitialized, "DT_ScriptCreatedItem::m_bInitialized");
	N_ADD_VARIABLE_OFFSET(CUtlVector<IVisualsDataProcessor*>, GetVisualsDataProcessors, "DT_ScriptCreatedItem::m_bInitialized", 0x1C); // @ida C_EconItemView::m_ppVisualsDataProcessors: client.dll -> ["81 C7 ? ? ? ? 8B 4F 0C 8B 57 04 89 4C" + 0x2] @xref: "Original material not found! Name: %s"
	N_ADD_VARIABLE(char[MAX_ITEM_CUSTOM_NAME_DATABASE_SIZE], GetCustomName, "DT_ScriptCreatedItem::m_szCustomName");
};

// @source: master/game/shared/baseviewmodel_shared.h
class CBaseViewModel : public CBaseAnimating
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseViewModel);

	N_ADD_VARIABLE(CBaseHandle, GetWeaponHandle, "DT_BaseViewModel::m_hWeapon");
	N_ADD_VARIABLE(CBaseHandle, GetOwnerHandle, "DT_BaseViewModel::m_hOwner");

	Q_INLINE void SendViewModelMatchingSequence(int nSequence)
	{
		CallVFunc<void, 247U>(this, nSequence);
	}

	Q_INLINE void SetWeaponModel(const char* szFileName, CBaseCombatWeapon* pWeapon)
	{
		// @ida C_BaseViewModel::SetWeaponModel(): client.dll -> "57 8B F9 8B 97 ? ? ? ? 83 FA FF 74 6A"
		CallVFunc<void, 248U>(this, szFileName, pWeapon);
	}
};

// @source: master/game/shared/basecombatweapon_shared.h
class CBaseWeaponWorldModel : public CBaseAnimatingOverlay
{
public:
	Q_CLASS_NO_INITIALIZER(CBaseWeaponWorldModel);

	N_ADD_VARIABLE_OFFSET(int, GetHoldPlayerAnimations, "DT_BaseWeaponWorldModel::m_hCombatWeaponParent", 0x4);

	/// @returns: true if this world model holds player animations, false otherwise
	[[nodiscard]] Q_INLINE bool IsHoldPlayerAnimations()
	{
		// @ida CBaseWeaponWorldModel::HoldsPlayerAnimations(): server.dll -> "57 8B F9 83 BF ? ? ? ? ? 75 6D"

		if (this->GetHoldPlayerAnimations() == WEAPON_PLAYER_ANIMS_UNKNOWN)
		{
			const CStudioHdr* pStudioHdr = this->GetModelPtr();
			this->GetHoldPlayerAnimations() = (pStudioHdr != nullptr && pStudioHdr->GetSequenceCount() > 2) ? WEAPON_PLAYER_ANIMS_AVAILABLE : WEAPON_PLAYER_ANIMS_NOT_AVAILABLE;
		}

		return (this->GetHoldPlayerAnimations() == WEAPON_PLAYER_ANIMS_AVAILABLE);
	}
};

// @source: master/game/client/cstrike15/c_cs_playerresource.h
class CCSPlayerResource // : public CPlayerResource
{
public:
	Q_CLASS_NO_INITIALIZER(CCSPlayerResource);

	N_ADD_VARIABLE(Vector_t, GetBombsiteCenterA, "DT_CSPlayerResource::m_bombsiteCenterA");
	N_ADD_VARIABLE(Vector_t, GetBombsiteCenterB, "DT_CSPlayerResource::m_bombsiteCenterB");
	N_ADD_VARIABLE(int[MAX_HOSTAGE_RESCUES], GetHostageResqueX, "DT_CSPlayerResource::m_hostageRescueX");
	N_ADD_VARIABLE(int[MAX_HOSTAGE_RESCUES], GetHostageResqueY, "DT_CSPlayerResource::m_hostageRescueY");
	N_ADD_VARIABLE(int[MAX_HOSTAGE_RESCUES], GetHostageResqueZ, "DT_CSPlayerResource::m_hostageRescueZ");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetMVPs, "DT_CSPlayerResource::m_iMVPs");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetArmor, "DT_CSPlayerResource::m_iArmor");
	N_ADD_VARIABLE(bool[MAX_PLAYERS + 1], HasHelmet, "DT_CSPlayerResource::m_bHasHelmet");
	N_ADD_VARIABLE(bool[MAX_PLAYERS + 1], HasDefuser, "DT_CSPlayerResource::m_bHasDefuser");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetScore, "DT_CSPlayerResource::m_iScore");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetCompetitiveRanking, "DT_CSPlayerResource::m_iCompetitiveRanking");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetCompetitiveWins, "DT_CSPlayerResource::m_iCompetitiveWins");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetCompetitiveRankType, "DT_CSPlayerResource::m_iCompetitiveRankType");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetCompetitiveTeammateColor, "DT_CSPlayerResource::m_iCompTeammateColor");
	N_ADD_VARIABLE(char[MAX_PLAYERS + 1][MAX_CLAN_TAG_LENGTH], GetClanTag, "DT_CSPlayerResource::m_szClan");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetActiveCoinRank, "DT_CSPlayerResource::m_nActiveCoinRank");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetMusicID, "DT_CSPlayerResource::m_nMusicID");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetPersonaDataPublicLevel, "DT_CSPlayerResource::m_nPersonaDataPublicLevel");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetPersonaDataPublicCommendsLeader, "DT_CSPlayerResource::m_nPersonaDataPublicCommendsLeader");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetPersonaDataPublicCommendsTeacher, "DT_CSPlayerResource::m_nPersonaDataPublicCommendsTeacher");
	N_ADD_VARIABLE(int[MAX_PLAYERS + 1], GetPersonaDataPublicCommendsFriendly, "DT_CSPlayerResource::m_nPersonaDataPublicCommendsFriendly");
};

// @source: master/game/shared/cstrike15/cs_gamerules.h
class CCSGameRulesProxy : public CBaseEntity
{
public:
	Q_CLASS_NO_INITIALIZER(CCSGameRulesProxy);

	N_ADD_VARIABLE(bool, IsFreezePeriod, "DT_CSGameRulesProxy::m_bFreezePeriod");
	N_ADD_VARIABLE(bool, IsWarmupPeriod, "DT_CSGameRulesProxy::m_bWarmupPeriod");
	N_ADD_VARIABLE(float, GetWarmupPeriodEnd, "DT_CSGameRulesProxy::m_fWarmupPeriodEnd");
	N_ADD_VARIABLE(float, GetWarmupPeriodStart, "DT_CSGameRulesProxy::m_fWarmupPeriodStart");
	N_ADD_VARIABLE(int, GetRoundTime, "DT_CSGameRulesProxy::m_iRoundTime");
	N_ADD_VARIABLE(bool, IsMapHasBombTarget, "DT_CSGameRulesProxy::m_bMapHasBombTarget");
	N_ADD_VARIABLE(bool, IsMapHasRescueZone, "DT_CSGameRulesProxy::m_bMapHasRescueZone");
	N_ADD_VARIABLE(bool, IsMapHasBuyZone, "DT_CSGameRulesProxy::m_bMapHasBuyZone");
	N_ADD_VARIABLE(bool, IsValveDS, "DT_CSGameRulesProxy::m_bIsValveDS");
};

// @source: master/game/client/c_env_tonemap_controller.cpp
class CEnvTonemapController
{
public:
	Q_CLASS_NO_INITIALIZER(CEnvTonemapController);

	N_ADD_VARIABLE(bool, IsUsingCustomAutoExposureMin, "DT_EnvTonemapController::m_bUseCustomAutoExposureMin");
	N_ADD_VARIABLE(bool, IsUsingCustomAutoExposureMax, "DT_EnvTonemapController::m_bUseCustomAutoExposureMax");
	N_ADD_VARIABLE(bool, IsUsingCustomBloomScale, "DT_EnvTonemapController::m_bUseCustomBloomScale");
	N_ADD_VARIABLE(float, GetCustomAutoExposureMin, "DT_EnvTonemapController::m_flCustomAutoExposureMin");
	N_ADD_VARIABLE(float, GetCustomAutoExposureMax, "DT_EnvTonemapController::m_flCustomAutoExposureMax");
	N_ADD_VARIABLE(float, GetCustomBloomScale, "DT_EnvTonemapController::m_flCustomBloomScale");
	N_ADD_VARIABLE(float, GetCustomBloomScaleMin, "DT_EnvTonemapController::m_flCustomBloomScaleMinimum");
	N_ADD_VARIABLE(float, GetBloomExponent, "DT_EnvTonemapController::m_flBloomExponent");
	N_ADD_VARIABLE(float, GetBloomSaturation, "DT_EnvTonemapController::m_flBloomSaturation");
};

// @source: master/game/client/c_env_fog_controller.h
class CFogController : public CBaseEntity
{
public:
	Q_CLASS_NO_INITIALIZER(CFogController);

	N_ADD_VARIABLE(bool, IsEnabled, "DT_FogController::m_fog.enable");
	N_ADD_VARIABLE(Color_t, GetColorPrimary, "DT_FogController::m_fog.colorPrimary");
	N_ADD_VARIABLE(float, GetStart, "DT_FogController::m_fog.start");
	N_ADD_VARIABLE(float, GetEnd, "DT_FogController::m_fog.end");
	N_ADD_VARIABLE(float, GetMaxDensity, "DT_FogController::m_fog.maxdensity");
	N_ADD_VARIABLE(float, GetHDRColorScale, "DT_FogController::m_fog.HDRColorScale");
};

// @source: master/game/client/c_func_breakablesurf.h
class CBreakableSurface : public CBaseEntity //, public IBreakableWithPropData
{
public:
	Q_CLASS_NO_INITIALIZER(CBreakableSurface);

	N_ADD_VARIABLE(bool, IsBroken, "DT_BreakableSurface::m_bIsBroken");
};
