#pragma once

#pragma region modelrender_definitions
// @source: master/public/model_types.h
enum EDrawModelFlags : int
{
	STUDIO_NONE =					0,
	STUDIO_RENDER =					(1 << 0),
	STUDIO_VIEWXFORMATTACHMENTS =	(1 << 1),
	STUDIO_DRAWTRANSLUCENTSUBMODELS = (1 << 2),
	STUDIO_TWOPASS =				(1 << 3),
	STUDIO_STATIC_LIGHTING =		(1 << 4),
	STUDIO_WIREFRAME =				(1 << 5),
	STUDIO_ITEM_BLINK =				(1 << 6),
	STUDIO_NOSHADOWS =				(1 << 7),
	STUDIO_WIREFRAME_VCOLLIDE =		(1 << 8),
	STUDIO_NOLIGHTING_OR_CUBEMAP =	(1 << 9),
	STUDIO_SKIP_FLEXES =			(1 << 10),
	STUDIO_DONOTMODIFYSTENCILSTATE = (1 << 11),
	STUDIO_SKIP_DECALS =			(1 << 12),
	STUDIO_SHADOWTEXTURE =			(1 << 13),
	STUDIO_SHADOWDEPTHTEXTURE =		(1 << 14),
	STUDIO_TRANSPARENCY =			(1 << 15)
};
#pragma endregion

// @source: master/public/engine/ivmodelrender.h

using ModelInstanceHandle_t = std::uint16_t;

#pragma pack(push, 4)
struct ModelRenderInfo_t
{
	Vector_t vecOrigin; // 0x00
	QAngle_t angAngles; // 0x0C
	std::byte pad0[0x4]; // 0x18
	IClientRenderable* pRenderable; // 0x1C
	const Model_t* pModel; // 0x20
	const Matrix3x4_t* pModelToWorld; // 0x24
	const Matrix3x4_t* pLightingOffset; // 0x28
	const Vector_t* pLightingOrigin; // 0x2C
	int iFlags; // 0x30
	int nEntityIndex; // 0x34
	int iSkin; // 0x38
	int iBody; // 0x3C
	int iHitboxSet; // 0x40
	ModelInstanceHandle_t hInstance; // 0x44
};
static_assert(sizeof(ModelRenderInfo_t) == 0x48);

struct DrawModelState_t
{
	studiohdr_t* pStudioHdr; // 0x00
	studiohwdata_t* pStudioHWData; // 0x04
	IClientRenderable* pRenderable; // 0x08
	const Matrix3x4_t* pModelToWorld; // 0x0C
	StudioDecalHandle_t* pDecals; // 0x10
	int iDrawFlags; // 0x14
	int iLOD; // 0x18
};
static_assert(sizeof(DrawModelState_t) == 0x1C);
#pragma pack(pop)

class IVModelRender : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType nOverrideType = OVERRIDE_NORMAL, int nOverrides = 0)
	{
		CallVFunc<void, 1U>(this, pMaterial, nOverrideType, nOverrides);
	}

	bool IsForcedMaterialOverride()
	{
		return CallVFunc<bool, 2U>(this);
	}

	void SuppressEngineLighting(bool bSuppress)
	{
		CallVFunc<void, 24U>(this, bSuppress);
	}
};
