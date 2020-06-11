// used: std::call_once
#include <mutex>

#include "visuals.h"
// used: camera origin global variable
#include "../global.h"
// used: keyvalues for materials
#include "../sdk/datatypes/keyvalues.h"
// used: cheat variables
#include "../core/variables.h"
// used: main window open state
#include "../core/menu.h"
// used: drawmodelexecute original for chams
#include "../core/hooks.h"
// used: render functions
#include "../utilities/draw.h"
// used: vectortransformer to get bounding box
#include "../utilities/math.h"
// used: get localplayer
#include "../utilities.h"

/*
 * @todo: do not read any data in the drawing hooks at all, use a cm/fsn and thread-safe container for this (very later)
 * @note: never call setupbones in endscene/present!
 * avoid store imcolor, store either u32 of imvec4
 */
void CVisuals::Run(ImDrawList* pDrawList, const ImVec2 vecScreenSize)
{
	// is valid drawlist
	if (pDrawList == nullptr)
		return;

	CBaseEntity* pLocal = U::GetLocalPlayer();

	if (pLocal == nullptr)
		return;

	float flServerTime = TICKS_TO_TIME(pLocal->GetTickBase());

	// disable postproccesing
	static CConVar* mat_postprocess_enable = I::ConVar->FindVar(XorStr("mat_postprocess_enable"));
	mat_postprocess_enable->fnChangeCallbacks.Size() = NULL;
	mat_postprocess_enable->SetValue(!(C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_POSTPROCESSING)));

	// doesnt show when menu is opened
	if (W::bMainOpened)
		return;

	// render scope lines
	if (auto pWeapon = pLocal->GetWeapon(); pWeapon != nullptr && C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_SCOPE))
	{
		CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(*pWeapon->GetItemDefinitionIndex());

		// is sniper and scoped
		if (pWeaponData != nullptr && pWeaponData->nWeaponType == WEAPONTYPE_SNIPER && pLocal->IsScoped())
		{
			float flWidth = pWeapon->GetInaccuracy() * 300.f;
			int iAlpha = std::min<int>(255, 255.f * pWeapon->GetInaccuracy());

			pDrawList->AddLine(ImVec2(0.f, vecScreenSize.y * 0.5f), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f), IM_COL32(0, 0, 0, 200));
			pDrawList->AddLine(ImVec2(vecScreenSize.x * 0.5f, 0.f), ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y), IM_COL32(0, 0, 0, 200));

			// horizontal
			pDrawList->AddRectFilledMultiColor(ImVec2(0.f, vecScreenSize.y * 0.5f), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f + flWidth), IM_COL32(0, 0, 0, iAlpha), IM_COL32(0, 0, 0, iAlpha), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0));
			pDrawList->AddRectFilledMultiColor(ImVec2(0.f, vecScreenSize.y * 0.5f - flWidth), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, iAlpha), IM_COL32(0, 0, 0, iAlpha));
			// vertical
			pDrawList->AddRectFilledMultiColor(ImVec2(vecScreenSize.x * 0.5f, 0.f), ImVec2(vecScreenSize.x * 0.5f + flWidth, vecScreenSize.y), IM_COL32(0, 0, 0, iAlpha), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, iAlpha));
			pDrawList->AddRectFilledMultiColor(ImVec2(vecScreenSize.x * 0.5f - flWidth, 0.f), ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, iAlpha), IM_COL32(0, 0, 0, iAlpha), IM_COL32(0, 0, 0, 0));
		}
	}

	// on-screen
	if (C::Get<bool>(Vars.bScreen))
	{
		if (C::Get<bool>(Vars.bScreenHitMarker))
			HitMarker(pDrawList, flServerTime, vecScreenSize, C::Get<Color>(Vars.colScreenHitMarker), C::Get<Color>(Vars.colScreenHitMarkerDamage));
	}

	for (int i = 1; i < I::ClientEntityList->GetMaxEntities(); i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr)
			continue;

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
			const CBaseEntity* pOwner = I::ClientEntityList->Get<CBaseEntity>(pEntity->GetOwnerEntity());

			// check only for dropped bomb, for bomb carrier need another way
			if (pOwner != nullptr)
				break;

			const Vector vecOrigin = pEntity->GetOrigin();
			Vector2D vecScreen = { };

			// get bomb on-screen position
			if (!D::WorldToScreen(vecOrigin, vecScreen))
				break;

			// create bomb context
			Context_t ctx = { };

			Bomb(pDrawList, vecScreen, ctx, Color(40, 40, 40, 200));
			break;
		}
		case EClassIndex::CPlantedC4:
		{
			if (!C::Get<bool>(Vars.bEsp) || !C::Get<bool>(Vars.bEspMain) || !C::Get<bool>(Vars.bEspMainBomb))
				break;

			// cast to planted bomb entity
			CPlantedC4* pBomb = (CPlantedC4*)pEntity;

			if (!pBomb->IsPlanted())
				break;

			const Vector vecOrigin = pEntity->GetOrigin();
			Vector2D vecScreen = { };

			// get planted bomb on-screen position
			if (!D::WorldToScreen(vecOrigin, vecScreen))
				break;

			// setup planted bomb context
			Context_t ctx = { };

			PlantedBomb(pDrawList, pBomb, flServerTime, vecScreen, ctx, Color(20, 20, 20, 150), Color(80, 180, 200, 200), Color(255, 100, 100), Color(40, 40, 40, 100), Color(0, 0, 0, 100));
			break;
		}
		case EClassIndex::CCSPlayer:
		{
			if (!C::Get<bool>(Vars.bEsp) || !C::Get<bool>(Vars.bEspMain))
				break;

			if (!pEntity->IsAlive() || pEntity->IsDormant())
				break;

			if (!pLocal->IsAlive())
			{
				// check is not spectating current entity
				if (const auto pObserverEntity = I::ClientEntityList->Get<CBaseEntity>(pLocal->GetObserverTarget()); pObserverEntity != nullptr && pObserverEntity == pEntity)
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
				if (C::Get<bool>(Vars.bEspMainFarRadar))
					*pEntity->GetSpotted() = true;

				// setup player context
				Context_t ctx = { };

				// get player bounding box
				if (!GetBoundingBox(pEntity, ctx.box))
					break;

				/*
				 * esp list:
				 1	boxes:
				 *		full
				 *		corner
				 2	bars:
				 *		health
				 *		ammo
				 3	texts:
				 *		money
				 *		name
				 *		rank
				 >		flags:
				 *			helmet
				 *			kit
				 *			defuser
				 *			zoom
				 *		weapons
				 *		distance
				 */
				Player(pDrawList, pLocal, pEntity, ctx, Color(255, 255, 255, 255), Color(20, 20, 20, 150), Color(0, 0, 0, 220));
			}

			break;
		}
		case EClassIndex::CEnvTonemapController:
		{
			NightMode((CEnvTonemapController*)pEntity);
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
			Vector2D vecScreen = { };

			// get grenade on-screen position
			if (!D::WorldToScreen(vecOrigin, vecScreen))
				break;

			// setup grenade context
			Context_t ctx = { };

			Grenade(pDrawList, nIndex, pEntity, flServerTime, vecScreen, ctx, Color(20, 20, 20, 150), Color(40, 40, 40, 100), Color(0, 0, 0, 100));
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
				CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)pEntity;

				if (pWeapon == nullptr)
					break;

				const short nDefinitionIndex = *pWeapon->GetItemDefinitionIndex();
				CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

				if (pWeaponData == nullptr || !pWeaponData->IsGun())
					break;

				// get weapon owner
				const CBaseEntity* pOwner = I::ClientEntityList->Get<CBaseEntity>(pEntity->GetOwnerEntity());

				// check only dropped weapons for active weapons we using another way
				if (pOwner != nullptr)
					break;

				// create weapon context
				Context_t ctx = { };

				// get weapon bounding box
				if (!GetBoundingBox(pEntity, ctx.box))
					break;

				DroppedWeapons(pDrawList, pWeapon, nDefinitionIndex, ctx, Color(255, 255, 255, 200), Color(80, 180, 200, 200), Color(40, 40, 40, 50), Color(0, 0, 0, 150));
			}

			break;
		}
		}
	}
}

void CVisuals::Event(IGameEvent* pEvent)
{
	if (pEvent == nullptr || !I::Engine->IsInGame())
		return;

	CBaseEntity* pLocal = U::GetLocalPlayer();

	if (pLocal == nullptr || !pLocal->IsAlive())
		return;

	float flServerTime = TICKS_TO_TIME(pLocal->GetTickBase());

	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());

	/* get hitmarker info */
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

				// add hit info
				vecHitMarks.emplace_back(HitMarkerObject_t{ pEntity->GetHitGroupPosition(pEvent->GetInt(XorStr("hitgroup"))), pEvent->GetInt(XorStr("dmg_health")), flServerTime + C::Get<float>(Vars.flScreenHitMarkerTime) });
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

	static std::string_view szScrollProxies = XorStr(R"#("texturescroll"
	{
		"texturescrollvar"		"$basetexturetransform"
		"texturescrollrate"		"0.2"
		"texturescrollangle"	"90"
	})#");

	/*
	 * materials navigation:
	 * [N]	[group]		[lit][proxy]
	 *					[1/2] [1/2]
	 *	0 - players		[+/-] [-/-]
	 *	1 - viewmodel	[+/-] [-/-]
	 *	2 - reflects	[+/+] [-/-]
	 *	3 - custom		[+/+] [+/-]
	 */
	static std::array<std::pair<IMaterial*, IMaterial*>, 4U> arrMaterials =
	{
		std::make_pair(CreateMaterial(XorStr("qo0_players"), XorStr("VertexLitGeneric")),
		CreateMaterial(XorStr("qo0_players_flat"), XorStr("UnlitGeneric"))),

		std::make_pair(CreateMaterial(XorStr("qo0_viewmodel"), XorStr("VertexLitGeneric")),
		CreateMaterial(XorStr("qo0_viewmodel_flat"), XorStr("UnlitGeneric"))),

		std::make_pair(CreateMaterial(XorStr("qo0_reflective"), XorStr("VertexLitGeneric"), XorStr("vgui/white"), XorStr("env_cubemap")),
		CreateMaterial(XorStr("qo0_glow"), XorStr("VertexLitGeneric"), XorStr("vgui/white"), XorStr("models/effects/cube_white"))),

		std::make_pair(CreateMaterial(XorStr("qo0_scroll"), XorStr("VertexLitGeneric"), XorStr("dev/screenhighlight_pulse"), "", false, false, szScrollProxies),
		I::MaterialSystem->FindMaterial(XorStr("models/inventory_items/hydra_crystal/hydra_crystal_detail"), TEXTURE_GROUP_OTHER))
	};
	
	std::string_view szModelName = info.pStudioHdr->szName;

	// check for players
	if (szModelName.find(XorStr("player")) != std::string_view::npos && szModelName.find(XorStr("shadow")) == std::string_view::npos && (C::Get<bool>(Vars.bEspChamsEnemies) || C::Get<bool>(Vars.bEspChamsAllies)))
	{
		// skip glow models
		if (nFlags & (STUDIO_RENDER | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_NOLIGHTING_OR_CUBEMAP | STUDIO_SKIP_DECALS))
			return false;

		if (!pEntity->IsAlive())
			return false;

		// team filters check
			// enemies
		if ((pLocal->IsEnemy(pEntity) && C::Get<bool>(Vars.bEspChamsEnemies)) ||
			// teammates & local
			(((pEntity == pLocal && I::Input->bCameraInThirdPerson) || !pLocal->IsEnemy(pEntity)) && C::Get<bool>(Vars.bEspChamsAllies)))
		{
			static IMaterial* pMaterial = nullptr;

			// set players material
			switch (C::Get<int>(Vars.iEspChamsPlayers))
			{
			case (int)EVisualsPlayersChams::FLAT:
				pMaterial = arrMaterials.at(0).second;
				break;
			case (int)EVisualsPlayersChams::REFLECTIVE:
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
			Color colVisible = pLocal->IsEnemy(pEntity) ? C::Get<Color>(Vars.colEspChamsEnemies) : C::Get<Color>(Vars.colEspChamsAllies);
			Color colHidden = pLocal->IsEnemy(pEntity) ? C::Get<Color>(Vars.colEspChamsEnemiesWall) : C::Get<Color>(Vars.colEspChamsAlliesWall);

			/* chams through walls */
			if (C::Get<bool>(Vars.bEspChamsXQZ))
			{
				pMaterial->IncrementReferenceCount();

				// enable "$ignorez" flag and it enables ignore the z axis
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

				// set xqz wireframe
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, C::Get<int>(Vars.iEspChamsPlayers) == (int)EVisualsPlayersChams::WIREFRAME ? true : false);

				// set xqz color
				I::StudioRender->SetColorModulation(colHidden.Base());

				// set xqz alpha
				I::StudioRender->SetAlphaModulation(colHidden.aBase());

				// override ignorez material
				I::StudioRender->ForcedMaterialOverride(pMaterial);

				// draw model with xqz material
				oDrawModel(I::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
			}

			/* visible chams */
			pMaterial->IncrementReferenceCount();

			// disable "$ignorez" flag
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

			// set wireframe
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, C::Get<int>(Vars.iEspChamsPlayers) == (int)EVisualsPlayersChams::WIREFRAME ? true : false);

			// set color
			I::StudioRender->SetColorModulation(colVisible.Base());

			// set alpha
			I::StudioRender->SetAlphaModulation((pEntity == pLocal && pLocal->IsScoped() && I::Input->bCameraInThirdPerson) ? 0.3f : colVisible.aBase());

			// override cuztomized material
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

		if (C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::NO_DRAW)
		{
			pViewModelMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			I::StudioRender->ForcedMaterialOverride(pViewModelMaterial);

			// then draw original model with our flags

			// we need to clear override
			return true;
		}

		static IMaterial* pMaterial = nullptr;

		// set viewmodel material
		switch (C::Get<int>(Vars.iEspChamsViewModel))
		{
		case (int)EVisualsViewModelChams::FLAT:
			pMaterial = arrMaterials.at(1).second;
			break;
		case (int)EVisualsViewModelChams::GLOW:
			pMaterial = arrMaterials.at(2).second;
			break;
		case (int)EVisualsViewModelChams::SCROLL:
			pMaterial = arrMaterials.at(3).first;
			break;
		case (int)EVisualsViewModelChams::CHROME:
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
		Color colAdditional = C::Get<Color>(Vars.colEspChamsViewModelAdditional);
		Color colViewModel = C::Get<Color>(Vars.colEspChamsViewModel);

		pMaterial->IncrementReferenceCount();

		// get material variables
		static bool bBaseTextureFound = false;
		IMaterialVar* pBaseTexture = pMaterial->FindVar(XorStr("$basetexture"), &bBaseTextureFound);

		// do not override base texture for glow
		if (C::Get<int>(Vars.iEspChamsPlayers) == (int)EVisualsViewModelChams::GLOW && bBaseTextureFound)
			pBaseTexture->SetString("");

		static bool bEnvMapFresnelFound = false;
		IMaterialVar* pEnvMapFresnel = pMaterial->FindVar(XorStr("$envmapfresnel"), &bEnvMapFresnelFound);

		// add fresnel effect for glow
		if (C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::GLOW && bEnvMapFresnelFound)
			pEnvMapFresnel->SetInt(1);

		static bool bTranslucentFound = false;
		IMaterialVar* pTranslucent = pMaterial->FindVar(XorStr("$translucent"), &bTranslucentFound);

		// add the materials colour values to the existing image
		if (C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::GLOW && bTranslucentFound)
			pTranslucent->SetInt(1);

		static bool bFoundEnvMapTint = false;
		IMaterialVar* pEnvMapTint = pMaterial->FindVar(XorStr("$envmaptint"), &bFoundEnvMapTint);

		// set color
		if (C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::GLOW && bFoundEnvMapTint)
			pEnvMapTint->SetVector(colAdditional.rBase(), colAdditional.gBase(), colAdditional.bBase());

		// set wireframe
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, C::Get<int>(Vars.iEspChamsViewModel) == (int)EVisualsViewModelChams::WIREFRAME ? true : false);

		// set "$ignorez" flag to 0 and disable it
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

		I::StudioRender->SetColorModulation(colViewModel.Base());

		// set alpha
		I::StudioRender->SetAlphaModulation(colViewModel.aBase());

		// override cuztomized material
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
					pLocal->IsVisible(pEntity, pEntity->GetBonePosition(BONE_HEAD)) ? C::Get<Color>(Vars.colEspGlowEnemies) : C::Get<Color>(Vars.colEspGlowEnemiesWall) :
					pLocal->IsVisible(pEntity, pEntity->GetBonePosition(BONE_HEAD)) ? C::Get<Color>(Vars.colEspGlowAllies) : C::Get<Color>(Vars.colEspGlowAlliesWall));
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

Vector* CVisuals::GetPoints(CBaseEntity* pEntity)
{
	const ICollideable* pCollideable = pEntity->GetCollideable();

	if (pCollideable == nullptr)
		return nullptr;

	// get mins/maxs
	const Vector vecMin = pCollideable->OBBMins();
	const Vector vecMax = pCollideable->OBBMaxs();

	/*
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
	Vector vecPoints[] =
	{
		{ vecMin.x, vecMin.y, vecMin.z },
		{ vecMin.x, vecMax.y, vecMin.z },
		{ vecMax.x, vecMax.y, vecMin.z },
		{ vecMax.x, vecMin.y, vecMin.z },
		{ vecMax.x, vecMax.y, vecMax.z },
		{ vecMin.x, vecMax.y, vecMax.z },
		{ vecMin.x, vecMin.y, vecMax.z },
		{ vecMax.x, vecMin.y, vecMax.z }
	};

	return vecPoints;
}

bool CVisuals::GetBoundingBox(CBaseEntity* pEntity, Box_t& box)
{
	// get basic points
	Vector* vecPoints = GetPoints(pEntity);

	if (vecPoints == nullptr)
		return false;

	// get transformation matrix
	const matrix3x4_t& matTransformed = pEntity->GetCoordinateFrame();

	// get screen points position
	Vector2D vecScreen[8] = { };
	for (int i = 0; i < 8; i++)
	{
		if (!D::WorldToScreen(M::VectorTransform(vecPoints[i], matTransformed), vecScreen[i]))
			return false;
	}

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
	float left = vecScreen[3].x;
	float top = vecScreen[3].y;
	float right = vecScreen[3].x;
	float bottom = vecScreen[3].y;

	for (int i = 0; i < 8; i++)
	{
		left = std::min<float>(left, vecScreen[i].x);
		top = std::min<float>(top, vecScreen[i].y);
		right = std::max<float>(right, vecScreen[i].x);
		bottom = std::max<float>(bottom, vecScreen[i].y);
	}

	// set calculated box
	box.left = left;
	box.top = top;
	box.right = right;
	box.bottom = bottom;
	box.width = right - left;
	box.height = bottom - top;
	return true;
}

IMaterial* CVisuals::CreateMaterial(std::string_view szName, std::string_view szShader, std::string_view szBaseTexture, std::string_view szEnvMap, bool bIgnorez, bool bWireframe, std::string_view szProxies)
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

	std::string szMaterial = fmt::format(XorStr(R"#("{shader}"
	{{
		"$basetexture"	"{texture}"
		"$envmap"		"{envmap}"
		"$model"		"1"
		"$ignorez"		"{ignorez}"
		"$selfillum"	"1"
		"$halflambert"	"1"
		"$wireframe"	"{wireframe}"
		"$nofog"		"1"
		"proxies"
		{{
			{proxies}
		}}
	}})#"), fmt::arg(XorStr("shader"), szShader), fmt::arg(XorStr("texture"), szBaseTexture), fmt::arg(XorStr("envmap"), szEnvMap), fmt::arg(XorStr("ignorez"), bIgnorez ? 1 : 0), fmt::arg(XorStr("wireframe"), bWireframe ? 1 : 0), fmt::arg(XorStr("proxies"), szProxies));

	// load to memory
	CKeyValues* pKeyValues = (CKeyValues*)CKeyValues::operator new(sizeof(CKeyValues));
	pKeyValues->Init(szShader.data());
	pKeyValues->LoadFromBuffer(szName.data(), szMaterial.c_str());

	// create from buffer
	return I::MaterialSystem->CreateMaterial(szName.data(), pKeyValues);
}

void CVisuals::HitMarker(ImDrawList* pDrawList, float flServerTime, const ImVec2 vecScreenSize, Color colLines, Color colDamage)
{
	float flAlpha = 0.f;

	for (std::size_t i = 0U; i < vecHitMarks.size(); i++)
	{
		float flDelta = vecHitMarks.at(i).flTime - flServerTime;

		if (flDelta < 0.f)
		{
			vecHitMarks.erase(vecHitMarks.cbegin() + i);
			continue;
		}

		if (!C::Get<bool>(Vars.bScreenHitMarkerDamage))
			continue;

		// max distance for floating damage
		constexpr int iDistance = 40;

		float flRatio = 1.f - (flDelta / C::Get<float>(Vars.flScreenHitMarkerTime));
		flAlpha = flDelta / C::Get<float>(Vars.flScreenHitMarkerTime);

		Vector2D vecScreen = { };
		if (D::WorldToScreen(vecHitMarks.at(i).vecPosition, vecScreen))
		{
			// set fade out alpha
			colDamage.arrColor.at(3) = std::min<float>(colDamage.aBase(), flAlpha) * 255.f;
			// draw dealt damage
			ImGui::AddText(pDrawList, F::SmallestPixel, 20.f, ImVec2(vecScreen.x, vecScreen.y - flRatio * iDistance), std::to_string(vecHitMarks.at(i).iDamage).c_str(), colDamage.GetU32(), true);
		}
	}

	if (vecHitMarks.size() > 0U)
	{
		constexpr int arrSides[4][2] = { { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 } };
		for (auto iSide : arrSides)
		{
			// set fade out alpha
			colLines.arrColor.at(3) = std::min<float>(colLines.aBase(), flAlpha) * 255.f;
			// draw mark cross
			pDrawList->AddLine(ImVec2(vecScreenSize.x * 0.5f + C::Get<int>(Vars.iScreenHitMarkerGap) * iSide[0], vecScreenSize.y * 0.5f + C::Get<int>(Vars.iScreenHitMarkerGap) * iSide[1]), ImVec2(vecScreenSize.x * 0.5f + C::Get<int>(Vars.iScreenHitMarkerLenght) * iSide[0], vecScreenSize.y * 0.5f + C::Get<int>(Vars.iScreenHitMarkerLenght) * iSide[1]), colLines.GetU32());
		}
	}
}

void CVisuals::NightMode(CEnvTonemapController* pController)
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

void CVisuals::Bomb(ImDrawList* pDrawList, Vector2D vecScreen, Context_t& ctx, Color colFrame)
{
	const char* szIcon = U::GetWeaponIcon(WEAPON_C4);
	const ImVec2 vecIconSize = F::Icons->CalcTextSizeA(14.f, FLT_MAX, 0.f, szIcon);

	const char* szName = XorStr("C4");
	const ImVec2 vecNameSize = F::Verdana->CalcTextSizeA(14.f, FLT_MAX, 0.f, szName);

	static ImVec2 vecSize = ImVec2(vecIconSize.x + vecNameSize.x + 10.f, vecNameSize.y + 6.f);

	// set custom box
	ctx.box = { vecScreen.x - vecSize.x * 0.5f, vecScreen.y - vecSize.y * 0.5f, vecScreen.x + vecSize.x * 0.5f, vecScreen.y + vecSize.y * 0.5f, vecSize.x, vecSize.y };

	// frame
	pDrawList->AddRectFilled(ImVec2(ctx.box.left - 1, ctx.box.top), ImVec2(ctx.box.right + 1, ctx.box.bottom), colFrame.GetU32(), 5.0f);
	// icon
	ImGui::AddText(pDrawList, F::Icons, 14.f, ImVec2(ctx.box.left + 5, ctx.box.top + 3), szIcon, Color(255, 255, 255).GetU32());
	// text
	ImGui::AddText(pDrawList, F::Verdana, 14.f, ImVec2(ctx.box.left + vecIconSize.x + 7, ctx.box.top + 3), szName, Color(255, 255, 255).GetU32());
}

void CVisuals::PlantedBomb(ImDrawList* pDrawList, CPlantedC4* pBomb, float flServerTime, Vector2D vecScreen, Context_t& ctx, Color colFrame, Color colDefuse, Color colFailDefuse, Color colBackground, Color colOutline)
{
	const char* szIcon = U::GetWeaponIcon(WEAPON_C4);
	static ImVec2 vecIconSize = F::Icons->CalcTextSizeA(14.f, FLT_MAX, 0.f, szIcon);

	const char* szName = XorStr("PLANTED C4");
	static ImVec2 vecNameSize = F::Verdana->CalcTextSizeA(14.f, FLT_MAX, 0.f, szName);

	static ImVec2 vecSize = ImVec2(vecIconSize.x + vecNameSize.x + 10.f, vecNameSize.y + 6.f);

	// set custom box
	ctx.box = { vecScreen.x - vecSize.x * 0.5f, vecScreen.y - vecSize.y * 0.5f, vecScreen.x + vecSize.x * 0.5f, vecScreen.y + vecSize.y * 0.5f, vecSize.x, vecSize.y };

	// get defuser entity
	const CBaseEntity* pDefuser = I::ClientEntityList->Get<CBaseEntity>(pBomb->GetDefuser());

	/* info */
	// frame
	pDrawList->AddRectFilled(ImVec2(ctx.box.left - 1, ctx.box.top), ImVec2(ctx.box.right + 1, ctx.box.bottom), colFrame.GetU32(), 5.0f);
	// icon
	ImGui::AddText(pDrawList, F::Icons, 14.f, ImVec2(ctx.box.left + 5, ctx.box.top + 3), szIcon, pDefuser != nullptr ? colDefuse.GetU32() : Color(255, 255, 255).GetU32());
	// text
	ImGui::AddText(pDrawList, F::Verdana, 14.f, ImVec2(ctx.box.left + vecIconSize.x + 10, ctx.box.top + 3), szName, Color(255, 255, 255).GetU32());

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
	pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.right, ctx.box.bottom + 4), colBackground.GetU32());
	// bar
	pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.bottom + 4), Color::FromHSB(flHue, 1.f, 1.f).GetU32());
	// outline
	pDrawList->AddRect(ImVec2(ctx.box.left - 1, ctx.box.bottom + 1), ImVec2(ctx.box.right + 1, ctx.box.bottom + 5), colOutline.GetU32());
	ctx.arrPadding.at(DIR_BOTTOM) += 5;

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
		pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.right, ctx.box.bottom + 4 + ctx.arrPadding.at(DIR_BOTTOM)), colBackground.GetU32());
		// bar
		pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.left + ctx.box.width * flDefuseFactor, ctx.box.bottom + 4 + ctx.arrPadding.at(DIR_BOTTOM)), (flDefuseTime < flCurrentTime) ? colDefuse.GetU32() : colFailDefuse.GetU32());
		// outline
		pDrawList->AddRect(ImVec2(ctx.box.left - 1, ctx.box.bottom + 1 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.right + 1, ctx.box.bottom + 5 + ctx.arrPadding.at(DIR_BOTTOM)), colOutline.GetU32());
	}
}

void CVisuals::Grenade(ImDrawList* pDrawList, EClassIndex nIndex, CBaseEntity* pGrenade, float flServerTime, Vector2D vecScreen, Context_t& ctx, Color colFrame, Color colBackground, Color colOutline)
{
	// setup temporary values
	const char* szName = XorStr("NONE");
	float flFactor = 0.f;
	Color colGrenade = Color(255, 255, 255);

	// get grenade model name
	if (std::string_view szModelName = I::ModelInfo->GetModelName(pGrenade->GetModel()); !szModelName.empty())
	{
		// get grenade info
		switch (nIndex)
		{
		case EClassIndex::CBaseCSGrenadeProjectile:
		{
			// separate greandes by model name
			if (szModelName.find(XorStr("fraggrenade")) != std::string::npos)
				szName = XorStr("HIGH-EXPLOSIVE");
			else if (szModelName.find(XorStr("flashbang")) != std::string::npos)
				szName = XorStr("FLASH");
			break;
		}
		case EClassIndex::CDecoyProjectile:
			szName = XorStr("DECOY");
			break;
		case EClassIndex::CSmokeGrenadeProjectile:
		{
			// cast to smoke grenade
			CSmokeGrenade* pSmoke = (CSmokeGrenade*)pGrenade;
			flFactor = ((TICKS_TO_TIME(pSmoke->GetEffectTickBegin()) + pSmoke->GetMaxTime()) - flServerTime) / pSmoke->GetMaxTime();
			colGrenade = Color(230, 130, 0);
			szName = XorStr("SMOKE");
			break;
		}
		case EClassIndex::CMolotovProjectile:
		case EClassIndex::CInferno:
		{
			// cast to inferno grenade
			CInferno* pInferno = (CInferno*)pGrenade;
			flFactor = ((TICKS_TO_TIME(pInferno->GetEffectTickBegin()) + pInferno->GetMaxTime()) - flServerTime) / pInferno->GetMaxTime();
			colGrenade = Color(255, 100, 100);

			// separate greandes by model name
			if (szModelName.find(XorStr("molotov")) != std::string::npos)
				szName = XorStr("MOLOTOV");
			else if (szModelName.find(XorStr("incendiary")) != std::string::npos)
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
	const ImVec2 vecSize = ImVec2(vecNameSize.x + 10.f, vecNameSize.y + 6.f);

	// set custom box
	ctx.box = { vecScreen.x - vecSize.x * 0.5f, vecScreen.y - vecSize.y * 0.5f, vecScreen.x + vecSize.x * 0.5f, vecScreen.y + vecSize.y * 0.5f, vecSize.x, vecSize.y };

	/* info */
	// frame
	pDrawList->AddRectFilled(ImVec2(ctx.box.left - 1, ctx.box.top), ImVec2(ctx.box.right + 1, ctx.box.bottom), colFrame.GetU32(), 5.f);
	// text
	ImGui::AddText(pDrawList, F::Verdana, 14.f, ImVec2(ctx.box.left + 5, ctx.box.top + 3), szName, Color(255, 255, 255).GetU32());

	if (flFactor > 0.f)
	{
		/* bar */
		// background
		pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.right, ctx.box.bottom + 4), colBackground.GetU32());
		// bar
		pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 2), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.bottom + 4), colGrenade.GetU32());
		// outline
		pDrawList->AddRect(ImVec2(ctx.box.left - 1, ctx.box.bottom + 1), ImVec2(ctx.box.right + 1, ctx.box.bottom + 5), colOutline.GetU32());
	}
}

void CVisuals::DroppedWeapons(ImDrawList* pDrawList, CBaseCombatWeapon* pWeapon, short nItemDefinitionIndex, Context_t& ctx, Color colPrimary, Color colAmmo, Color colBackground, Color colOutline)
{
	// @note: for text weapon names
	//const char* szHudName = pWeaponData->szHudName;
	//wchar_t* wszLocalizeName = I::Localize->Find(szHudName);

	// get dropped weapon icon
	const char* szIcon = U::GetWeaponIcon(nItemDefinitionIndex);

	// draw weapon icon
	const ImVec2 vecNameSize = F::Icons->CalcTextSizeA(10.f, FLT_MAX, 0.f, szIcon);
	ImGui::AddText(pDrawList, F::Icons, 10.f, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecNameSize.x * 0.5f, ctx.box.bottom), szIcon, colPrimary.GetU32(), true, colOutline.GetU32());
	ctx.arrPadding.at(DIR_BOTTOM) += vecNameSize.y;

	// ammo bar
	AmmoBar(pDrawList, nullptr, pWeapon, ctx, colAmmo, colBackground, colOutline);

	// @todo: add ammo count and distance
}

void CVisuals::Player(ImDrawList* pDrawList, CBaseEntity* pLocal, CBaseEntity* pEntity, Context_t& ctx, Color colInfo, Color colFrame, Color colOutline)
{
	PlayerInfo_t pInfo;
	if (!I::Engine->GetPlayerInfo(pEntity->GetIndex(), &pInfo))
		return;

	// @todo: add snaplines with distance scaling and automatic joining to the bounding box

	if (C::Get<int>(Vars.iEspMainBox) > (int)EVisualsBoxType::NONE)
	{
		// get box color based on visibility & enmity
		Color colBox = pEntity->IsEnemy(pLocal) ?
			pLocal->IsVisible(pEntity, pEntity->GetEyePosition()) ? C::Get<Color>(Vars.colEspMainBoxEnemies) : C::Get<Color>(Vars.colEspMainBoxEnemiesWall) :
			pLocal->IsVisible(pEntity, pEntity->GetEyePosition()) ? C::Get<Color>(Vars.colEspMainBoxAllies) : C::Get<Color>(Vars.colEspMainBoxAlliesWall);

		Box(pDrawList, ctx.box, colBox, Color(0, 0, 0, 150));
	}

	// info's master check
	if (!C::Get<bool>(Vars.bEspMainInfo))
		return;

	// @note: distance font scale
	const float flDistance = std::fabsf((pEntity->GetRenderOrigin() - G::vecCamera).Length2D());
	const float flFontSize = std::clamp<float>(70.f / (flDistance / 70.f), 10.f, 40.f);

	#pragma region visuals_player_top
	if (C::Get<bool>(Vars.bEspMainInfoFlash) && pEntity->GetFlashDuration() > 0.2f)
		FlashBar(pDrawList, pEntity, ctx, Color(255, 255, 255, 220), Color(40, 40, 40, 100), Color(0, 0, 0, 150));

	if (C::Get<bool>(Vars.bEspMainInfoRank) && !pInfo.bFakePlayer)
	{
		// @todo: make with images from vpk
	}

	if (C::Get<bool>(Vars.bEspMainInfoName))
	{
		// get player name
		std::string szName = pInfo.szName;

		// truncate name
		if (szName.length() > 24U)
			szName = szName.substr(0U, 24U).append(XorStr("..."));

		static const char* szBot = XorStr("[BOT]");
		ImVec2 vecBotSize{ };
		const ImVec2 vecNameSize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szName.c_str());

		// add prefix for bots
		if (pInfo.bFakePlayer)
		{
			vecBotSize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szBot);
			ImGui::AddText(pDrawList, F::SmallestPixel, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f + 1 + vecNameSize.x * 0.5f - vecBotSize.x * 0.5f, ctx.box.top - 2 - vecBotSize.y - ctx.arrPadding.at(DIR_TOP)), szBot, Color(140, 140, 140).GetU32(), true, colOutline.GetU32());
		}

		ImGui::AddText(pDrawList, F::SmallestPixel, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecNameSize.x * 0.5f - vecBotSize.x * 0.5f, ctx.box.top - 2 - vecNameSize.y - ctx.arrPadding.at(DIR_TOP)), szName.c_str(), colInfo.GetU32(), true, colOutline.GetU32());
		ctx.arrPadding.at(DIR_TOP) += vecNameSize.y;
	}
	#pragma endregion

	#pragma region visuals_player_bottom
	// get active weapon
	if (CBaseCombatWeapon* pActiveWeapon = pEntity->GetWeapon(); pActiveWeapon != nullptr)
	{
		// ammo bar
		if (C::Get<bool>(Vars.bEspMainInfoAmmo))
			AmmoBar(pDrawList, pEntity, pActiveWeapon, ctx, Color(80, 180, 200), Color(40, 40, 40, 100), Color(0, 0, 0, 150));

		// get all other weapons
		if (C::Get<bool>(Vars.bEspMainInfoWeapons))
		{
			if (const auto hWeapons = pEntity->GetWeapons(); hWeapons != nullptr)
			{
				// -1 to prevent double active weapon
				for (int nIndex = MAX_WEAPONS - 1; hWeapons[nIndex]; nIndex--)
				{
					// get current weapon
					CBaseCombatWeapon* pCurrentWeapon = I::ClientEntityList->Get<CBaseCombatWeapon>(hWeapons[nIndex]);

					if (pCurrentWeapon == nullptr)
						continue;

					const short nDefinitionIndex = *pCurrentWeapon->GetItemDefinitionIndex();

					// do not draw some useless dangerzone weapons (lmao i just dont add icons)
					if (nDefinitionIndex == WEAPON_SHIELD || nDefinitionIndex == WEAPON_BREACHCHARGE || nDefinitionIndex == WEAPON_BUMPMINE)
						continue;

					CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

					// pass only active grenades/fists/tablet // @todo: or make it on right/left bottom side
					if (pWeaponData == nullptr || ((pWeaponData->nWeaponType == WEAPONTYPE_GRENADE || nDefinitionIndex == WEAPON_FISTS || nDefinitionIndex == WEAPON_TABLET) && pCurrentWeapon != pActiveWeapon))
						continue;

					// draw weapons list
					const char* szIcon = U::GetWeaponIcon(nDefinitionIndex);
					const ImVec2 vecIconSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szIcon);
					ImGui::AddText(pDrawList, F::Icons, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecIconSize.x * 0.5f, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), szIcon, pCurrentWeapon == pActiveWeapon ? colInfo.GetU32() : Color(160, 160, 160).GetU32(), true, colOutline.GetU32());
					ctx.arrPadding.at(DIR_BOTTOM) += vecIconSize.y;
				}
			}
		}
	}

	if (C::Get<bool>(Vars.bEspMainInfoDistance))
	{
		const int iDistance = M_UNIT2METRE(flDistance);
		std::string szDistance = std::to_string(iDistance).append(XorStr("M"));
		const ImVec2 vecDistanceSize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szDistance.c_str());
		ImGui::AddText(pDrawList, F::SmallestPixel, flFontSize, ImVec2(ctx.box.left + ctx.box.width * 0.5f - vecDistanceSize.x * 0.5f, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), szDistance.c_str(), colInfo.GetU32(), true, colOutline.GetU32());
		ctx.arrPadding.at(DIR_BOTTOM) += vecDistanceSize.y;
	}
	#pragma endregion

	#pragma region visuals_player_left
	if (C::Get<bool>(Vars.bEspMainInfoHealth))
	{
		// calculate hp-based color
		const float flFactor = (float)pEntity->GetHealth() / (float)pEntity->GetMaxHealth();
		const float flHue = (flFactor * 120.f) / 360.f;
		HealthBar(pDrawList, flFactor, ctx, Color::FromHSB(flHue, 1.f, 1.f), Color(40, 40, 40, 100), Color(0, 0, 0, 150));
	}

	if (C::Get<bool>(Vars.bEspMainInfoMoney))
	{
		std::string szMoney = std::to_string(pEntity->GetMoney()).insert(0U, XorStr("$"));
		const ImVec2 vecMoneySize = F::SmallestPixel->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szMoney.c_str());
		ImGui::AddText(pDrawList, F::SmallestPixel, flFontSize, ImVec2(ctx.box.left - 2 - vecMoneySize.x - ctx.arrPadding.at(DIR_LEFT), ctx.box.top), szMoney.c_str(), Color(140, 195, 75).GetU32(), true, colOutline.GetU32());
		ctx.arrPadding.at(DIR_LEFT) += vecMoneySize.y;
	}
	#pragma endregion

	#pragma region visuals_player_right
	if (C::Get<std::vector<bool>>(Vars.vecEspMainInfoFlags).at(INFO_FLAG_HELMET) && pEntity->HasHelmet())
	{
		static const char* szHelmetIcon = u8"\uE20E";
		const ImVec2 vecHelmetSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szHelmetIcon);
		ImGui::AddText(pDrawList, F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szHelmetIcon, colInfo.GetU32(), true, colOutline.GetU32());
		ctx.arrPadding.at(DIR_RIGHT) += vecHelmetSize.y;
	}

	if (C::Get<std::vector<bool>>(Vars.vecEspMainInfoFlags).at(INFO_FLAG_KEVLAR) && pEntity->GetArmor() > 0)
	{
		static const char* szKevlarIcon = u8"\uE210";
		const ImVec2 vecKevlarSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szKevlarIcon);
		ImGui::AddText(pDrawList, F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szKevlarIcon, colInfo.GetU32(), true, colOutline.GetU32());
		ctx.arrPadding.at(DIR_RIGHT) += vecKevlarSize.y;
	}

	if (C::Get<std::vector<bool>>(Vars.vecEspMainInfoFlags).at(INFO_FLAG_KIT) && pEntity->HasDefuser())
	{
		static const char* szKitIcon = u8"\uE20F";
		const ImVec2 vecKitSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szKitIcon);
		ImGui::AddText(pDrawList, F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szKitIcon, pEntity->IsDefusing() ? Color(80, 180, 200).GetU32() : colInfo.GetU32(), true, colOutline.GetU32());
		ctx.arrPadding.at(DIR_RIGHT) += vecKitSize.y;
	}

	if (C::Get<std::vector<bool>>(Vars.vecEspMainInfoFlags).at(INFO_FLAG_ZOOM) && pEntity->IsScoped())
	{
		static const char* szTargetIcon = u8"\uE212";
		const ImVec2 vecZoomSize = F::Icons->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szTargetIcon);
		ImGui::AddText(pDrawList, F::Icons, flFontSize, ImVec2(ctx.box.right + 2, ctx.box.top + ctx.arrPadding.at(DIR_RIGHT)), szTargetIcon, colInfo.GetU32(), true, colOutline.GetU32());
		ctx.arrPadding.at(DIR_RIGHT) += vecZoomSize.y;
	}
	#pragma endregion
}

void CVisuals::Box(ImDrawList* pDrawList, const Box_t& box, Color colPrimary, Color colOutline)
{
	if (C::Get<int>(Vars.iEspMainBox) == (int)EVisualsBoxType::FULL)
	{
		/* box */
		pDrawList->AddRect(ImVec2(box.left, box.top), ImVec2(box.right, box.bottom), colPrimary.GetU32());
		/* outline */
		// outer
		pDrawList->AddRect(ImVec2(box.left - 1, box.top - 1), ImVec2(box.right + 1, box.bottom + 1), colOutline.GetU32());
		// inner
		pDrawList->AddRect(ImVec2(box.left + 1, box.top + 1), ImVec2(box.right - 1, box.bottom - 1), colOutline.GetU32());
	}
	else if (C::Get<int>(Vars.iEspMainBox) == (int)EVisualsBoxType::CORNERS)
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

		for (const auto arrPoint : arrPoints)
		{
			// outline
			// not a best way of doing that but one-lined :(
			//pDrawList->AddRect(point.first, point.second, colOutline.GetU32(), 0.0f, 15, 1.4f);
			// box
			pDrawList->AddLine(arrPoint.first, arrPoint.second, colPrimary.GetU32());
		}
	}
}

void CVisuals::HealthBar(ImDrawList* pDrawList, float flFactor, Context_t& ctx, Color colPrimary, Color colBackground, Color colOutline)
{
	// background
	pDrawList->AddRectFilled(ImVec2(ctx.box.left - 5 - ctx.arrPadding.at(DIR_LEFT), ctx.box.top), ImVec2(ctx.box.left - 3 - ctx.arrPadding.at(DIR_LEFT), ctx.box.bottom), colBackground.GetU32());
	// bar
	pDrawList->AddRectFilled(ImVec2(ctx.box.left - 5 - ctx.arrPadding.at(DIR_LEFT), ctx.box.bottom - (ctx.box.height * flFactor)), ImVec2(ctx.box.left - 3 - ctx.arrPadding.at(DIR_LEFT), ctx.box.bottom), colPrimary.GetU32());
	// outline
	pDrawList->AddRect(ImVec2(ctx.box.left - 6 - ctx.arrPadding.at(DIR_LEFT), ctx.box.top - 1), ImVec2(ctx.box.left - 2 - ctx.arrPadding.at(DIR_LEFT), ctx.box.bottom + 1), colOutline.GetU32());
	ctx.arrPadding.at(DIR_LEFT) += 6;
}

void CVisuals::AmmoBar(ImDrawList* pDrawList, CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, Context_t& ctx, Color colPrimary, Color colBackground, Color colOutline)
{
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(*pWeapon->GetItemDefinitionIndex());

	if (pWeaponData == nullptr)
		return;

	// shown only for guns
	if (!pWeaponData->IsGun())
		return;

	// get ammo count
	const int iAmmo = pWeapon->GetAmmo();
	const int iMaxAmmo = pWeaponData->iMaxClip1;

	// add modifiable width factor
	float flFactor = 0.f;

	CAnimationLayer* pLayer = nullptr;
	int nActivity = 0;

	// get entity animation activity
	if (pEntity != nullptr)
	{
		pLayer = pEntity->GetAnimationLayer(1);
		nActivity = pEntity->GetSequenceActivity(pLayer->nSequence);
	}

	// calculate ammo-based width factor
		// check for reloading animation
	if (pEntity != nullptr && pLayer != nullptr && nActivity == ACT_CSGO_RELOAD && pLayer->flWeight != 0.0f)
		// reloading width
		flFactor = pLayer->flCycle;
	else
		// if not reloading
		flFactor = (float)iAmmo / (float)iMaxAmmo;

	// background
	pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 3 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.right, ctx.box.bottom + 5 + ctx.arrPadding.at(DIR_BOTTOM)), colBackground.GetU32());
	// bar
	pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.bottom + 3 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.bottom + 5 + ctx.arrPadding.at(DIR_BOTTOM)), colPrimary.GetU32());
	// outline
	pDrawList->AddRect(ImVec2(ctx.box.left - 1, ctx.box.bottom + 2 + ctx.arrPadding.at(DIR_BOTTOM)), ImVec2(ctx.box.right + 1, ctx.box.bottom + 6 + ctx.arrPadding.at(DIR_BOTTOM)), colOutline.GetU32());
	ctx.arrPadding.at(DIR_BOTTOM) += 6;
}

void CVisuals::FlashBar(ImDrawList* pDrawList, CBaseEntity* pEntity, Context_t& ctx, Color colPrimary, Color colBackground, Color colOutline)
{
	// calculate flash alpha-based width factor
	float flFactor = pEntity->GetFlashAlpha() / *pEntity->GetFlashMaxAlpha();

	// background
	pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.top - 3 - ctx.arrPadding.at(DIR_TOP)), ImVec2(ctx.box.right, ctx.box.top - 5 - ctx.arrPadding.at(DIR_TOP)), colBackground.GetU32());
	// bar
	pDrawList->AddRectFilled(ImVec2(ctx.box.left, ctx.box.top - 3 - ctx.arrPadding.at(DIR_TOP)), ImVec2(ctx.box.left + ctx.box.width * flFactor, ctx.box.top - 5 - ctx.arrPadding.at(DIR_TOP)), colPrimary.GetU32());
	// outline
	pDrawList->AddRect(ImVec2(ctx.box.left - 1, ctx.box.top - 3 - ctx.arrPadding.at(DIR_TOP)), ImVec2(ctx.box.right + 1, ctx.box.top - 5 - ctx.arrPadding.at(DIR_TOP)), colOutline.GetU32()); // hmm, why offset here like for bar or bg?
	ctx.arrPadding.at(DIR_TOP) += 6;
}
