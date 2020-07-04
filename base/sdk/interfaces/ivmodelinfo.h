#pragma once
// used: vector
#include "../datatypes/vector.h"
// used: angle
#include "../datatypes/qangle.h"
// used: matrix
#include "../datatypes/matrix.h"
// used: bone masks, studio classes
#include "../studio.h"
// used: trace, cplane
#include "ienginetrace.h"

#pragma region modelinfo_enumerations
enum EBoneIndex : int
{
	BONE_INVALID = -1,
	BONE_PELVIS,
	BONE_LEAN_ROOT,
	BONE_CAM_DRIVER,
	BONE_SPINE_0,
	BONE_SPINE_1,
	BONE_SPINE_2,
	BONE_SPINE_3,
	BONE_NECK,
	BONE_HEAD,
	BONE_CLAVICLE_L,
	BONE_ARM_UPPER_L,
	BONE_ARM_LOWER_L,
	BONE_HAND_L,
	BONE_FINGER_MIDDLE_META_L,
	BONE_FINGER_MIDDLE_0_L,
	BONE_FINGER_MIDDLE_1_L,
	BONE_FINGER_MIDDLE_2_L,
	BONE_FINGER_PINKY_META_L,
	BONE_FINGER_PINKY_0_L,
	BONE_FINGER_PINKY_1_L,
	BONE_FINGER_PINKY_2_L,
	BONE_FINGER_INDEX_META_L,
	BONE_FINGER_INDEX_0_L,
	BONE_FINGER_INDEX_1_L,
	BONE_FINGER_INDEX_2_L,
	BONE_FINGER_THUMB_0_L,
	BONE_FINGER_THUMB_1_L,
	BONE_FINGER_THUMB_2_L,
	BONE_FINGER_RING_META_L,
	BONE_FINGER_RING_0_L,
	BONE_FINGER_RING_1_L,
	BONE_FINGER_RING_2_L,
	BONE_WEAPON_HAND_L,
	BONE_ARM_LOWER_L_TWIST,
	BONE_ARM_LOWER_L_TWIST1,
	BONE_ARM_UPPER_L_TWIST,
	BONE_ARM_UPPER_L_TWIST1,
	BONE_CLAVICLE_R,
	BONE_ARM_UPPER_R,
	BONE_ARM_LOWER_R,
	BONE_HAND_R,
	BONE_FINGER_MIDDLE_META_R,
	BONE_FINGER_MIDDLE_0_R,
	BONE_FINGER_MIDDLE_1_R,
	BONE_FINGER_MIDDLE_2_R,
	BONE_FINGER_PINKY_META_R,
	BONE_FINGER_PINKY_0_R,
	BONE_FINGER_PINKY_1_R,
	BONE_FINGER_PINKY_2_R,
	BONE_FINGER_INDEX_META_R,
	BONE_FINGER_INDEX_0_R,
	BONE_FINGER_INDEX_1_R,
	BONE_FINGER_INDEX_2_R,
	BONE_FINGER_THUMB_0_R,
	BONE_FINGER_THUMB_1_R,
	BONE_FINGER_THUMB_2_R,
	BONE_FINGER_RING_META_R,
	BONE_FINGER_RING_0_R,
	BONE_FINGER_RING_1_R,
	BONE_FINGER_RING_2_R,
	BONE_WEAPON_HAND_R,
	BONE_ARM_LOWER_R_TWIST,
	BONE_ARM_LOWER_R_TWIST1,
	BONE_ARM_UPPER_R_TWIST,
	BONE_ARM_UPPER_R_TWIST1,
	BONE_LEG_UPPER_L,
	BONE_LEG_LOWER_L,
	BONE_ANKLE_L,
	BONE_BALL_L,
	BONE_LFOOT_LOCK,
	BONE_LEG_UPPER_L_TWIST,
	BONE_LEG_UPPER_L_TWIST1,
	BONE_LEG_UPPER_R,
	BONE_LEG_LOWER_R,
	BONE_ANKLE_R,
	BONE_BALL_R,
	BONE_RFOOT_LOCK,
	BONE_LEG_UPPER_R_TWIST,
	BONE_LEG_UPPER_R_TWIST1,
	BONE_FINGER_PINKY_L_END,
	BONE_FINGER_PINKY_R_END,
	BONE_VALVEBIPED_WEAPON_BONE,
	BONE_LH_IK_DRIVER,
	BONE_PRIMARY_JIGGLE_JNT,
};

enum EHitboxIndex : int
{
	HITBOX_INVALID = -1,
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_STOMACH,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

enum ERenderFlags : unsigned int
{
	RENDER_FLAGS_DISABLE_RENDERING = 0x01,
	RENDER_FLAGS_HASCHANGED = 0x02,
	RENDER_FLAGS_ALTERNATE_SORTING = 0x04,
	RENDER_FLAGS_RENDER_WITH_VIEWMODELS = 0x08,
	RENDER_FLAGS_BLOAT_BOUNDS = 0x10,
	RENDER_FLAGS_BOUNDS_VALID = 0x20,
	RENDER_FLAGS_BOUNDS_ALWAYS_RECOMPUTE = 0x40,
	RENDER_FLAGS_IS_SPRITE = 0x80,
	RENDER_FLAGS_FORCE_OPAQUE_PASS = 0x100,
};

enum ETranslucencyType : int
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS
};
#pragma endregion

using MDLHandle_t = std::uint16_t;

struct Model_t
{
	std::byte	pad0[0x4];		//0x0000
	char		szName[260];	//0x0004
	int			nLoadFlags;		//0x0108
	int			nServerCount;	//0x010C
	int			iType;			//0x0110
	int			uFlags;			//0x0114
	Vector		vecMins;		//0x0118
	Vector		vecMaxs;		//0x0124
	float		flRadius;		//0x0130
	std::byte	pad1[0x1C];		//0x0134
};

class CPhysCollide;
class IMaterial;
class CUtlBuffer;
class IClientRenderable;

class IVModelInfo
{
public:
	virtual							~IVModelInfo() { }
	virtual const Model_t*			GetModel(int nModelIndex) const = 0;
	virtual int						GetModelIndex(const char* szName) const = 0;
	virtual const char*				GetModelName(const Model_t* pModel) const = 0;
	virtual vcollide_t*				GetVCollide(const Model_t* pModel) const = 0;
	virtual vcollide_t*				GetVCollide(int nModelIndex) const = 0;
	virtual vcollide_t*				GetVCollide(const Model_t* pModel, float flScale) const = 0;
	virtual vcollide_t*				GetVCollide(int nModelIndex, float flScale) const = 0;
	virtual void					GetModelBounds(const Model_t* pModel, Vector& mins, Vector& maxs) const = 0;
	virtual void					GetModelRenderBounds(const Model_t* pModel, Vector& mins, Vector& maxs) const = 0;
	virtual int						GetModelFrameCount(const Model_t* pModel) const = 0;
	virtual int						GetModelType(const Model_t* pModel) const = 0;
	virtual void*					GetModelExtraData(const Model_t* pModel) = 0;
	virtual bool					ModelHasMaterialProxy(const Model_t* pModel) const = 0;
	virtual bool					IsTranslucent(Model_t const* pModel) const = 0;
	virtual bool					IsTranslucentTwoPass(const Model_t* pModel) const = 0;
private:
	virtual void					unused0() { }
public:
	virtual ETranslucencyType		ComputeTranslucencyType(const Model_t* pModel, int nSkin, int nBody) = 0;
	virtual int						GetModelMaterialCount(const Model_t* pModel) const = 0;
	virtual void					GetModelMaterials(const Model_t* pModel, int iCount, IMaterial** ppMaterials) = 0;
	virtual bool					IsModelVertexLit(const Model_t* pModel) const = 0;
	virtual const char*				GetModelKeyValueText(const Model_t* pModel) = 0;
	virtual bool					GetModelKeyValue(const Model_t* pModel, CUtlBuffer& buf) = 0;
	virtual float					GetModelRadius(const Model_t* pModel) = 0;
	virtual const studiohdr_t*		FindModel(const studiohdr_t* pStudioHdr, void** ppCache, const char* szModelName) const = 0;
	virtual const studiohdr_t*		FindModel(void* pCache) const = 0;
	virtual virtualmodel_t*			GetVirtualModel(const studiohdr_t* pStudioHdr) const = 0;
	virtual std::uint8_t*			GetAnimBlock(const studiohdr_t* pStudioHdr, int iBlock) const = 0;
	virtual bool					HasAnimBlockBeenPreloaded(studiohdr_t const* pStudioHdr, int iBlock) const = 0;
	virtual void					GetModelMaterialColorAndLighting(const Model_t* pModel, const Vector& vecOrigin, const QAngle& angles, Trace_t* pTrace, Vector& vecLighting, Vector& matColor) = 0;
	virtual void					GetIlluminationPoint(const Model_t* pModel, IClientRenderable* pRenderable, const Vector& vecOrigin, const QAngle& angles, Vector* pLightingCenter) = 0;
	virtual int						GetModelContents(int nModelIndex) const = 0;
	virtual studiohdr_t*			GetStudioModel(const Model_t* pModel) = 0;
	virtual int						GetModelSpriteWidth(const Model_t* pModel) const = 0;
	virtual int						GetModelSpriteHeight(const Model_t* pModel) const = 0;
	virtual void					SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual void					GetLevelScreenFadeRange(float* pMinArea, float* pMaxArea) const = 0;
	virtual void					SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual unsigned char			ComputeLevelScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual unsigned char			ComputeViewScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual int						GetAutoplayList(const studiohdr_t* pStudioHdr, unsigned short** pAutoplayList) const = 0;
	virtual CPhysCollide*			GetCollideForVirtualTerrain(int nIndex) = 0;
	virtual bool					IsUsingFBTexture(const Model_t* pModel, int nSkin, int nBody, IClientRenderable* pClientRenderable) const = 0;
	virtual const Model_t*			FindOrLoadModel(const char* szName) const = 0;
	virtual MDLHandle_t				GetCacheHandle(const Model_t* pModel) const = 0;
	virtual int						GetBrushModelPlaneCount(const Model_t* pModel) const = 0;
	virtual void					GetBrushModelPlane(const Model_t* pModel, int nIndex, cplane_t& plane, Vector* pOrigin) const = 0;
	virtual int						GetSurfacepropsForVirtualTerrain(int nIndex) = 0;
	virtual bool					UsesEnvCubemap(const Model_t* pModel) const = 0;
	virtual bool					UsesStaticLighting(const Model_t* pModel) const = 0;
	virtual int						RegisterDynamicModel(const char* name, bool bClientSide) = 0;
	virtual int						RegisterCombinedDynamicModel(const char* pszName, MDLHandle_t Handle) = 0;
	virtual void					UpdateCombinedDynamicModel(int nModelIndex, MDLHandle_t Handle) = 0;
	virtual int						BeginCombinedModel(const char* pszName, bool bReuseExisting) = 0;
};
