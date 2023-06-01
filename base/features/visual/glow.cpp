#include "glow.h"

// used: cheat variables
#include "../../core/variables.h"
// used: stringstring
#include "../../utilities/crt.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../../sdk/interfaces/ibaseclientdll.h"
#include "../../sdk/interfaces/iglowmanager.h"

using namespace F::VISUAL;

#pragma region visual_glow_callbacks
void GLOW::OnDoPostScreenSpaceEffects(CCSPlayer* pLocal)
{
	if (!C::Get<bool>(Vars.bVisual) || !C::Get<bool>(Vars.bVisualGlow))
		return;

	for (int i = 0; i < I::GlowManager->vecGlowObjectDefinitions.Count(); i++)
	{
		IGlowObjectManager::GlowObject_t& glowObject = I::GlowManager->vecGlowObjectDefinitions[i];

		// is current object not used
		if (glowObject.IsEmpty())
			continue;

		// get current entity from object handle
		CBaseEntity* pEntity = glowObject.pEntity;

		if (pEntity == nullptr)
			continue;

		// set bloom state
		glowObject.bFullBloomRender = C::Get<bool>(Vars.bVisualGlowBloom);

		const CClientClass* pClientClass = pEntity->GetClientClass();

		if (pClientClass == nullptr)
			continue;

		const EClassIndex nIndex = pClientClass->nClassID;
		switch (nIndex)
		{
		case EClassIndex::CC4:
			[[fallthrough]];
		case EClassIndex::CPlantedC4:
		{
			if (!C::Get<bool>(Vars.bVisualGlowBomb))
				break;

			const Color_t colBomb = (nIndex == EClassIndex::CC4 ? C::Get<Color_t>(Vars.colVisualGlowBomb) : C::Get<Color_t>(Vars.colVisualGlowBombPlanted));
			colBomb.BaseAlpha(glowObject.arrColor);
			glowObject.flBloomAmount = 1.0f;
			glowObject.bRenderWhenOccluded = true;
			glowObject.bRenderWhenUnoccluded = false;
			glowObject.nRenderStyle = GLOWRENDERSTYLE_DEFAULT;
			break;
		}
		case EClassIndex::CCSPlayer:
		{
			CCSPlayer* pCSPlayer = static_cast<CCSPlayer*>(pEntity);

			if (pCSPlayer->IsDormant() || !pCSPlayer->IsAlive())
				break;

			bool bIsLocal = (pCSPlayer == pLocal && pLocal->ShouldDraw());
			bool bIsEnemy = (pLocal->IsOtherEnemy(pCSPlayer));
			bool bIsAlly = (!bIsLocal && !bIsEnemy);

			bIsLocal &= C::Get<bool>(Vars.bVisualGlowLocal);
			bIsEnemy &= C::Get<bool>(Vars.bVisualGlowEnemies);
			bIsAlly &= C::Get<bool>(Vars.bVisualGlowAllies);

			// check is any of filters are enabled and pass
			if (bIsLocal || bIsEnemy || bIsAlly)
			{
				// @todo: use glow material overrides with xqz instead
				const bool bIsVisible = pLocal->IsOtherVisible(pCSPlayer, pCSPlayer->GetBonePosition(HITBOX_HEAD));

				const Color_t colVisible = (bIsLocal ? C::Get<Color_t>(Vars.colVisualGlowLocal) : (bIsEnemy ? C::Get<Color_t>(Vars.colVisualGlowEnemies) : C::Get<Color_t>(Vars.colVisualGlowAllies)));
				const Color_t colHidden = (bIsLocal ? C::Get<Color_t>(Vars.colVisualGlowLocalHidden) : (bIsEnemy ? C::Get<Color_t>(Vars.colVisualGlowEnemiesHidden) : C::Get<Color_t>(Vars.colVisualGlowAlliesHidden)));
				const Color_t colPlayer = (bIsVisible ? colVisible : colHidden);

				colPlayer.BaseAlpha(glowObject.arrColor);
				glowObject.flBloomAmount = 1.0f;
				glowObject.bRenderWhenOccluded = true;
				glowObject.bRenderWhenUnoccluded = false;
				glowObject.nRenderStyle = GLOWRENDERSTYLE_DEFAULT;
			}

			break;
		}
		case EClassIndex::CBaseCSGrenadeProjectile:
		case EClassIndex::CDecoyProjectile:
		case EClassIndex::CMolotovProjectile:
		case EClassIndex::CSensorGrenadeProjectile:
		case EClassIndex::CSmokeGrenadeProjectile:
		{
			if (!C::Get<bool>(Vars.bVisualGlowGrenades))
				break;

			C::Get<Color_t>(Vars.colVisualGlowGrenades).BaseAlpha(glowObject.arrColor);
			glowObject.flBloomAmount = 1.0f;
			glowObject.bRenderWhenOccluded = true;
			glowObject.bRenderWhenUnoccluded = false;
			glowObject.nRenderStyle = GLOWRENDERSTYLE_DEFAULT;
			break;
		}
		default:
		{
			if (!C::Get<bool>(Vars.bVisualGlowWeapons))
				break;

			// check are entity is world weapon
			if (nIndex == EClassIndex::CBaseWeaponWorldModel || (nIndex != EClassIndex::CAK47 && nIndex != EClassIndex::CDEagle && CRT::StringString(pClientClass->szNetworkName, Q_XOR("CWeapon")) == nullptr))
				break;

			C::Get<Color_t>(Vars.colVisualGlowWeapons).BaseAlpha(glowObject.arrColor);
			glowObject.flBloomAmount = 1.0f;
			glowObject.bRenderWhenOccluded = true;
			glowObject.bRenderWhenUnoccluded = false;
			glowObject.nRenderStyle = GLOWRENDERSTYLE_DEFAULT;
			break;
		}
		}
	}
}
#pragma endregion
