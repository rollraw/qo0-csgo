#pragma once
// used: visualchams_t
#include "../../core/variables.h"

// used: ccsplayer
#include "../../sdk/entity.h"
// used: imaterial
#include "../../sdk/interfaces/imaterialsystem.h"
// used: drawmodelresults_t, drawmodelinfo_t
#include "../../sdk/interfaces/istudiorender.h"

/*
 * CHAMS
 * - material overriding of entities models
 */
namespace F::VISUAL::CHAMS
{
	struct OverrideParameters_t
	{
		VisualChams_t nType = VISUAL_CHAMS_NONE;
		bool bIgnoreZ = false;
		bool bWireframe = false;
		Color_t colVisible = { };
		Color_t colHidden = { };
	};

	struct CustomMaterialObject_t
	{
		// pointer to the material
		IMaterial* pMaterial = nullptr;
		// true when material was created manually by us, false when material was taken from the game and we shouldn't free it
		bool bIsOwner = false;
	};

	bool Setup();
	void Destroy();

	/* @section: callbacks */
	/// colorize models by overriding materials used by the game
	/// @returns: true if drawn one more model with custom material, false otherwise
	bool OnDrawModel(CCSPlayer* pLocal, DrawModelResults_t* pResults, const DrawModelInfo_t& info, Matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector_t& vecModelOrigin, int nFlags);

	/* @section: main */
	/// @returns: true if original model material was overridden, false otherwise
	bool OverrideMaterial(const OverrideParameters_t& parameters, DrawModelResults_t* pResults, const DrawModelInfo_t& info, Matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector_t& vecModelOrigin, int nFlags);

	/* @section: get */
	/// create .vmt material from memory
	/// @param[in] szName name of material file
	/// @param[in] pMaterialData data of material serialized with valve's material type (VMT) syntax by keyvalues
	/// @remarks: created material already has incremented reference count. @a'pMaterialData' must be valid for the entire lifetime of the material, auto-freed when the material is destroyed
	/// @returns: pointer to created material on success, null otherwise
	IMaterial* CreateMaterial(const char* szName, CKeyValues* pMaterialData);

	/* @section: values */
	// storage of custom materials
	inline CustomMaterialObject_t arrCustomMaterials[VISUAL_CHAMS_MAX - 1] = { };
}
