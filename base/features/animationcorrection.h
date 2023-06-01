#pragma once
// used: ccsgoplayeranimstate
#include "../sdk/animation.h"
// used: maxstudiobones
#include "../sdk/studio.h"
// used: eclientframestage
#include "../sdk/interfaces/iengineclient.h"

/*
 * ANIMATION CORRECTION
 * - correct players animation to retrieve exact data from server on client
 */
namespace F::ANIMATION
{
	/* @section: callbacks */
	void OnFrame(const EClientFrameStage nStage);
	void OnPlayerCreated(CCSPlayer* pPlayer);
	void OnPlayerDeleted(CCSPlayer* pPlayer);

	/* @section: get */
	/// @returns: true if game is allowed to call 'C_CSPlayer::UpdateClientSideAnimation' for player with given @a'nPlayerIndex', false otherwise
	bool IsGameAllowedToUpdateAnimations(const int nPlayerIndex);
	/// @returns: true if game is allowed to call 'C_CSPlayer::SetupBones' for player with given @a'nPlayerIndex', false otherwise
	bool IsGameAllowedToSetupBones(const int nPlayerIndex);
	/// @returns: last bone matrices of local player built with adjusted client-side data, origins stored in local space, note that you should convert them
	std::add_lvalue_reference_t<Matrix3x4a_t[MAXSTUDIOBONES]> GetClientBoneMatrices();
	/// @returns: last bone matrices of player built with adjusted server-side data, origins stored in local space, note that you should convert them
	std::add_lvalue_reference_t<Matrix3x4a_t[MAXSTUDIOBONES]> GetPlayerBoneMatrices(const int nIndex);

	/* @section: extra */
	/// subtract given interpolate origin from origin of the given bones matrices to convert them to local space
	/// @param[in,out] arrBonesToWorld bone matrices to convert from world space to local space
	/// @param[in] nPlayerIndex index of the player, used for debugging
	/// @param[in] vecPlayerOrigin origin of the player on which the transform should be based
	void ConvertBonesPositionToLocalSpace(Matrix3x4a_t (&arrBonesToWorld)[MAXSTUDIOBONES], const int nPlayerIndex, const Vector_t& vecPlayerOrigin);
	/// add given player origin to the origin of the given bones matrices to convert them to world space
	/// @note: since we're building bones once per tick, but we may need to access them more times per tick, we need to convert it back to the local space after all operations we did, otherwise it will lead to wrong position of bones
	/// @param[in,out] arrBonesToLocal bone matrices to convert from local space to world space
	/// @param[in] nPlayerIndex index of the player, used for debugging
	/// @param[in] vecPlayerOrigin origin of the player on which the transform should be based
	void ConvertBonesPositionToWorldSpace(Matrix3x4a_t (&arrBonesToLocal)[MAXSTUDIOBONES], const int nPlayerIndex, const Vector_t& vecPlayerOrigin);

	/* @section: values */
	// time of next lower body yaw update for local player
	inline float flNextLowerBodyYawUpdateTime = 0.f;
}
