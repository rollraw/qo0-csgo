#pragma once
// @source: master/public/istudiorender.h

#pragma region studiorender_enumerations
enum : int
{
	STUDIORENDER_DRAW_ENTIRE_MODEL =	0,
	STUDIORENDER_DRAW_OPAQUE_ONLY =		(1 << 0),
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = (1 << 1),
	STUDIORENDER_DRAW_GROUP_MASK = (STUDIORENDER_DRAW_OPAQUE_ONLY | STUDIORENDER_DRAW_TRANSLUCENT_ONLY),
	STUDIORENDER_DRAW_NO_FLEXES =		(1 << 2),
	STUDIORENDER_DRAW_STATIC_LIGHTING =	(1 << 3),
	STUDIORENDER_DRAW_ACCURATETIME =	(1 << 4),
	STUDIORENDER_DRAW_NO_SHADOWS =		(1 << 5),
	STUDIORENDER_DRAW_GET_PERF_STATS =	(1 << 6),
	STUDIORENDER_DRAW_WIREFRAME =		(1 << 7),
	STUDIORENDER_DRAW_ITEM_BLINK =		(1 << 8),
	STUDIORENDER_SHADOWDEPTHTEXTURE =	(1 << 9),
	STUDIORENDER_UNUSED =				(1 << 10),
	STUDIORENDER_SKIP_DECALS =			(1 << 11),
	STUDIORENDER_MODEL_IS_CACHEABLE =	(1 << 12),
	STUDIORENDER_SHADOWDEPTHTEXTURE_INCLUDE_TRANSLUCENT_MATERIALS = (1 << 13),
	STUDIORENDER_NO_PRIMARY_DRAW =		(1 << 14),
	STUDIORENDER_SSAODEPTHTEXTURE =		(1 << 15)
};

enum EOverrideType : int
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SELECTIVE,
	OVERRIDE_SSAO_DEPTH_WRITE
};
#pragma endregion

// forward declarations
struct studiohdr_t;
struct studiohwdata_t;
struct DrawModelResults_t;
struct ColorMeshInfo_t;
class IClientRenderable;

using StudioDecalHandle_t = void*;

#pragma pack(push, 4)
struct MaterialLightingState_t
{
	Vector_t vecAmbientCube[6]; // 0x00
	Vector_t vecLightingOrigin; // 0x48
	int nLocalLightCount; // 0x54
	LightDesc_t localLightDesc[4]; // 0x58
};
static_assert(sizeof(MaterialLightingState_t) == 0x1B8);

struct DrawModelInfo_t
{
	studiohdr_t* pStudioHdr; // 0x00
	studiohwdata_t* pHardwareData; // 0x04
	StudioDecalHandle_t hDecals; // 0x08
	int iSkin; // 0x0C
	int iBody; // 0x10
	int iHitboxSet; // 0x14
	IClientRenderable* pClientEntity; // 0x18
	int iLOD; // 0x1C
	ColorMeshInfo_t* pColorMeshes; // 0x20
	bool bStaticLighting; // 0x24
	MaterialLightingState_t lightingState; // 0x28
};
static_assert(sizeof(DrawModelInfo_t) == 0x1E0);
#pragma pack(pop)

class IStudioRender : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void SetAmbientLightColors(const Vector4D_t* pAmbientOnlyColors)
	{
		CallVFunc<void, 20U>(this, pAmbientOnlyColors);
	}

	void SetAmbientLightColors(const Vector_t* pAmbientOnlyColors)
	{
		CallVFunc<void, 21U>(this, pAmbientOnlyColors);
	}

	void SetLocalLights(int nLights, const LightDesc_t* pLights)
	{
		CallVFunc<void, 22U>(this, nLights, pLights);
	}

	void SetColorModulation(const float* arrColor)
	{
		CallVFunc<void, 27U>(this, arrColor);
	}

	void SetAlphaModulation(float flAlpha)
	{
		CallVFunc<void, 28U>(this, flAlpha);
	}

	void DrawModel(DrawModelResults_t* pResults, const DrawModelInfo_t& info, Matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector_t& vecModelOrigin, int nFlags)
	{
		CallVFunc<void, 29U>(this, pResults, &info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, &vecModelOrigin, nFlags);
	}

	void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType nOverrideType = OVERRIDE_NORMAL, int nOverrides = 0)
	{
		CallVFunc<void, 33U>(this, pMaterial, nOverrideType, nOverrides);
	}
};
