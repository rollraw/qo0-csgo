#pragma once
// used: keyvalues
#include "../datatypes/keyvalues.h"
// used: call virtual function
#include "../../utilities/memory.h"

using VertexFormat_t = std::uint64_t;

#pragma region material_enumerations
enum EPreviewImageRetVal : int
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum EMaterialPropertyType : int
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,
	MATERIAL_PROPERTY_OPACITY,
	MATERIAL_PROPERTY_REFLECTIVITY,
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS
};

enum EMaterialVarFlags : int
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_PSEUDO_TRANSLUCENT = (1 << 9), // used to mark water materials for rendering after opaques but before translucents (with alpha blending but also with depth writes)
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	MATERIAL_VAR_AOPREPASS = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_MULTIPLY = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31)
};

enum EImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,	// note: D3D9 does not have this format
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,

	IMAGE_FORMAT_NULL,			// dummy format which takes no video memory

	// compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// one-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// two-surface ATI1N format

	IMAGE_FORMAT_RGBA1010102,	// 10 bit-per component render targets
	IMAGE_FORMAT_BGRA1010102,
	IMAGE_FORMAT_R16F,			// 16 bit FP format

	// depth-stencil texture formats
	IMAGE_FORMAT_D16,
	IMAGE_FORMAT_D15S1,
	IMAGE_FORMAT_D32,
	IMAGE_FORMAT_D24S8,
	IMAGE_FORMAT_LINEAR_D24S8,
	IMAGE_FORMAT_D24X8,
	IMAGE_FORMAT_D24X4S4,
	IMAGE_FORMAT_D24FS8,
	IMAGE_FORMAT_D16_SHADOW,	// specific formats for shadow mapping
	IMAGE_FORMAT_D24X8_SHADOW,	// specific formats for shadow mapping

	// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,

	NUM_IMAGE_FORMATS
};
#pragma endregion

class IMaterialVar
{
public:
	ITexture* GetTexture()
	{
		return MEM::CallVFunc<ITexture*>(this, 1);
	}

	void SetFloat(float flValue)
	{
		MEM::CallVFunc<void>(this, 4, flValue);
	}

	void SetInt(int iValue)
	{
		MEM::CallVFunc<void>(this, 5, iValue);
	}

	void SetString(const char* szValue)
	{
		MEM::CallVFunc<void>(this, 6, szValue);
	}

	void SetVector(float x, float y)
	{
		MEM::CallVFunc<void>(this, 10, x, y);
	}

	void SetVector(float x, float y, float z)
	{
		MEM::CallVFunc<void>(this, 11, x, y, z);
	}

	void SetTexture(ITexture* pTexture)
	{
		MEM::CallVFunc<void>(this, 15, pTexture);
	}

	void SetVectorComponent(float flValue, int iComponent)
	{
		MEM::CallVFunc<void>(this, 26, flValue, iComponent);
	}
};

class IMaterial
{
public:
	virtual const char*		GetName() const = 0;
	virtual const char*		GetTextureGroupName() const = 0;
	virtual EPreviewImageRetVal GetPreviewImageProperties(int* iWidth, int* iHeight, EImageFormat* pImageFormat, bool* bTranslucent) const = 0;
	virtual EPreviewImageRetVal GetPreviewImage(unsigned char* pData, int iWidth, int iHeight, EImageFormat imageFormat) const = 0;
	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;
	virtual int				GetNumAnimationFrames() = 0;
	virtual bool			InMaterialPage() = 0;
	virtual	void			GetMaterialOffset(float* flOffset) = 0;
	virtual void			GetMaterialScale(float* flScale) = 0;
	virtual IMaterial*		GetMaterialPage() = 0;
	virtual IMaterialVar*	FindVar(const char* szName, bool* bFound, bool bComplain = true) = 0;
	virtual void			IncrementReferenceCount() = 0;
	virtual void			DecrementReferenceCount() = 0;
	inline void				AddReference() { IncrementReferenceCount(); }
	inline void				Release() { DecrementReferenceCount(); }
	virtual int 			GetEnumerationID() const = 0;
	virtual void			GetLowResColorSample(float s, float t, float* arrColor) const = 0;
	virtual void			RecomputeStateSnapshots() = 0;
	virtual bool			IsTranslucent() = 0;
	virtual bool			IsAlphaTested() = 0;
	virtual bool			IsVertexLit() = 0;
	virtual VertexFormat_t	GetVertexFormat() const = 0;
	virtual bool			HasProxy() const = 0;
	virtual bool			UsesEnvCubemap() = 0;
	virtual bool			NeedsTangentSpace() = 0;
	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsSoftwareSkinning() = 0;
	virtual void			AlphaModulate(float flAlpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;
	virtual void			SetMaterialVarFlag(EMaterialVarFlags flag, bool bEnable) = 0;
	virtual bool			GetMaterialVarFlag(EMaterialVarFlags flag) = 0;
	virtual void			GetReflectivity(Vector& vecReflect) = 0;
	virtual bool			GetPropertyFlag(EMaterialPropertyType type) = 0;
	virtual bool			IsTwoSided() = 0;
	virtual void			SetShader(const char* szShaderName) = 0;
	virtual int				GetNumPasses() = 0;
	virtual int				GetTextureMemoryBytes() = 0;
	virtual void			Refresh() = 0;
	virtual bool			NeedsLightmapBlendAlpha() = 0;
	virtual bool			NeedsSoftwareLighting() = 0;
	virtual int				ShaderParamCount() const = 0;
	virtual IMaterialVar**	GetShaderParams() = 0;
	virtual bool			IsErrorMaterial() const = 0;
	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float* r, float* g, float* b) = 0;
	virtual bool			IsTranslucentUnderModulation(float flAlphaModulation = 1.0f) const = 0;
	virtual IMaterialVar*	FindVarFast(char const* szName, unsigned int* puToken) = 0;
	virtual void			SetShaderAndParams(CKeyValues* pKeyValues) = 0;
	virtual const char*		GetShaderName() const = 0;
	virtual void			DeleteIfUnreferenced() = 0;
	virtual bool			IsSpriteCard() = 0;
	virtual void			CallBindProxy(void* pProxyData) = 0;
	virtual void			RefreshPreservingMaterialVars() = 0;
	virtual bool			WasReloadedFromWhitelist() = 0;
	virtual bool			SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
	virtual int				GetReferenceCount() const = 0;
};
