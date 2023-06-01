#pragma once
#include "../datatypes/utlvector.h"

// used: imatrendercontext, stencilstate, material, texture
#include "imatrendercontext.h"

// @source: master/public/materialsystem/imaterialsystem.h

#pragma region materialsystem_definitions
// @source: master/public/texture_group_names.h
#define TEXTURE_GROUP_LIGHTMAP "Lightmaps"
#define TEXTURE_GROUP_WORLD "World textures"
#define TEXTURE_GROUP_MODEL "Model textures"
#define TEXTURE_GROUP_STATIC_PROP "StaticProp textures"
#define TEXTURE_GROUP_COMBINED "Combined Textures"
#define TEXTURE_GROUP_COMPOSITE "Composited Textures"
#define TEXTURE_GROUP_VGUI "VGUI textures"
#define TEXTURE_GROUP_PARTICLE "Particle textures"
#define TEXTURE_GROUP_DECAL "Decal textures"
#define TEXTURE_GROUP_SKYBOX "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS "ClientEffect textures"
#define TEXTURE_GROUP_OTHER "Other textures"
#define TEXTURE_GROUP_PRECACHED "Precached"
#define TEXTURE_GROUP_CUBE_MAP "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED "Unaccounted textures"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP "Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR "Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD "World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS "Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER "Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS "Morph Targets"
#define TEXTURE_GROUP_SCALEFORM "Scaleform textures"
#pragma endregion

#pragma region materialsystem_enumerations
enum ECreateRenderTargetFlags : unsigned int
{
	CREATERENDERTARGETFLAGS_HDR = (1 << 0),
	CREATERENDERTARGETFLAGS_AUTOMIPMAP = (1 << 1),
	CREATERENDERTARGETFLAGS_UNFILTERABLE_OK = (1 << 2),
	CREATERENDERTARGETFLAGS_NOEDRAM = (1 << 3),
	CREATERENDERTARGETFLAGS_TEMP = (1 << 4)
};

enum ETextureFlags : unsigned int
{
	TEXTUREFLAGS_POINTSAMPLE =			(1U << 0U),
	TEXTUREFLAGS_TRILINEAR =			(1U << 1U),
	TEXTUREFLAGS_CLAMPS =				(1U << 2U),
	TEXTUREFLAGS_CLAMPT =				(1U << 3U),
	TEXTUREFLAGS_ANISOTROPIC =			(1U << 4U),
	TEXTUREFLAGS_HINT_DXT5 =			(1U << 5U),
	TEXTUREFLAGS_SRGB =					(1U << 6U),
	TEXTUREFLAGS_NORMAL =				(1U << 7U),
	TEXTUREFLAGS_NOMIP =				(1U << 8U),
	TEXTUREFLAGS_NOLOD =				(1U << 9U),
	TEXTUREFLAGS_ALL_MIPS =				(1U << 10U),
	TEXTUREFLAGS_PROCEDURAL =			(1U << 11U),
	TEXTUREFLAGS_ONEBITALPHA =			(1U << 12U),
	TEXTUREFLAGS_EIGHTBITALPHA =		(1U << 13U),
	TEXTUREFLAGS_ENVMAP =				(1U << 14U),
	TEXTUREFLAGS_RENDERTARGET =			(1U << 15U),
	TEXTUREFLAGS_DEPTHRENDERTARGET =	(1U << 16U),
	TEXTUREFLAGS_NODEBUGOVERRIDE =		(1U << 17U),
	TEXTUREFLAGS_SINGLECOPY =			(1U << 18U),
	TEXTUREFLAGS_UNUSED_00080000 =		(1U << 19U),
	TEXTUREFLAGS_UNUSED_00100000 =		(1U << 20U),
	TEXTUREFLAGS_UNUSED_00200000 =		(1U << 21U),
	TEXTUREFLAGS_UNUSED_00400000 =		(1U << 22U),
	TEXTUREFLAGS_NODEPTHBUFFER =		(1U << 23U),
	TEXTUREFLAGS_UNUSED_01000000 =		(1U << 24U),
	TEXTUREFLAGS_CLAMPU =				(1U << 25U),
	TEXTUREFLAGS_VERTEXTEXTURE =		(1U << 26U),
	TEXTUREFLAGS_SSBUMP =				(1U << 27U),
	TEXTUREFLAGS_UNUSED_10000000 =		(1U << 28U),
	TEXTUREFLAGS_BORDER =				(1U << 29U),
	TEXTUREFLAGS_UNUSED_40000000 =		(1U << 30U),
	TEXTUREFLAGS_UNUSED_80000000 =		(1U << 31U)
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
	RT_SIZE_NO_CHANGE = 0, // only allowed for render targets that don't want a depth buffer
	RT_SIZE_DEFAULT, // don't play with the specified width and height other than making sure it fits in the framebuffer.
	RT_SIZE_PICMIP, // apply picmip to the render target's width and height.
	RT_SIZE_HDR, // frame_buffer_width / 4
	RT_SIZE_FULL_FRAME_BUFFER, // same size as frame buffer, or next lower power of 2 if we can't do that.
	RT_SIZE_OFFSCREEN, // target of specified size, don't mess with dimensions
	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP, // same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
	RT_SIZE_REPLAY_SCREENSHOT, // rounded down to power of 2, essentially...
	RT_SIZE_LITERAL // use the size passed in. Don't clamp it to the frame buffer size. Really.
};

enum EMaterialRenderTargetDepth : unsigned int
{
	MATERIAL_RT_DEPTH_SHARED = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE = 0x2,
	MATERIAL_RT_DEPTH_ONLY = 0x3
};

enum ECSMQualityMode : int
{
	CSMQUALITY_VERY_LOW,
	CSMQUALITY_LOW,
	CSMQUALITY_MEDIUM,
	CSMQUALITY_HIGH,
	CSMQUALITY_TOTAL_MODES
};
#pragma endregion

using MaterialHandle_t = std::uint16_t;

#pragma pack(push, 4)
// @source: master/public/materialsystem/materialsystem_config.h
struct MaterialVideoMode_t
{
	int iWidth; // 0x00
	int iHeight; // 0x04
	EImageFormat nFormat; // 0x08
	int	iRefreshRate; // 0x0C
};
static_assert(sizeof(MaterialVideoMode_t) == 0x10);

struct MaterialSystemConfig_t
{
	MaterialVideoMode_t VideoMode; // 0x00
	float flMonitorGamma; // 0x10
	float flGammaTVRangeMin; // 0x14
	float flGammaTVRangeMax; // 0x18
	float flGammaTVExponent; // 0x1C
	bool bGammaTVEnabled; // 0x20
	bool bTripleBuffered; // 0x21
	int nAASamples; // 0x24
	int nForceAnisotropicLevel; // 0x28
	int iSkipMipLevels; // 0x2C
	int nDxSupportLevel; // 0x30
	std::uint32_t uFlags; // 0x34
	bool bEditMode; // 0x38
	std::uint8_t nProxiesTestMode; // 0x39
	bool bCompressedTextures; // 0x3A
	bool bFilterLightmaps; // 0x3B
	bool bFilterTextures; // 0x3C
	bool bReverseDepth; // 0x3D
	bool bBufferPrimitives; // 0x3E
	bool bDrawFlat; // 0x3F
	bool bMeasureFillRate; // 0x40
	bool bVisualizeFillRate; // 0x41
	bool bNoTransparency; // 0x42
	bool bSoftwareLighting; // 0x43
	bool bAllowCheats; // 0x44
	std::int8_t nShowMipLevels; // 0x45
	bool bShowLowResImage; // 0x46
	bool bShowNormalMap; // 0x47
	bool bMipMapTextures; // 0x48
	std::uint8_t uFullbright; // 0x49
	bool bFastNoBump; // 0x4A
	bool bSuppressRendering; // 0x4B
	bool bDrawGray; // 0x4C
	bool bShowSpecular; // 0x4D
	bool bShowDiffuse; // 0x4E
	std::uint32_t uWindowedSizeLimitWidth; // 0x50
	std::uint32_t uWindowedSizeLimitHeight; // 0x54
	int nAAQuality; // 0x58
	bool bShadowDepthTexture; // 0x5C
	bool bMotionBlur; // 0x5D
	bool bSupportFlashlight; // 0x5E
	bool bPaintInGame; // 0x5F
	bool bPaintInMap; // 0x60
	ECSMQualityMode nCSMQuality; // 0x64
	bool bCSMAccurateBlending; // 0x68
};
static_assert(sizeof(MaterialSystemConfig_t) == 0x6C);

// @source: master/public/materialsystem/icompositetexture.h
class ICompositeTexture
{
	public:
	virtual bool IsReady() const = 0;
	virtual bool GenerationComplete() const = 0;
	virtual void*/*IVTFTexture**/ GetResultVTF() = 0;
	virtual const char* GetName() = 0;
};

// @source: master/materialsystem/custom_material.h
class ICustomMaterial : public CRefCounted
{
public:
	virtual IMaterial* GetMaterial() = 0;
	virtual void AddTexture(ICompositeTexture* pTexture) = 0;
	virtual ICompositeTexture* GetTexture(int nIndex) = 0;
	virtual bool IsValid() const = 0;
	virtual bool CheckRegenerate(int nSize) = 0;
	virtual const char* GetBaseMaterialName() = 0;

public:
	IMaterial* pMaterial; // 0x08
	CUtlVector<ICompositeTexture*> vecTextures; // 0x0C
	bool bValid; // 0x20
	int nModelMaterialIndex; // 0x24
	CKeyValues* pVMTKeyValues; // 0x28
	const char* szBaseMaterialName; // 0x2C
};
static_assert(sizeof(ICustomMaterial) == 0x30);

// @source: master/public/materialsystem/custommaterialowner.h
class CCustomMaterialOwner
{
public:
	virtual ~CCustomMaterialOwner() { }
	virtual void SetCustomMaterial(ICustomMaterial* pCustomMaterial, int nIndex = 0) = 0;
	virtual void OnCustomMaterialsUpdated() = 0;
	virtual void DuplicateCustomMaterialsToOther(CCustomMaterialOwner* pOther) const = 0;

public:
	CUtlVector<ICustomMaterial*> vecCustomMaterials; // 0x04
};
static_assert(sizeof(CCustomMaterialOwner) == 0x18);

// @source: master/public/materialsystem/ivisualsdataprocessor.h
class IVisualsDataProcessor : public CRefCounted
{
protected:
	virtual ~IVisualsDataProcessor() { }
public:
	virtual CKeyValues* GenerateCustomMaterialKeyValues() = 0;
	virtual CKeyValues* GenerateCompositeMaterialKeyValues(int nMaterialParamId) = 0;
	virtual void* GetCompareObject() = 0;
	virtual bool HasCustomMaterial() const = 0;
	virtual const char* GetOriginalMaterialName() const = 0;
	virtual const char* GetOriginalMaterialBaseName() const = 0;
	virtual const char* GetPatternVTFName() const = 0;
	virtual void Refresh() = 0;
};
static_assert(sizeof(IVisualsDataProcessor) == 0x8);
#pragma pack(pop)

class IMaterialSystem : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	EImageFormat GetBackBufferFormat()
	{
		return CallVFunc<EImageFormat, 36U>(this);
	}

	IMaterial* CreateMaterial(const char* szName, CKeyValues* pKeyValues)
	{
		return CallVFunc<IMaterial*, 83U>(this, szName, pKeyValues);
	}

	IMaterial* FindMaterial(const char* szMaterialName, const char* szTextureGroupName = TEXTURE_GROUP_MODEL, bool bComplain = true, const char* pComplainPrefix = nullptr)
	{
		return CallVFunc<IMaterial*, 84U>(this, szMaterialName, szTextureGroupName, bComplain, pComplainPrefix);
	}

	MaterialHandle_t FirstMaterial()
	{
		return CallVFunc<MaterialHandle_t, 86U>(this);
	}

	MaterialHandle_t NextMaterial(MaterialHandle_t hMaterial)
	{
		return CallVFunc<MaterialHandle_t, 87U>(this, hMaterial);
	}

	MaterialHandle_t InvalidMaterial()
	{
		return CallVFunc<MaterialHandle_t, 88U>(this);
	}

	IMaterial* GetMaterial(MaterialHandle_t hMaterial)
	{
		return CallVFunc<IMaterial*, 89U>(this, hMaterial);
	}

	int	GetNumMaterials()
	{
		return CallVFunc<int, 90U>(this);
	}

	ITexture* FindTexture(const char* szTextureName, const char* szTextureGroupName, bool bComplain = true, int nAdditionalCreationFlags = 0)
	{
		return CallVFunc<ITexture*, 91U>(this, szTextureName, szTextureGroupName, bComplain, nAdditionalCreationFlags);
	}

	void BeginRenderTargetAllocation()
	{
		CallVFunc<void, 94U>(this);
	}

	void EndRenderTargetAllocation()
	{
		CallVFunc<void, 95U>(this);
	}

	void ForceBeginRenderTargetAllocation()
	{
		const bool bOldState = bDisableRenderTargetAllocationForever;

		bDisableRenderTargetAllocationForever = false;
		BeginRenderTargetAllocation();
		bDisableRenderTargetAllocationForever = bOldState;
	}

	void ForceEndRenderTargetAllocation()
	{
		const bool bOldState = bDisableRenderTargetAllocationForever;

		bDisableRenderTargetAllocationForever = false;
		EndRenderTargetAllocation();
		bDisableRenderTargetAllocationForever = bOldState;
	}

	ITexture* CreateRenderTargetTexture(int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED)
	{
		return CallVFunc<ITexture*, 96U>(this, iWidth, iHeight, sizeMode, format, depth);
	}

	ITexture* CreateNamedRenderTargetTextureEx(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		return CallVFunc<ITexture*, 97U>(this, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
	}

	ITexture* CreateNamedRenderTargetTexture(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false)
	{
		return CallVFunc<ITexture*, 98U>(this, szName, iWidth, iHeight, sizeMode, format, depth, bClampTexCoords, bAutoMipMap);
	}

	ITexture* CreateNamedRenderTargetTextureEx2(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		return CallVFunc<ITexture*, 99U>(this, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
	}

	IMatRenderContext* GetRenderContext()
	{
		return CallVFunc<IMatRenderContext*, 115U>(this);
	}

	void FinishRenderTargetAllocation()
	{
		CallVFunc<void, 136U>(this);
	}

	// i realize if i call this all textures will be unloaded and load time will suffer horribly
	void ReEnableRenderTargetAllocation()
	{
		CallVFunc<void, 137U>(this);
	}

public:
	std::byte pad0[0x2B94]; // 0x0000
	bool bRequestedEditorMaterials; // 0x2B94 // @xref: "-tools"
	std::byte pad1[0x7F]; // 0x2B95
	int nAllocatingRenderTargets; // 0x2C14
	bool bDisableRenderTargetAllocationForever; // 0x2C18 // @ida: materialsystem.dll -> ["80 B9 ? ? ? ? ? 74 0F" + 0x2] // @xref: "Tried BeginRenderTargetAllocation after game startup. If I let you do this, all users would suffer.\n"
};
static_assert(sizeof(IMaterialSystem) == 0x2C1C);

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
