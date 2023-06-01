#pragma once
// used: ccsplayer
#include "../../sdk/entity.h"

/*
 * WORLD
 * - modulate world entities and draw world information
 */
namespace F::VISUAL::WORLD
{
	/* @section: callbacks */
	void OnDraw(CCSPlayer* pLocal);

	/* @section: main */
	// change the exposure to make the world looking like at night time
	void NightMode(CEnvTonemapController* pController); // @credits: sapphyrus
	// removal of post-processing (anti-aliasing, vignette, etc)
	void PostProcessingRemoval();
	// removal of visual weapon punch
	void PunchRemoval(CCSPlayer* pPlayer, const bool bState, QAngle_t* pangOldViewPunch, QAngle_t* pangOldAimPunch);
	// removal of flashbang overlay
	void FlashRemoval(const bool bState);
	// removal of smoke grenade particles
	void SmokeRemoval(const bool bState);
	// switch camera view to third-person
	void ThirdPerson(CCSPlayer* pLocal);
}
