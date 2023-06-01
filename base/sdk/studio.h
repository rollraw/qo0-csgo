#pragma once
#include "datatypes/qangle.h"
#include "datatypes/matrix.h"
#include "datatypes/utldict.h"
#include "datatypes/utlvector.h"
#include "datatypes/quaternion.h"

// used: cthreadfastmutex
#include "thread.h"

using RadianEuler_t = float[3];

// @source: master/public/studio.h

#pragma region studio_definitions
#define MAX_NUM_LODS 8
#define BONE_USED_BY_VERTEX_AT_LOD(LOD) (BONE_USED_BY_VERTEX_LOD0 << (LOD))
#define BONE_USED_BY_ANYTHING_AT_LOD(LOD) ((BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK) | BONE_USED_BY_VERTEX_AT_LOD(LOD))
#pragma endregion

#pragma region studio_enumerations
enum : int
{
	MAXSTUDIOBONECTRLS = 4,
	MAXSTUDIOPOSEPARAM = 24,
	MAXSTUDIOSKINS = 32, // total textures
	MAXSTUDIOFLEXCTRL = 96, // maximum number of flexcontrollers (input sliders)
	MAXSTUDIOBONES = 256, // total bones actually used // @ida CBoneMergeCache::BuildMatricesWithBoneMerge(): server.dll -> ["68 ? ? ? ? 8D 85 ? ? ? ? 6A 00 50 E8 ? ? ? ? 33" + 0x1]
	MAXSTUDIOANIMBLOCKS = 256,
	MAXSTUDIOFLEXDESC = 1024 // maximum number of low level flexes (actual morph targets)
};

enum EStudioProcedure : int
{
	STUDIO_PROC_AXISINTERP = 1,
	STUDIO_PROC_QUATINTERP,
	STUDIO_PROC_AIMATBONE,
	STUDIO_PROC_AIMATATTACH,
	STUDIO_PROC_JIGGLE,
	STUDIO_PROC_TWIST_MASTER,
	STUDIO_PROC_TWIST_SLAVE,
	STUDIO_PROC_POINT_CONSTRAINT,
	STUDIO_PROC_ORIENT_CONSTRAINT,
	STUDIO_PROC_AIM_CONSTRAINT,
	STUDIO_PROC_IK_CONSTRAINT,
	STUDIO_PROC_PARENT_CONSTRAINT,
	STUDIO_PROC_SOFTBODY = 16
};

enum EStudioBoneFlags : int
{
	BONE_PHYSICALLY_SIMULATED = (1 << 0), // bone is physically simulated when physics are active
	BONE_PHYSICS_PROCEDURAL = (1 << 1), // bone is procedural when physics is active
	BONE_ALWAYS_PROCEDURAL = (1 << 2), // bone is always procedurally animated
	BONE_SCREEN_ALIGN_SPHERE = (1 << 3), // bone aligns to the screen, not constrained in motion
	BONE_SCREEN_ALIGN_CYLINDER = (1 << 4), // bone aligns to the screen, constrained by it's own axis
	BONE_WORLD_ALIGN = (1 << 5) // bone is rigidly aligned to the world (but can still translate)
};

enum ESetupBoneFlags : int
{
	BONE_USED_BY_HITBOX = (1 << 8),
	BONE_USED_BY_ATTACHMENT = (1 << 9),
	BONE_USED_BY_VERTEX_LOD0 = (1 << 10),
	BONE_USED_BY_VERTEX_LOD1 = (1 << 11),
	BONE_USED_BY_VERTEX_LOD2 = (1 << 12),
	BONE_USED_BY_VERTEX_LOD3 = (1 << 13),
	BONE_USED_BY_VERTEX_LOD4 = (1 << 14),
	BONE_USED_BY_VERTEX_LOD5 = (1 << 15),
	BONE_USED_BY_VERTEX_LOD6 = (1 << 16),
	BONE_USED_BY_VERTEX_LOD7 = (1 << 17),
	BONE_USED_BY_VERTEX_MASK = (BONE_USED_BY_VERTEX_LOD0 | BONE_USED_BY_VERTEX_LOD1 | BONE_USED_BY_VERTEX_LOD2 | BONE_USED_BY_VERTEX_LOD3 | BONE_USED_BY_VERTEX_LOD4 | BONE_USED_BY_VERTEX_LOD5 | BONE_USED_BY_VERTEX_LOD6 | BONE_USED_BY_VERTEX_LOD7),
	BONE_USED_BY_BONE_MERGE = (1 << 18),
	BONE_USED_BY_ANYTHING = (BONE_USED_BY_HITBOX | BONE_USED_BY_ATTACHMENT | BONE_USED_BY_VERTEX_MASK | BONE_USED_BY_BONE_MERGE),
	BONE_USED_MASK = BONE_USED_BY_ANYTHING,
	BONE_ALWAYS_SETUP = (1 << 19)
};

enum EStudioBoneTypeFlags : int
{
	BONE_FIXED_ALIGNMENT = (1 << 20),
	BONE_HAS_SAVEFRAME_POS = (1 << 21),
	BONE_HAS_SAVEFRAME_ROT64 = (1 << 22),
	BONE_HAS_SAVEFRAME_ROT32 = (1 << 23),
	BONE_TYPE_MASK = (BONE_FIXED_ALIGNMENT | BONE_HAS_SAVEFRAME_POS | BONE_HAS_SAVEFRAME_ROT64 | BONE_HAS_SAVEFRAME_ROT32)
};
#pragma endregion

// forward declarations
struct studiohdr_t;

#pragma pack(push, 4)
template <class T>
struct alignas(4) serializedstudioptr_t
{
	serializedstudioptr_t()
	{
		pData = nullptr;
	}

	inline operator T*()
	{
		return pData;
	}

	inline operator const T*() const
	{
		return pData;
	}

	inline T* operator->()
	{
		return pData;
	}

	inline const T* operator->() const
	{
		return pData;
	}

	inline T* operator=(T* pNewData)
	{
		return pData = pNewData;
	}

	T* pData;
	std::int32_t iPadding;
};
#pragma pack(pop)

struct mstudioikchain_t
{
	Q_INLINE const char* GetName() const
	{
		return reinterpret_cast<const char*>(this) + iNameOffset;
	}

	int iNameOffset; // 0x00
	int nLinkType; // 0x04
	int nLinkCount; // 0x08
	int iLinkOffset; // 0x0C
	// FIXME: add unused entries
};
static_assert(sizeof(mstudioikchain_t) == 0x10);

struct mstudio_modelvertexdata_t
{
	const void* pVertexData; // 0x00
	const void* pTangentData; // 0x04
	const void* pExtraData; // 0x08
};
static_assert(sizeof(mstudio_modelvertexdata_t) == 0xC);

struct mstudiomodel_t
{
	Q_INLINE const char* GetName() const
	{
		return szName;
	}

	char szName[64]; // 0x00
	int nType; // 0x40

	float flBoundingRadius; // 0x44

	int nMeshCount; // 0x48
	int iMeshOffset; // 0x4C

	int nVerticesCount; // 0x50 // number of unique vertices/normals/texcoords
	int iVertexOffset; // 0x54 // vertex vector
	int iTangentsOffset; // 0x58 // tangents vector

	int nAttachmentCount; // 0x5C
	int iAttachmentOffset; // 0x60

	int nEyeballCount; // 0x64
	int iEyeballOffset; // 0x68

	mstudio_modelvertexdata_t vertexData; // 0x6C
	std::byte unused0[0x1C]; // 0x78
};
static_assert(sizeof(mstudiomodel_t) == 0x94);

struct mstudiobone_t
{
	Q_INLINE const char* GetName() const
	{
		if (iNameOffset == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iNameOffset;
	}

	Q_INLINE void* GetProcedure() const
	{
		if (iProcedureOffset == 0)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iProcedureOffset);
	}

	Q_INLINE const char* GetSurfacePropName() const
	{
		if (iSurfacePropNameOffset == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iSurfacePropNameOffset;
	}

	Q_INLINE int GetSurfaceProp() const
	{
		return iSurfacePropLookup;
	}

	int iNameOffset; // 0x00
	int iParent; // 0x04 // parent bone
	int arrBoneController[6]; // 0x08 // bone controller index, -1 == none

	// default values
	Vector_t vecPosition; // 0x20
	Quaternion_t quatWorld; // 0x2C
	RadianEuler_t radRotation; // 0x3C

	// compression scale
	Vector_t vecPositionScale; // 0x48
	Vector_t vecRotationScale; // 0x54

	Matrix3x4_t matPoseToBone; // 0x60
	Quaternion_t quatAlignment; // 0x90
	int nFlags; // 0xA0
	int nProcedureType; // 0xA4
	int iProcedureOffset; // 0xA8
	mutable int iPhysicsBone; // 0xAC
	int iSurfacePropNameOffset; // 0xB0
	int iContents; // 0xB4
	int iSurfacePropLookup; // 0xB8
	std::byte pad0[0x1C]; // 0xBC
};
static_assert(sizeof(mstudiobone_t) == 0xD8); // size verify @ida: client.dll -> ["69 C0 ? ? ? ? 03 C2 8B" + 0x2]

struct mstudiobonecontroller_t
{
	int iBone; // 0x00 // -1 == 0
	int nType; // 0x04 // X, Y, Z, XR, YR, ZR, M
	float flStart; // 0x08
	float flEnd; // 0x0C
	int iRest; // 0x10 // byte index value at rest
	int iInputField; // 0x14 // 0-3 user set controller, 4 mouth
	std::byte unused0[0x40]; // 0x18
};
static_assert(sizeof(mstudiobonecontroller_t) == 0x58);

struct mstudiobbox_t
{
	Q_INLINE const char* GetHitboxName() const
	{
		if (iHitboxNameOffset == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iHitboxNameOffset;
	}

	int iBone; // 0x00
	int iGroup; // 0x04 // intersection group
	Vector_t vecBBMin; // 0x08 // bounding box
	Vector_t vecBBMax; // 0x14
	int iHitboxNameOffset; // 0x20 // offset to the name of the hitbox
	QAngle_t angOffsetOrientation; // 0x24
	float flCapsuleRadius; // 0x30
	std::byte unused0[0x10]; // 0x34
};
static_assert(sizeof(mstudiobbox_t) == 0x44);

struct mstudiobodyparts_t
{
	Q_INLINE const char* GetName() const
	{
		return reinterpret_cast<const char*>(this) + iNameOffset;
	}

	Q_INLINE mstudiomodel_t* GetModel(int nIndex) const
	{
		// @note: valve don't have this check
		if (nIndex < 0 || nIndex >= nModelCount)
			return nullptr;

		return reinterpret_cast<mstudiomodel_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iModelOffset)) + nIndex;
	}

	int iNameOffset; // 0x00
	int nModelCount; // 0x04
	int iBase; // 0x08
	int iModelOffset; // 0x0C
};
static_assert(sizeof(mstudiobodyparts_t) == 0x10);

struct mstudiomouth_t
{
	int iBone; // 0x00
	Vector_t vecForward; // 0x04
	int iFlexDesc; // 0x10
};
static_assert(sizeof(mstudiomouth_t) == 0x14);

struct mstudiohitboxset_t
{
	Q_INLINE const char* GetName() const
	{
		if (iNameOffset == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iNameOffset;
	}

	Q_INLINE mstudiobbox_t* GetHitbox(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nHitboxCount)
			return nullptr;

		return reinterpret_cast<mstudiobbox_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iHitboxOffset)) + nIndex;
	}

	int iNameOffset; // 0x00
	int nHitboxCount; // 0x04
	int iHitboxOffset; // 0x08
};
static_assert(sizeof(mstudiohitboxset_t) == 0xC);

struct virtualgroup_t
{
	const studiohdr_t* GetStudioHdr() const;

	void* pCache; // 0x00
	CUtlVector<int> vecBoneMap; // 0x04 // maps global bone to local bone
	CUtlVector<int> vecMasterBones; // 0x18 // maps local bone to global bone
	CUtlVector<int> vecMasterSequences; // 0x2C // maps local sequence to master sequence
	CUtlVector<int> vecMasterAnimations; // 0x40 // maps local animation to master animation
	CUtlVector<int> vecMasterAttachment; // 0x54 // maps local attachment to global
	CUtlVector<int> vecMasterPoses; // 0x68 // maps local pose parameter to global
	CUtlVector<int> vecMasterNodes; // 0x7C // maps local transition nodes to global
};
static_assert(sizeof(virtualgroup_t) == 0x90);

struct virtualsequence_t
{
	int nFlags; // 0x00
	int iActivity; // 0x04
	int iGroup; // 0x08
	int nIndex; // 0x0C
};
static_assert(sizeof(virtualsequence_t) == 0x10);

struct virtualgeneric_t
{
	int iGroup; // 0x00
	int nIndex; // 0x04
};
static_assert(sizeof(virtualgeneric_t) == 0x8);

struct virtualmodel_t
{
	// @note: user must manage mutex for this
	Q_INLINE virtualgroup_t* GetAnimationGroup(const int iAnimation)
	{
		return &vecGroups[vecAnimations[iAnimation].iGroup];
	}

	// @note: user must manage mutex for this
	Q_INLINE virtualgroup_t* GetSequenceGroup(const int iSequence)
	{
		return &vecGroups[vecSequences[iSequence].iGroup];
	}

	CThreadFastMutex lock; // 0x00
	CUtlVector<virtualsequence_t> vecSequences; // 0x08
	CUtlVector<virtualgeneric_t> vecAnimations; // 0x1C
	CUtlVector<virtualgeneric_t> vecAttachments; // 0x30
	CUtlVector<virtualgeneric_t> vecPoses; // 0x44
	CUtlVector<virtualgroup_t> vecGroups; // 0x58
	CUtlVector<virtualgeneric_t> vecNodes; // 0x6C
	CUtlVector<virtualgeneric_t> vecIKLocks; // 0x80
	CUtlVector<unsigned short> vecAutoplaySequences; // 0x94
};
static_assert(sizeof(virtualmodel_t) == 0xA8);

struct studiohdr_t;

struct mstudioseqdesc_t
{
	Q_INLINE studiohdr_t* GetStudioHdr() const
	{
		return reinterpret_cast<studiohdr_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + nBaseStudio));
	}

	Q_INLINE const char* GetLabel() const
	{
		return reinterpret_cast<const char*>(this) + iLabelOffset;
	}

	Q_INLINE const char* GetActivityName() const
	{
		return reinterpret_cast<const char*>(this) + iActivityNameOffset;
	}

	Q_INLINE void* GetEvent(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nEventCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iEventOffset) + nIndex;
	}

	Q_INLINE int GetAnimValue(int x, int y) const
	{
		if (x >= iGroupSize[0])
			x = iGroupSize[0] - 1;

		if (y >= iGroupSize[1])
			y = iGroupSize[1] - 1;

		const int iOffset = y * iGroupSize[0] + x;
		const short* arrBlends = reinterpret_cast<short*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iBlendOffset));
		return arrBlends[iOffset];
	}

	Q_INLINE void* GetAutoLayer(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nAutoLayerCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iAutoLayerOffset) + nIndex;
	}

	Q_INLINE float* GetBoneWeight(const int nIndex) const
	{
		return reinterpret_cast<float*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iWeightListOffset)) + nIndex;
	}

	Q_INLINE float* GetPoseKey(const int iParameter, const int iAnimation) const
	{
		return reinterpret_cast<float*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iPoseKeyOffset)) + iParameter * iGroupSize[0] + iAnimation;
	}

	Q_INLINE void* GetIKLock(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIKLockCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iIKLockOffset) + nIndex;
	}

	Q_INLINE const char* GetKeyValueText() const
	{
		if (nKeyValueSize == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iKeyValueOffset;
	}

	int nBaseStudio; // 0x00

	int iLabelOffset; // 0x04

	int iActivityNameOffset; // 0x08

	int nFlags; // 0x0C

	int iActivity; // 0x10
	int iActivityWeight; // 0x14

	int nEventCount; // 0x18
	int iEventOffset; // 0x1C

	Vector_t vecBBMin; // 0x20
	Vector_t vecBBMax; // 0x2C

	int nBlendCount; // 0x38
	int iBlendOffset; // 0x3C

	int iMovementOffset; // 0x40 // [blend] float array for blended movement

	int iGroupSize[2]; // 0x44
	int nParamIndex[2]; // 0x4C // X, Y, Z, XR, YR, ZR
	float flParamStart[2]; // 0x54// local (0..1) starting value
	float flParamEnd[2]; // 0x5C // local (0..1) ending value
	int iParamParent; // 0x64

	float flFadeInTime; // 0x68 // ideal cross fate in time (0.2 default)
	float flFadeOutTime; // 0x6C // ideal cross fade out time (0.2 default)

	int iEntryNode; // 0x70 // transition node at entry
	int iExitNode; // 0x74 // transition node at exit
	int nNodeFlags; // 0x78 // transition rules

	float flEntryPhase; // 0x7C // used to match entry gait
	float flExitPhase; // 0x80 // used to match exit gait

	float flLastFrame; // 0x84 // frame that should generation EndOfSequence

	int iNextSequence; // 0x88 // auto advancing sequences

	int iPose; // 0x8C // index of delta animation between end and nextseq

	int nIKRuleCount; // 0x90

	int nAutoLayerCount; // 0x94
	int iAutoLayerOffset; // 0x98

	int iWeightListOffset; // 0x9C

	int iPoseKeyOffset; // 0xA0

	int nIKLockCount; // 0xA4
	int iIKLockOffset; // 0xA8

	int iKeyValueOffset; // 0xAC
	int nKeyValueSize; // 0xB0

	int iCyclePoseOffset; // 0xB4

	int iActivityModifierOffset; // 0xB8
	int nActivityModifierCount; // 0xBC

	int iAnimTagOffset; // 0xC0
	int nAnimTagCount; // 0xC4

	int iRootDriverOffset; // 0xC8
	std::byte unused0[0x8]; // 0xCC
};
static_assert(sizeof(mstudioseqdesc_t) == 0xD4);

struct mstudioposeparamdesc_t
{
	Q_INLINE const char* GetName() const
	{
		return reinterpret_cast<const char*>(this) + iNameOffset;
	}

	int iNameOffset; // 0x00
	int nFlags; // 0x04
	float flStart; // 0x08 // starting value
	float flEnd; // 0x0C // ending value
	float flLoop; // 0x10 // looping range, 0 for no looping, 360 for rotations, etc.
};
static_assert(sizeof(mstudioposeparamdesc_t) == 0x14);

struct mstudioflexdesc_t
{
	Q_INLINE const char* GetFACS() const
	{
		return reinterpret_cast<const char*>(this) + iFACSOffset;
	}

	int iFACSOffset; // 0x00 // wtf this naming
};
static_assert(sizeof(mstudioflexdesc_t) == 0x4);

struct mstudioflexcontroller_t
{
	Q_INLINE const char* GetType() const
	{
		return reinterpret_cast<const char*>(this) + iTypeOffset;
	}

	Q_INLINE const char* GetName() const
	{
		return reinterpret_cast<const char*>(this) + iNameOffset;
	}

	int iTypeOffset; // 0x00
	int iNameOffset; // 0x04
	mutable int iLocalToGlobal; // 0x08
	float flMin; // 0x0C
	float flMax; // 0x10
};
static_assert(sizeof(mstudioflexcontroller_t) == 0x14);

class IMaterial;

struct mstudiotexture_t
{
	Q_INLINE const char* GetName() const
	{
		return reinterpret_cast<const char*>(this) + iNameOffset;
	}

	int iNameOffset; // 0x00
	int nFlags; // 0x04
	int iUsed; // 0x08
	std::byte unused0[0x4]; // 0x0C
	mutable IMaterial* pMaterial; // 0x10
	mutable void* pClientMaterial; // 0x14
	std::byte unused1[0x28]; // 0x18
};
static_assert(sizeof(mstudiotexture_t) == 0x40);

struct studiohdr2_t
{
	char* GetName()
	{
		if (iNameOffset == 0)
			return nullptr;

		return reinterpret_cast<char*>(reinterpret_cast<std::uint8_t*>(this) + iNameOffset);
	}

	int nSrcBoneTransformCount;
	int iSrcBoneTransformOffset;

	int iIlumPositionAttachmentOffset;

	float flMaxEyeDeflection;

	int nLinearBodyIndex;

	int iNameOffset;

	int nBoneFlexDriverCount;
	int iBoneFlexDriverOffset;

	void* pFeModel;

	int nBodyGroupPresetCount;
	int iBodyGroupPresetOffset;

	int iUnused;

	mutable serializedstudioptr_t<void> pVirtualModel;
	mutable serializedstudioptr_t<void> pAnimBlockModel;

	serializedstudioptr_t<void> pVertexBase;
	serializedstudioptr_t<void> pIndexBase;

	int iReserved[44];
};

struct studiohwdata_t;

struct studiohdr_t
{
	virtualmodel_t* GetVirtualModel() const;

	Q_INLINE const char* GetName() const
	{
		// @note: doesnt handle studiohdr2
		// @ida: client.dll -> "8B 11 8B 82 ? ? ? ? 85 C0 74 1B"
		if (iStudioHdr2Offset != 0 && GetStudioHdr2()->GetName())
			return GetStudioHdr2()->GetName();
		return szName;
	}

	Q_INLINE mstudiobone_t* GetBone(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nBoneCount)
			return nullptr;

		return reinterpret_cast<mstudiobone_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iBoneOffset)) + nIndex;
	}

	Q_INLINE mstudiobonecontroller_t* GetBoneController(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nBoneControllerCount)
			return nullptr;

		return reinterpret_cast<mstudiobonecontroller_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iBoneControllerOffset)) + nIndex;
	}

	Q_INLINE mstudiohitboxset_t* GetHitboxSet(const int iSet) const
	{
		if (iSet < 0 || iSet >= nHitboxSetCount)
			return nullptr;

		return reinterpret_cast<mstudiohitboxset_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iHitboxSetOffset)) + iSet;
	}

	Q_INLINE mstudiobbox_t* GetHitbox(const int iHitBox, const int iSet) const
	{
		const mstudiohitboxset_t* pHitboxSet = GetHitboxSet(iSet);

		if (pHitboxSet == nullptr)
			return nullptr;

		return pHitboxSet->GetHitbox(iHitBox);
	}

	Q_INLINE int GetHitboxCount(const int iSet) const
	{
		const mstudiohitboxset_t* pHitboxSet = GetHitboxSet(iSet);

		if (pHitboxSet == nullptr)
			return 0;

		return pHitboxSet->nHitboxCount;
	}

	Q_INLINE void* GetAnimDescription(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nLocalAnimationCount)
			nIndex = 0;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iLocalAnimationOffset) + nIndex;
	}

	Q_INLINE mstudioseqdesc_t* GetLocalSequenceDescription(int iSequence) const
	{
		if (iSequence < 0 || iSequence >= nLocalSequenceCount)
			iSequence = 0;

		return reinterpret_cast<mstudioseqdesc_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iLocalSequenceOffset)) + iSequence;
	}

	Q_INLINE mstudioseqdesc_t& GetSequenceDescription(const int nSequence) const
	{
		if (nIncludeModelCount == 0)
			return *GetLocalSequenceDescription(nSequence);

		virtualmodel_t* pVirtualModel = GetVirtualModel();
		if (pVirtualModel == nullptr)
		{
			Q_ASSERT(false);
			return *GetLocalSequenceDescription(nSequence);
		}

		const virtualgroup_t* pGroup = &pVirtualModel->vecGroups[pVirtualModel->vecSequences[nSequence].iGroup];
		const studiohdr_t* pStudioHdr = pGroup->GetStudioHdr();
		Q_ASSERT(pStudioHdr != nullptr);

		return *pStudioHdr->GetLocalSequenceDescription(pVirtualModel->vecSequences[nSequence].nIndex);
	}

	Q_INLINE void* GetTexture(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nTextureCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iTextureOffset) + nIndex;
	}

	Q_INLINE const char* GetCdTexture(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nCdTextureCount)
			return nullptr;

		return reinterpret_cast<const char*>(this) + *(reinterpret_cast<int*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iCdTextureOffset)) + nIndex);
	}

	Q_INLINE short* GetSkinReference(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nSkinReferenceCount)
			return nullptr;

		return reinterpret_cast<short*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iSkinOffset)) + nIndex;
	}

	Q_INLINE mstudiobodyparts_t* GetBodyPart(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nBodyPartCount)
			return nullptr;

		return reinterpret_cast<mstudiobodyparts_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iBodyPartOffset)) + nIndex;
	}

	Q_INLINE void* GetAttachment(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nAttachmentCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iAttachmentOffset) + nIndex;
	}

	Q_INLINE std::uint8_t* GetTransition(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= (nTransitionCount * nTransitionCount))
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iTransitionOffset) + nIndex;
	}

	Q_INLINE mstudioflexdesc_t* GetFlexDesc(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nFlexDescCount)
			return nullptr;

		return reinterpret_cast<mstudioflexdesc_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iFlexDescOffset)) + nIndex;
	}

	Q_INLINE mstudioflexcontroller_t* GetFlexController(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nFlexControllerCount)
			return nullptr;

		return reinterpret_cast<mstudioflexcontroller_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iFlexControllerOffset)) + nIndex;
	}

	Q_INLINE void* GetFlexRule(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nFlexRuleCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iFlexRuleOffset) + nIndex;
	}

	Q_INLINE mstudioikchain_t* GetIKChain(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIKChainCount)
			return nullptr;

		return reinterpret_cast<mstudioikchain_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iIKChainOffset)) + nIndex;
	}

	Q_INLINE void* GetMouth(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nMouthCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iMouthOffset) + nIndex;
	}

	Q_INLINE void* GetPoseParameter(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nLocalPoseParameterCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iLocalPoseParameterOffset) + nIndex;
	}

	Q_INLINE const char* GetSurfacePropName() const
	{
		if (iSurfacePropNameOffset == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iSurfacePropNameOffset;
	}

	Q_INLINE const char* GetKeyValueText() const
	{
		if (nKeyValueSize == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iKeyValueOffset;
	}

	Q_INLINE void* GetIKAutoplayLock(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIKAutoplayLockCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iIKAutoplayLockOffset) + nIndex;
	}

	Q_INLINE void* GetModelGroup(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIncludeModelCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iIncludeModelOffset) + nIndex;
	}

	Q_INLINE const char* GetAnimBlockName() const
	{
		return reinterpret_cast<const char*>(this) + iAnimBlockNameOffset;
	}

	Q_INLINE void* GetAnimBlock(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nAnimBlockCount)
			return nullptr;

		return reinterpret_cast<std::uintptr_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iAnimBlockOffset)) + nIndex;
	}

	Q_INLINE const std::uint8_t* GetBoneTableSortedByName() const
	{
		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iBoneTableByNameOffset);
	}

	Q_INLINE void* GetFlexControllerUI(const int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nFlexControllerUICount)
			return nullptr;

		return reinterpret_cast<std::uintptr_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iFlexControllerUIOffset)) + nIndex;
	}

	studiohdr2_t* GetStudioHdr2() const
	{
		return reinterpret_cast<studiohdr2_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iStudioHdr2Offset));
	}

	void* GetVirtualModelHandle() const
	{
		if (iStudioHdr2Offset == 0)
			return nullptr;

		return GetStudioHdr2()->pVirtualModel;
	}

	int nID; // 0x0000
	int iVersion; // 0x0004
	int iChecksum; // 0x0008

	char szName[64]; // 0x000C

	int iLength; // 0x004C // @ida: datacache.dll -> ["8B 45 F4 01 47" + 0x5]

	Vector_t vecEyePosition; // 0x0050
	Vector_t vecIlluminationPosition; // 0x005C
	Vector_t vecHullMin; // 0x0068
	Vector_t vecHullMax; // 0x0074
	Vector_t vecViewBBMin; // 0x0080
	Vector_t vecViewBBMax; // 0x008C

	int nFlags; // 0x0098 // @ida: datacache.dll -> ["81 8E ? ? ? ? 00" + 0x2]

	int nBoneCount; // 0x009C
	int iBoneOffset; // 0x00A0 // @ida: client.dll -> ["8B 90 ? ? ? ? 03 C8" + 0x2]

	int nBoneControllerCount; // 0x00A4
	int iBoneControllerOffset; // 0x00A8

	int nHitboxSetCount; // 0x00AC
	int iHitboxSetOffset; // 0x00B0

	int nLocalAnimationCount; // 0x00B4
	int iLocalAnimationOffset; // 0x00B8

	int nLocalSequenceCount; // 0x00BC
	int iLocalSequenceOffset; // 0x00C0

	mutable int iActivityListVersion; // 0x00C4
	mutable int iEventsIndexed; // 0x00C8

	int nTextureCount; // 0x00CC
	int iTextureOffset; // 0x00D0

	int nCdTextureCount; // 0x00D4
	int iCdTextureOffset; // 0x00D8

	int nSkinReferenceCount; // 0x00DC
	int nSkinFamilyCount; // 0x00E0
	int iSkinOffset; // 0x00E4

	int nBodyPartCount; // 0x00E8
	int iBodyPartOffset; // 0x00EC

	int nAttachmentCount; // 0x00F0
	int iAttachmentOffset; // 0x00F4

	int nTransitionCount; // 0x00F8
	int iTransitionOffset; // 0x00FC
	int iTransitionNameOffset; // 0x0100

	int nFlexDescCount; // 0x0104
	int iFlexDescOffset; // 0x0108

	int nFlexControllerCount; // 0x010C
	int iFlexControllerOffset; // 0x0110

	int nFlexRuleCount; // 0x0114
	int iFlexRuleOffset; // 0x0118

	int nIKChainCount; // 0x011C
	int iIKChainOffset; // 0x0120

	int nMouthCount; // 0x0124
	int iMouthOffset; // 0x0128

	int nLocalPoseParameterCount; // 0x012C
	int iLocalPoseParameterOffset; // 0x0130

	int iSurfacePropNameOffset; // 0x0134

	int iKeyValueOffset; // 0x0138
	int nKeyValueSize; // 0x013C

	int nIKAutoplayLockCount; // 0x0140
	int iIKAutoplayLockOffset; // 0x0144

	float flMass; // 0x0148
	int iContents; // 0x014C

	int nIncludeModelCount; // 0x0150 // @ida: datacache.dll -> ["83 BF ? ? ? ? ? 75 07" + 0x2]
	int iIncludeModelOffset; // 0x0154

	int iVirtualModel; // 0x0158

	int iAnimBlockNameOffset; // 0x015C

	int nAnimBlockCount; // 0x0160
	int iAnimBlockOffset; // 0x0164

	int iAnimBlockModel; // 0x0168

	int iBoneTableByNameOffset; // 0x016C // @ida: client.dll -> ["8B 88 ? ? ? ? 8B 90 ? ? ? ? 03" + 0x8]

	int nVertexBase; // 0x0170
	int nIndexBase; // 0x0174

	std::uint8_t uDirectionalLightDot; // 0x0178
	std::uint8_t uRootLOD; // 0x0179
	std::uint8_t nAllowedRootLODs; // 0x017A
	std::byte unused0[0x5]; // 0x017B

	int nFlexControllerUICount; // 0x0180
	int iFlexControllerUIOffset; // 0x0184

	float flVertAnimFixedPointScale; // 0x0188
	mutable int iSurfacePropLookup; // 0x018C

	int iStudioHdr2Offset; // 0x0190

	std::byte unused1[0x4]; // 0x0194
};
static_assert(sizeof(studiohdr_t) == 0x198);

class CStudioHdr
{
public:
	Q_INLINE bool IsVirtual() const
	{
		return (pVirtualModel != nullptr);
	}

	Q_INLINE bool IsValid() const
	{
		return (pStudioHdr != nullptr);
	}

	Q_INLINE bool IsReadyForAccess() const
	{
		return (pStudioHdr != nullptr);
	}

	Q_INLINE virtualmodel_t* GetVirtualModel() const
	{
		return pVirtualModel;
	}

	Q_INLINE const studiohdr_t* GetRenderHdr() const
	{
		return pStudioHdr;
	}

	Q_INLINE void* GetSoftbody() const
	{
		return pSoftbody;
	}

	Q_INLINE void SetSoftbody(void* pNewSoftbody)
	{
		pSoftbody = pNewSoftbody;
	}

	Q_INLINE const char* GetName() const
	{
		return pStudioHdr->GetName();
	}

	Q_INLINE int GetBoneCount() const
	{
		return pStudioHdr->nBoneCount;
	}

	Q_INLINE const mstudiobone_t* GetBone(const int nIndex) const
	{
		return pStudioHdr->GetBone(nIndex);
	}

	Q_INLINE int GetBoneControllerCount() const
	{
		return pStudioHdr->nBoneControllerCount;
	}

	Q_INLINE mstudiobonecontroller_t* GetBoneController(const int nIndex) const
	{
		return pStudioHdr->GetBoneController(nIndex);
	}

	Q_INLINE int GetSequenceCount() const
	{
		if (pVirtualModel == nullptr)
			return pStudioHdr->nLocalSequenceCount;

		return pVirtualModel->vecSequences.Count();
	}

	mstudioseqdesc_t& GetSequenceDescription(int iSequence)
	{
		static auto fnpSeqdesc = reinterpret_cast<mstudioseqdesc_t&(Q_THISCALL*)(CStudioHdr*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 56 8B 75 08 57 8B F9 85 F6 78 18")));
		return fnpSeqdesc(this, iSequence);
	}

	Q_INLINE int GetPoseParameterCount() const
	{
		if (pVirtualModel == nullptr)
			return pStudioHdr != nullptr ? pStudioHdr->nLocalPoseParameterCount : 0;

		return pVirtualModel->vecPoses.Count();
	}

	mstudioposeparamdesc_t& GetPoseParameterDescription(int iParameter)
	{
		static auto fnpPoseParameter = reinterpret_cast<mstudioposeparamdesc_t&(Q_THISCALL*)(CStudioHdr*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 8B 45 08 57 8B F9 8B 4F 04 85 C9 75 15")));
		return fnpPoseParameter(this, iParameter);
	}

	Q_INLINE int GetIKChainCount() const
	{
		return pStudioHdr->nIKChainCount;
	}

	Q_INLINE mstudioikchain_t* GetIKChain(const int nIndex) const
	{
		return pStudioHdr->GetIKChain(nIndex);
	}

	Q_INLINE int GetFlexRulesCount() const
	{
		return pStudioHdr->nFlexRuleCount;
	}

	Q_INLINE void* /*mstudioflexrule_t*/ GetFlexRule(const int nIndex) const
	{
		return pStudioHdr->GetFlexRule(nIndex);
	}

	Q_INLINE int GetFlexDescCount() const
	{
		return pStudioHdr->nFlexDescCount;
	}

	Q_INLINE mstudioflexdesc_t* GetFlexDesc(const int nIndex) const
	{
		return pStudioHdr->GetFlexDesc(nIndex);
	}

	Q_INLINE int GetFlexControllerCount() const
	{
		return pStudioHdr->nFlexControllerCount;
	}

	Q_INLINE mstudioflexcontroller_t* GetFlexController(const int nIndex) const
	{
		return pStudioHdr->GetFlexController(nIndex);
	}

	Q_INLINE int GetFlexControllerUICount() const
	{
		return pStudioHdr->nFlexControllerUICount;
	}

	Q_INLINE void* /*mstudioflexcontrollerui_t*/ GetFlexControllerUI(const int nIndex) const
	{
		return pStudioHdr->GetFlexControllerUI(nIndex);
	}

	Q_INLINE int GetHitboxSetCount() const
	{
		return pStudioHdr->nHitboxSetCount;
	}

	Q_INLINE mstudiohitboxset_t* GetHitboxSet(const int nIndex) const
	{
		return pStudioHdr->GetHitboxSet(nIndex);
	}

	Q_INLINE int GetHitboxCount(const int iSet) const
	{
		return pStudioHdr->GetHitboxCount(iSet);
	}

	Q_INLINE mstudiobbox_t* GetHitbox(const int nIndex, const int iSet) const
	{
		return pStudioHdr->GetHitbox(nIndex, iSet);
	}

	Q_INLINE int GetBodyPartCount() const
	{
		return pStudioHdr->nBodyPartCount;
	}

	Q_INLINE mstudiobodyparts_t* GetBodyPart(const int nIndex) const
	{
		return pStudioHdr->GetBodyPart(nIndex);
	}

	Q_INLINE int GetSkinFamilyCount() const
	{
		return pStudioHdr->nSkinFamilyCount;
	}

	Q_INLINE Vector_t GetEyePosition() const
	{
		return pStudioHdr->vecEyePosition;
	}

	Q_INLINE int GetFlags() const
	{
		return pStudioHdr->nFlags;
	}

	Q_INLINE int GetSurfaceProp() const
	{
		return pStudioHdr->iSurfacePropLookup;
	}

	Q_INLINE const char* GetSurfacePropName() const
	{
		return pStudioHdr->GetSurfacePropName();
	}

	Q_INLINE float GetMass() const
	{
		return pStudioHdr->flMass;
	}

	Q_INLINE int GetContents() const
	{
		return pStudioHdr->iContents;
	}

	Q_INLINE Vector_t GetIlluminationPosition() const
	{
		return pStudioHdr->vecIlluminationPosition;
	}

	Q_INLINE Vector_t GetHullMin() const
	{
		return pStudioHdr->vecHullMin;
	}

	Q_INLINE Vector_t GetHullMax() const
	{
		return pStudioHdr->vecHullMax;
	}

	Q_INLINE Vector_t GetViewBBMin() const
	{
		return pStudioHdr->vecViewBBMin;
	}

	Q_INLINE Vector_t GetViewBBMax() const
	{
		return pStudioHdr->vecViewBBMax;
	}

	Q_INLINE int GetTextureCount() const
	{
		return pStudioHdr->nTextureCount;
	}

public:
	mutable const studiohdr_t* pStudioHdr; // 0x00
	mutable virtualmodel_t* pVirtualModel; // 0x04
	mutable void* pSoftbody; // 0x08
	mutable CUtlVector<const studiohdr_t*> vecStudioHdrCache; // 0x0C
	mutable int nFrameUnlockCounter; // 0x20
	int* pFrameUnlockCounter; // 0x24
	CThreadFastMutex frameUnlockCounterMutex; // 0x28
	CUtlVector<int> vecBoneFlags; // 0x30
	CUtlVector<int> vecBoneParent; // 0x44
	void* pActivityToSequence; // 0x58
	CUtlDict<int, int> mapNamedSequence; // 0x5C
	mutable int nPerfAnimatedBones; // 0x80
	mutable int nPerfUsedBones; // 0x84
	mutable int nPerfAnimatedLayers; // 0x88
};
static_assert(sizeof(CStudioHdr) == 0x8C); // size verify @ida: (C_BaseAnimating::LockStudioHdr) client.dll -> ["74 3C A1 ? ? ? ? 68" + 0x8]
