#include "world.h"

// used: cheat variables
#include "../../core/variables.h"
// used: getbindstate
#include "../../utilities/inputsystem.h"
// used: render functions
#include "../../utilities/draw.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../../sdk/interfaces/ibaseclientdll.h"
#include "../../sdk/interfaces/icliententitylist.h"
#include "../../sdk/interfaces/iengineclient.h"
#include "../../sdk/interfaces/iglobalvars.h"
#include "../../sdk/interfaces/iinput.h"
#include "../../sdk/interfaces/iweaponsystem.h"

using namespace F::VISUAL;

#pragma region visual_world_callbacks
void WORLD::OnDraw(CCSPlayer* pLocal)
{
	const int nHighestEntityIndex = I::ClientEntityList->GetHighestEntityIndex();
	for (int i = I::Globals->nMaxClients; i < nHighestEntityIndex; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr)
			continue;

		if (const CClientClass* pClientClass = pEntity->GetClientClass(); pClientClass != nullptr && pClientClass->nClassID == EClassIndex::CEnvTonemapController)
			NightMode(reinterpret_cast<CEnvTonemapController*>(pEntity));
	}

	if (!C::Get<bool>(Vars.bVisualWorld))
		return;

	// render scope lines
	if (C::Get<unsigned int>(Vars.nVisualWorldRemovals) & VISUAL_WORLD_REMOVAL_FLAG_SCOPE)
	{
		if (const auto pWeaponCSBase = static_cast<CWeaponCSBase*>(pLocal->GetActiveWeapon()); pWeaponCSBase != nullptr)
		{
			// check is weapon sniper and player is scoped
			if (const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeaponCSBase->GetEconItemView()->GetItemDefinitionIndex()); pWeaponData != nullptr && pWeaponData->nWeaponType == WEAPONTYPE_SNIPER && pLocal->IsScoped())
			{
				const ImVec2 vecScreenSize = ImGui::GetIO().DisplaySize;

				const float flInaccuracy = pWeaponCSBase->GetInaccuracy();
				const float flWidth = flInaccuracy * (vecScreenSize.y / 5.0f);
				const int iAlpha = CRT::Min(255, static_cast<int>(flInaccuracy * 255.f));

				D::AddDrawListLine(nullptr, ImVec2(0.0f, vecScreenSize.y * 0.5f), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f), Color_t(0, 0, 0, 200));
				D::AddDrawListLine(nullptr, ImVec2(vecScreenSize.x * 0.5f, 0.0f), ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y), Color_t(0, 0, 0, 200));

				// horizontal
				D::AddDrawListRectMultiColor(nullptr, ImVec2(0.0f, vecScreenSize.y * 0.5f), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f + flWidth), Color_t(0, 0, 0, iAlpha), Color_t(0, 0, 0, iAlpha), Color_t(0, 0, 0, 0), Color_t(0, 0, 0, 0));
				D::AddDrawListRectMultiColor(nullptr, ImVec2(0.0f, vecScreenSize.y * 0.5f - flWidth), ImVec2(vecScreenSize.x, vecScreenSize.y * 0.5f), Color_t(0, 0, 0, 0), Color_t(0, 0, 0, 0), Color_t(0, 0, 0, iAlpha), Color_t(0, 0, 0, iAlpha));
				// vertical
				D::AddDrawListRectMultiColor(nullptr, ImVec2(vecScreenSize.x * 0.5f, 0.0f), ImVec2(vecScreenSize.x * 0.5f + flWidth, vecScreenSize.y), Color_t(0, 0, 0, iAlpha), Color_t(0, 0, 0, 0), Color_t(0, 0, 0, 0), Color_t(0, 0, 0, iAlpha));
				D::AddDrawListRectMultiColor(nullptr, ImVec2(vecScreenSize.x * 0.5f - flWidth, 0.0f), ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y), Color_t(0, 0, 0, 0), Color_t(0, 0, 0, iAlpha), Color_t(0, 0, 0, iAlpha), Color_t(0, 0, 0, 0));
			}
		}
	}
}
#pragma endregion

#pragma region visual_world_main
void WORLD::NightMode(CEnvTonemapController* pController)
{
	static bool bUpdateExposure = true;
	static bool bOldUseCustomAutoExposureMin = false;
	static bool bOldUseCustomAutoExposureMax = false;
	static float flOldCustomAutoExposureMin = 0.0f;
	static float flOldCustomAutoExposureMax = 0.0f;

	if (C::Get<bool>(Vars.bVisualWorld) && C::Get<bool>(Vars.bVisualWorldNightMode))
	{
		if (bUpdateExposure)
		{
			bOldUseCustomAutoExposureMin = pController->IsUsingCustomAutoExposureMin();
			bOldUseCustomAutoExposureMax = pController->IsUsingCustomAutoExposureMax();
			flOldCustomAutoExposureMin = pController->GetCustomAutoExposureMin();
			flOldCustomAutoExposureMax = pController->GetCustomAutoExposureMax();

			bUpdateExposure = false;
		}

		pController->IsUsingCustomAutoExposureMin() = true;
		pController->IsUsingCustomAutoExposureMax() = true;
		pController->GetCustomAutoExposureMin() = 0.2f;
		pController->GetCustomAutoExposureMax() = 0.2f;
	}
	else if (!bUpdateExposure)
	{
		pController->IsUsingCustomAutoExposureMin() = bOldUseCustomAutoExposureMin;
		pController->IsUsingCustomAutoExposureMax() = bOldUseCustomAutoExposureMax;
		pController->GetCustomAutoExposureMin() = flOldCustomAutoExposureMin;
		pController->GetCustomAutoExposureMax() = flOldCustomAutoExposureMax;

		bUpdateExposure = true;
	}
}

void WORLD::PostProcessingRemoval()
{
	static bool* bOverridePostProcessingDisable = *reinterpret_cast<bool**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("80 3D ? ? ? ? ? 53 56 57 0F 85")) + 0x2); // @xref: "mat_software_aa_strength", "dev/engine_post_splitscreen", "dev/engine_post"
	*bOverridePostProcessingDisable = (C::Get<bool>(Vars.bVisualWorld) && (C::Get<unsigned int>(Vars.nVisualWorldRemovals) & VISUAL_WORLD_REMOVAL_FLAG_POSTPROCESSING));
}

void WORLD::PunchRemoval(CCSPlayer* pPlayer, const bool bState, QAngle_t* pangOldViewPunch, QAngle_t* pangOldAimPunch)
{
	// @ida C_BasePlayer::CalcPlayerView(): client.dll -> "55 8B EC A1 ? ? ? ? 83 EC 0C 8B 40"

	CPlayerLocalData* pLocalData = pPlayer->GetLocalData();

	if (bState)
	{
		*pangOldViewPunch = pLocalData->GetViewPunch();
		*pangOldAimPunch = pLocalData->GetAimPunch();

		pLocalData->GetViewPunch() = { };
		pLocalData->GetAimPunch() = { };
	}
	else
	{
		pLocalData->GetViewPunch() = *pangOldViewPunch;
		pLocalData->GetAimPunch() = *pangOldAimPunch;
	}
}

void WORLD::FlashRemoval(const bool bState)
{
	// @test: does this works as expected in release mode when xor getting used?
	for (const char* szFlashBangMaterial :
		{
			Q_XOR("effects/flashbang_white"),
			Q_XOR("effects/flashbang")
		})
	{
		if (IMaterial* pMaterial = I::MaterialSystem->FindMaterial(szFlashBangMaterial, TEXTURE_GROUP_CLIENT_EFFECTS); pMaterial != nullptr && !pMaterial->IsErrorMaterial())
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, bState);
	}
}

void WORLD::SmokeRemoval(const bool bState)
{
	// @test: does this works as expected in release mode when xor getting used?
	for (const char* szSmokeMaterial :
		{
			Q_XOR("particle/vistasmokev1/vistasmokev1_fire"),
			Q_XOR("particle/vistasmokev1/vistasmokev1_smokegrenade"),
			Q_XOR("particle/vistasmokev1/vistasmokev1_emods"),
			Q_XOR("particle/vistasmokev1/vistasmokev1_emods_impactdust"),
		})
	{
		if (IMaterial* pMaterial = I::MaterialSystem->FindMaterial(szSmokeMaterial, TEXTURE_GROUP_OTHER); pMaterial != nullptr && !pMaterial->IsErrorMaterial())
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, bState);
	}
}

void WORLD::ThirdPerson(CCSPlayer* pLocal)
{
	// @todo: we doesn't need to disable it manually, 'CInput::CAM_Think()' will do this for us
	// @ida CInput::CAM_Think(): client.dll -> "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 8B F1 8B 0D ? ? ? ? 57 85 C9" @xref: "view angles", "ideal angles", "camera offset", "sv_cheats"

	if (C::Get<bool>(Vars.bVisualWorld) && IPT::GetBindState(C::Get<KeyBind_t>(Vars.keyVisualWorldThirdPerson)) && pLocal->IsAlive())
	{
		QAngle_t angThirdpersonView = { };
		I::Engine->GetViewAngles(angThirdpersonView);
		angThirdpersonView.z = C::Get<float>(Vars.flVisualWorldThirdPersonOffset);

		QAngle_t angInverseView = angThirdpersonView;
		angInverseView.x = -angInverseView.x;
		angInverseView.y += 180.f;

		Vector_t vecDirection = { };
		angInverseView.ToDirections(&vecDirection);

		constexpr Vector_t vecCameraHullMin = { -14.0f, -14.0f, -14.0f };
		constexpr Vector_t vecCameraHullMax = { 14.0f, 14.0f, 14.0f };

		const Vector_t vecStart = pLocal->GetEyePosition();
		const Vector_t vecEnd = vecStart + (vecDirection * C::Get<float>(Vars.flVisualWorldThirdPersonOffset));
		const Ray_t ray(vecStart, vecEnd, vecCameraHullMin, vecCameraHullMax);

		CTraceFilterSimple filter(pLocal);
		Trace_t trace = { };
		I::EngineTrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

		// clip distance
		if (trace.flFraction < 1.0f)
			angThirdpersonView.z *= trace.flFraction;

		I::Input->bCameraInThirdPerson = true;
		I::Input->vecCameraOffset = { angThirdpersonView.x, angThirdpersonView.y, angThirdpersonView.z };
	}
	else
		I::Input->bCameraInThirdPerson = false;
}
#pragma endregion
