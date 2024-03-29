#pragma once
// used: cbaseentity, cbasecombatweapon, envtonemapcontroller
#include "../sdk/entity.h"
// used: eclientframestage
#include "../sdk/interfaces/iengineclient.h"
// used: igameevent
#include "../sdk/interfaces/igameeventmanager.h"
// used: drawmodelresults_t, drawmodelinfo_t
#include "../sdk/interfaces/istudiorender.h"

/*
 * VISUAL
 * - visualized assistance to the user such as extrasensory perception
 */
namespace F::VISUAL
{
	bool Setup();
	void Destroy();

	/* @section: callbacks */
	void OnFrame(const EClientFrameStage nStage);
	bool OnDrawModel(DrawModelResults_t* pResults, const DrawModelInfo_t* pInfo, Matrix3x4_t* pBoneToWorld, float* pflFlexWeights, float* pflFlexDelayedWeights, const Vector_t* pvecModelOrigin, int nFlags);
	void OnDoPostScreenSpaceEffects();
	void OnPreRenderView(const CViewSetup* pViewSetup, int* nWhatToDraw);
	void OnPostRenderView(const CViewSetup* pViewSetup);
	void OnOverrideView(CViewSetup* pViewSetup);
	void OnGetViewModelFOV(float* pflViewModelFOV);
	void OnPreCalcView(CCSPlayer* pPlayer, QAngle_t* pangOldViewPunch, QAngle_t* pangOldAimPunch);
	void OnPostCalcView(CCSPlayer* pPlayer, QAngle_t* pangOldViewPunch, QAngle_t* pangOldAimPunch);
	void OnEvent(const FNV1A_t uEventHash, IGameEvent& gameEvent);

	/* @section: values */
	// in-game camera origin
	inline Vector_t vecCameraOrigin = { };
}
