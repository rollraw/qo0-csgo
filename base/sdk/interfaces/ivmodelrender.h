#pragma once

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/ivmodelrender.h
using ModelInstanceHandle_t = std::uint16_t;

struct ModelRenderInfo_t
{
	Vector					vecOrigin;
	QAngle					angAngles;
	std::byte				pad0[0x4];
	IClientRenderable*		pRenderable;
	const Model_t*			pModel;
	const matrix3x4_t*		pModelToWorld;
	const matrix3x4_t*		pLightingOffset;
	const Vector*			pLightingOrigin;
	int						iFlags;
	int						nEntityIndex;
	int						iSkin;
	int						iBody;
	int						iHitboxSet;
	ModelInstanceHandle_t	hInstance;
};

struct RenderableInfo_t
{
	IClientRenderable*		pRenderable;
	IClientAlphaProperty*	pAlphaProperty;
	int						iEnumCount;
	int						nRenderFrame;
	unsigned short			uFirstShadow;
	unsigned short			uLeafList;
	short					sArea;
	std::uint16_t			uFlags;
	std::uint16_t			uFlags2;
	Vector					vecBloatedAbsMins;
	Vector					vecBloatedAbsMaxs;
	Vector					vecAbsMins;
	Vector					vecAbsMaxs;
	std::byte				pad0[0x4];
};

struct studiohwdata_t;
struct StudioDecalHandle_t;
struct DrawModelState_t
{
	studiohdr_t*			pStudioHdr;
	studiohwdata_t*			pStudioHWData;
	IClientRenderable*		pRenderable;
	const matrix3x4_t*		pModelToWorld;
	StudioDecalHandle_t*	pDecals;
	int						iDrawFlags;
	int						iLOD;
};

class IVModelRender
{
public:
	void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType overrideType = OVERRIDE_NORMAL, int nOverrides = 0)
	{
		return MEM::CallVFunc<void>(this, 1, pMaterial, overrideType, nOverrides);
	}

	bool IsForcedMaterialOverride()
	{
		return MEM::CallVFunc<bool>(this, 2);
	}
};
