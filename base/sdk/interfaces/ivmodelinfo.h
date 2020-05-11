#pragma once
// used: vector
#include "../datatypes/vector.h"
// used: angle
#include "../datatypes/qangle.h"
// used: matrix
#include "../datatypes/matrix.h"
// used: trace, cplane
#include "ienginetrace.h"

#pragma region modelinfo_definitions
#define MAXSTUDIOBONECTRLS			4
#define MAXSTUDIOPOSEPARAM			24
#define MAXSTUDIOSKINS				32
#define MAXSTUDIOFLEXCTRL			96
#define MAXSTUDIOBONES				128
#define MAXSTUDIOANIMBLOCKS			256
#define MAXSTUDIOFLEXDESC			1024

#define BONE_CALCULATE_MASK			0x1F
#define BONE_PHYSICALLY_SIMULATED	0x01
#define BONE_PHYSICS_PROCEDURAL		0x02
#define BONE_ALWAYS_PROCEDURAL		0x04
#define BONE_SCREEN_ALIGN_SPHERE	0x08
#define BONE_SCREEN_ALIGN_CYLINDER	0x10

#define BONE_USED_MASK				0x0007FF00
#define BONE_USED_BY_ANYTHING		0x0007FF00
#define BONE_USED_BY_HITBOX			0x00000100
#define BONE_USED_BY_ATTACHMENT		0x00000200
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400
#define BONE_USED_BY_VERTEX_LOD1	0x00000800
#define BONE_USED_BY_VERTEX_LOD2	0x00001000
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS				8

#define BONE_TYPE_MASK				0x00F00000
#define BONE_FIXED_ALIGNMENT		0x00100000
#define BONE_HAS_SAVEFRAME_POS		0x00200000
#define BONE_HAS_SAVEFRAME_ROT64	0x00400000
#define BONE_HAS_SAVEFRAME_ROT32	0x00800000
#pragma endregion

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
using RadianEuler = float[3];
using Quaternion = float[4];

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

struct mstudiobone_t
{
	int	nNameIndex;
	const char* GetName()
	{
		if (!nNameIndex) return nullptr;
		return (const char*)((std::uint8_t*)this + nNameIndex);
	}

	int	iParent; // parent bone
	int	iBoneController[6]; // bone controller index, -1 == none

	// default values
	Vector vecPosition;
	Quaternion qQuat;
	RadianEuler radRot;

	Vector vecPositionScale;
	Vector vecEulerScale;

	matrix3x4_t matPoseToBone;
	Quaternion qAlignment;
	int	iFlags;
	int	iProcType;
	int	nProcIndex; // procedural rule
	void* Procedure() const
	{
		if (!nProcIndex) return nullptr;
		return (void*)((std::uint8_t*)this + nProcIndex);
	}

	mutable int miPhysicsBone; // index into physically simulated bone
	int nSurfacePropIndex; // index into string tablefor property name
	const char* GetSurfacePropName()
	{
		if (!nSurfacePropIndex) return nullptr;
		return (const char*)((std::uint8_t*)this + nSurfacePropIndex);
	}

	int iContents; // See BSPFlags.h for the contents flags
	int iSurfaceProp; // this index must be cached by the loader, not saved in the file
	int unused[7]; // remove as appropriate
};

struct mstudiobonecontroller_t
{
	int		iBone; // -1 == 0
	int		iType; // X, Y, Z, XR, YR, ZR, M
	float	flStart;
	float	flEnd;
	int		iRest; // byte index value at rest
	int		iInputField; // 0-3 user set controller, 4 mouth
	int		unused[8];
};

struct mstudiobbox_t
{
	int			iBone;
	int			iGroup; // intersection group
	Vector		vecBBMin; // bounding box
	Vector		vecBBMax;
	int			nHitboxNameIndex; // offset to the name of the hitbox
	std::byte	pad0[0xC];
	float		flRadius;
	std::byte	pad1[0x10];

	inline const char* GetHitboxName()
	{
		if (!nHitboxNameIndex) return nullptr;
		return (const char*)((std::uint8_t*)this + nHitboxNameIndex);
	}
};

struct mstudiohitboxset_t
{
	int nNameIndex;
	int nHitboxes;
	int nHitboxIndex;

	inline  const char* GetName() const
	{
		if (!nNameIndex) return nullptr;
		return (const char*)((std::uint8_t*)this + nNameIndex);
	}

	inline mstudiobbox_t* GetHitbox(int i) const
	{
		if (i > nHitboxes) return nullptr;
		return (mstudiobbox_t*)((std::uint8_t*)this + nHitboxIndex) + i;
	}
};

struct mstudioseqdesc_t
{
	int	nLabelIndex;
	inline const char* GetLabel() const
	{
		if (!nLabelIndex) return nullptr;
		return (const char*)((std::uint8_t*)this + nLabelIndex);
	}

	int	nActivityNameIndex;
	inline const char* GetActivityName() const
	{
		if (!nActivityNameIndex) return nullptr;
		return (const char*)((std::uint8_t*)this + nActivityNameIndex);
	}

	int	iFlags;

	int	iActivity;
	int	iActivityWeight;

	int nEvents;
	int nEventIndex;
	inline void* Event(int i) const
	{
		return ((std::uint8_t*)this + nEventIndex) + i;
	}

	Vector vecBBMin;
	Vector vecBBMax;

	int	nBlends;
	int	nBlendIndex;
	inline unsigned short* GetBlend(int i) const
	{
		return nBlends != 0 ? (unsigned short*)((std::uint8_t*)this + nBlendIndex) + i : NULL;
	}

	int	nSequenceGroup; // sequence group for demand loading
	int iGroupSize[2];
	inline unsigned short& GetAnimValue(int nIndex0, int nIndex1) const
	{
		if (nIndex0 >= iGroupSize[0])
			nIndex0 = iGroupSize[0] - 1;

		if (nIndex1 >= iGroupSize[1])
			nIndex1 = iGroupSize[1] - 1;

		return *GetBlend(nIndex1 * iGroupSize[0] + nIndex0);
	}

	int nParamIndex[2];		// X, Y, Z, XR, YR, ZR
	float flParamStart[2];	// local (0..1) starting value
	float flParamEnd[2];	// local (0..1) ending value
	int	iParamParent;

	float flFadeInTime;		// ideal cross fate in time (0.2 default)
	float flFadeOutTime;	// ideal cross fade out time (0.2 default)

	int	iEntryNode;			// transition node at entry
	int	iExitNode;			// transition node at exit
	int	iNodeFlags;			// transition rules

	float flEntryPhase;		// used to match entry gait
	float flExitPhase;		// used to match exit gait

	float flLastFrame;		// frame that should generation EndOfSequence

	int	iNextSequence;		// auto advancing sequences
	int	iPose;				// index of delta animation between end and nextseq

	int	nIKRules;

	int	nAutoLayers;
	int	nAutoLayerIndex;
	inline void* GetAutoLayer(int i) const
	{
		return ((std::uint8_t*)this + nAutoLayerIndex) + i;
	}

	int	nWeightListIndex;
	inline float* GetBoneWeight(int i) const
	{
		return ((float*)((std::uint8_t*)this + nWeightListIndex) + i);
	}

	int	nPoseKeyIndex;
	inline float* GetPoseKey(int iParam, int iAnim) const
	{
		return (float*)((std::uint8_t*)this + nPoseKeyIndex) + iParam * iGroupSize[0] + iAnim;
	}

	int	nIKLocks;
	int	nIKLockIndex;
	inline void* IKLock(int i) const
	{
		if (i > nIKLocks) return nullptr;
		return ((std::uint8_t*)this + nIKLockIndex) + i;
	}

	int	nKeyValueIndex;
	int	iKeyValueSize;
	inline const char* KeyValueText() const
	{
		if (!iKeyValueSize) return nullptr;
		return (const char*)((std::uint8_t*)this + nKeyValueIndex);
	}

	int	unused[3];
};

struct studiohwdata_t;
struct studiohdr_t
{
	int	nIndex;
	int	iVersion;
	long lChecksum;
	char szName[64];
	int	iLength;

	Vector vecEyePosition;
	Vector vecIllumPosition;
	Vector vecHullMin;
	Vector vecHullMax;
	Vector vecViewBBMin;
	Vector vecViewBBMax;

	int	iFlags;

	int	nBones;
	int	nBoneIndex;
	inline mstudiobone_t* GetBone(int i) const
	{
		if (i > nBones) return nullptr;
		return (mstudiobone_t*)((std::uint8_t*)this + nBoneIndex) + i;
	}

	int	nBoneControllers;
	int	nBoneControllerIndex;
	inline void* GetBoneController(int i) const
	{
		if (i > nBoneControllers) return nullptr;
		return ((std::uint8_t*)this + nBoneControllerIndex) + i;
	}

	int	nHitboxSets;
	int	nHitboxSetIndex;
	inline mstudiohitboxset_t* GetHitboxSet(int i) const
	{
		if (i > nHitboxSets) return nullptr;
		return (mstudiohitboxset_t*)((std::uint8_t*)this + nHitboxSetIndex) + i;
	}

	mstudiobbox_t* GetHitbox(int i, int set) const
	{
		const mstudiohitboxset_t* pHitboxSet = GetHitboxSet(set);

		if (pHitboxSet == nullptr)
			return nullptr;

		return pHitboxSet->GetHitbox(i);
	}

	int GetHitboxCount(int set) const
	{
		const mstudiohitboxset_t* pHitboxSet = GetHitboxSet(set);

		if (pHitboxSet == nullptr)
			return 0;

		return pHitboxSet->nHitboxes;
	}

	int	nAnimations;
	int	nAnimDescIndex;
	inline void* GetAnimDescription(int i) const
	{
		if (i > nAnimations) return nullptr;
		return ((std::uint8_t*)this + nAnimDescIndex) + i;
	}

	int nAnimGroups;
	int nAnimGroupIndex;
	inline void* GetAnimGroup(int i) const
	{
		if (i > nAnimGroups) return nullptr;
		return ((std::uint8_t*)this + nAnimGroupIndex) + i;
	}

	int nBoneDescs;
	int nBoneDescIndex;
	inline void* GetBoneDescription(int i) const
	{
		if (i > nBoneDescs) return nullptr;
		return ((std::uint8_t*)this + nBoneDescIndex) + i;
	}

	int	nSequences;
	int	nSequenceIndex;
	inline mstudioseqdesc_t* GetSequenceDescription(int i) const
	{
		if (i < 0 || i >= nSequences) return nullptr;
		return (mstudioseqdesc_t*)((std::uint8_t*)this + nSequenceIndex) + i;
	}

	int	nSequencesIndexed;

	int	nSequenceGroups;
	int	nSequenceGroupIndex;
	inline void* GetSequenceGroup(int i) const
	{
		if (i > nSequenceGroups) return nullptr;
		return ((std::uint8_t*)this + nSequenceGroupIndex) + i;
	}

	int	nTextures;
	int	nTextureIndex;
	inline void* GetTexture(int i) const
	{
		if (i > nTextures) return nullptr;
		return ((std::uint8_t*)this + nTextureIndex) + i;
	}

	int	nCdTextures;
	int	nCdTextureIndex;
	inline char* GetCdTexture(int i) const
	{
		if (i > nCdTextures) return nullptr;
		return ((char*)this + *(int*)((std::uint8_t*)this + nCdTextureIndex) + i);
	}

	int	nSkinRefs;
	int	nSkinFamilies;
	int	nSkinIndex;
	inline short* GetSkinRef(int i) const
	{
		if (i > nSkinRefs) return nullptr;
		return (short*)((std::uint8_t*)this + nSkinIndex) + i;
	}

	int	nBodyParts;
	int	nBodyPartIndex;
	inline void* GetBodyPart(int i) const
	{
		if (i > nBodyParts) return nullptr;
		return ((std::uint8_t*)this + nBodyPartIndex) + i;
	}

	int	nAttachments;
	int	nAttachmentIndex;
	inline void* GetAttachment(int i) const
	{
		if (i > nAttachments) return nullptr;
		return ((std::uint8_t*)this + nAttachmentIndex) + i;
	}

	int	nTransitions;
	int	nTransitionIndex;
	inline std::uint8_t* GetTransition(int i) const
	{
		if (i > nTransitions) return nullptr;
		return (std::uint8_t*)((std::uint8_t*)this + nTransitionIndex) + i;
	}

	int	nFlexDescs;
	int	nFlexDescIndex;
	inline void* GetFlexDescription(int i) const
	{
		if (i > nFlexDescs) return nullptr;
		return ((std::uint8_t*)this + nFlexDescIndex) + i;
	}

	int	nFlexControllers;
	int	nFlexControllerIndex;
	inline void* GetFlexController(int i) const
	{
		if (i > nFlexControllers) return nullptr;
		return ((std::uint8_t*)this + nFlexControllerIndex) + i;
	}

	int	nFlexRules;
	int	nFlexRuleIndex;
	inline void* GetFlexRule(int i) const
	{
		if (i > nFlexRules) return nullptr;
		return ((std::uint8_t*)this + nFlexRuleIndex) + i;
	}

	int	nIkChains;
	int	nIkChainIndex;
	inline void* GetIKChain(int i) const
	{
		if (i > nIkChains) return nullptr;
		return ((std::uint8_t*)this + nIkChainIndex) + i;
	}

	int	nMouths;
	int	nMouthIndex;
	inline void* GetMouth(int i) const
	{
		if (i > nMouths) return nullptr;
		return ((std::uint8_t*)this + nMouthIndex) + i;
	}

	int	nPoseParameters;
	int	nPoseParameterIndex;
	inline void* GetPoseParameter(int i) const
	{
		if (i > nPoseParameters) return nullptr;
		return ((std::uint8_t*)this + nPoseParameterIndex) + i;
	}

	int	nSurfacePropIndex;
	inline const char* GetSurfaceProp() const
	{
		if (!nSurfacePropIndex) return nullptr;
		return ((char*)this) + nSurfacePropIndex;
	}

	int	nKeyValueIndex;
	int	nKeyValueSize;
	inline const char* KeyValueText() const
	{
		if (!nKeyValueSize) return nullptr;
		return ((char*)this) + nKeyValueIndex;
	}

	int	nIkAutoplayLocks;
	int	nIkAutoplayLockIndex;
	inline void* GetIKAutoplayLock(int i) const
	{
		if (i > nIkAutoplayLocks) return nullptr;
		return ((std::uint8_t*)this + nIkAutoplayLockIndex) + i;
	}
};

class	CPhysCollide;
class	IMaterial;
class	CUtlBuffer;
class	IClientRenderable;

struct	vcollide_t;
struct	virtualmodel_t;
struct	vertexfileheader_t;

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
	virtual BYTE*					GetAnimBlock(const studiohdr_t* pStudioHdr, int iBlock) const = 0;
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
