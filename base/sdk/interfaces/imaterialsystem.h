#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h

// used: imatrendercontext, stencilstate
#include "imatrendercontext.h"
// used: texture_group definitions
#include "../definitions.h"

using MaterialHandle_t = std::uint16_t;

#pragma region materialsystem_enumerations
enum ECreateRenderTargetFlags : unsigned int
{
	CREATERENDERTARGETFLAGS_HDR = 0x00000001,
	CREATERENDERTARGETFLAGS_AUTOMIPMAP = 0x00000002,
	CREATERENDERTARGETFLAGS_UNFILTERABLE_OK = 0x00000004,
	CREATERENDERTARGETFLAGS_NOEDRAM = 0x00000008,
	CREATERENDERTARGETFLAGS_TEMP = 0x00000010
};

enum ETextureFlags : unsigned int
{
	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
	TEXTUREFLAGS_TRILINEAR = 0x00000002,
	TEXTUREFLAGS_CLAMPS = 0x00000004,
	TEXTUREFLAGS_CLAMPT = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
	TEXTUREFLAGS_SRGB = 0x00000040,
	TEXTUREFLAGS_NORMAL = 0x00000080,
	TEXTUREFLAGS_NOMIP = 0x00000100,
	TEXTUREFLAGS_NOLOD = 0x00000200,
	TEXTUREFLAGS_ALL_MIPS = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL = 0x00000800,
	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
	TEXTUREFLAGS_ENVMAP = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY = 0x00040000,
	TEXTUREFLAGS_UNUSED_00080000 = 0x00080000,
	TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
	TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
	TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
	TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
	TEXTUREFLAGS_CLAMPU = 0x02000000,
	TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
	TEXTUREFLAGS_SSBUMP = 0x08000000,
	TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
	TEXTUREFLAGS_BORDER = 0x20000000,
	TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
	TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
};

enum EMaterialPropertyType : int
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,
	MATERIAL_PROPERTY_OPACITY,
	MATERIAL_PROPERTY_REFLECTIVITY,
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS
};

enum EPreviewImageRetVal : int
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum EClearFlags : unsigned int
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10,
	VIEW_CLEAR_STENCIL = 0x20
};

enum ERenderTargetSizeMode : int
{
	RT_SIZE_NO_CHANGE = 0,					// only allowed for render targets that don't want a depth buffer
	RT_SIZE_DEFAULT,						// don't play with the specified width and height other than making sure it fits in the framebuffer.
	RT_SIZE_PICMIP,							// apply picmip to the render target's width and height.
	RT_SIZE_HDR,							// frame_buffer_width / 4
	RT_SIZE_FULL_FRAME_BUFFER,				// same size as frame buffer, or next lower power of 2 if we can't do that.
	RT_SIZE_OFFSCREEN,						// target of specified size, don't mess with dimensions
	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP,	// same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
	RT_SIZE_REPLAY_SCREENSHOT,				// rounded down to power of 2, essentially...
	RT_SIZE_LITERAL							// use the size passed in. Don't clamp it to the frame buffer size. Really.
};

enum EMaterialVarFlags
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

enum EMaterialRenderTargetDepth : unsigned int
{
	MATERIAL_RT_DEPTH_SHARED = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE = 0x2,
	MATERIAL_RT_DEPTH_ONLY = 0x3
};
#pragma endregion

// @credits: https://github.com/pmrowla/hl2sdk-csgo/blob/master/public/materialsystem/materialsystem_config.h
struct MaterialVideoMode_t
{
	int				iWidth;
	int				iHeight;
	EImageFormat	Format;
	int				iRefreshRate;
};

struct MaterialSystemConfig_t
{
	MaterialVideoMode_t VideoMode;
	float				flMonitorGamma;
	float				flGammaTVRangeMin;
	float				flGammaTVRangeMax;
	float				flGammaTVExponent;
	bool				bGammaTVEnabled;
	bool				bTripleBuffered;
	int					nAASamples;
	int					nForceAnisotropicLevel;
	int					iSkipMipLevels;
	int					nDxSupportLevel;
	unsigned int		uFlags;
	bool				bEditMode;
	unsigned char		dProxiesTestMode;
	bool				bCompressedTextures;
	bool				bFilterLightmaps;
	bool				bFilterTextures;
	bool				bReverseDepth;
	bool				bBufferPrimitives;
	bool				bDrawFlat;
	bool				bMeasureFillRate;
	bool				bVisualizeFillRate;
	bool				bNoTransparency;
	bool				bSoftwareLighting;
	bool				bAllowCheats;
	char				nShowMipLevels;
	bool				bShowLowResImage;
	bool				bShowNormalMap;
	bool				bMipMapTextures;
	unsigned char		uFullbright;
	bool				bFastNoBump;
	bool				bSuppressRendering;
	bool				bDrawGray;
	bool				bShowSpecular;
	bool				bShowDiffuse;
	std::uint32_t		uWindowedSizeLimitWidth;
	std::uint32_t		uWindowedSizeLimitHeight;
	int					nAAQuality;
	bool				bShadowDepthTexture;
	bool				bMotionBlur;
	bool				bSupportFlashlight;
	bool				bPaintEnabled;
	std::byte			pad0[0xC];
};

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

class IMaterialSystem
{
public:
	EImageFormat GetBackBufferFormat()
	{
		return MEM::CallVFunc<EImageFormat>(this, 36);
	}

	IMaterial* CreateMaterial(const char* szName, CKeyValues* pKeyValues)
	{
		return MEM::CallVFunc<IMaterial*>(this, 83, szName, pKeyValues);
	}

	IMaterial* FindMaterial(char const* szMaterialName, const char* szTextureGroupName = TEXTURE_GROUP_MODEL, bool bComplain = true, const char* pComplainPrefix = nullptr)
	{
		return MEM::CallVFunc<IMaterial*>(this, 84, szMaterialName, szTextureGroupName, bComplain, pComplainPrefix);
	}

	MaterialHandle_t FirstMaterial()
	{
		return MEM::CallVFunc<MaterialHandle_t>(this, 86);
	}

	MaterialHandle_t NextMaterial(MaterialHandle_t hMaterial)
	{
		return MEM::CallVFunc<MaterialHandle_t>(this, 87, hMaterial);
	}

	MaterialHandle_t InvalidMaterial()
	{
		return MEM::CallVFunc<MaterialHandle_t>(this, 88);
	}

	IMaterial* GetMaterial(MaterialHandle_t hMaterial)
	{
		return MEM::CallVFunc<IMaterial*>(this, 89, hMaterial);
	}

	int	GetNumMaterials()
	{
		return MEM::CallVFunc<int>(this, 90);
	}

	ITexture* FindTexture(char const* szTextureName, const char* szTextureGroupName, bool bComplain = true, int nAdditionalCreationFlags = 0)
	{
		return MEM::CallVFunc<ITexture*>(this, 91, szTextureName, szTextureGroupName, bComplain, nAdditionalCreationFlags);
	}

	void BeginRenderTargetAllocation()
	{
		MEM::CallVFunc<void>(this, 94);
	}

	void EndRenderTargetAllocation()
	{
		MEM::CallVFunc<void>(this, 95);
	}

	void ForceBeginRenderTargetAllocation()
	{
		const bool bOldState = DisableRenderTargetAllocationForever();

		DisableRenderTargetAllocationForever() = false;
		BeginRenderTargetAllocation();
		DisableRenderTargetAllocationForever() = bOldState;
	}

	void ForceEndRenderTargetAllocation()
	{
		const bool bOldState = DisableRenderTargetAllocationForever();

		DisableRenderTargetAllocationForever() = false;
		EndRenderTargetAllocation();
		DisableRenderTargetAllocationForever() = bOldState;
	}

	ITexture* CreateNamedRenderTargetTextureEx(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		return MEM::CallVFunc<ITexture*>(this, 97, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
	}

	// must be called between the above begin-end calls
	ITexture* CreateNamedRenderTargetTextureEx2(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		return MEM::CallVFunc<ITexture*>(this, 99, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
	}

	IMatRenderContext* GetRenderContext()
	{
		return MEM::CallVFunc<IMatRenderContext*>(this, 115);
	}

	void FinishRenderTargetAllocation()
	{
		MEM::CallVFunc<void>(this, 136);
	}

	// i realize if i call this all textures will be unloaded and load time will suffer horribly
	void ReEnableRenderTargetAllocation()
	{
		MEM::CallVFunc<void>(this, 137);
	}

	bool& DisableRenderTargetAllocationForever()
	{
		// @xref: "Tried BeginRenderTargetAllocation after game startup. If I let you do this, all users would suffer.\n"
		static auto uDisableRenderTargetAllocationForever = *reinterpret_cast<std::uintptr_t*>(MEM::FindPattern(MATERIALSYSTEM_DLL, XorStr("80 B9 ? ? ? ? ? 74 0F")) + 0x2);
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + uDisableRenderTargetAllocationForever);
	}
};

class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
	typedef CRefPtr<IMatRenderContext> CBaseClass;
public:
	CMatRenderContextPtr() = default;

	CMatRenderContextPtr(IMatRenderContext* pInit) : CBaseClass(pInit)
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->BeginRender();
	}

	CMatRenderContextPtr(IMaterialSystem* pFrom) : CBaseClass(pFrom->GetRenderContext())
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->BeginRender();
	}

	~CMatRenderContextPtr()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->EndRender();
	}

	IMatRenderContext* operator=(IMatRenderContext* pSecondContext)
	{
		if (pSecondContext != nullptr)
			pSecondContext->BeginRender();

		return CBaseClass::operator=(pSecondContext);
	}

	void SafeRelease()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->EndRender();

		CBaseClass::SafeRelease();
	}

	void AssignAddReference(IMatRenderContext* pFrom)
	{
		if (CBaseClass::pObject)
			CBaseClass::pObject->EndRender();

		CBaseClass::AssignAddReference(pFrom);
		CBaseClass::pObject->BeginRender();
	}

	void GetFrom(IMaterialSystem* pFrom)
	{
		AssignAddReference(pFrom->GetRenderContext());
	}


private:
	CMatRenderContextPtr(const CMatRenderContextPtr& pRefPtr);
	void operator=(const CMatRenderContextPtr& pSecondRefPtr);
};
