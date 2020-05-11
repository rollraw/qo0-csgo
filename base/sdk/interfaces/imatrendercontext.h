#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/glow_outline_effect.cpp

// used: irefcouted
#include "irefcount.h"
// used: texture
#include "itexture.h"

using VertexFormat_t = std::uint64_t;

#pragma region matrendercontext_enumerations
enum ELightType : int
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT,
};

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
#pragma endregion

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

struct LightDesc_t
{
	ELightType	type;
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

/* enumerations forward declarations */
enum EMorphFormat;
enum EMaterialMatrixMode;
enum EMaterialFogMode;
enum EMaterialCullMode;
enum EMaterialHeightClipMode;
enum EMaterialIndexFormat;
enum EMaterialPrimitiveType;
enum EMaterialNonInteractiveMode;

/* structures forward declarations */
struct Rect_t;
struct MorphWeight_t;
struct FlashlightState_t;
struct OcclusionQueryObjectHandle_t;
struct DeformationBase_t;
struct ColorCorrectionHandle_t;

/* classes forward declarations */
class IMesh;
class IMorph;
class IVertexBuffer;
class IIndexBuffer;
class ICallQueue;

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h
class IMatRenderContext : public IRefCounted
{
public:
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual void Flush(bool bFlushHardware = false) = 0;
	virtual void BindLocalCubemap(ITexture* pTexture) = 0;
	virtual void SetRenderTarget(ITexture* pTexture) = 0;
	virtual ITexture* GetRenderTarget() = 0;
	virtual void GetRenderTargetDimensions(int& iWidth, int& iHeight) const = 0;
	virtual void Bind(IMaterial* pMaterial, void* pProxyData = 0) = 0;
	virtual void BindLightmapPage(int nLightmapPageID) = 0;
	virtual void DepthRange(float zNear, float zFar) = 0;
	virtual void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;
	virtual void ReadPixels(int x, int y, int iWidth, int iHeight, unsigned char* pData, EImageFormat format) = 0;
	virtual void unused1() { }
	virtual void unused2() { }
	virtual void SetAmbientLight(float r, float g, float b) = 0;
	virtual void SetLight(int nLights, const LightDesc_t& desc) = 0;
	virtual void SetAmbientLightCube(Vector4D vecCube[6]) = 0;
	virtual void CopyRenderTargetToTexture(ITexture* pTexture) = 0;
	virtual void SetFrameBufferCopyTexture(ITexture* pTexture, int nTextureIndex = 0) = 0;
	virtual ITexture* GetFrameBufferCopyTexture(int nTextureIndex) = 0;
	virtual void MatrixMode(EMaterialMatrixMode matrixMode) = 0;
	virtual void PushMatrix() = 0;
	virtual void PopMatrix() = 0;
	virtual void LoadMatrix(float const& flMatrix) = 0;
	virtual void LoadMatrix(matrix3x4_t const& matLoad) = 0;
	virtual void MultMatrix(float const flMatrix) = 0;
	virtual void MultMatrix(matrix3x4_t const& matLoad) = 0;
	virtual void MultMatrixLocal(float const& flMatrix) = 0;
	virtual void MultMatrixLocal(matrix3x4_t const& matLoad) = 0;
	virtual void GetMatrix(EMaterialMatrixMode matrixMode, float* flMatrix) = 0;
	virtual void GetMatrix(EMaterialMatrixMode matrixMode, matrix3x4_t* pMatrix) = 0;
	virtual void LoadIdentity() = 0;
	virtual void Ortho(double dLeft, double dTop, double dRight, double dBottom, double zNear, double zFar) = 0;
	virtual void PerspectiveX(double dFovX, double dAspect, double zNear, double zFar) = 0;
	virtual void PickMatrix(int x, int y, int iWidth, int iHeight) = 0;
	virtual void Rotate(float flAngle, float x, float y, float z) = 0;
	virtual void Translate(float x, float y, float z) = 0;
	virtual void Scale(float x, float y, float z) = 0;
	virtual void Viewport(int x, int y, int iWidth, int iHeight) = 0;
	virtual void GetViewport(int& x, int& y, int& iWidth, int& iHeight) const = 0;
	virtual void CullMode(EMaterialCullMode cullMode) = 0;
	virtual void SetHeightClipMode(EMaterialHeightClipMode heightClipMode) = 0;
	virtual void SetHeightClipZ(float z) = 0;
	virtual void SetFogMode(EMaterialFogMode fogMode) = 0;
	virtual void SetFogStart(float flStart) = 0;
	virtual void SetFogEnd(float flEnd) = 0;
	virtual void SetFogZ(float flFogZ) = 0;
	virtual EMaterialFogMode GetFogMode() = 0;
	virtual void SetFogColor(float r, float g, float b) = 0;
	virtual void SetFogColor3fv(float const* rgb) = 0;
	virtual void SetFogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void SetFogColor3ubv(unsigned char const* rgb) = 0;
	virtual void GetFogColor(unsigned char* rgb) = 0;
	virtual void SetNumBoneWeights(int nBones) = 0;
	virtual IMesh* CreateStaticMesh(VertexFormat_t vertexFormat, const char* pTextureBudgetGroup, IMaterial* pMaterial = nullptr) = 0;
	virtual void DestroyStaticMesh(IMesh* mesh) = 0;
	virtual IMesh* GetDynamicMesh(bool bBuffered = true, IMesh* pVertexOverride = nullptr, IMesh* pIndexOverride = nullptr, IMaterial* pAutoBind = nullptr) = 0;
	virtual IVertexBuffer* CreateStaticVertexBuffer(VertexFormat_t vertexFormat, int nVertexCount, const char* pTextureBudgetGroup) = 0;
	virtual IIndexBuffer* CreateStaticIndexBuffer(EMaterialIndexFormat indexFormat, int nIndexCount, const char* pTextureBudgetGroup) = 0;
	virtual void DestroyVertexBuffer(IVertexBuffer* pBuffer) = 0;
	virtual void DestroyIndexBuffer(IIndexBuffer* pBuffer) = 0;
	virtual IVertexBuffer* GetDynamicVertexBuffer(int nStreamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
	virtual IIndexBuffer* GetDynamicIndexBuffer(EMaterialIndexFormat indexFormat, bool bBuffered = true) = 0;
	virtual void BindVertexBuffer(int nStreamID, IVertexBuffer* pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t vertexFormat, int nRepetitions = 1) = 0;
	virtual void BindIndexBuffer(IIndexBuffer* pIndexBuffer, int nOffsetInBytes) = 0;
	virtual void Draw(EMaterialPrimitiveType primitiveType, int nFirstIndex, int nIndices) = 0;
	virtual int SelectionMode(bool bSelectionMode) = 0;
	virtual void SelectionBuffer(unsigned int* pBuffer, int iSize) = 0;
	virtual void ClearSelectionNames() = 0;
	virtual void LoadSelectionName(int iName) = 0;
	virtual void PushSelectionName(int iName) = 0;
	virtual void PopSelectionName() = 0;
	virtual void OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;
	virtual void DrawScreenSpaceQuad(IMaterial* pMaterial) = 0;
	virtual void SyncToken(const char* pToken) = 0;
	virtual float ComputePixelWidthOfSphere(const Vector& vecOrigin, float flRadius) = 0;
	virtual void SetClearColor(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void SetClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject() = 0;
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t) = 0;
	virtual void SetFlashlightMode(bool bEnable) = 0;
	virtual void SetFlashlightState(const FlashlightState_t& state, const matrix3x4_t& worldToTexture) = 0;
	virtual EMaterialHeightClipMode GetHeightClipMode() = 0;
	virtual float ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) = 0;
	virtual void EnableUserClipTransformOverride(bool bEnable) = 0;
	virtual void UserClipTransform(const matrix3x4_t& matWorldToView) = 0;
	virtual bool GetFlashlightMode() const = 0;
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;
	virtual void unused3() { }
	virtual IMorph* CreateMorph(EMorphFormat morphFormat, const char* pDebugName) = 0;
	virtual void DestroyMorph(IMorph* pMorph) = 0;
	virtual void BindMorph(IMorph* pMorph) = 0;
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) = 0;
	virtual void unused4() { };
	virtual void unused5() { };
	virtual void unused6() { };
	virtual void unused7() { };
	virtual void unused8() { };
	virtual void ReadPixelsAndStretch(Rect_t* pSrcRect, Rect_t* pDstRect, unsigned char* pBuffer, EImageFormat dstFormat, int nDstStride) = 0;
	virtual void GetWindowSize(int& width, int& height) const = 0;
	virtual void DrawScreenSpaceRectangle(IMaterial* pMaterial, int iDestX, int iDestY, int iWidth, int iHeight, float flTextureX0, float flTextureY0, float flTextureX1, float flTextureY1, int iTextureWidth, int iTextureHeight, void* pClientRenderable = nullptr, int nXDice = 1, int nYDice = 1) = 0;
	virtual void LoadBoneMatrix(int nBoneIndex, const matrix3x4_t& matrix) = 0;
	virtual void PushRenderTargetAndViewport() = 0;
	virtual void PushRenderTargetAndViewport(ITexture* pTexture) = 0;
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, ITexture* pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
	virtual void PopRenderTargetAndViewport() = 0;
	virtual void BindLightmapTexture(ITexture* pLightmapTexture) = 0;
	virtual void CopyRenderTargetToTextureEx(ITexture* pTexture, int nRenderTargetID, Rect_t* pSrcRect, Rect_t* pDstRect = nullptr) = 0;
	virtual void CopyTextureToRenderTargetEx(int nRenderTargetID, ITexture* pTexture, Rect_t* pSrcRect, Rect_t* pDstRect = nullptr) = 0;
	virtual void PerspectiveOffCenterX(double dFovX, double dAspect, double zNear, double zFar, double dBottom, double dTop, double dLeft, double dRight) = 0;
	virtual void SetFloatRenderingParameter(int nParameter, float flValue) = 0;
	virtual void SetIntRenderingParameter(int nParameter, int iValue) = 0;
	virtual void SetVectorRenderingParameter(int nParameter, Vector const& vecValue) = 0;
	virtual void SetStencilEnable(bool bEnable) = 0;
	virtual void SetStencilFailOperation(EStencilOperation nOperation) = 0;
	virtual void SetStencilZFailOperation(EStencilOperation nOperation) = 0;
	virtual void SetStencilPassOperation(EStencilOperation nOperation) = 0;
	virtual void SetStencilCompareFunction(EStencilComparisonFunction nFunction) = 0;
	virtual void SetStencilReferenceValue(int nReference) = 0;
	virtual void SetStencilTestMask(std::uint32_t uMask) = 0;
	virtual void SetStencilWriteMask(std::uint32_t uMask) = 0;
	virtual void ClearStencilBufferRectangle(int iXmin, int iYmin, int iXmax, int iYmax, int iValue) = 0;
	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture* pTexture) = 0;
	virtual void PushCustomClipPlane(const float* pPlane) = 0;
	virtual void PopCustomClipPlane() = 0;
	virtual void GetMaxToRender(IMesh* pMesh, bool bMaxUntilFlush, int* pMaxVerts, int* pMaxIndices) = 0;
	virtual int GetMaxVerticesToRender(IMaterial* pMaterial) = 0;
	virtual int GetMaxIndicesToRender() = 0;
	virtual void DisableAllLocalLights() = 0;
	virtual int CompareMaterialCombos(IMaterial* pMaterial1, IMaterial* pMaterial2, int lightMapID1, int lightMapID2) = 0;
	virtual IMesh* GetFlexMesh() = 0;
	virtual void SetFlashlightStateEx(const FlashlightState_t& state, const matrix3x4_t& worldToTexture, ITexture* pFlashlightDepthTexture) = 0;
	virtual ITexture* GetLocalCubemap() = 0;
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;
	virtual bool EnableClipping(bool bEnable) = 0;
	virtual void GetFogDistances(float* flStart, float* flEnd, float* flFogZ) = 0;
	virtual void BeginPIXEvent(unsigned long ulColor, const char* szName) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker(unsigned long ulColor, const char* szName) = 0;
	virtual void BeginBatch(IMesh* pIndices) = 0;
	virtual void BindBatch(IMesh* pVertices, IMaterial* pAutoBind = nullptr) = 0;
	virtual void DrawBatch(int nFirstIndex, int nIndices) = 0;
	virtual void EndBatch() = 0;
	virtual ICallQueue* GetCallQueue() = 0;
	virtual void GetWorldSpaceCameraPosition(Vector* pCameraPos) = 0;
	virtual void GetWorldSpaceCameraVectors(Vector* pVecForward, Vector* pVecRight, Vector* pVecUp) = 0;
	virtual void ResetToneMappingScale(float flMonoscale) = 0;
	virtual void SetGoalToneMappingScale(float flMonoscale) = 0;
	virtual void TurnOnToneMapping() = 0;
	virtual void SetToneMappingScaleLinear(const Vector& vecScale) = 0;
	virtual Vector GetToneMappingScaleLinear() = 0;
	virtual void SetShadowDepthBiasFactors(float flSlopeScaleDepthBias, float flDepthBias) = 0;
	virtual void PerformFullScreenStencilOperation() = 0;
	virtual void SetLightingOrigin(Vector vecLightingOrigin) = 0;
	virtual void SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;
	virtual void BeginMorphAccumulation() = 0;
	virtual void EndMorphAccumulation() = 0;
	virtual void AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) = 0;
	virtual void PushDeformation(DeformationBase_t const* Deformation) = 0;
	virtual void PopDeformation() = 0;
	virtual int GetNumActiveDeformations() const = 0;
	virtual bool GetMorphAccumulatorTexCoord(Vector* pTexCoord, IMorph* pMorph, int nVertex) = 0;
	virtual IMesh* GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true, IMesh* pVertexOverride = nullptr, IMesh* pIndexOverride = nullptr, IMaterial* pAutoBind = nullptr) = 0;
	virtual void FogMaxDensity(float flMaxDensity) = 0;
	virtual IMaterial* GetCurrentMaterial() = 0;
	virtual int GetCurrentNumBones() const = 0;
	virtual void* GetCurrentProxy() = 0;
	virtual void EnableColorCorrection(bool bEnable) = 0;
	virtual ColorCorrectionHandle_t AddLookup(const char* szName) = 0;
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char* szLookupName) = 0;
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) = 0;
	virtual void ResetLookupWeights() = 0;
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) = 0;
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;
	virtual void SetNonInteractivePacifierTexture(ITexture* pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture* pTexture, EMaterialNonInteractiveMode mode) = 0;
	virtual void EnableNonInteractiveMode(EMaterialNonInteractiveMode mode) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;
	virtual void* LockRenderData(int nSizeInBytes) = 0;
	virtual void UnlockRenderData(void* pData) = 0;
	virtual void AddReferenceRenderData() = 0;
	virtual void ReleaseRenderData() = 0;
	virtual bool IsRenderData(const void* pData) const = 0;
	virtual void PrintfVA(char* fmt, va_list vargs) = 0;
	virtual void Printf(const char* fmt, ...) = 0;
	virtual float Knob(char* szKnobName, float* flValue = nullptr) = 0;
	virtual void OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable) = 0;
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;
	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;

	inline void SetStencilState(ShaderStencilState_t stencil)
	{
		SetStencilEnable(stencil.bEnable);
		SetStencilFailOperation(stencil.FailOperation);
		SetStencilZFailOperation(stencil.ZFailOperation);
		SetStencilPassOperation(stencil.PassOperation);
		SetStencilCompareFunction(stencil.CompareFunction);
		SetStencilReferenceValue(stencil.nReferenceValue);
		SetStencilTestMask(stencil.uTestMask);
		SetStencilWriteMask(stencil.uWriteMask);
	}
};
