#pragma once
#include "../datatypes/vector.h"

// used: irefcouted
#include "irefcount.h"
// used: texture
#include "itexture.h"
// used: material
#include "imaterial.h"
// used: callvfunc
#include "../../utilities/memory.h"

enum EMaterialMatrixMode : int
{
	MATERIAL_VIEW = 0,
	MATERIAL_PROJECTION,

	MATERIAL_TEXTURE0,
	MATERIAL_TEXTURE1,
	MATERIAL_TEXTURE2,
	MATERIAL_TEXTURE3,
	MATERIAL_TEXTURE4,
	MATERIAL_TEXTURE5,
	MATERIAL_TEXTURE6,
	MATERIAL_TEXTURE7,

	MATERIAL_MODEL,

	// total number of matrices
	NUM_MATRIX_MODES = MATERIAL_MODEL + 1,

	// number of texture transforms
	NUM_TEXTURE_TRANSFORMS = MATERIAL_TEXTURE7 - MATERIAL_TEXTURE0 + 1
};

// @source: master/public/mathlib/lightdesc.h
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
	void InitDirectional(const Vector_t& vecLightDirection, const Vector_t& vecLightColor)
	{
		this->nType = MATERIAL_LIGHT_DIRECTIONAL;
		this->vecColor = vecLightColor;
		this->vecDirection = vecLightDirection;
		this->flRange = 0.0f;
		this->flAttenuation0 = 1.0f;
		this->flAttenuation1 = 0.f;
		this->flAttenuation2 = 0.f;

		this->nFlags = LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED;

		if (this->flAttenuation0 > 0.f)
			this->nFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0;
		if (this->flAttenuation1 > 0.f)
			this->nFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1;
		if (this->flAttenuation2 > 0.f)
			this->nFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2;

		if (this->nType == MATERIAL_LIGHT_SPOT)
		{
			this->flThetaDot = std::cosf(this->flTheta);
			this->flPhiDot = std::cosf(this->flPhi);

			if (const float flSpread = this->flThetaDot - this->flPhiDot; flSpread > 1.0e-10f)
				// note - this quantity is very sensitive to round off error. the sse reciprocal approximation won't cut it here.
				this->flOneOverThetaDotMinusPhiDot = 1.0f / flSpread;
			else
				// hard falloff instead of divide by zero
				this->flOneOverThetaDotMinusPhiDot = 1.0f;
		}
		else if (this->nType == MATERIAL_LIGHT_DIRECTIONAL)
		{
			// set position to be real far away in the right direction
			this->vecPosition = vecDirection;
			this->vecPosition *= 2.0e6f;
		}

		this->flRangeSquared = this->flRange * this->flRange;
	}

	ELightType nType; // 0x00
	Vector_t vecColor; // 0x04
	Vector_t vecPosition; // 0x10
	Vector_t vecDirection; // 0x1C
	float flRange; // 0x28
	float flFalloff; // 0x2C
	float flAttenuation0; // 0x30
	float flAttenuation1; // 0x34
	float flAttenuation2; // 0x38
	float flTheta; // 0x3C
	float flPhi; // 0x40
	float flThetaDot; // 0x44
	float flPhiDot; // 0x48
	float flOneOverThetaDotMinusPhiDot; // 0x4C
	std::uint32_t nFlags; // 0x50
	float flRangeSquared; // 0x54
};
static_assert(sizeof(LightDesc_t) == 0x58);

// @source: master/public/shaderapi/ishaderapi.h
enum EShaderStencilOp : int
{
	SHADER_STENCILOP_KEEP = D3DSTENCILOP_KEEP,
	SHADER_STENCILOP_ZERO = D3DSTENCILOP_ZERO,
	SHADER_STENCILOP_SET_TO_REFERENCE = D3DSTENCILOP_REPLACE,
	SHADER_STENCILOP_INCREMENT_CLAMP = D3DSTENCILOP_INCRSAT,
	SHADER_STENCILOP_DECREMENT_CLAMP = D3DSTENCILOP_DECRSAT,
	SHADER_STENCILOP_INVERT = D3DSTENCILOP_INVERT,
	SHADER_STENCILOP_INCREMENT_WRAP = D3DSTENCILOP_INCR,
	SHADER_STENCILOP_DECREMENT_WRAP = D3DSTENCILOP_DECR,
	SHADER_STENCILOP_FORCE_DWORD = 0x7FFFFFFF
};

enum EShaderStencilFunc : int
{
	SHADER_STENCILFUNC_NEVER = 0,
	SHADER_STENCILFUNC_LESS,
	SHADER_STENCILFUNC_EQUAL,
	SHADER_STENCILFUNC_LEQUAL,
	SHADER_STENCILFUNC_GREATER,
	SHADER_STENCILFUNC_NOTEQUAL,
	SHADER_STENCILFUNC_GEQUAL,
	SHADER_STENCILFUNC_ALWAYS
};

#pragma pack(push, 4)
struct ShaderStencilState_t
{
	ShaderStencilState_t() :
		bEnable(false), iFailOperation(SHADER_STENCILOP_KEEP), iZFailOperation(SHADER_STENCILOP_KEEP), iPassOperation(SHADER_STENCILOP_KEEP), iCompareFunction(SHADER_STENCILFUNC_ALWAYS), nReferenceValue(0), uTestMask(0xFFFFFFFF), uWriteMask(0xFFFFFFFF) { }

	bool bEnable; // 0x00
	EShaderStencilOp iFailOperation; // 0x04
	EShaderStencilOp iZFailOperation; // 0x08
	EShaderStencilOp iPassOperation; // 0x0C
	EShaderStencilFunc iCompareFunction; // 0x10
	int nReferenceValue; // 0x14
	std::uint32_t uTestMask; // 0x18
	std::uint32_t uWriteMask; // 0x1C
};
static_assert(sizeof(ShaderStencilState_t) == 0x20);

// @source: master/public/materialsystem/imaterialsystem.h
class IMatRenderContext : public IRefCounted, ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void BeginRender()
	{
		CallVFunc<void, 2U>(this);
	}

	void EndRender()
	{
		CallVFunc<void, 3U>(this);
	}

	void BindLocalCubemap(ITexture* pTexture)
	{
		CallVFunc<void, 5U>(this, pTexture);
	}

	void SetRenderTarget(ITexture* pTexture)
	{
		CallVFunc<void, 6U>(this, pTexture);
	}

	ITexture* GetRenderTarget()
	{
		return CallVFunc<ITexture*, 7U>(this);
	}

	void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false)
	{
		CallVFunc<void, 12U>(this, bClearColor, bClearDepth, bClearStencil);
	}

	void SetLights(int nCount, const LightDesc_t* pLights)
	{
		CallVFunc<void, 17U>(this, nCount, pLights);
	}

	void SetAmbientLightCube(Vector4D_t vecCube[6])
	{
		CallVFunc<void, 18U>(this, vecCube);
	}

	void MatrixMode(EMaterialMatrixMode nMatrixMode)
	{
		CallVFunc<void, 22U>(this, nMatrixMode);
	}

	void PushMatrix()
	{
		CallVFunc<void, 23U>(this);
	}

	void PopMatrix()
	{
		CallVFunc<void, 24U>(this);
	}

	void LoadMatrix(const Matrix3x4_t& matView)
	{
		CallVFunc<void, 25U>(this, &matView);
	}

	void LoadMatrix(const ViewMatrix_t& matView)
	{
		CallVFunc<void, 26U>(this, &matView);
	}

	void GetMatrix(EMaterialMatrixMode nMatrixMode, Matrix3x4_t* pMatrix)
	{
		CallVFunc<void, 31U>(this, nMatrixMode, pMatrix);
	}

	void GetMatrix(EMaterialMatrixMode nMatrixMode, ViewMatrix_t* pMatrix)
	{
		CallVFunc<void, 32U>(this, nMatrixMode, pMatrix);
	}

	void Viewport(int x, int y, int iWidth, int iHeight)
	{
		CallVFunc<void, 40U>(this, x, y, iWidth, iHeight);
	}

	void GetViewport(int& x, int& y, int& iWidth, int& iHeight) const
	{
		CallVFunc<void, 41U>(this, &x, &y, &iWidth, &iHeight);
	}

	void ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
	{
		CallVFunc<void, 78U>(this, r, g, b);
	}

	void ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		CallVFunc<void, 79U>(this, r, g, b, a);
	}

	void DrawScreenSpaceRectangle(IMaterial* pMaterial, int iDestX, int iDestY, int iWidth, int iHeight, float flTextureX0, float flTextureY0, float flTextureX1, float flTextureY1, int iTextureWidth, int iTextureHeight, void* pClientRenderable = nullptr, int nXDice = 1, int nYDice = 1)
	{
		CallVFunc<void, 114U>(this, pMaterial, iDestX, iDestY, iWidth, iHeight, flTextureX0, flTextureY0, flTextureX1, flTextureY1, iTextureWidth, iTextureHeight, pClientRenderable, nXDice, nYDice);
	}

	void PushRenderTargetAndViewport()
	{
		// @ida: (CRender::ViewDrawFade) engine.dll -> ["FF 90 ? ? ? ? 8B 06 8B CE 6A 00 53" + 0x2] / sizeof(std::uintptr_t)
		CallVFunc<void, 119U>(this);
	}

	void PopRenderTargetAndViewport()
	{
		// @ida: (CRender::ViewDrawFade) engine.dll -> ["FF 90 ? ? ? ? 8B 8C 24" + 0x2] / sizeof(std::uintptr_t)
		CallVFunc<void, 120U>(this);
	}

	void SetStencilState(const ShaderStencilState_t& state)
	{
		// @ida: (CGlowObjectManager::ApplyEntityGlowEffects) client.dll -> ["FF 90 ? ? ? ? 8B 0F 03 CE E8 ? ? ? ? 8B" + 0x2] / sizeof(std::uintptr_t)
		CallVFunc<void, 128U>(this, &state);
	}

	void SetLightingOrigin(Vector_t vecLightingOrigin)
	{
		// @ida CMatRenderContext::SetLightingOrigin(): materialsystem.dll -> "55 8B EC 8B 0D ? ? ? ? F3 0F 7E"
		CallVFunc<void, 158U>(this, vecLightingOrigin);
	}

	void PushScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom)
	{
		// @ida CMatRenderContext::PushScissorRect(): materialsystem.dll -> "55 8B EC 83 EC 14 53 8B D9 57" @xref: "Scissor Rect Stack overflow.  Too many Pushes?\n"
		CallVFunc<void, 159U>(this, nLeft, nTop, nRight, nBottom);
	}

	void PopScissorRect()
	{
		CallVFunc<void, 160U>(this);
	}
};
#pragma pack(pop)
