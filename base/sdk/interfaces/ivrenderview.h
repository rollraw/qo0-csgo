#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/ivrenderview.h

struct VisibleFogVolumeInfo_t
{
	int			iVisibleFogVolume;
	int			iVisibleFogVolumeLeaf;
	bool		bEyeInFogVolume;
	float		flDistanceToWater;
	float		flWaterHeight;
	IMaterial*	pFogVolumeMaterial;
};

struct ViewPlane_t
{
	ViewPlane_t() = default;

	ViewPlane_t(const Vector& vecNormal, const float flDistance) :
		vecNormal(vecNormal), flDistance(flDistance) { }

	Vector vecNormal;
	float flDistance;
};

enum
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

struct WorldListLeafData_t
{
	LeafIndex_t		nLeafIndex;
	std::int16_t	sWaterData;
	std::uint16_t 	uFirstTranslucentSurface;	// engine-internal list index
	std::uint16_t	uTranslucentSurfaceCount;	// count of translucent surfaces+disps
};

struct WorldListInfo_t
{
	int		iViewFogVolume;
	int		iLeafCount;
	bool	bHasWater;
	WorldListLeafData_t* pLeafDataList;
};

class IWorldRenderList : public IRefCounted
{

};

struct VisOverrideData_t
{
	Vector		vecVisOrigin;					// the point to to use as the viewpoint for area portal backface cull checks.
	float		flDistToAreaPortalTolerance;	// the distance from an area portal before using the full screen as the viewable portion.
	Vector		vecPortalCorners[4];			// when rendering a portal view, these are the 4 corners of the portal you are looking through (used to shrink the frustum)
	bool		bTrimFrustumToPortalCorners;
	Vector		vecPortalOrigin;
	Vector		vecPortalForward;
	float		flPortalRadius;
};

struct ColorVec_t
{
	unsigned r, g, b, a;
};

struct BrushVertex_t
{
	Vector		vecPos;
	Vector		vecNormal;
	Vector		vecTangentS;
	Vector		vecTangentT;
	Vector2D	vecTexCoord;
	Vector2D	vecLightmapCoord;

private:
	BrushVertex_t(const BrushVertex_t& src);
};

class IBrushSurface
{
public:
	virtual void ComputeTextureCoordinate(Vector const& worldPos, Vector2D& texCoord) = 0;
	virtual void ComputeLightmapCoordinate(Vector const& worldPos, Vector2D& lightmapCoord) = 0;
	virtual int  GetVertexCount() const = 0;
	virtual void GetVertexData(BrushVertex_t* pVerts) = 0;
	virtual IMaterial* GetMaterial() = 0;
};

class IBrushRenderer
{
public:
	virtual bool RenderBrushModelSurface(CBaseEntity* pBaseEntity, IBrushSurface* pBrushSurface) = 0;
};

class IVRenderView
{
public:
	virtual void			DrawBrushModel(CBaseEntity* pBaseEntity, Model_t* pModel, const Vector& vecOrigin, const QAngle& angView, bool bSort) = 0;
	virtual void			DrawIdentityBrushModel(IWorldRenderList* pList, Model_t* pModel) = 0;
	virtual void			TouchLight(struct dlight_t* dLight) = 0;
	virtual void			Draw3DDebugOverlays() = 0;
	virtual void			SetBlend(float flBlend) = 0;
	virtual float			GetBlend() = 0;
	virtual void			SetColorModulation(float const* flBlend) = 0;
	virtual void			GetColorModulation(float* flBlend) = 0;
	virtual void			SceneBegin() = 0;
	virtual void			SceneEnd() = 0;
	virtual void			GetVisibleFogVolume(const Vector& vecEyePoint, VisibleFogVolumeInfo_t* pInfo) = 0;
	virtual IWorldRenderList* CreateWorldList() = 0;
	virtual void			BuildWorldLists_Epilogue(IWorldRenderList* pList, WorldListInfo_t* pInfo, bool bShadowDepth) = 0;
	virtual void			BuildWorldLists(IWorldRenderList* pList, WorldListInfo_t* pInfo, int iForceFViewLeaf, const VisOverrideData_t* pVisData = nullptr, bool bShadowDepth = false, float* pReflectionWaterHeight = nullptr) = 0;
	virtual void			DrawWorldLists(IWorldRenderList* pList, unsigned long flags, float flWaterZAdjust) = 0;
	virtual void			GetWorldListIndicesInfo(void* pIndicesInfoOut, IWorldRenderList* pList, unsigned long nFlags) = 0;
	virtual void			DrawTopView(bool bEnable) = 0;
	virtual void			TopViewNoBackfaceCulling(bool bDisable) = 0;
	virtual void			TopViewNoVisCheck(bool bDisable) = 0;
	virtual void			TopViewBounds(Vector2D const& vecMins, Vector2D const& vecMaxs) = 0;
	virtual void			SetTopViewVolumeCuller(const void* pVolumeCuller) = 0;
	virtual void			DrawLights() = 0;
	virtual void			DrawMaskEntities() = 0;
	virtual void			DrawTranslucentSurfaces(IWorldRenderList* pList, int* pSortList, int iSortCount, unsigned long fFlags) = 0;
	virtual void			DrawLineFile() = 0;
	virtual void			DrawLightmaps(IWorldRenderList* pList, int iPageID) = 0;
	virtual void			ViewSetupVis(bool bNoVis, int nOrigins, const Vector vecOrigin[]) = 0;
	virtual bool			AreAnyLeavesVisible(int* pLeafList, int nLeaves) = 0;
	virtual	void			VguiPaint() = 0;
	virtual void			ViewDrawFade(uint8_t* pColor, IMaterial* pMaterial) = 0;
	virtual void			OLD_SetProjectionMatrix(float flFov, float zNear, float zFar) = 0;
	virtual ColorVec_t		GetLightAtPoint(Vector& vecPosition) = 0;
	virtual int				GetViewEntity() = 0;
	virtual bool			IsViewEntity(int nEntityIndex) = 0;
	virtual float			GetFieldOfView() = 0;
	virtual unsigned char** GetAreaBits() = 0;
	virtual void			SetFogVolumeState(int nVisibleFogVolume, bool bUseHeightFog) = 0;
	virtual void			InstallBrushSurfaceRenderer(IBrushRenderer* pBrushRenderer) = 0;
	virtual void			DrawBrushModelShadow(IClientRenderable* pRenderable) = 0;
	virtual	bool			LeafContainsTranslucentSurfaces(IWorldRenderList* pList, int nSortIndex, unsigned long fFlags) = 0;
	virtual bool			DoesBoxIntersectWaterVolume(const Vector& vecMins, const Vector& vecMaxs, int nLeafWaterDataID) = 0;
	virtual void			SetAreaState(unsigned char uAreaBits[MAX_AREA_STATE_BYTES], unsigned char uAreaPortalBits[MAX_AREA_PORTAL_STATE_BYTES]) = 0;
	virtual void			VGui_Paint(int nMode) = 0;
	virtual void			Push3DView(IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum_t frustumPlanes) = 0;
	virtual void			Push2DView(IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum_t frustumPlanes) = 0;
	virtual void			PopView(IMatRenderContext* pRenderContext, Frustum_t frustumPlanes) = 0;
	virtual void			SetMainView(const Vector& vecOrigin, const QAngle& angView) = 0;
	virtual void			ViewSetupVisEx(bool bNoVis, int nOrigins, const Vector arrOrigin[], unsigned int& uReturnFlags) = 0;
	virtual void			OverrideViewFrustum(Frustum_t custom) = 0;
	virtual void			DrawBrushModelShadowDepth(IClientEntity* pEntity, Model_t* pModel, const Vector& vecOrigin, const QAngle& angView, int nDepthMode) = 0;
	virtual void			UpdateBrushModelLightmap(Model_t* pModel, IClientRenderable* pRenderable) = 0;
	virtual void			BeginUpdateLightmaps() = 0;
	virtual void			EndUpdateLightmaps() = 0;
	virtual void			OLD_SetOffCenterProjectionMatrix(float flFOV, float flNearZ, float flFarZ, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight) = 0;
	virtual void			OLD_SetProjectionMatrixOrtho(float flLeft, float flTop, float flRight, float flBottom, float flNearZ, float flFarZ) = 0;
	virtual void			Push3DView(IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum_t frustumPlanes, ITexture* pDepthTexture) = 0;
	virtual void			GetMatricesForView(const CViewSetup& view, ViewMatrix_t* pWorldToView, ViewMatrix_t* pViewToProjection, ViewMatrix_t* pWorldToProjection, ViewMatrix_t* pWorldToPixels) = 0;
	virtual void			DrawBrushModelEx(IClientEntity* pEntity, Model_t* pModel, const Vector& vecOrigin, const QAngle& angView, int nMode) = 0;
	virtual bool			DoesBrushModelNeedPowerOf2Framebuffer(const Model_t* pModel) = 0;
	virtual void			DrawBrushModelArray(IMatRenderContext* pContext, int nCount, const void* pInstanceData, int nModelTypeFlags) = 0;
};
