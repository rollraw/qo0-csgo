#pragma once
// used: ccsplayer
#include "../../sdk/entity.h"
// used: igameevent
#include "../../sdk/interfaces/igameeventmanager.h"

/*
 * SCREEN
 * - modulate viewport setting and draw screen information
 */
namespace F::VISUAL::SCREEN
{
	struct HitMarkerObject_t
	{
		HitMarkerObject_t(const Vector_t& vecPosition, const int iDamage, const float flExpireTime) :
			vecPosition(vecPosition), iDamage(iDamage), flExpireTime(flExpireTime) { }

		Vector_t vecPosition = { };
		int iDamage = 0;
		float flExpireTime = 0.0f;
	};

	/* @section: callbacks */
	void OnDraw(CCSPlayer* pLocal);
	void OnEvent(const FNV1A_t uEventHash, IGameEvent* pEvent);

	/* @section: main */
	// override the camera field of view
	void OverrideFOV(CCSPlayer* pLocal, float* pflFieldOfViewOut);
	// override the player viewmodel field of view
	void OverrideViewModelFOV(CCSPlayer* pLocal, float* pflFieldOfViewOut);
	// show cross and damage when hitting players
	void HitMarker(const Color_t& colLines, const Color_t& colDamage);

	/* @section: values */
	// storage of hitmarkers data
	inline std::vector<HitMarkerObject_t> vecHitMarks = { };
}
