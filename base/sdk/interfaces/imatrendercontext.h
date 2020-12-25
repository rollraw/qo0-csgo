#pragma once
// used: irefcouted
#include "irefcount.h"
// used: texture
#include "itexture.h"

using VertexFormat_t = std::uint64_t;

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/mathlib/lightdesc.h
enum ELightType : int
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT
};

enum ELightTypeOptimizationFlags
{
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
	LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED = 8
};

struct LightDesc_t
{
	void InitDirectional(const Vector& vecDirection, const Vector& vecColor)
	{
		this->nType = MATERIAL_LIGHT_DIRECTIONAL;
		this->vecColor = vecColor;
		this->vecDirection = vecDirection;
		this->flRange = 0.0f;
		this->flAttenuation0 = 1.0f;
		this->flAttenuation1 = 0.f;
		this->flAttenuation2 = 0.f;

		fFlags = LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED;

		if (flAttenuation0)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0;
		if (flAttenuation1)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1;
		if (flAttenuation2)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2;

		if (nType == MATERIAL_LIGHT_SPOT)
		{
			flThetaDot = std::cosf(flTheta);
			flPhiDot = std::cosf(flPhi);

			if (const float flSpread = flThetaDot - flPhiDot; flSpread > 1.0e-10f)
				// note - this quantity is very sensitive to round off error. the sse reciprocal approximation won't cut it here.
				flOneOverThetaDotMinusPhiDot = 1.0f / flSpread;
			else
				// hard falloff instead of divide by zero
				flOneOverThetaDotMinusPhiDot = 1.0f;
		}
		else if (nType == MATERIAL_LIGHT_DIRECTIONAL)
		{
			// set position to be real far away in the right direction
			vecPosition = vecDirection;
			vecPosition *= 2.0e6;
		}

		flRangeSquared = flRange * flRange;
	}

	ELightType	nType;
	Vector		vecColor;
	Vector		vecPosition;
	Vector		vecDirection;
	float		flRange;
	float		flFalloff;
	float		flAttenuation0;
	float		flAttenuation1;
	float		flAttenuation2;
	float		flTheta;
	float		flPhi;
	float		flThetaDot;
	float		flPhiDot;
	float		flOneOverThetaDotMinusPhiDot;
	std::uint32_t fFlags;
protected:
	float		flRangeSquared;
};

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/glow_outline_effect.cpp
enum EStencilOperation : int
{
	STENCILOPERATION_KEEP = 1,
	STENCILOPERATION_ZERO = 2,
	STENCILOPERATION_REPLACE = 3,
	STENCILOPERATION_INCRSAT = 4,
	STENCILOPERATION_DECRSAT = 5,
	STENCILOPERATION_INVERT = 6,
	STENCILOPERATION_INCR = 7,
	STENCILOPERATION_DECR = 8,
	STENCILOPERATION_FORCE_DWORD = 0x7FFFFFFF
};

enum EStencilComparisonFunction : int
{
	STENCILCOMPARISONFUNCTION_NEVER = 1,
	STENCILCOMPARISONFUNCTION_LESS = 2,
	STENCILCOMPARISONFUNCTION_EQUAL = 3,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
	STENCILCOMPARISONFUNCTION_GREATER = 5,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
	STENCILCOMPARISONFUNCTION_ALWAYS = 8,
	STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7FFFFFFF
};

struct ShaderStencilState_t
{
	ShaderStencilState_t()
	{
		bEnable = false;
		PassOperation = FailOperation = ZFailOperation = STENCILOPERATION_KEEP;
		CompareFunction = STENCILCOMPARISONFUNCTION_ALWAYS;
		nReferenceValue = 0;
		uTestMask = uWriteMask = 0xFFFFFFFF;
	}

	bool						bEnable;
	EStencilOperation			FailOperation;
	EStencilOperation			ZFailOperation;
	EStencilOperation			PassOperation;
	EStencilComparisonFunction	CompareFunction;
	int							nReferenceValue;
	std::uint32_t				uTestMask;
	std::uint32_t				uWriteMask;
};

enum EImageFormat : int
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

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h
class IMatRenderContext : public IRefCounted
{
public:
	void BeginRender()
	{
		MEM::CallVFunc<void>(this, 2);
	}

	void EndRender()
	{
		MEM::CallVFunc<void>(this, 3);
	}

	void BindLocalCubemap(ITexture* pTexture)
	{
		MEM::CallVFunc<void>(this, 5, pTexture);
	}

	void SetRenderTarget(ITexture* pTexture)
	{
		MEM::CallVFunc<void>(this, 6, pTexture);
	}

	ITexture* GetRenderTarget()
	{
		return MEM::CallVFunc<ITexture*>(this, 7);
	}

	void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false)
	{
		MEM::CallVFunc<void>(this, 12, bClearColor, bClearDepth, bClearStencil);
	}

	void SetLights(int nCount, const LightDesc_t* pLights)
	{
		MEM::CallVFunc<void>(this, 17, nCount, pLights);
	}

	void SetAmbientLightCube(Vector4D vecCube[6])
	{
		MEM::CallVFunc<void>(this, 18, vecCube);
	}

	void Viewport(int x, int y, int iWidth, int iHeight)
	{
		MEM::CallVFunc<void>(this, 40, x, y, iWidth, iHeight);
	}

	void GetViewport(int& x, int& y, int& iWidth, int& iHeight)
	{
		MEM::CallVFunc<void, int&, int&, int&, int&>(this, 41, x, y, iWidth, iHeight);
	}

	void ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
	{
		MEM::CallVFunc<void>(this, 78, r, g, b);
	}

	void ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		MEM::CallVFunc<void>(this, 79, r, g, b, a);
	}

	void DrawScreenSpaceRectangle(IMaterial* pMaterial, int iDestX, int iDestY, int iWidth, int iHeight, float flTextureX0, float flTextureY0, float flTextureX1, float flTextureY1, int iTextureWidth, int iTextureHeight, void* pClientRenderable = nullptr, int nXDice = 1, int nYDice = 1)
	{
		MEM::CallVFunc<void>(this, 114, pMaterial, iDestX, iDestY, iWidth, iHeight, flTextureX0, flTextureY0, flTextureX1, flTextureY1, iTextureWidth, iTextureHeight, pClientRenderable, nXDice, nYDice);
	}

	void PushRenderTargetAndViewport()
	{
		MEM::CallVFunc<void>(this, 119);
	}

	void PopRenderTargetAndViewport()
	{
		MEM::CallVFunc<void>(this, 120);
	}

	void SetLightingOrigin(/*Vector vecLightingOrigin*/float x, float y, float z)
	{
		MEM::CallVFunc<void>(this, 158, x, y, z);
	}
};
