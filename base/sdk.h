#pragma once
#include "common.h"

// used: iclientnetworkable
#include "sdk/entity.h"

/*
 * SOURCE ENGINE SDK
 * - other game utilities and helper functions
 */
namespace SDK
{
	/* @section: panorama */
	/// decode valve font format
	/// @param[in,out] bufferFont encoded buffer of font to decode
	/// @returns: true if the font buffer was successfully decoded, otherwise false
	bool DecodeVFONT(CUtlBuffer& bufferFont);
	// force to clear hud weapon icons
	void ClearHudWeaponIcons();
	/// tell matchmaking this player is ready to play a queued match (which leave penalties)
	/// @param[in] szReason set "deferred" for delayed game accept, any other for instant accept
	void SetLocalPlayerReady(const char* szReason = "");

	/* @section: model */
	// load given model into memory
	bool PrecacheModel(const char* szModelName);
	// allocate client dll data for the object
	IClientNetworkable* CreateDLLEntity(const int nEntity, const EClassIndex nClassID, const int iSerial);

	/* @section: trace */
	// clip trace by ray to closest player with smallest fraction
	void ClipTraceToPlayers(const Vector_t& vecAbsStart, const Vector_t& vecAbsEnd, const int nContentsMask, ITraceFilter* pFilter, Trace_t* pTrace, const float flMinRange = 0.0f, const float flMaxRange = 60.f);
	/// @returns: true if given line goes through smoke, false otherwise
	bool LineGoesThroughSmoke(const Vector_t& vecStart, const Vector_t& vecEnd, const bool flGrenadeBloat = true); // wait what the fuck... valve trying to store float to bool... anyway it always 1.0

	/* @section: network */
	// send given string as your scoreboard clantag
	void SendClanTag(const char* szClanTag, const char* szIdentifier);
}
