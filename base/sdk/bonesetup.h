#pragma once
#include "datatypes/qangle.h"
#include "datatypes/vector.h"
#include "datatypes/matrix.h"
#include "datatypes/bitvec.h"

// @source: master/public/bone_setup.h

using BoneVector_t = Vector_t;
using BoneQuaternion_t = Quaternion_t;
using BoneQuaternionAligned_t = QuaternionAligned_t;

using CBoneBitList = CBitVec<MAXSTUDIOBONES>;

#pragma pack(push, 4)
class CIKTarget
{
public:
	int iChain; // 0x0000
	int nType; // 0x0004

	struct
	{
		char* szAttachmentName; // 0x0008
		Vector_t vecPosition; // 0x000C
		Quaternion_t quatView; // 0x0018
	} offset; // accumulated offset from ideal footplant location

	struct
	{
		Vector_t vecPosition; // 0x0028
		Quaternion_t quatView; // 0x0034
	} ideal;

	struct
	{
		float flLatched; // 0x0044
		float flRelease; // 0x0048
		float flHeight; // 0x004C
		float flFloor; // 0x0050
		float flRadius; // 0x0054
		float flTime; // 0x0058
		float flWeight; // 0x005C
		Vector_t vecPosition; // 0x0060
		Quaternion_t quatView; // 0x006C
		bool bOnWorld; // 0x007C
	} est; // estimate contact position

	struct
	{
		float flHipToFoot; // 0x0080 // distance from hip
		float flHipToKnee; // 0x0084 // distance from hip to knee
		float flKneeToFoot; // 0x0088 // distance from knee to foot
		Vector_t vecHip; // 0x008C // location of hip
		Vector_t vecClosest; // 0x0098 // closest valid location from hip to foot that the foot can move to
		Vector_t vecKnee; // 0x00A4 // pre-ik location of knee
		Vector_t vecFarthest; // 0x00B0 // farthest valid location from hip to foot that the foot can move to
		Vector_t vecLowest; // 0x00BC // lowest position directly below hip that the foot can drop to
	} trace;

	struct
	{
		bool bNeedsLatch; // 0x00C8
		bool bHasLatch; // 0x00C9
		float flInfluence; // 0x00CC
		int nFrameCount; // 0x00D0
		int iOwner; // 0x00D4
		Vector_t vecAbsOrigin; // 0x00D8
		QAngle_t angAbsView; // 0x00E4
		Vector_t vecPosition; // 0x00F0
		Quaternion_t quatView; // 0x00FC
		Vector_t vecDeltaPosition; // 0x0010C
		Quaternion_t quatDeltaView; // 0x0118
		Vector_t vecDebouncePosition; // 0x0128
		Quaternion_t quatDebounceView; // 0x0134
	} latched; // internally latched footset, position

	struct
	{
		float flTime; // 0x0144
		float flErrorTime; // 0x148
		float flRamp; // 0x014C
		bool bInError; // 0x0150
	} error;
};
static_assert(sizeof(CIKTarget) == 0x154); // size verify @ida: server.dll -> ["8D 92 ? ? ? ? 40 3B" + 0x2]

class CIKContext
{
public:
	CIKContext()
	{
		// @note: there is legit debug break
		// @ida CIKContext::CIKContext(): server.dll -> "53 8B D9 F6 C3"
		static auto fnConstructor = reinterpret_cast<void(Q_THISCALL*)(CIKContext*)>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, Q_XOR("E8 ? ? ? ? A1 ? ? ? ? FF 75 18")) + 0x1));
		fnConstructor(this);
	}

	~CIKContext()
	{
		static auto fnDestructor = reinterpret_cast<void(Q_THISCALL*)(CIKContext*)>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, Q_XOR("E8 ? ? ? ? 8B 44 24 14 85 C0 74 0B")) + 0x1));
		fnDestructor(this);
	}

	void* operator new(const std::size_t nSize)
	{
		return I::MemAlloc->Alloc(nSize);
	}

	void operator delete(void* pMemory)
	{
		I::MemAlloc->Free(pMemory);
	}

	void Init(const CStudioHdr* pInitialStudioHdr, const QAngle_t& angInitialView, const Vector_t& vecInitialPosition, float flInitialTime, int nInitialFrameCount, int nInitialBoneMask)
	{
		static auto fnInit = reinterpret_cast<void(Q_THISCALL*)(CIKContext*, const CStudioHdr*, const QAngle_t&, const Vector_t&, float, int, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D")));
		fnInit(this, pInitialStudioHdr, angInitialView, vecInitialPosition, flInitialTime, nInitialFrameCount, nInitialBoneMask);
	}

	void AddDependencies(mstudioseqdesc_t& sequenceDescription, int iSequence, float flCycle, const float* arrPoseParameters, float flWeight = 1.0f)
	{
		static auto fnAddDependencies = MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 81 EC ? ? ? ? 53 56 57 8B F9 0F"));
		std::uintptr_t uAddDependencies = reinterpret_cast<std::uintptr_t>(fnAddDependencies); // @todo: clang compiles direct (E8) call instead of indirect (FF 15) without this
		std::uintptr_t uSequenceDescription = reinterpret_cast<std::uintptr_t>(&sequenceDescription); // clang mess with registers without this

		__asm
		{
			mov ecx, this
			movss xmm3, flCycle
			push flWeight
			push arrPoseParameters
			push iSequence
			push uSequenceDescription
			call uAddDependencies
		}
	}

	void ClearTargets()
	{
		for (auto& ikTarget : arrTargets)
			ikTarget.latched.nFrameCount = -9999;
	}

	void UpdateTargets(BoneVector_t* arrPositions, BoneQuaternion_t* arrRotations, Matrix3x4a_t* arrBonesToWorld, CBoneBitList& arrBonesComputed)
	{
		static auto fnUpdateTargets = reinterpret_cast<void(Q_THISCALL*)(CIKContext*, BoneVector_t*, BoneQuaternion_t*, Matrix3x4a_t*, CBoneBitList&)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F0 81 EC ? ? ? ? 33 D2 89")));
		fnUpdateTargets(this, arrPositions, arrRotations, arrBonesToWorld, arrBonesComputed);
	}

	void SolveDependencies(BoneVector_t* arrPositions, BoneQuaternion_t* arrRotations, Matrix3x4a_t* arrBonesToWorld, CBoneBitList& arrBonesComputed)
	{
		static auto fnSolveDependencies = reinterpret_cast<void(Q_THISCALL*)(CIKContext*, BoneVector_t*, BoneQuaternion_t*, Matrix3x4a_t*, CBoneBitList&)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F0 81 EC ? ? ? ? 8B 81")));
		fnSolveDependencies(this, arrPositions, arrRotations, arrBonesToWorld, arrBonesComputed);
	}

	void CopyTo(CIKContext* pOther, const unsigned short* arrRemapping)
	{
		static auto fnCopyTo = reinterpret_cast<void(Q_THISCALL*)(CIKContext*, CIKContext*, const unsigned short*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 24 8B 45 08 57 8B F9 89 7D F4 85 C0")));
		fnCopyTo(this, pOther, arrRemapping);
	}

private:
	CUtlVectorFixed<CIKTarget, 12> arrTargets; // 0x0000
	const CStudioHdr* pStudioHdr; // 0x0FF8
	std::byte pad0[0x34]; // 0x0FFC
	Matrix3x4a_t matRootTransform; // 0x1030 // @ida: server.dll -> ["8D 97 ? ? ? ? E8 ? ? ? ? 8B 4D" + 0x2]
	int nFrameCount; // 0x1060
	float flTime; // 0x1064
	int nBoneMask; // 0x1068
	std::byte pad1[0x4]; // 0x106C
};
static_assert(sizeof(CIKContext) == 0x1070); // size verify @ida: server.dll -> ["68 ? ? ? ? 8B 08 8B 01 FF 50 04 85 C0 74 0E 8B C8 E8 ? ? ? ? 89 06" + 0x1]

class CBoneSetup
{
public:
	CBoneSetup(const CStudioHdr* pStudioHdr, const int nBoneMask, const float* arrPoseParameters, void* pPoseDebugger = nullptr) :
		pStudioHdr(pStudioHdr), nBoneMask(nBoneMask), pPoseParameters(arrPoseParameters), pPoseDebugger(pPoseDebugger) { }

	Q_CLASS_NO_ALLOC()

	void InitPose(BoneVector_t* arrBonesPosition, BoneQuaternionAligned_t* arrBonesRotation) const
	{
		for (int i = 0; i < pStudioHdr->GetBoneCount(); i++)
		{
			if (const mstudiobone_t* pBone = pStudioHdr->GetBone(i); pBone->nFlags & nBoneMask)
			{
				arrBonesPosition[i] = pBone->vecPosition;
				arrBonesRotation[i] = pBone->quatWorld;
			}
		}
	}

	void AccumulatePose(BoneVector_t* arrBonesPosition, BoneQuaternion_t* arrBonesRotation, int nSequence, float flCycle, float flWeight, float flTime, CIKContext* pIKContext)
	{
		static auto fnAccumulatePose = reinterpret_cast<void(Q_THISCALL*)(CBoneSetup*, BoneVector_t*, BoneQuaternion_t*, int, float, float, float, CIKContext*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? A1")));
		fnAccumulatePose(this, arrBonesPosition, arrBonesRotation, nSequence, flCycle, flWeight, flTime, pIKContext);
	}

	void CalcAutoplaySequences(BoneVector_t* arrBonesPosition, BoneQuaternion_t* arrBonesRotation, float flRealTime, CIKContext* pIKContext)
	{
		static auto fnCalcAutoplaySequences = MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 10 53 56 57 8B 7D 10"));
		std::uintptr_t uCalcAutoplaySequences = reinterpret_cast<std::uintptr_t>(fnCalcAutoplaySequences); // @todo: clang compiles direct (E8) call instead of indirect (FF 15) without this

		__asm
		{
			mov ecx, this
			movss xmm3, flRealTime
			push pIKContext
			push arrBonesRotation
			push arrBonesPosition
			call uCalcAutoplaySequences
		}
	}

	// blend together two bone positions and rotations
	void CalcBoneAdjustment(BoneVector_t* arrBonesPosition, BoneQuaternion_t* arrBonesRotation, const float* arrEncodedControllers) const
	{
		static auto fnCalcBoneAdj = MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 81 EC ? ? ? ? 8B C1 89"));

	#ifdef Q_COMPILER_CLANG
		std::uintptr_t uCalcBoneAdj = reinterpret_cast<std::uintptr_t>(fnCalcBoneAdj); // @todo: clang compiles direct (E8) call instead of indirect (FF 15) without this
		const CStudioHdr* pSelfStudioHdr = pStudioHdr;
		int nSelfBoneMask = nBoneMask;

		__asm
		{
			mov eax, this
			mov ecx, pSelfStudioHdr
			mov edx, arrBonesPosition
			push nSelfBoneMask
			push arrEncodedControllers
			push arrBonesRotation
			call uCalcBoneAdj
			add esp, 0Ch
		}
	#else
		__asm
		{
			mov eax, this
			mov ecx, [eax + pStudioHdr]
			mov edx, arrBonesPosition
			push [eax + nBoneMask]
			push arrEncodedControllers
			push arrBonesRotation
			call fnCalcBoneAdj
			add esp, 0Ch
		}
	#endif
	}

public:
	const CStudioHdr* pStudioHdr; // 0x00
	int nBoneMask; // 0x04
	const float* pPoseParameters; // 0x08
	void* pPoseDebugger; // 0x0C
};
static_assert(sizeof(CBoneSetup) == 0x10);
#pragma pack(pop)

/// converts a [0.0 .. 1.0] mapped pose parameter value into a ranged value
/// @returns: ranged pose parameter value
inline float Studio_GetPoseParameter(const CStudioHdr* pStudioHdr, const int iParameter, const float flFactor)
{
	// @ida Studio_GetPoseParameter(): client.dll | server.dll -> "55 8B EC 51 F3 0F 11 55"

	if (iParameter < 0 || iParameter >= pStudioHdr->GetPoseParameterCount())
		return 0.0f;

	const mstudioposeparamdesc_t& poseParameterDescription = const_cast<CStudioHdr*>(pStudioHdr)->GetPoseParameterDescription(iParameter);
	return poseParameterDescription.flStart + (poseParameterDescription.flEnd - poseParameterDescription.flStart) * flFactor;
}

/// converts a ranged pose parameter value into a [0.0 .. 1.0] mapped value
/// @returns: range clamped pose parameter value
inline float Studio_SetPoseParameter(const CStudioHdr* pStudioHdr, const int iParameter, float flValue, float& flOutFactor)
{
	/*
	 * @ida Studio_SetPoseParameter():
	 * client.dll -> "55 8B EC 83 E4 F8 83 EC 08 F3 0F 11 54 24 ? 85"
	 * server.dll -> ABS["E8 ? ? ? ? D9 45 08" + 0x1]
	 */

	if (iParameter < 0 || iParameter >= pStudioHdr->GetPoseParameterCount())
	{
		flOutFactor = 0.0f;
		return 0.0f;
	}

	const mstudioposeparamdesc_t& poseParameterDescription = const_cast<CStudioHdr*>(pStudioHdr)->GetPoseParameterDescription(iParameter);
	if (poseParameterDescription.flLoop > 0.0f)
	{
		const float flWrap = (poseParameterDescription.flStart + poseParameterDescription.flEnd) * 0.5f + poseParameterDescription.flLoop * 0.5f;
		const float flShift = poseParameterDescription.flLoop - flWrap;

		flValue = flValue - poseParameterDescription.flLoop * std::floorf((flValue + flShift) / poseParameterDescription.flLoop);
	}

	flOutFactor = CRT::Clamp((flValue - poseParameterDescription.flStart) / (poseParameterDescription.flEnd - poseParameterDescription.flStart), 0.0f, 1.0f);
	return poseParameterDescription.flStart + (poseParameterDescription.flEnd - poseParameterDescription.flStart) * flOutFactor;
}

// [side change] removed 'iBone' parameter, since currently it isn't used in CS:GO
inline void Studio_BuildMatrices(const CStudioHdr* pStudioHdr, const QAngle_t& angView, const Vector_t& vecOrigin, const BoneVector_t* arrBonesPosition, const BoneQuaternion_t* arrBonesRotation, const float flScale, Matrix3x4a_t arrBonesToWorld[MAXSTUDIOBONES], const int nBoneMask, const CBoneBitList& arrBonesComputed)
{
	// [side change] since 'iBone' parameter is removed, simplified logic here, reduced checks count
	int arrChain[MAXSTUDIOBONES] = { };
	int nChainLength = pStudioHdr->GetBoneCount();

	// build list of what bones to use
	for (int i = 0; i < nChainLength; i++)
		arrChain[nChainLength - i - 1] = i;

	// get model to world transformation
	Matrix3x4a_t matRotation = angView.ToMatrix(vecOrigin);

	// account for a change in scale
	if (flScale < 1.0f - FLT_EPSILON || flScale > 1.0f + FLT_EPSILON)
	{
		Vector_t vecOffset = matRotation.GetOrigin();
		vecOffset -= vecOrigin;
		vecOffset *= flScale;
		vecOffset += vecOrigin;
		matRotation.SetOrigin(vecOffset);

		// scale it uniformly
		*reinterpret_cast<Vector_t*>(matRotation[0]) *= flScale;
		*reinterpret_cast<Vector_t*>(matRotation[1]) *= flScale;
		*reinterpret_cast<Vector_t*>(matRotation[2]) *= flScale;
	}

	Q_ASSERT((reinterpret_cast<std::uintptr_t>(arrBonesToWorld) & 0xF) == 0); // bone matrices aren't 16 byte aligned

	// [side change] simplified loop
	while (nChainLength-- > 0)
	{
		const int iChain = arrChain[nChainLength];

		// [side change] fixed old valve's fixme, if this bone has already been computed - skip transforms
		if (arrBonesComputed[iChain])
			continue;

		if (pStudioHdr->vecBoneFlags[iChain] & nBoneMask)
		{
			Matrix3x4a_t matBone = arrBonesRotation[iChain].ToMatrix(arrBonesPosition[iChain]);

			const int iParentBone = pStudioHdr->vecBoneParent[iChain];
			arrBonesToWorld[iChain] = (iParentBone == -1 ? matRotation.ConcatTransforms(matBone) : arrBonesToWorld[iParentBone].ConcatTransforms(matBone));
		}
	}
}
