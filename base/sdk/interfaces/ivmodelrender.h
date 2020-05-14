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
	IClientRenderable*		pRenderable;		// 0x00
	IClientAlphaProperty*	pAlphaProperty;		// 0x04
	int						iEnumCount;			// 0x08
	int						nRenderFrame;		// 0x0C
	unsigned short			uFirstShadow;		// 0x10
	unsigned short			uLeafList;			// 0x12
	short					sArea;				// 0x14
	std::uint16_t			uFlags;				// 0x16
	std::uint16_t			uFlags2;			// 0x18
	Vector					vecBloatedAbsMins;	// 0x1A
	Vector					vecBloatedAbsMaxs;	// 0x26
	Vector					vecAbsMins;			// 0x32
	Vector					vecAbsMaxs;			// 0x3E
	std::byte				pad0[0x4];			// 0x4A
}; // Size: 0x4E

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
