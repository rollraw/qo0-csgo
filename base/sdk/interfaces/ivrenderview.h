#pragma once

// forward declarations
class ITexture;
class IMaterial;
class IMatRenderContext;

// @source: master/public/ivrenderview.h

struct ViewPlane_t
{
	ViewPlane_t() = default;

	Vector_t vecNormal = { };
	float flDistance = 0.0f;
};

enum EFrustumPlane : int
{
	FRUSTUM_RIGHT = 0,
	FRUSTUM_LEFT = 1,
	FRUSTUM_TOP = 2,
	FRUSTUM_BOTTOM = 3,
	FRUSTUM_NEARZ = 4,
	FRUSTUM_FARZ = 5,
	FRUSTUM_NUMPLANES = 6
};

using Frustum_t = ViewPlane_t[FRUSTUM_NUMPLANES];
using LeafIndex_t = std::uint16_t;

enum
{
	INVALID_LEAF_INDEX = static_cast<LeafIndex_t>(~0)
};

struct BrushVertex_t
{
	Vector_t vecPosition;
	Vector_t vecNormal;
	Vector_t vecTangentS;
	Vector_t vecTangentT;
	Vector2D_t vecTexCoord;
	Vector2D_t vecLightmapCoord;
};

class IBrushSurface
{
public:
	virtual void ComputeTextureCoordinate(const Vector_t& vecWorldPosition, Vector2D_t& vecTextureCoordinate) = 0;
	virtual void ComputeLightmapCoordinate(const Vector_t& vecWorldPosition, Vector2D_t& vecLightmapCoordinate) = 0;
	virtual int GetVertexCount() const = 0;
	virtual void GetVertexData(BrushVertex_t* pVertices) = 0;
	virtual IMaterial* GetMaterial() = 0;
};

class IBrushRenderer
{
public:
	virtual bool RenderBrushModelSurface(CBaseEntity* pBaseEntity, IBrushSurface* pBrushSurface) = 0;
};

class IVRenderView : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void SetBlend(float flBlend)
	{
		CallVFunc<void, 4U>(this, flBlend);
	}

	float GetBlend()
	{
		return CallVFunc<float, 5U>(this);
	}

	void SetColorModulation(const float* arrColor)
	{
		CallVFunc<void, 6U>(this, arrColor);
	}

	void GetColorModulation(float* arrColor)
	{
		CallVFunc<void, 7U>(this, arrColor);
	}

	void SceneBegin()
	{
		CallVFunc<void, 8U>(this);
	}

	void SceneEnd()
	{
		CallVFunc<void, 9U>(this);
	}

	float GetFieldOfView()
	{
		return CallVFunc<float, 34U>(this);
	}

	void Push3DView(IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum_t frustumPlanes, ITexture* pDepthTexture)
	{
		CallVFunc<void, 43U>(this, pRenderContext, &view, nFlags, pRenderTarget, frustumPlanes, pDepthTexture);
	}

	void Push3DView(IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum_t frustumPlanes)
	{
		CallVFunc<void, 44U>(this, pRenderContext, &view, nFlags, pRenderTarget, frustumPlanes);
	}

	void Push2DView(IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum_t frustumPlanes)
	{
		CallVFunc<void, 45U>(this, pRenderContext, &view, nFlags, pRenderTarget, frustumPlanes);
	}

	void PopView(IMatRenderContext* pRenderContext, Frustum_t frustumPlanes)
	{
		CallVFunc<void, 46U>(this, pRenderContext, frustumPlanes);
	}

	void SetMainView(const Vector_t& vecOrigin, const QAngle_t& angView)
	{
		CallVFunc<void, 47U>(this, &vecOrigin, &angView);
	}

	void GetMatricesForView(const CViewSetup& view, ViewMatrix_t* pWorldToView, ViewMatrix_t* pViewToProjection, ViewMatrix_t* pWorldToProjection, ViewMatrix_t* pWorldToPixels)
	{
		CallVFunc<void, 56U>(this, &view, pWorldToView, pViewToProjection, pWorldToProjection, pWorldToPixels);
	}
};
