#pragma once
#include "../datatypes/keyvalues.h"

// @source: master/public/engine/ivmodelinfo.h

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

enum ERenderableTranslucencyType : int
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS
};
#pragma endregion

// forward declarations
struct vcollide_t;
struct virtualmodel_t;
struct studiohdr_t;

using MDLHandle_t = std::uint16_t;

#pragma pack(push, 4)
struct BrushData_t
{
	void* pShared; // 0x00
	int iFirstModelSurface; // 0x04
	int nModelSurfaceCount; // 0x08
	int nLightstyleLastComputedFrame; // 0x0C
	unsigned short nLightstyleIndex; // 0x10
	unsigned short nLightstyleCount; // 0x12
	unsigned short hRender; // 0x14
	unsigned short hFirstNode; // 0x16
};
static_assert(sizeof(BrushData_t) == 0x18);

// only models with type "mod_sprite" have this data
struct SpriteData_t
{
	int nFrames; // 0x00
	int iWidth; // 0x04
	int iHeight; // 0x08
	void* pSprite; // 0x0C
};
static_assert(sizeof(SpriteData_t) == 0x10);

struct Model_t
{
	FileNameHandle_t hFileName; // 0x0000
	char szPathName[260]; // 0x0004
	int nLoadFlags; // 0x0108
	int nServerCount; // 0x010C
	int nType; // 0x0110
	int iFlags; // 0x0114
	Vector_t vecMins; // 0x0118
	Vector_t vecMaxs; // 0x0124
	float flRadius; // 0x0130
	CKeyValues* pKeyValues; // 0x0134
	union
	{
		MDLHandle_t hStudio;
		BrushData_t brush;
		SpriteData_t sprite;
	}; // 0x0138
};
static_assert(sizeof(Model_t) == 0x150);
#pragma pack(pop)

class IVModelInfo : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	[[nodiscard]] const Model_t* GetModel(int nModelIndex) const
	{
		return CallVFunc<const Model_t*, 1U>(this, nModelIndex);
	}

	[[nodiscard]] int GetModelIndex(const char* szName) const
	{
		return CallVFunc<int, 2U>(this, szName);
	}

	[[nodiscard]] const char* GetModelName(const Model_t* pModel) const
	{
		return CallVFunc<const char*, 3U>(this, pModel);
	}

	[[nodiscard]] vcollide_t* GetVCollide(const Model_t* pModel) const
	{
		return CallVFunc<vcollide_t*, 4U>(this, pModel);
	}

	[[nodiscard]] vcollide_t* GetVCollide(int nModelIndex) const
	{
		return CallVFunc<vcollide_t*, 5U>(this, nModelIndex);
	}

	[[nodiscard]] vcollide_t* GetVCollide(const Model_t* pModel, float flScale) const
	{
		return CallVFunc<vcollide_t*, 6U>(this, pModel, flScale);
	}

	[[nodiscard]] vcollide_t* GetVCollide(int nModelIndex, float flScale) const
	{
		return CallVFunc<vcollide_t*, 7U>(this, nModelIndex, flScale);
	}

	void GetModelBounds(const Model_t* pModel, Vector_t& vecMins, Vector_t& vecMaxs) const
	{
		CallVFunc<void, 8U>(this, pModel, &vecMins, &vecMaxs);
	}

	void GetModelRenderBounds(const Model_t* pModel, Vector_t& vecMins, Vector_t& vecMaxs) const
	{
		CallVFunc<void, 9U>(this, pModel, &vecMins, &vecMaxs);
	}

	[[nodiscard]] const studiohdr_t* FindModel(const studiohdr_t* pStudioHdr, void** ppCache, const char* szModelName) const
	{
		return CallVFunc<const studiohdr_t*, 24U>(this, pStudioHdr, ppCache, szModelName);
	}

	[[nodiscard]] const studiohdr_t* FindModel(void* pCache) const
	{
		return CallVFunc<const studiohdr_t*, 25U>(this, pCache);
	}

	[[nodiscard]] virtualmodel_t* GetVirtualModel(const studiohdr_t* pStudioHdr) const
	{
		return CallVFunc<virtualmodel_t*, 26U>(this, pStudioHdr);
	}

	[[nodiscard]] std::uint8_t* GetAnimBlock(const studiohdr_t* pStudioHdr, int iBlock) const
	{
		return CallVFunc<std::uint8_t*, 27U>(this, pStudioHdr, iBlock);
	}

	[[nodiscard]] bool HasAnimBlockBeenPreloaded(const studiohdr_t* pStudioHdr, int iBlock) const
	{
		return CallVFunc<bool, 28U>(this, pStudioHdr, iBlock);
	}

	[[nodiscard]] int GetModelContents(int nModelIndex) const
	{
		return CallVFunc<int, 31U>(this, nModelIndex);
	}

	[[nodiscard]] studiohdr_t* GetStudioModel(const Model_t* pModel)
	{
		return CallVFunc<studiohdr_t*, 32U>(this, pModel);
	}

	const Model_t* FindOrLoadModel(const char* szName) const
	{
		return CallVFunc<const Model_t*, 43U>(this, szName);
	}
};
