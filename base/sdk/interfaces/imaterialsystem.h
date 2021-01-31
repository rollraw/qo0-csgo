#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h

// used: imatrendercontext, stencilstate, material, texture
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
	CREATERENDERTARGETFLAGS_NOEDRAM	= 0x00000008,
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

	ITexture* CreateRenderTargetTexture(int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED)
	{
		return MEM::CallVFunc<ITexture*>(this, 96, iWidth, iHeight, sizeMode, format, depth);
	}

	ITexture* CreateNamedRenderTargetTextureEx(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		return MEM::CallVFunc<ITexture*>(this, 97, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
	}

	ITexture* CreateNamedRenderTargetTexture(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false)
	{
		return MEM::CallVFunc<ITexture*>(this, 98, szName, iWidth, iHeight, sizeMode, format, depth, bClampTexCoords, bAutoMipMap);
	}

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
