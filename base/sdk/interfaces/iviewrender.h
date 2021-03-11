#pragma once
// used: viewplane definition
#include "ivrenderview.h"

struct vrect_t;
class IViewRender
{
public:
	virtual void                Init() = 0;
	virtual void                LevelInit() = 0;
	virtual void                LevelShutdown() = 0;
	virtual void                Shutdown() = 0;
	virtual void                OnRenderStart() = 0;
	virtual	void                Render(vrect_t* pRect) = 0;
	virtual void                RenderView(const CViewSetup& viewSetup, const CViewSetup& viewHUD, int nClearFlags, int nWhatToDraw) = 0;
	virtual int                 GetDrawFlags() = 0;
	virtual void                StartPitchDrift() = 0;
	virtual void                StopPitchDrift() = 0;
	virtual ViewPlane_t*		GetFrustum() = 0;
	virtual bool                ShouldDrawBrushModels() = 0;
	virtual const CViewSetup*	GetPlayerViewSetup(int nSlot = -1) const = 0;
	virtual const CViewSetup*	GetViewSetup() const = 0;
	virtual void                DisableVis() = 0;
	virtual int                 BuildWorldListsNumber() const = 0;
	virtual void                SetCheapWaterStartDistance(float flCheapWaterStartDistance) = 0;
	virtual void                SetCheapWaterEndDistance(float flCheapWaterEndDistance) = 0;
	virtual void                GetWaterLODParams(float& flCheapWaterStartDistance, float& flCheapWaterEndDistance) = 0;
	virtual void                DriftPitch() = 0;
	virtual void                SetScreenOverlayMaterial(IMaterial* pMaterial) = 0;
	virtual IMaterial*			GetScreenOverlayMaterial() = 0;
	virtual void                WriteSaveGameScreenshot(const char* szFileName) = 0;
	virtual void                WriteSaveGameScreenshotOfSize(const char* szFileName, int iWidth, int iHeight) = 0;
	virtual void                QueueOverlayRenderView(const CViewSetup& viewSetup, int nClearFlags, int nWhatToDraw) = 0;
	virtual float               GetZNear() = 0;
	virtual float               GetZFar() = 0;
	virtual void                GetScreenFadeDistances(float* pMin, float* pMax, float* pScale) = 0;
	virtual CBaseEntity*		GetCurrentlyDrawingEntity() = 0;
	virtual void                SetCurrentlyDrawingEntity(CBaseEntity* pEntity) = 0;
	virtual bool				UpdateShadowDepthTexture(ITexture* pRenderTarget, ITexture* pDepthTexture, const CViewSetup& viewShadow, bool bRenderWorldAndObjects = true, bool bRenderViewModels = false) = 0;
	virtual void				FreezeFrame(float flFreezeTime) = 0;
	virtual void				InitFadeData() = 0;
};
