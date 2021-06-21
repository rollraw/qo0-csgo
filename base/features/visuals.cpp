// used: std::call_once
#include <mutex>

#include "visuals.h"
// used: camera origin global variable
#include "../global.h"
// used: keyvalues for materials
#include "../sdk/datatypes/keyvalues.h"
// used: cheat variables
#include "../core/variables.h"
// used: drawmodelexecute original for chams
#include "../core/hooks.h"
// used: render functions
#include "../utilities/draw.h"
// used: vectortransformer to get bounding box
#include "../utilities/math.h"
// used: get weapon icon
#include "../utilities.h"

// @note: avoid store imcolor, store either u32 of imvec4
void CVisuals::Store()
{
	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

	if (pLocal == nullptr)
		return;

	float flServerTime = TICKS_TO_TIME(pLocal->GetTickBase());

	// disable post-processing
	static CConVar* mat_postprocess_enable = I::ConVar->FindVar(XorStr("mat_postprocess_enable"));
	mat_postprocess_enable->fnChangeCallbacks.Size() = NULL;
	mat_postprocess_enable->SetValue(!(C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_POSTPROCESSING)));

	// check is render initialized
	if (!D::bInitialized)
		return;

	const ImVec2 vecScreenSize = ImGui::GetIO().DisplaySize;

	#pragma region visuals_store_world
	// render scope lines
	if (auto pWeapon = pLocal->GetWeapon(); pWeapon != nullptr && C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_SCOPE))
	{
		CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeapon->GetItemDefinitionIndex());

		// is sniper and scoped
		if (pWeaponData != nullptr && pWeaponData->nWeaponType == WEAPONTYPE_SNIPER && pLocal->IsScoped())
		{
			float flWidth = pWeapon->GetInaccuracy() * 300.f;
			int iAlpha = std::min(255, static_cast<int>(255.f * pWeapon->GetInaccuracy()));

			D::AddLine(ImVec2(0.f, vecScreenSize.y * 0.5f), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f), Color(0, 0, 0, 200));
			D::AddLine(ImVec2(vecScreenSize.x * 0.5f, 0.f), ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y), Color(0, 0, 0, 200));

			// horizontal
			D::AddRectMultiColor(ImVec2(0.f, vecScreenSize.y * 0.5f), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f + flWidth), Color(0, 0, 0, iAlpha), Color(0, 0, 0, iAlpha), Color(0, 0, 0, 0), Color(0, 0, 0, 0));
			D::AddRectMultiColor(ImVec2(0.f, vecScreenSize.y * 0.5f - flWidth), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f), Color(0, 0, 0, 0), Color(0, 0, 0, 0), Color(0, 0, 0, iAlpha), Color(0, 0, 0, iAlpha));
			// vertical
			D::AddRectMultiColor(ImVec2(vecScreenSize.x * 0.5f, 0.f), ImVec2(vecScreenSize.x * 0.5f + flWidth, vecScreenSize.y), Color(0, 0, 0, iAlpha), Color(0, 0, 0, 0), Color(0, 0, 0, 0), Color(0, 0, 0, iAlpha));
			D::AddRectMultiColor(ImVec2(vecScreenSize.x * 0.5f - flWidth, 0.f), ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y), Color(0, 0, 0, 0), Color(0, 0, 0, iAlpha), Color(0, 0, 0, iAlpha), Color(0, 0, 0, 0));
		}
	}
	#pragma endregion

	#pragma region visuals_store_screen
	if (C::Get<bool>(Vars.bScreen))
	{
		if (C::Get<bool>(Vars.bScreenHitMarker))
			HitMarker(vecScreenSize, flServerTime, C::Get<Color>(Vars.colScreenHitMarker), C::Get<Color>(Vars.colScreenHitMarkerDamage));
	}
	#pragma endregion

	#pragma region visuals_store_esp
	std::vector<std::pair<CBaseEntity*, float>> vecOrder = { };

	for (int i = 1; i < I::ClientEntityList->GetMaxEntities(); i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr || pEntity->IsDormant())
			continue;

		// save entities and calculated distance for sort
		vecOrder.emplace_back(std::make_pair(pEntity, (pEntity->GetRenderOrigin() - G::vecCamera).Length()));
	}

	// sort entities by distance to make closest entity drawn last to make it easy readable and look nicer
	std::sort(vecOrder.begin(), vecOrder.end(), [](const auto& a, const auto& b)
		{
			return a.second > b.second;
		});

	for (const auto& [pEntity, flDistance] : vecOrder)
	{
		CBaseClient* pClientClass = pEntity->GetClientClass();

		if (pClientClass == nullptr)
			continue;

		const EClassIndex nIndex = pClientClass->nClassID;

		switch (nIndex)
		{
		case EClassIndex::CC4:
		{
			if (!C::Get<bool>(Vars.bEsp) || !C::Get<bool>(Vars.bEspMain) || !C::Get<bool>(Vars.bEspMainBomb))
				break;

			// get bomb owner
			const CBaseEntity* pOwner = I::ClientEntityList->Get<CBaseEntity>(pEntity->GetOwnerEntityHandle());

			// check only for dropped bomb, for bomb carrier need another way
			if (pOwner != nullptr)
				break;

			const Vector vecOrigin = pEntity->GetOrigin();
			ImVec2 vecScreen = { };

			// get bomb on-screen position
			if (!D::WorldToScreen(vecOrigin, vecScreen))
				break;

			// create bomb context
			Context_t ctx = { };

			Bomb(vecScreen, ctx, Color(40, 40, 40, 200));
			break;
		}
		case EClassIndex::CPlantedC4:
		{
			if (!C::Get<bool>(Vars.bEsp) || !C::Get<bool>(Vars.bEspMain) || !C::Get<bool>(Vars.bEspMainBomb))
				break;

			// cast to planted bomb entity
			CPlantedC4* pBomb = reinterpret_cast<CPlantedC4*>(pEntity);

			if (!pBomb->IsPlanted())
				break;

			const Vector vecOrigin = pEntity->GetOrigin();
			ImVec2 vecScreen = { };

			// get planted bomb on-screen position
			if (!D::WorldToScreen(vecOrigin, vecScreen))
				break;

			// setup planted bomb context
			Context_t ctx = { };

			PlantedBomb(pBomb, flServerTime, vecScreen, ctx, Color(20, 20, 20, 150), Color(80, 180, 200, 200), Color(255, 100, 100), Color(40, 40, 40, 100), Color(0, 0, 0, 100));
			break;
		}
		case EClassIndex::CCSPlayer:
		{
			if (!C::Get<bool>(Vars.bEsp) || !C::Get<bool>(Vars.bEspMain))
				break;

			if (!pEntity->IsAlive())
				break;

			if (!pLocal->IsAlive())
			{
				// check is not spectating current entity
				if (const auto pObserverEntity = I::ClientEntityList->Get<CBaseEntity>(pLocal->GetObserverTargetHandle()); pObserverEntity != nullptr && pObserverEntity == pEntity && *pLocal->GetObserverMode() == OBS_MODE_IN_EYE)
					break;
			}

			// team filters check
			if (pEntity != pLocal &&
				// enemies
				((pLocal->IsEnemy(pEntity) && C::Get<bool>(Vars.bEspMainEnemies)) ||
				// teammates
				(!pLocal->IsEnemy(pEntity) && C::Get<bool>(Vars.bEspMainAllies))))
			{
				// set entity spotted in-game radar
				if (C::Get<bool>(Vars.bEspMainPlayerFarRadar))
					*pEntity->IsSpotted() = true;

				// setup player context
				Context_t ctx = { };

				// get player bounding box
				if (!GetBoundingBox(pEntity, &ctx.box))
					break;

				/*
				 * esp list:
				 1	boxes:
				 *		full
				 *		corner
				 2	bars:
				 *		flash
				 *		health
				 *		ammo
				 3	texts:
				 *		money
				 *		name
				 >		flags:
				 *			helmet
				 *			kit
				 *			defuser
				 *			zoom
				 *		weapons
				 *		distance
				 */
				Player(pLocal, pEntity, ctx, flDistance, Color(255, 255, 255, 255), Color(20, 20, 20, 150), Color(0, 0, 0, 220));
			}

			break;
		}
		case EClassIndex::CEnvTonemapController:
		{
			NightMode(reinterpret_cast<CEnvTonemapController*>(pEntity));
			break;
		}
		case EClassIndex::CBaseCSGrenadeProjectile:
		case EClassIndex::CDecoyProjectile:
		case EClassIndex::CSmokeGrenadeProjectile:
		case EClassIndex::CMolotovProjectile:
		case EClassIndex::CInferno:
		{
			if (!C::Get<bool>(Vars.bEsp) || !C::Get<bool>(Vars.bEspMain) || !C::Get<bool>(Vars.bEspMainGrenades))
				break;

			const Vector vecOrigin = pEntity->GetOrigin();
			ImVec2 vecScreen = { };

			// get grenade on-screen position
			if (!D::WorldToScreen(vecOrigin, vecScreen))
				break;

			// setup grenade context
			Context_t ctx = { };

			Grenade(pEntity, nIndex, flServerTime, vecScreen, ctx, Color(20, 20, 20, 150), Color(40, 40, 40, 100), Color(0, 0, 0, 100));
			break;
		}
		default:
		{
			// check for esp state and skip weapon in hands
			if (!C::Get<bool>(Vars.bEsp) || !C::Get<bool>(Vars.bEspMain) || !C::Get<bool>(Vars.bEspMainWeapons) || nIndex == EClassIndex::CBaseWeaponWorldModel)
				break;

			// world weapons check
			if (strstr(pClientClass->szNetworkName, XorStr("CWeapon")) != nullptr || nIndex == EClassIndex::CDEagle || nIndex == EClassIndex::CAK47)
			{
				// cast entity to weapon
				CBaseCombatWeapon* pWeapon = reinterpret_cast<CBaseCombatWeapon*>(pEntity);

				if (pWeapon == nullptr)
					break;

				const short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
				CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

				if (pWeaponData == nullptr || !pWeaponData->IsGun())
					break;

				// get weapon owner
				const CBaseEntity* pOwner = I::ClientEntityList->Get<CBaseEntity>(pEntity->GetOwnerEntityHandle());

				// check only dropped weapons for active weapons we using another way
				if (pOwner != nullptr)
					break;

				// create weapon context
				Context_t ctx = { };

				// get weapon bounding box
				if (!GetBoundingBox(pEntity, &ctx.box))
					break;

				DroppedWeapons(pWeapon, nDefinitionIndex, ctx, flDistance, Color(255, 255, 255, 200), Color(80, 180, 200, 200), Color(40, 40, 40, 50), Color(0, 0, 0, 150));
			}

			break;
		}
		}
	}
	#pragma endregion
}

void CVisuals::Event(IGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::Engine->IsInGame())
		return;

	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

	if (pLocal == nullptr || !pLocal->IsAlive())
		return;

	const float flServerTime = TICKS_TO_TIME(pLocal->GetTickBase());

	// get hitmarker info
	if (C::Get<bool>(Vars.bScreen) && C::Get<bool>(Vars.bScreenHitMarker) && uNameHash == FNV1A::HashConst("player_hurt"))
	{
		CBaseEntity* pAttacker = I::ClientEntityList->Get<CBaseEntity>(I::Engine->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker"))));

		if (pAttacker == pLocal)
		{
			CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(I::Engine->GetPlayerForUserID(pEvent->GetInt(XorStr("userid"))));

			if (pEntity != nullptr && pEntity != pLocal)
			{
				// play hit sound
				if (C::Get<bool>(Vars.bScreenHitMarkerSound))
					I::Surface->PlaySoundSurface(XorStr("buttons\\arena_switch_press_02.wav"));

				const auto vecPosition = pEntity->GetHitGroupPosition(pEvent->GetInt(XorStr("hitgroup")));

				if (!vecPosition.has_value())
					return;

				// add hit info
				vecHitMarks.emplace_back(HitMarkerObject_t{ vecPosition.value(), pEvent->GetInt(XorStr("dmg_health")), flServerTime });
			}
		}
	}
}

bool CVisuals::Chams(CBaseEntity* pLocal, DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector& vecModelOrigin, int nFlags)
{
	static auto oDrawModel = DTR::DrawModel.GetOriginal<decltype(&H::hkDrawModel)>();
	IClientRenderable* pRenderable = info.pClientEntity;

	if (pRenderable == nullptr)
		return false;

	// get entity from renderable
	CBaseEntity* pEntity = pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (pEntity == nullptr)
		return false;

	const std::string_view szModelName = info.pStudioHdr->szName;

	// check for players
	if (pEntity->IsPlayer() && pEntity->IsAlive() && (C::Get<bool>(Vars.bEspChamsEnemies) || C::Get<bool>(Vars.bEspChamsAllies)))
	{
		// skip glow models
		if (nFlags & (STUDIO_RENDER | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_NOLIGHTING_OR_CUBEMAP | STUDIO_SKIP_DECALS))
			return false;

		// team filters check
			// enemies
		if ((pLocal->IsEnemy(pEntity) && C::Get<bool>(Vars.bEspChamsEnemies)) ||
			// teammates & local
			(((pEntity == pLocal && I::Input->bCameraInThirdPerson) || !pLocal->IsEnemy(pEntity)) && C::Get<bool>(Vars.bEspChamsAllies)))
		{
			static IMaterial* pMaterial = nullptr;

			// set players material
			switch (static_cast<EVisualsPlayersChams>(C::Get<int>(Vars.iEspChamsPlayer)))
			{
			case EVisualsPlayersChams::FLAT:
				pMaterial = arrMaterials.at(0).second;
				break;
			case EVisualsPlayersChams::REFLECTIVE:
				pMaterial = arrMaterials.at(2).first;
				break;
			default:
				pMaterial = arrMaterials.at(0).first;
				break;
			}

			// check is valid material
			if (pMaterial == nullptr || pMaterial->IsErrorMaterial())
				return false;

			// get colors
			const Color colVisible = pLocal->IsEnemy(pEntity) ? C::Get<Color>(Vars.colEspChamsEnemies) : C::Get<Color>(Vars.colEspChamsAllies);
			const Color colHidden = pLocal->IsEnemy(pEntity) ? C::Get<Color>(Vars.colEspChamsEnemiesWall) : C::Get<Color>(Vars.colEspChamsAlliesWall);

			// do chams through walls
			if (C::Get<bool>(Vars.bEspChamsXQZ))
			{
				// set xqz color
				I::StudioRender->SetColorModulation(colHidden.Base().data());

				// set xqz alpha
				I::StudioRender->SetAlphaModulation(colHidden.Base<COLOR_A>());

				// enable "$ignorez" flag and it enables ignore the z axis
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

				// set xqz wireframe
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, C::Get<int>(Vars.iEspChamsPlayer) == (int)EVisualsPlayersChams::WIREFRAME ? true : false);

				// override ignorez material
				I::StudioRender->ForcedMaterialOverride(pMaterial);

				// draw model with xqz material
				oDrawModel(I::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}

			// do visible chams
			// set color
			I::StudioRender->SetColorModulation(colVisible.Base().data());

			// set alpha
			I::StudioRender->SetAlphaModulation((pEntity == pLocal && pLocal->IsScoped() && I::Input->bCameraInThirdPerson) ? 0.3f : colVisible.Base<COLOR_A>());

			// disable "$ignorez" flag
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

			// set wireframe
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, C::Get<int>(Vars.iEspChamsPlayer) == (int)EVisualsPlayersChams::WIREFRAME ? true : false);

			// override customized material
			I::StudioRender->ForcedMaterialOverride(pMaterial);

			// then draw original with our material

			// we need to clear override
			return true;
		}
	}
	// check for viewmodel sleeves
	else if (szModelName.find(XorStr("sleeve")) != std::string_view::npos && C::Get<bool>(Vars.bEspChamsViewModel) && C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::NO_DRAW)
	{
		// get original sleeves material
		IMaterial* pSleeveMaterial = I::MaterialSystem->FindMaterial(szModelName.data(), XorStr(TEXTURE_GROUP_MODEL));

		// check is valid material
		if (pSleeveMaterial == nullptr)
			return false;

		pSleeveMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		I::StudioRender->ForcedMaterialOverride(pSleeveMaterial);

		// then draw original model with our flags

		// we need to clear override
		return true;
	}
	// check for viewmodel @note: u can separate this
	else if ((szModelName.find(XorStr("weapons\\v_")) != std::string_view::npos || szModelName.find(XorStr("arms")) != std::string_view::npos) && C::Get<bool>(Vars.bEspChamsViewModel))
	{
		// get original viewmodel material
		IMaterial* pViewModelMaterial = I::MaterialSystem->FindMaterial(szModelName.data(), XorStr(TEXTURE_GROUP_MODEL));

		// check is valid material
		if (pViewModelMaterial == nullptr)
			return false;

		if (C::Get<int>(Vars.iEspChamsViewModel) == static_cast<int>(EVisualsViewModelChams::NO_DRAW))
		{
			pViewModelMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			I::StudioRender->ForcedMaterialOverride(pViewModelMaterial);

			// then draw original model with our flags

			// we need to clear override
			return true;
		}

		static IMaterial* pMaterial = nullptr;

		// set viewmodel material
		switch (static_cast<EVisualsViewModelChams>(C::Get<int>(Vars.iEspChamsViewModel)))
		{
		case EVisualsViewModelChams::FLAT:
			pMaterial = arrMaterials.at(1).second;
			break;
		case EVisualsViewModelChams::GLOW:
			pMaterial = arrMaterials.at(2).second;
			break;
		case EVisualsViewModelChams::SCROLL:
			pMaterial = arrMaterials.at(3).first;
			break;
		case EVisualsViewModelChams::CHROME:
			pMaterial = arrMaterials.at(3).second;
			break;
		default:
			pMaterial = arrMaterials.at(1).first;
			break;
		}

		// check is valid material
		if (pMaterial == nullptr || pMaterial->IsErrorMaterial())
			return false;

		// get color
		const Color colAdditional = C::Get<Color>(Vars.colEspChamsViewModelAdditional);
		const Color colViewModel = C::Get<Color>(Vars.colEspChamsViewModel);

		// change material variables
		if (C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::GLOW)
		{
			static bool bEnvMapFresnelFound = false;
			IMaterialVar* pEnvMapFresnel = pMaterial->FindVar(XorStr("$envmapfresnel"), &bEnvMapFresnelFound);

			// add fresnel effect for glow
			if (bEnvMapFresnelFound)
				pEnvMapFresnel->SetInt(1);

			static bool bFoundEnvMapTint = false;
			IMaterialVar* pEnvMapTint = pMaterial->FindVar(XorStr("$envmaptint"), &bFoundEnvMapTint);

			// set additional color
			if (bFoundEnvMapTint)
				pEnvMapTint->SetVector(colAdditional.Base<COLOR_R>(), colAdditional.Base<COLOR_G>(), colAdditional.Base<COLOR_B>());

			// set alpha
			pMaterial->AlphaModulate(colAdditional.Base<COLOR_A>());

			// set color fusion for glow
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);

			// set "$ignorez" flag to 0 and disable it
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

			// set wireframe
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::WIREFRAME ? true : false);

			// override customized material
			I::StudioRender->ForcedMaterialOverride(pMaterial);

			// then draw original with our material
			oDrawModel(I::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

			// clear overrides
			I::StudioRender->ForcedMaterialOverride(nullptr);
		}

		// set color
		I::StudioRender->SetColorModulation(colViewModel.Base().data());

		// set alpha
		I::StudioRender->SetAlphaModulation(colViewModel.Base<COLOR_A>());

		// disable color fusion
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_ADDITIVE, false);

		// set wireframe
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::WIREFRAME ? true : false);

		// set "$ignorez" flag to 0 and disable it
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

		// override customized material
		I::StudioRender->ForcedMaterialOverride(pMaterial);

		// then draw original with our material

		// we need to clear override
		return true;
	}

	return false;
}

void CVisuals::Glow(CBaseEntity* pLocal)
{
	for (int i = 0; i < I::GlowManager->vecGlowObjectDefinitions.Count(); i++)
	{
		IGlowObjectManager::GlowObject_t& hGlowObject = I::GlowManager->vecGlowObjectDefinitions[i];

		// is current object not used
		if (hGlowObject.IsEmpty())
			continue;

		// get current entity from object handle
		CBaseEntity* pEntity = hGlowObject.pEntity;

		if (pEntity == nullptr)
			continue;

		// set bloom state
		hGlowObject.bFullBloomRender = C::Get<bool>(Vars.bEspGlowBloom);

		CBaseClient* pClientClass = pEntity->GetClientClass();

		if (pClientClass == nullptr)
			continue;

		// get class id
		const EClassIndex nIndex = pClientClass->nClassID;

		switch (nIndex)
		{
		case EClassIndex::CC4:
		case EClassIndex::CPlantedC4:
			if (C::Get<bool>(Vars.bEspGlowBomb))
				hGlowObject.Set(nIndex == EClassIndex::CC4 ? C::Get<Color>(Vars.colEspGlowBomb) : C::Get<Color>(Vars.colEspGlowBombPlanted));
			break;
		case EClassIndex::CCSPlayer:
		{
			if (pEntity->IsDormant() || !pEntity->IsAlive())
				break;

			// team filters check
				// enemies
			if ((pLocal->IsEnemy(pEntity) && C::Get<bool>(Vars.bEspGlowEnemies)) ||
				// teammates & local
				(((pEntity == pLocal && I::Input->bCameraInThirdPerson) || !pLocal->IsEnemy(pEntity)) && C::Get<bool>(Vars.bEspGlowAllies)))
			{
				hGlowObject.Set(pLocal->IsEnemy(pEntity) ?
					pLocal->IsVisible(pEntity, pEntity->GetBonePosition(BONE_HEAD).value_or(pEntity->GetEyePosition(false))) ? C::Get<Color>(Vars.colEspGlowEnemies) : C::Get<Color>(Vars.colEspGlowEnemiesWall) :
					pLocal->IsVisible(pEntity, pEntity->GetBonePosition(BONE_HEAD).value_or(pEntity->GetEyePosition(false))) ? C::Get<Color>(Vars.colEspGlowAllies) : C::Get<Color>(Vars.colEspGlowAlliesWall));
			}
			break;
		}
		case EClassIndex::CBaseCSGrenadeProjectile:
		case EClassIndex::CDecoyProjectile:
		case EClassIndex::CMolotovProjectile:
		case EClassIndex::CSensorGrenadeProjectile:
		case EClassIndex::CSmokeGrenadeProjectile:
			if (C::Get<bool>(Vars.bEspGlowGrenades))
				hGlowObject.Set(C::Get<Color>(Vars.colEspGlowGrenades));
			break;
		default:
			if (C::Get<bool>(Vars.bEspGlowWeapons) && (strstr(pClientClass->szNetworkName, XorStr("CWeapon")) != nullptr || nIndex == EClassIndex::CDEagle || nIndex == EClassIndex::CAK47))
				hGlowObject.Set(C::Get<Color>(Vars.colEspGlowWeapons));
			break;
		}
	}
}

bool CVisuals::GetBoundingBox(CBaseEntity* pEntity, Box_t* pBox) const
{
	const ICollideable* pCollideable = pEntity->GetCollideable();

	if (pCollideable == nullptr)
		return false;

	// get mins/maxs
	const Vector vecMin = pCollideable->OBBMins();
	const Vector vecMax = pCollideable->OBBMaxs();

	/*
	 * build AABB points
	 *
	 * points navigation:
	 * [N] [back/front][left/right][bottom/top]
	 *	0 - blb
	 *	1 - brb
	 *	2 - frb
	 *	3 - flb
	 *	4 - frt
	 *	5 - brt
	 *	6 - blt
	 *	7 - flt
	 */
	std::array<Vector, 8U> arrPoints =
	{
		Vector(vecMin.x, vecMin.y, vecMin.z),
		Vector(vecMin.x, vecMax.y, vecMin.z),
		Vector(vecMax.x, vecMax.y, vecMin.z),
		Vector(vecMax.x, vecMin.y, vecMin.z),
		Vector(vecMax.x, vecMax.y, vecMax.z),
		Vector(vecMin.x, vecMax.y, vecMax.z),
		Vector(vecMin.x, vecMin.y, vecMax.z),
		Vector(vecMax.x, vecMin.y, vecMax.z)
	};

	if (arrPoints.data() == nullptr)
		return false;

	// get transformation matrix
	const matrix3x4_t& matTransformed = pEntity->GetCoordinateFrame();

	float flLeft = std::numeric_limits<float>::max();
	float flTop = std::numeric_limits<float>::max();
	float flRight = std::numeric_limits<float>::lowest();
	float flBottom = std::numeric_limits<float>::lowest();

	// get screen points position
	std::array<ImVec2, 8U> arrScreen = { };
	for (std::size_t i = 0U; i < 8U; i++)
	{
		if (!D::WorldToScreen(M::VectorTransform(arrPoints.at(i), matTransformed), arrScreen.at(i)))
			return false;

		/*
		 * transformed points navigation:
		 * [N] [back/front][left/right][bottom/top]
		 *	0 - frb
		 *	1 - flb
		 *	2 - blb
		 *	3 - brb
		 *	4 - frt
		 *	5 - blt
		 *	6 - brt
		 *	7 - flt
		 */
		flLeft = std::min(flLeft, arrScreen.at(i).x);
		flTop = std::min(flTop, arrScreen.at(i).y);
		flRight = std::max(flRight, arrScreen.at(i).x);
		flBottom = std::max(flBottom, arrScreen.at(i).y);
	}

	// set calculated box
	pBox->left = flLeft;
	pBox->top = flTop;
	pBox->right = flRight;
	pBox->bottom = flBottom;
	pBox->width = flRight - flLeft;
	pBox->height = flBottom - flTop;
	return true;
}

IMaterial* CVisuals::CreateMaterial(std::string_view szName, std::string_view szShader, std::string_view szBaseTexture, std::string_view szEnvMap, bool bIgnorez, bool bWireframe, std::string_view szProxies) const
{
	/*
	 * @note: materials info:
	 1	shaders: https://developer.valvesoftware.com/wiki/Category:Shaders
	 -		parameters: https://developer.valvesoftware.com/wiki/Category:List_of_Shader_Parameters
	 2	proxies: https://developer.valvesoftware.com/wiki/Material_proxies
	 -		list: https://developer.valvesoftware.com/wiki/List_Of_Material_Proxies
	 *
	 * use "mat_texture_list 1" command to see full materials list
	 */

	const std::string szMaterial = fmt::format(XorStr(R"#("{shader}"
	{{
		"$basetexture"		"{texture}"
		"$envmap"			"{envmap}"
		"$envmapfresnel"	"0"
		"$model"			"1"
		"$translucent"		"0"
		"$ignorez"			"{ignorez}"
		"$selfillum"		"1"
		"$halflambert"		"1"
		"$wireframe"		"{wireframe}"
		"$nofog"			"1"
		"proxies"
		{{
			{proxies}
		}}
	}})#"), fmt::arg(XorStr("shader"), szShader), fmt::arg(XorStr("texture"), szBaseTexture), fmt::arg(XorStr("envmap"), szEnvMap), fmt::arg(XorStr("ignorez"), bIgnorez ? 1 : 0), fmt::arg(XorStr("wireframe"), bWireframe ? 1 : 0), fmt::arg(XorStr("proxies"), szProxies));

	// load to memory
	CKeyValues* pKeyValues = new CKeyValues;
	pKeyValues->Init(szShader.data());
	pKeyValues->LoadFromBuffer(szName.data(), szMaterial.c_str());

	// create from buffer
	return I::MaterialSystem->CreateMaterial(szName.data(), pKeyValues);
}

void CVisuals::HitMarker(const ImVec2& vecScreenSize, float flServerTime, Color colLines, Color colDamage)
{
	if (vecHitMarks.empty())
		return;

	// get last time delta for lines
	const float flLastDelta = vecHitMarks.back().flTime + C::Get<float>(Vars.flScreenHitMarkerTime) - flServerTime;

	if (flLastDelta <= 0.f)
		return;

	const float flMaxLinesAlpha = colLines.Base<COLOR_A>();
	constexpr std::array<std::array<float, 2U>, 4U> arrSides = { { { -1.0f, -1.0f }, { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f } } };

	for (const auto& arrSide : arrSides)
		// draw mark cross
		D::AddLine(ImVec2(vecScreenSize.x * 0.5f + C::Get<int>(Vars.iScreenHitMarkerGap) * arrSide[0], vecScreenSize.y * 0.5f + C::Get<int>(Vars.iScreenHitMarkerGap) * arrSide[1]), ImVec2(vecScreenSize.x * 0.5f + C::Get<int>(Vars.iScreenHitMarkerLenght) * arrSide[0], vecScreenSize.y * 0.5f + C::Get<int>(Vars.iScreenHitMarkerLenght) * arrSide[1]), colLines.Set<COLOR_A>(static_cast<std::uint8_t>(std::min(flMaxLinesAlpha, flLastDelta / C::Get<float>(Vars.flScreenHitMarkerTime)) * 255.f)));

	if (!C::Get<bool>(Vars.bScreenHitMarkerDamage))
		return;

	const float flMaxDamageAlpha = colDamage.Base<COLOR_A>();

	std::erase_if(vecHitMarks, [&](const auto& hitmarkObject)
		{
			const float flDelta = hitmarkObject.flTime + C::Get<float>(Vars.flScreenHitMarkerTime) - I::Globals->flCurrentTime;

			if (flDelta <= 0.f)
				return true;

			if (ImVec2 vecScreen = { }; D::WorldToScreen(hitmarkObject.vecPosition, vecScreen))
			{
				// max distance for floating damage
				constexpr float flDistance = 40.f;
				const float flRatio = 1.0f - (flDelta / C::Get<float>(Vars.flScreenHitMarkerTime));

				// calculate fade out alpha
				const int iAlpha = static_cast<int>(std::min(flMaxDamageAlpha, flDelta / C::Get<float>(Vars.flScreenHitMarkerTime)) * 255.f);

				// draw dealt damage
				D::AddText(F::SmallestPixel, 24.f, ImVec2(vecScreen.x, vecScreen.y - flRatio * flDistance), std::to_string(hitmarkObject.iDamage), colDamage.Set<COLOR_A>(static_cast<std::uint8_t>(iAlpha)), DRAW_TEXT_OUTLINE, Color(0, 0, 0, iAlpha));
			}

			return false;
		});
}

void CVisuals::NightMode(CEnvTonemapController* pController) const
{
	static bool bSwitch = false;

	// prevent change every tick
	if (bSwitch)
	{
		pController->IsUseCustomAutoExposureMin() = true;
		pController->IsUseCustomAutoExposureMax() = true;
		pController->GetCustomAutoExposureMin() = (C::Get<bool>(Vars.bWorld) && C::Get<bool>(Vars.bWorldNightMode)) ? 0.2f : 1.0f;
		pController->GetCustomAutoExposureMax() = (C::Get<bool>(Vars.bWorld) && C::Get<bool>(Vars.bWorldNightMode)) ? 0.2f : 1.0f;
	}

	if (bSwitch != (C::Get<bool>(Vars.bWorld) && C::Get<bool>(Vars.bWorldNightMode)))
		bSwitch = (C::Get<bool>(Vars.bWorld) && C::Get<bool>(Vars.bWorldNightMode));
}

void CVisuals::Bomb(const ImVec2& vecScreen, Context_t& ctx, const Color& colFrame) const
{
	const char* szIcon = reinterpret_cast<const char*>(U::GetWeaponIcon(WEAPON_C4));
	const ImVec2 vecIconSize = F::Icons->CalcTextSizeA(14.f, FLT_MAX, 0.f, szIcon);

	const char* szName = XorStr("C4");
	const ImVec2 vecNameSize = F::Verdana->CalcTextSizeA(14.f, FLT_MAX, 0.f, szName);

	static ImVec2 vecSize(vecIconSize.x + vecNameSize.x + 10.f, vecNameSize.y + 6.f);

	// set custom box
	ctx.box = { vecScreen.x - vecSize.x * 0.5f, vecScreen.y - vecSize.y * 0.5f, vecScreen.x + vecSize.x * 0.5f, vecScreen.y + vecSize.y * 0.5f, vecSize.x, vecSize.y };

	// frame
	D::AddRect(ImVec2(ctx.box.left - 1, ctx.box.top), ImVec2(ctx.box.right + 1, ctx.box.bottom), colFrame, DRAW_RECT_FILLED, Color(0, 0, 0, 255), 5.0f);
	// icon
	D::AddText(F::Icons, 14.f, ImVec2(ctx.box.left + 5, ctx.box.top + 3), szIcon, Color(255, 255, 255));
	// text
	D::AddText(F::Verdana, 14.f, ImVec2(ctx.box.left + vecIconSize.x + 7, ctx.box.top + 3), szName, Color(255, 255, 255));
}

void CVisuals::PlantedBomb(CPlantedC4* pBomb, float flServerTime, const ImVec2& vecScreen, Context_t& ctx, const Color& colFrame, const Color& colDefuse, const Color& colFailDefuse, const Color& colBackground, const Color& colOutline) const
{
	const char* szIcon = reinterpret_cast<const char*>(U::GetWeaponIcon(WEAPON_C4));
	static ImVec2 vecIconSize = F::Icons->CalcTextSizeA(14.f, FLT_MAX, 0.f, szIcon);

	const char* szName = XorStr("PLANTED C4");
	static ImVec2 vecNameSize = F::Verdana->CalcTextSizeA(14.f, FLT_MAX, 0.f, szName);

	static ImVec2 vecSize(vecIconSize.x + vecNameSize.x + 10.f, vecNameSize.y + 6.f);

	// set custom box
	ctx.box = { vecScreen.x - vecSize.x * 0.5f, vecScreen.y - vecSize.y * 0.5f, vecScreen.x + vecSize.x * 0.5f, vecScreen.y + vecSize.y * 0.5f, vecSize.x, vecSize.y };

	// get defuser entity
	const CBaseEntity* pDefuser = I::ClientEntityList->Get<CBaseEntity>(pBomb->GetDefuserHandle());

	/* info */
	// frame
	D::AddRect(ImVec2(ctx.box.left - 1, ctx.box.top), ImVec2(ctx.box.right + 1, ctx.box.bottom), colFrame, DRAW_RECT_FILLED, Color(0, 0, 0, 255), 5.0f, ImDrawCornerFlags_Top);
	// icon
	D::AddText(F::Icons, 14.f, ImVec2(ctx.box.left + 5, ctx.box.top + 3), szIcon, pDefuser != nullptr ? colDefuse : Color(255, 255, 255));
	// text
	D::AddText(F::Verdana, 14.f, ImVec2(ctx.box.left + vecIconSize.x + 10, ctx.box.top + 3), szName, Color(255, 255, 255));

	/* bar */
	// get timer values
	const float flCurrentTime = pBomb->GetTimer(flServerTime);
	const float flMaxTime = pBomb->GetTimerLength();

	// calculate bomb timer-based width factor
	const float flFactor = flCurrentTime / flMaxTime;
	// calculate bomb timer-based color
	const float flHue = (flFactor * 120.f) / 360.f;

	/* timer bar */
	// background
	D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.right, ctx.box.bottom + 4), colBackground, DRAW_RECT_FILLED | DRAW_RECT_OUTLINE, colOutline);
	// bar
	D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.bottom + 4), Color::FromHSB(flHue, 1.f, 1.f), DRAW_RECT_FILLED);
	ctx.arrPadding.at(DIR_BOTTOM) += 5.0f;

	// check for defuser to update defusing time
	if (pDefuser != nullptr)
	{
		// get defuse timer value
		const float flDefuseTime = pBomb->GetDefuseTimer(flServerTime);
		const float flMaxDefuseTime = pBomb->GetDefuseLength();

		// calculate bomb defuse timer-based width factor
		const float flDefuseFactor = flDefuseTime / flMaxDefuseTime;

		/* defusing bar */
		// background
		D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.right, ctx.box.bottom + 4 + ctx.arrPadding.at(DIR_BOTTOM)), colBackground, DRAW_RECT_FILLED | DRAW_RECT_OUTLINE, colOutline);
		// bar
		D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.left + ctx.box.width * flDefuseFactor, ctx.box.bottom + 4 + ctx.arrPadding.at(DIR_BOTTOM)), (flDefuseTime < flCurrentTime) ? colDefuse : colFailDefuse, DRAW_RECT_FILLED);
	}
}

void CVisuals::Grenade(CBaseEntity* pGrenade, EClassIndex nIndex, float flServerTime, const ImVec2& vecScreen, Context_t& ctx, const Color& colFrame, const Color& colBackground, const Color& colOutline) const
{
	// setup temporary values
	const char* szName = XorStr("NONE");
	float flFactor = 0.f;
	Color colGrenade(255, 255, 255);

	// get grenade model name
	if (std::string_view szModelName = I::ModelInfo->GetModelName(pGrenade->GetModel()); !szModelName.empty())
	{
		// get grenade info
		switch (nIndex)
		{
		case EClassIndex::CBaseCSGrenadeProjectile:
		{
			// separate greandes by model name
			if (szModelName.find(XorStr("fraggrenade")) != std::string_view::npos)
				szName = XorStr("HIGH-EXPLOSIVE");
			else if (szModelName.find(XorStr("flashbang")) != std::string_view::npos)
				szName = XorStr("FLASH");
			break;
		}
		case EClassIndex::CDecoyProjectile:
		{
			szName = XorStr("DECOY");
			break;
		}
		case EClassIndex::CSmokeGrenadeProjectile:
		{
			// cast to smoke grenade
			CSmokeGrenade* pSmoke = reinterpret_cast<CSmokeGrenade*>(pGrenade);
			flFactor = ((TICKS_TO_TIME(pSmoke->GetEffectTickBegin()) + pSmoke->GetMaxTime()) - flServerTime) / pSmoke->GetMaxTime();
			colGrenade = Color(230, 130, 0);
			szName = XorStr("SMOKE");
			break;
		}
		case EClassIndex::CMolotovProjectile:
		case EClassIndex::CInferno:
		{
			// cast to inferno grenade
			CInferno* pInferno = reinterpret_cast<CInferno*>(pGrenade);
			flFactor = ((TICKS_TO_TIME(pInferno->GetEffectTickBegin()) + pInferno->GetMaxTime()) - flServerTime) / pInferno->GetMaxTime();
			colGrenade = Color(255, 100, 100);

			// separate grenades by model name
			if (szModelName.find(XorStr("molotov")) != std::string_view::npos)
				szName = XorStr("MOLOTOV");
			else if (szModelName.find(XorStr("incendiary")) != std::string_view::npos)
				szName = XorStr("INCENDIARY");
			else
				szName = XorStr("FIRE");
			break;
		}
		default:
			break;
		}
	}

	const ImVec2 vecNameSize = F::Verdana->CalcTextSizeA(14.f, FLT_MAX, 0.0f, szName);
	const ImVec2 vecSize(vecNameSize.x + 10.f, vecNameSize.y + 6.f);

	// set custom box
	ctx.box = { vecScreen.x - vecSize.x * 0.5f, vecScreen.y - vecSize.y * 0.5f, vecScreen.x + vecSize.x * 0.5f, vecScreen.y + vecSize.y * 0.5f, vecSize.x, vecSize.y };

	/* info */
	// frame
	D::AddRect(ImVec2(ctx.box.left - 1, ctx.box.top), ImVec2(ctx.box.right + 1, ctx.box.bottom), colFrame, DRAW_RECT_FILLED, colOutline, 5.0f, (nIndex == EClassIndex::CBaseCSGrenadeProjectile || flFactor <= 0.f) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Top);
	// text
	D::AddText(F::Verdana, 14.f, ImVec2(ctx.box.left + 5, ctx.box.top + 3), szName, Color(255, 255, 255));

	if (flFactor > 0.f)
	{
		/* bar */
		// background
		D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.right, ctx.box.bottom + 4), colBackground, DRAW_RECT_FILLED | DRAW_RECT_OUTLINE, colOutline);
		// bar
		D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.bottom + 4), colGrenade, DRAW_RECT_FILLED);
	}
}

void CVisuals::DroppedWeapons(CBaseCombatWeapon* pWeapon, short nItemDefinitionIndex, Context_t& ctx, const float flDistance, const Color& colPrimary, const Color& colAmmo, const Color& colBackground, const Color& colOutline)
{
	if (C::Get<int>(Vars.iEspMainWeaponBox) > (int)EVisualsBoxType::NONE)
		Box(ctx.box, C::Get<int>(Vars.iEspMainWeaponBox), C::Get<Color>(Vars.colEspMainBoxWeapons), Color(0, 0, 0, 150));

	// info's master check
	if (!C::Get<bool>(Vars.bEspMainWeaponInfo))
		return;

	// @note: for text weapon names
	// const char* szHudName = pWeaponData->szHudName;
	// wchar_t* wszLocalizeName = I::Localize->Find(szHudName);

	// weapon icon
	if (C::Get<bool>(Vars.bEspMainWeaponIcon))
	{
		// get dropped weapon icon
		const char* szIcon = reinterpret_cast<const char*>(U::GetWeaponIcon(nItemDefinitionIndex));
		const ImVec2 vecNameSize = F::Icons->CalcTextSizeA(10.f, FLT_MAX, 0.f, szIcon);

		D::AddText(F::Icons, 10.f, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecNameSize.x * 0.5f, ctx.box.bottom + 3), szIcon, colPrimary, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_BOTTOM) += 3.0f + vecNameSize.y;
	}

	// ammo bar
	if (C::Get<bool>(Vars.bEspMainWeaponAmmo))
		AmmoBar(nullptr, pWeapon, ctx, colAmmo, colBackground, colOutline);

	// distance
	if (C::Get<bool>(Vars.bEspMainWeaponDistance))
	{
		const int iDistance = static_cast<int>(M_INCH2METRE(flDistance));
		std::string szDistance = std::to_string(iDistance).append(XorStr("M"));
		const ImVec2 vecDistanceSize = F::SmallestPixel->CalcTextSizeA(12.f, FLT_MAX, 0.0f, szDistance.c_str());

		D::AddText(F::SmallestPixel, 12.f, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecDistanceSize.x * 0.5f, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), szDistance, colPrimary, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_BOTTOM) += vecDistanceSize.y;
	}
}

void CVisuals::Player(CBaseEntity* pLocal, CBaseEntity* pEntity, Context_t& ctx, const float flDistance, const Color& colInfo, const Color& colFrame, const Color& colOutline)
{
	PlayerInfo_t playerInfo = { };

	if (!I::Engine->GetPlayerInfo(pEntity->GetIndex(), &playerInfo))
		return;

	if (C::Get<int>(Vars.iEspMainPlayerBox) > static_cast<int>(EVisualsBoxType::NONE))
	{
		// get box color based on visibility & enmity
		const Color colBox = pEntity->IsEnemy(pLocal) ?
			pLocal->IsVisible(pEntity, pEntity->GetBonePosition(BONE_HEAD).value_or(pEntity->GetEyePosition(false))) ? C::Get<Color>(Vars.colEspMainBoxEnemies) : C::Get<Color>(Vars.colEspMainBoxEnemiesWall) :
			pLocal->IsVisible(pEntity, pEntity->GetBonePosition(BONE_HEAD).value_or(pEntity->GetEyePosition(false))) ? C::Get<Color>(Vars.colEspMainBoxAllies) : C::Get<Color>(Vars.colEspMainBoxAlliesWall);

		Box(ctx.box, C::Get<int>(Vars.iEspMainPlayerBox), colBox, Color(0, 0, 0, 150));
	}

	// info's master check
	if (!C::Get<bool>(Vars.bEspMainPlayerInfo))
		return;

	// @note: distance font scale
	const float flFontSize = std::clamp(90.f / (flDistance / 90.f), 10.f, 40.f);

	#pragma region visuals_player_top
	if (C::Get<bool>(Vars.bEspMainPlayerFlash) && pEntity->GetFlashDuration() > 0.2f)
		FlashBar(pEntity, ctx, Color(255, 255, 255, 220), Color(40, 40, 40, 100), Color(0, 0, 0, 150));

	if (C::Get<bool>(Vars.bEspMainPlayerName))
	{
		// get player name
		std::string szName = playerInfo.szName;

		// truncate name
		if (szName.length() > 24U)
			szName = szName.substr(0U, 24U).append(XorStr("..."));

		const char* const szBot = XorStr("[BOT]");
		ImVec2 vecBotSize{ };
		const ImVec2 vecNameSize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szName.c_str());

		// add prefix for bots
		if (playerInfo.bFakePlayer)
		{
			vecBotSize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szBot);
			D::AddText(F::SmallestPixel, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f + 1 + vecNameSize.x * 0.5f - vecBotSize.x * 0.5f, ctx.box.top - 2 - vecBotSize.y - ctx.arrPadding.at(DIR_TOP)), szBot, Color(140, 140, 140), DRAW_TEXT_OUTLINE, colOutline);
		}

		D::AddText(F::SmallestPixel, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecNameSize.x * 0.5f - vecBotSize.x * 0.5f, ctx.box.top - 2 - vecNameSize.y - ctx.arrPadding.at(DIR_TOP)), szName, colInfo, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_TOP) += vecNameSize.y;
	}
	#pragma endregion

	#pragma region visuals_player_bottom
	// get active weapon
	if (CBaseCombatWeapon* pActiveWeapon = pEntity->GetWeapon(); pActiveWeapon != nullptr)
	{
		// ammo bar
		if (C::Get<bool>(Vars.bEspMainPlayerAmmo))
			AmmoBar(pEntity, pActiveWeapon, ctx, Color(80, 180, 200), Color(40, 40, 40, 100), Color(0, 0, 0, 150));

		// get all other weapons
		if (C::Get<bool>(Vars.bEspMainPlayerWeapons))
		{
			if (const auto hWeapons = pEntity->GetWeaponsHandle(); hWeapons != nullptr)
			{
				// -1 to prevent double active weapon
				for (int nIndex = MAX_WEAPONS - 1; hWeapons[nIndex]; nIndex--)
				{
					// get current weapon
					CBaseCombatWeapon* pCurrentWeapon = I::ClientEntityList->Get<CBaseCombatWeapon>(hWeapons[nIndex]);

					if (pCurrentWeapon == nullptr)
						continue;

					const short nDefinitionIndex = pCurrentWeapon->GetItemDefinitionIndex();

					// do not draw some useless dangerzone weapons (lmao i just dont add icons)
					if (nDefinitionIndex == WEAPON_SHIELD || nDefinitionIndex == WEAPON_BREACHCHARGE || nDefinitionIndex == WEAPON_BUMPMINE)
						continue;

					CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

					// pass only active grenades/fists/tablet // @todo: or make it on right/left bottom side
					if (pWeaponData == nullptr || ((pWeaponData->nWeaponType == WEAPONTYPE_GRENADE || nDefinitionIndex == WEAPON_FISTS || nDefinitionIndex == WEAPON_TABLET) && pCurrentWeapon != pActiveWeapon))
						continue;

					// draw weapons list
					const char* szIcon = reinterpret_cast<const char*>(U::GetWeaponIcon(nDefinitionIndex));
					const ImVec2 vecIconSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szIcon);
					D::AddText(F::Icons, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecIconSize.x * 0.5f, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), szIcon, pCurrentWeapon == pActiveWeapon ? colInfo : Color(160, 160, 160), DRAW_TEXT_OUTLINE, colOutline);
					ctx.arrPadding.at(DIR_BOTTOM) += vecIconSize.y;
				}
			}
		}
	}

	if (C::Get<bool>(Vars.bEspMainPlayerDistance))
	{
		const int iDistance = static_cast<int>(M_INCH2METRE(flDistance));
		std::string szDistance = std::to_string(iDistance).append(XorStr("M"));
		const ImVec2 vecDistanceSize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szDistance.c_str());
		D::AddText(F::SmallestPixel, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecDistanceSize.x * 0.5f, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), szDistance, colInfo, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_BOTTOM) += vecDistanceSize.y;
	}
	#pragma endregion

	#pragma region visuals_player_left
	if (C::Get<bool>(Vars.bEspMainPlayerHealth))
	{
		// calculate hp-based color
		const float flFactor = static_cast<float>(pEntity->GetHealth()) / static_cast<float>(pEntity->GetMaxHealth());
		const float flHue = (flFactor * 120.f) / 360.f;
		HealthBar(ctx, flFactor, Color::FromHSB(flHue, 1.f, 1.f), Color(40, 40, 40, 100), Color(0, 0, 0, 150));
	}

	if (C::Get<bool>(Vars.bEspMainPlayerMoney))
	{
		std::string szMoney = std::to_string(pEntity->GetMoney()).insert(0U, XorStr("$"));
		const ImVec2 vecMoneySize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szMoney.c_str());
		D::AddText(F::SmallestPixel, flFontSize, ImVec2(ctx.box.left - 2 - vecMoneySize.x - ctx.arrPadding.at(DIR_LEFT), ctx.box.top), szMoney, Color(140, 195, 75), DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_LEFT) += vecMoneySize.x;
	}
	#pragma endregion

	#pragma region visuals_player_right
	if (C::Get<std::vector<bool>>(Vars.vecEspMainPlayerFlags).at(INFO_FLAG_HELMET) && pEntity->HasHelmet())
	{
		constexpr const char* szHelmetIcon = "\xEE\x88\x8E";
		const ImVec2 vecHelmetSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szHelmetIcon);
		D::AddText(F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szHelmetIcon, colInfo, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_RIGHT) += vecHelmetSize.y;
	}

	if (C::Get<std::vector<bool>>(Vars.vecEspMainPlayerFlags).at(INFO_FLAG_KEVLAR) && pEntity->GetArmor() > 0)
	{
		constexpr const char* szKevlarIcon = "\xEE\x88\x90";
		const ImVec2 vecKevlarSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szKevlarIcon);
		D::AddText(F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szKevlarIcon, colInfo, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_RIGHT) += vecKevlarSize.y;
	}

	if (C::Get<std::vector<bool>>(Vars.vecEspMainPlayerFlags).at(INFO_FLAG_KIT) && pEntity->HasDefuser())
	{
		constexpr const char* szKitIcon = "\xEE\x88\x8F";
		const ImVec2 vecKitSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szKitIcon);
		D::AddText(F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szKitIcon, pEntity->IsDefusing() ? Color(80, 180, 200) : colInfo, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_RIGHT) += vecKitSize.y;
	}

	if (C::Get<std::vector<bool>>(Vars.vecEspMainPlayerFlags).at(INFO_FLAG_ZOOM) && pEntity->IsScoped())
	{
		constexpr const char* szTargetIcon = "\xEE\x88\x92";
		const ImVec2 vecZoomSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szTargetIcon);
		D::AddText(F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szTargetIcon, colInfo, DRAW_TEXT_OUTLINE, colOutline);
		ctx.arrPadding.at(DIR_RIGHT) += vecZoomSize.y;
	}
	#pragma endregion
}

void CVisuals::Box(const Box_t& box, const int nBoxType, const Color& colPrimary, const Color& colOutline)
{
	switch (static_cast<EVisualsBoxType>(nBoxType))
	{
	case EVisualsBoxType::FULL:
	{
		D::AddRect(ImVec2(box.left, box.top), ImVec2(box.right, box.bottom), colPrimary, DRAW_RECT_OUTLINE | DRAW_RECT_BORDER, colOutline);
		break;
	}
	case EVisualsBoxType::CORNERS:
	{
		// num of parts we divide the whole line
		constexpr int nDivideParts = 5;

		const std::pair<ImVec2, ImVec2> arrPoints[] =
		{
			// left vertical lines
			{ ImVec2(box.left, box.top), ImVec2(box.left, box.top + box.height / nDivideParts) },
			{ ImVec2(box.left, box.bottom), ImVec2(box.left, box.bottom - box.height / nDivideParts) },

			// right vertical lines
			{ ImVec2(box.right, box.top), ImVec2(box.right, box.top + box.height / nDivideParts) },
			{ ImVec2(box.right, box.bottom), ImVec2(box.right, box.bottom - box.height / nDivideParts) },

			// left horizontal lines
			{ ImVec2(box.left, box.top), ImVec2(box.left + box.width / nDivideParts, box.top) },
			{ ImVec2(box.left, box.bottom), ImVec2(box.left + box.width / nDivideParts, box.bottom) },

			// right horizontal lines
			{ ImVec2(box.right, box.top), ImVec2(box.right - box.width / nDivideParts, box.top) },
			{ ImVec2(box.right, box.bottom), ImVec2(box.right - box.width / nDivideParts, box.bottom) }
		};

		for (const auto& arrPoint : arrPoints)
			D::AddLine(arrPoint.first, arrPoint.second, colPrimary);

		break;
	}
	default:
		break;
	}
}

void CVisuals::HealthBar(Context_t& ctx, const float flFactor, const Color& colPrimary, const Color& colBackground, const Color& colOutline)
{
	// background
	D::AddRect(ImVec2(ctx.box.left - 5 - ctx.arrPadding.at(DIR_LEFT), ctx.box.top), ImVec2(ctx.box.left - 3 - ctx.arrPadding.at(DIR_LEFT), ctx.box.bottom), colBackground, DRAW_RECT_FILLED | DRAW_RECT_OUTLINE, colOutline);
	// bar
	D::AddRect(ImVec2(ctx.box.left - 5 - ctx.arrPadding.at(DIR_LEFT), ctx.box.bottom - (ctx.box.height * flFactor)), ImVec2(ctx.box.left - 3 - ctx.arrPadding.at(DIR_LEFT), ctx.box.bottom), colPrimary, DRAW_RECT_FILLED);
	ctx.arrPadding.at(DIR_LEFT) += 6.0f;
}

void CVisuals::AmmoBar(CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, Context_t& ctx, const Color& colPrimary, const Color& colBackground, const Color& colOutline)
{
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeapon->GetItemDefinitionIndex());

	if (pWeaponData == nullptr)
		return;

	// shown only for guns
	if (!pWeaponData->IsGun())
		return;

	// get ammo count
	const int iAmmo = pWeapon->GetAmmo();
	const int iMaxAmmo = pWeaponData->iMaxClip1;

	CAnimationLayer* pLayer = nullptr;
	int nActivity = 0;

	// get entity animation activity
	if (pEntity != nullptr)
	{
		pLayer = pEntity->GetAnimationLayer(1);
		nActivity = pEntity->GetSequenceActivity(pLayer->nSequence);
	}

	// add modifiable width factor
	float flFactor = 0.f;

	// calculate ammo-based width factor
		// check for reloading animation
	if (pEntity != nullptr && pLayer != nullptr && nActivity == ACT_CSGO_RELOAD && pLayer->flWeight != 0.0f)
		// reloading width
		flFactor = pLayer->flCycle;
	else
		// if not reloading
		flFactor = static_cast<float>(iAmmo) / static_cast<float>(iMaxAmmo);

	// background
	D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 3 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.right, ctx.box.bottom + 5 + ctx.arrPadding.at(DIR_BOTTOM)), colBackground, DRAW_RECT_FILLED | DRAW_RECT_OUTLINE, colOutline);
	// bar
	D::AddRect(ImVec2(ctx.box.left, ctx.box.bottom + 3 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.bottom + 5 + ctx.arrPadding.at(DIR_BOTTOM)), colPrimary, DRAW_RECT_FILLED);
	ctx.arrPadding.at(DIR_BOTTOM) += 6.0f;
}

void CVisuals::FlashBar(CBaseEntity* pEntity, Context_t& ctx, const Color& colPrimary, const Color& colBackground, const Color& colOutline)
{
	// calculate flash alpha-based width factor
	const float flFactor = pEntity->GetFlashAlpha() / *pEntity->GetFlashMaxAlpha();

	// background
	D::AddRect(ImVec2(ctx.box.left, ctx.box.top - 5 - ctx.arrPadding.at(DIR_TOP)), ImVec2(ctx.box.right, ctx.box.top - 3 - ctx.arrPadding.at(DIR_TOP)), colBackground, DRAW_RECT_FILLED | DRAW_RECT_OUTLINE);
	// bar
	D::AddRect(ImVec2(ctx.box.left, ctx.box.top - 5 - ctx.arrPadding.at(DIR_TOP)), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.top - 3 - ctx.arrPadding.at(DIR_TOP)), colPrimary, DRAW_RECT_FILLED);
	ctx.arrPadding.at(DIR_TOP) += 6.0f;
}
