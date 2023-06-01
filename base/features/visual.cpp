#include "visual.h"

// used: cheat variables
#include "../core/variables.h"
// used: addtolistener
#include "../core/eventlistener.h"
// used: render functions
#include "../utilities/draw.h"

// used: sub-features
#include "visual/overlay.h"
#include "visual/glow.h"
#include "visual/chams.h"
#include "visual/world.h"
#include "visual/screen.h"

using namespace F;

bool VISUAL::Setup()
{
	if (!CHAMS::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to setup chams");
		return false;
	}

	return true;
}

void VISUAL::Destroy()
{
	CHAMS::Destroy();
}

#pragma region visual_callbacks
void VISUAL::OnFrame(const EClientFrameStage nStage)
{
	if (nStage == FRAME_RENDER_END)
	{
		// check is render initialized
		if (!D::bInitialized)
			return;
		
		/*
		 * game and our gui are based on immediate render mode principe
		 * this means that we should always reset draw data from previous frame and re-store it again
		 */
		D::ResetDrawData();

		if (CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer(); pLocal != nullptr)
		{
			/*
			 * thread-safe render is designed to be called once per frame and only at one place
			 * here should be called all functions that intend to use game data but render it in other, non-game thread
			 */
			OVERLAY::OnDraw(pLocal);
			WORLD::OnDraw(pLocal);
			SCREEN::OnDraw(pLocal);
		}

		D::SwapDrawData();
	}
}

void VISUAL::OnPreRenderView(const CViewSetup& viewSetup, int* nWhatToDraw)
{
	if (!C::Get<bool>(Vars.bVisualWorld))
		return;

	const VisualWorldRemovalFlags_t nVisualWorldRemovals = C::Get<unsigned int>(Vars.nVisualWorldRemovals);
	
	WORLD::PostProcessingRemoval();

	if (nVisualWorldRemovals & VISUAL_WORLD_REMOVAL_FLAG_FLASH)
		WORLD::FlashRemoval(true);

	if (nVisualWorldRemovals & VISUAL_WORLD_REMOVAL_FLAG_SMOKE)
		WORLD::SmokeRemoval(true);

	if (CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer(); pLocal != nullptr && pLocal->IsAlive() && (nVisualWorldRemovals & VISUAL_WORLD_REMOVAL_FLAG_SCOPE))
	{
		if (CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon(); pWeapon != nullptr)
		{
			if (const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeapon->GetEconItemView()->GetItemDefinitionIndex()); pWeaponData != nullptr && pWeaponData->nWeaponType == WEAPONTYPE_SNIPER && pLocal->IsScoped())
				*nWhatToDraw &= ~(RENDERVIEW_DRAWVIEWMODEL | RENDERVIEW_DRAWHUD);
		}
	}
}

void VISUAL::OnPostRenderView(const CViewSetup& viewSetup)
{
	if (!C::Get<bool>(Vars.bVisualWorld))
		return;

	const VisualWorldRemovalFlags_t nVisualWorldRemovals = C::Get<unsigned int>(Vars.nVisualWorldRemovals);

	if (nVisualWorldRemovals & VISUAL_WORLD_REMOVAL_FLAG_FLASH)
		WORLD::FlashRemoval(false);

	if (nVisualWorldRemovals & VISUAL_WORLD_REMOVAL_FLAG_SMOKE)
		WORLD::SmokeRemoval(false);
}

void VISUAL::OnOverrideView(CViewSetup* pViewSetup)
{
	CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer();

	if (pLocal == nullptr)
		return;

	WORLD::ThirdPerson(pLocal);
	SCREEN::OverrideFOV(pLocal, &pViewSetup->flFOV);
}

void VISUAL::OnGetViewModelFOV(float* pflViewModelFOV)
{
	if (CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer(); pLocal != nullptr)
		SCREEN::OverrideViewModelFOV(pLocal, pflViewModelFOV);
}

void VISUAL::OnPreCalcView(CCSPlayer* pPlayer, QAngle_t* pangOldViewPunch, QAngle_t* pangOldAimPunch)
{
	if (C::Get<bool>(Vars.bVisualWorld) && (C::Get<unsigned int>(Vars.nVisualWorldRemovals) & VISUAL_WORLD_REMOVAL_FLAG_PUNCH))
		WORLD::PunchRemoval(pPlayer, true, pangOldViewPunch, pangOldAimPunch);
}

void VISUAL::OnPostCalcView(CCSPlayer* pPlayer, QAngle_t* pangOldViewPunch, QAngle_t* pangOldAimPunch)
{
	if (C::Get<bool>(Vars.bVisualWorld) && (C::Get<unsigned int>(Vars.nVisualWorldRemovals) & VISUAL_WORLD_REMOVAL_FLAG_PUNCH))
		WORLD::PunchRemoval(pPlayer, false, pangOldViewPunch, pangOldAimPunch);
}

bool VISUAL::OnDrawModel(DrawModelResults_t* pResults, const DrawModelInfo_t& info, Matrix3x4_t* pBoneToWorld, float* pflFlexWeights, float* pflFlexDelayedWeights, const Vector_t& vecModelOrigin, int nFlags)
{
	CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer();

	if (pLocal == nullptr)
		return false;

	bool bDrawnModel = false;

	bDrawnModel |= CHAMS::OnDrawModel(pLocal, pResults, info, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, vecModelOrigin, nFlags);

	return bDrawnModel;
}

void VISUAL::OnDoPostScreenSpaceEffects()
{
	CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer();

	if (pLocal == nullptr)
		return;

	GLOW::OnDoPostScreenSpaceEffects(pLocal);
}

void VISUAL::OnEvent(const FNV1A_t uEventHash, IGameEvent& gameEvent)
{
	SCREEN::OnEvent(uEventHash, gameEvent);
}
#pragma endregion
