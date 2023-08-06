#include "chams.h"

// used: drawmodel hook manager
#include "../../core/hooks.h"
// used: fake bone matrices
#include "../animationcorrection.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../../sdk/interfaces/icliententitylist.h"
#include "../../sdk/interfaces/ivmodelrender.h"

using namespace F::VISUAL;

bool CHAMS::Setup()
{
	/*
	 * @note: whenever you add game materials here, you should set that we're not a owner of it
	 * materials are arranged according to 'EVisualChams'
	 *
	 * materials info:
	 * use "mat_texture_list 1" command to see full materials list
	 1	shaders: https://developer.valvesoftware.com/wiki/Category:Shaders
	 -		parameters: https://developer.valvesoftware.com/wiki/Category:List_of_Shader_Parameters
	 2	proxies: https://developer.valvesoftware.com/wiki/Material_proxies
	 -		list: https://developer.valvesoftware.com/wiki/List_Of_Material_Proxies
	 */

	CKeyValues* pCoveredVMT = new CKeyValues(Q_XOR("VertexLitGeneric"));
	pCoveredVMT->SetString(Q_XOR("$ambientonly"), Q_XOR("1"));
	pCoveredVMT->SetString(Q_XOR("$basetexture"), Q_XOR("vgui/white"));

	arrCustomMaterials[VISUAL_CHAMS_COVERED - 1] =
	{
		.pMaterial = CreateMaterial(Q_XOR("qo0_covered.vmt"), pCoveredVMT),
		.bIsOwner = true
	};

	CKeyValues* pFlatVMT = new CKeyValues(Q_XOR("UnlitGeneric"));
	pFlatVMT->SetString(Q_XOR("$basetexture"), Q_XOR("vgui/white"));

	arrCustomMaterials[VISUAL_CHAMS_FLAT - 1] =
	{
		.pMaterial = CreateMaterial(Q_XOR("qo0_flat.vmt"), pFlatVMT),
		.bIsOwner = true
	};

	CKeyValues* pGlowVMT = new CKeyValues(Q_XOR("VertexLitGeneric"));
	pGlowVMT->SetString(Q_XOR("$additive"), Q_XOR("1"));
	pGlowVMT->SetString(Q_XOR("$basetexture"), Q_XOR("vgui/white"));
	pGlowVMT->SetString(Q_XOR("$envmap"), Q_XOR("models/effects/cube_white"));
	pGlowVMT->SetString(Q_XOR("$envmapfresnel"), Q_XOR("1.0"));

	arrCustomMaterials[VISUAL_CHAMS_GLOW - 1] =
	{
		.pMaterial = CreateMaterial(Q_XOR("qo0_glow.vmt"), pGlowVMT),
		.bIsOwner = true
	};

	CKeyValues* pReflectiveVMT = new CKeyValues(Q_XOR("VertexLitGeneric"));
	pReflectiveVMT->SetString(Q_XOR("$basetexture"), Q_XOR("vgui/white"));
	pReflectiveVMT->SetString(Q_XOR("$envmap"), Q_XOR("env_cubemap"));

	arrCustomMaterials[VISUAL_CHAMS_REFLECTIVE - 1] =
	{
		.pMaterial = CreateMaterial(Q_XOR("qo0_reflective.vmt"), pReflectiveVMT),
		.bIsOwner = true
	};

	CKeyValues* pScrollVMT = new CKeyValues(Q_XOR("VertexLitGeneric"));
	pScrollVMT->SetString(Q_XOR("$basetexture"), Q_XOR("dev/screenhighlight_pulse"));
	if (CKeyValues* pScrollProxies = pScrollVMT->CreateKey(Q_XOR("proxies")); pScrollProxies != nullptr)
	{
		if (CKeyValues* pTextureScroll = pScrollProxies->CreateKey(Q_XOR("texturescroll")); pTextureScroll != nullptr)
		{
			pTextureScroll->SetString(Q_XOR("texturescrollvar"), Q_XOR("$basetexturetransform"));
			pTextureScroll->SetString(Q_XOR("texturescrollrate"), Q_XOR("0.2"));
			pTextureScroll->SetString(Q_XOR("texturescrollangle"), Q_XOR("90"));
		}
	}

	arrCustomMaterials[VISUAL_CHAMS_SCROLL - 1] =
	{
		.pMaterial = CreateMaterial(Q_XOR("qo0_scroll.vmt"), pScrollVMT),
		.bIsOwner = true
	};

	for (auto& [pMaterial, bIsOwner] : arrCustomMaterials)
	{
		// check does all materials were created successfully
		if (pMaterial == nullptr || pMaterial->IsErrorMaterial())
			return false;

		// increment references once for each game material
		if (!bIsOwner)
			pMaterial->IncrementReferenceCount();
	}

	return true;
}

void CHAMS::Destroy()
{
	for (auto& [pMaterial, bIsOwner] : arrCustomMaterials)
	{
		if (pMaterial != nullptr)
			// decrement references for all materials and game will deallocate needed automatically on next map load
			pMaterial->DecrementReferenceCount();
	}
}

#pragma region visual_chams_callback
bool CHAMS::OnDrawModel(CCSPlayer* pLocal, DrawModelResults_t* pResults, const DrawModelInfo_t* pInfo, Matrix3x4_t* pBoneToWorld, float* pflFlexWeights, float* pflFlexDelayedWeights, const Vector_t* pvecModelOrigin, int nFlags)
{
	if (!C::Get<bool>(Vars.bVisual) || !C::Get<bool>(Vars.bVisualChams))
		return false;

	// state showing when we've drawn custom model and don't need to draw original
	// @note: you can also use it to not draw model at all
	bool bDrawnModel = false;

	// skip glow models
	if (nFlags & (STUDIO_RENDER | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_NOLIGHTING_OR_CUBEMAP | STUDIO_SKIP_DECALS))
		return false;

	IClientRenderable* pRenderable = pInfo->pClientEntity;

	if (pRenderable == nullptr)
		return false;

	// get entity from renderable
	CBaseEntity* pEntity = pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (pEntity == nullptr)
		return false;

	// check is entity a player
	if (pEntity->IsPlayer() && pEntity->IsAlive())
	{
		bool bIsLocal = (pEntity == pLocal);
		bool bIsEnemy = (pLocal->IsOtherEnemy(static_cast<CCSPlayer*>(pEntity)));
		bool bIsAlly = (!bIsLocal && !bIsEnemy);

		bIsLocal &= C::Get<bool>(Vars.bVisualChamsLocal);
		bIsEnemy &= C::Get<bool>(Vars.bVisualChamsEnemies);
		bIsAlly &= C::Get<bool>(Vars.bVisualChamsAllies);

		// check is any of filters are enabled and pass
		if (bIsLocal || bIsEnemy || bIsAlly)
		{
			if (const VisualChams_t nType = (bIsLocal ? C::Get<int>(Vars.iVisualChamsLocal) : (bIsEnemy ? C::Get<int>(Vars.iVisualChamsEnemies) : C::Get<int>(Vars.iVisualChamsAllies))); nType != VISUAL_CHAMS_NONE)
			{
				const OverrideParameters_t playerParameters =
				{
					.nType = nType,
					.bIgnoreZ = (bIsLocal ? C::Get<bool>(Vars.bVisualChamsLocalXQZ) : (bIsEnemy ? C::Get<bool>(Vars.bVisualChamsEnemiesXQZ) : C::Get<bool>(Vars.bVisualChamsAlliesXQZ))),
					.bWireframe = (bIsLocal ? C::Get<bool>(Vars.bVisualChamsLocalWireframe) : (bIsEnemy ? C::Get<bool>(Vars.bVisualChamsEnemiesWireframe) : C::Get<bool>(Vars.bVisualChamsAlliesWireframe))),
					.colVisible = (bIsLocal ? C::Get<Color_t>(Vars.colVisualChamsLocal) : (bIsEnemy ? C::Get<Color_t>(Vars.colVisualChamsEnemies) : C::Get<Color_t>(Vars.colVisualChamsAllies))),
					.colHidden = (bIsLocal ? C::Get<Color_t>(Vars.colVisualChamsLocalHidden) : (bIsEnemy ? C::Get<Color_t>(Vars.colVisualChamsEnemiesHidden) : C::Get<Color_t>(Vars.colVisualChamsAlliesHidden)))
				};

				bDrawnModel = OverrideMaterial(playerParameters, pResults, pInfo, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, pvecModelOrigin, nFlags);
			}

			// desync
			if (const VisualChams_t nType = C::Get<int>(Vars.iVisualChamsLocalDesync); bIsLocal && nType != VISUAL_CHAMS_NONE)
			{
				const OverrideParameters_t desyncParameters =
				{
					.nType = nType,
					.bIgnoreZ = C::Get<bool>(Vars.bVisualChamsLocalDesyncXQZ),
					.bWireframe = C::Get<bool>(Vars.bVisualChamsLocalDesyncWireframe),
					.colVisible = C::Get<Color_t>(Vars.colVisualChamsLocalDesync),
					.colHidden = C::Get<Color_t>(Vars.colVisualChamsLocalDesyncHidden)
				};

				const int nLocalIndex = pLocal->GetIndex();
				auto& arrDesyncBones = ANIMATION::GetClientBoneMatrices();
				ANIMATION::ConvertBonesPositionToWorldSpace(arrDesyncBones, nLocalIndex, *pvecModelOrigin);

				OverrideMaterial(desyncParameters, pResults, pInfo, arrDesyncBones, pflFlexWeights, pflFlexDelayedWeights, pvecModelOrigin, nFlags);

				ANIMATION::ConvertBonesPositionToLocalSpace(arrDesyncBones, nLocalIndex, *pvecModelOrigin);
			}
		}
	}
	// check is entity a viewmodel
	else if (C::Get<bool>(Vars.bVisualChamsViewModel))
	{
		if (CBaseAnimating* pAnimating = pEntity->GetBaseAnimating(); pAnimating != nullptr)
		{
			const CBaseAnimating* pParentViewModel = I::ClientEntityList->Get<CBaseAnimating>(pAnimating->GetMoveParentHandle());

			// check either entity is viewmodel base (weapon) or it's parent (hands)
			if (pAnimating->IsViewModel() || (pParentViewModel != nullptr && pParentViewModel->IsViewModel()))
			{
				if (const VisualChams_t nType = C::Get<int>(Vars.iVisualChamsViewModel); nType != VISUAL_CHAMS_NONE)
				{
					const OverrideParameters_t overrideParameters =
					{
						.nType = nType,
						.bIgnoreZ = C::Get<bool>(Vars.bVisualChamsViewModelXQZ),
						.bWireframe = C::Get<bool>(Vars.bVisualChamsViewModelWireframe),
						.colVisible = C::Get<Color_t>(Vars.colVisualChamsViewModel),
						.colHidden = C::Get<Color_t>(Vars.colVisualChamsViewModelHidden)
					};

					bDrawnModel = OverrideMaterial(overrideParameters, pResults, pInfo, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, pvecModelOrigin, nFlags);
				}
			}
		}
	}

	return bDrawnModel;
}
#pragma endregion

#pragma region visual_chams_main
bool CHAMS::OverrideMaterial(const OverrideParameters_t& parameters, DrawModelResults_t* pResults, const DrawModelInfo_t* pInfo, Matrix3x4_t* pBoneToWorld, float* pflFlexWeights, float* pflFlexDelayedWeights, const Vector_t* pvecModelOrigin, int nFlags)
{
	Q_ASSERT(parameters.nType != VISUAL_CHAMS_NONE); // not allowed type

	// get material for specified type
	IMaterial* pMaterial = arrCustomMaterials[parameters.nType - 1].pMaterial;

	// second-pass ignorez override
	if (parameters.bIgnoreZ)
	{
		// glow requires one more pass
		if (parameters.nType == VISUAL_CHAMS_GLOW)
		{
			// set glow color
			//pMaterial->FindVar(Q_XOR("$envmaptint"), nullptr, false)->SetVector(colAdditionalHidden.Base<COLOR_R>(), colAdditionalHidden.Base<COLOR_G>(), colAdditionalHidden.Base<COLOR_B>());

			// add fresnel effect for glow
			pMaterial->FindVar(Q_XOR("$envmapfresnel"), nullptr, false)->SetFloat(1.0f);

			// set glow alpha
			//pMaterial->AlphaModulate(colAdditionalHidden.Base<COLOR_A>());

			// enable "$ignorez" flag
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

			// set "$wireframe" flag
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, parameters.bWireframe);

			// draw model with our material
			I::StudioRender->ForcedMaterialOverride(pMaterial);
			H::hkDrawModel.CallOriginal<ROP::EngineGadget_t>(I::StudioRender, nullptr, pResults, pInfo, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, pvecModelOrigin, nFlags);

			// clear overrides
			I::StudioRender->ForcedMaterialOverride(nullptr);

			// @todo: set just covered material for second-pass
		}

		// set xqz color & alpha
		float arrHiddenColorBase[4];
		parameters.colHidden.BaseAlpha(arrHiddenColorBase);
		pMaterial->ColorModulate(arrHiddenColorBase[COLOR_R], arrHiddenColorBase[COLOR_G], arrHiddenColorBase[COLOR_B]);
		pMaterial->AlphaModulate(arrHiddenColorBase[COLOR_A]);

		// enable "$ignorez" flag
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

		// set "$wireframe" flag
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, parameters.bWireframe);

		// draw model with ignorez material
		I::StudioRender->ForcedMaterialOverride(pMaterial);
		H::hkDrawModel.CallOriginal<ROP::EngineGadget_t>(I::StudioRender, nullptr, pResults, pInfo, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, pvecModelOrigin, nFlags);

		// clear overrides
		I::StudioRender->ForcedMaterialOverride(nullptr);
	}

	// glow requires one more pass
	if (parameters.nType == VISUAL_CHAMS_GLOW)
	{
		// @todo: additional colors
		// set glow color
		//pMaterial->FindVar(Q_XOR("$envmaptint"), nullptr, false)->SetVector(colAdditionalVisible.Base<COLOR_R>(), colAdditionalVisible.Base<COLOR_G>(), colAdditionalVisible.Base<COLOR_B>());

		// add fresnel effect for glow
		pMaterial->FindVar(Q_XOR("$envmapfresnel"), nullptr, false)->SetFloat(1.0f);

		// set glow alpha
		//pMaterial->AlphaModulate(colAdditionalVisible.Base<COLOR_A>());

		// disable color fusion for glow
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);

		// disable "$ignorez" flag
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

		// set "$wireframe" flag
		pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, parameters.bWireframe);

		// draw model with our material
		I::StudioRender->ForcedMaterialOverride(pMaterial);
		H::hkDrawModel.CallOriginal<ROP::EngineGadget_t>(I::StudioRender, nullptr, pResults, pInfo, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, pvecModelOrigin, nFlags);

		// clear overrides
		I::StudioRender->ForcedMaterialOverride(nullptr);

		// @todo: set just covered material for second-pass
	}

	// set color & alpha
	float colVisibleBase[4];
	parameters.colVisible.BaseAlpha(colVisibleBase);
	pMaterial->ColorModulate(colVisibleBase[COLOR_R], colVisibleBase[COLOR_G], colVisibleBase[COLOR_B]);
	pMaterial->AlphaModulate(colVisibleBase[COLOR_A]);

	// disable color fusion
	pMaterial->SetMaterialVarFlag(MATERIAL_VAR_ADDITIVE, false);

	// disable "$ignorez" flag
	pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

	// set "$wireframe" flag
	pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, parameters.bWireframe);

	// draw original model with our material
	I::StudioRender->ForcedMaterialOverride(pMaterial);
	H::hkDrawModel.CallOriginal<ROP::EngineGadget_t>(I::StudioRender, nullptr, pResults, pInfo, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, pvecModelOrigin, nFlags);

	// final clear overrides, otherwise it may affect on glow or other models
	I::StudioRender->ForcedMaterialOverride(nullptr);

	// we've drawn model, tell the caller to not draw original model
	return true;
}
#pragma endregion

#pragma region visual_chams_callback
IMaterial* CHAMS::CreateMaterial(const char* szName, CKeyValues* pMaterialData)
{
	/*
	 * create material from buffer
	 * note that it automatically increments the reference count
	 */
	if (IMaterial* pMaterial = I::MaterialSystem->CreateMaterial(szName, pMaterialData); pMaterial != nullptr)
		return pMaterial;

	L_PRINT(LOG_ERROR) << Q_XOR("failed to create material: \"") << szName << Q_XOR("\"");
	return nullptr;
}
#pragma endregion
