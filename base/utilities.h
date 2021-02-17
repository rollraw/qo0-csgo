#pragma once
// used: winapi, fmt includes
#include "common.h"
// used: vector
#include "sdk/datatypes/vector.h"
// used: material
#include "sdk/interfaces/imaterialsystem.h"
// used: events listerner setup
#include "utilities/eventlistener.h"
// used: entity listerner setup
#include "utilities/entitylistener.h"

/* internal implementation for measuring specific time intervals */
class CTimer
{
public:
	CTimer(bool bStart = false)
	{
		if (bStart)
			Reset();
	}

	/* set time point to now */
	void Reset()
	{
		timePoint = std::chrono::high_resolution_clock::now();
	}

	/* returns elapsed time between last time point and now in given duration type (default: milliseconds) */
	template <class C = std::chrono::milliseconds>
	long long Elapsed() const
	{
		return std::chrono::duration_cast<C>(std::chrono::high_resolution_clock::now() - timePoint).count();
	}

private:
	std::chrono::high_resolution_clock::time_point timePoint = { };
};

/*
 * UTILITIES
 * third-party functions-helpers
 */
namespace U
{
	// Get
	/* returns class of hud element with given name */
	std::uintptr_t* FindHudElement(const char* szName);

	// Game
	/* updates hud, clears delta tick */
	void ForceFullUpdate();
	/* return true if ray goes through smoke */
	bool LineGoesThroughSmoke(const Vector& vecStart, const Vector& vecEnd, const bool bGrenadeBloat = true);
	/* set localplayer accepted competitive game */
	void SetLocalPlayerReady(); // @credits: oneshotgh
	/* set given string to nickname */
	void SendName(const char* szName);
	/* set given string to clantag */
	void SendClanTag(const char* szClanTag, const char* szIdentifier);
	/* load specified model before you will be in-game */
	bool PrecacheModel(const char* szModelName);
	/* allocate client dll data for the object */
	IClientNetworkable* CreateDLLEntity(int iEntity, EClassIndex nClassID, int nSerial);
	/* returns icon of given index for custom font */
	const char8_t* GetWeaponIcon(short nItemDefinitionIndex);

	// Extra
	/* flashes the specified window */
	void FlashWindow(HWND pWindow);

	// String
	/* converts from unicode to ascii string */
	std::string UnicodeAscii(std::wstring_view wszUnicode);
	/* converts from ascii to unicode string */
	std::wstring AsciiUnicode(std::string_view szAscii);

	// Links
	/* event listener implementation */
	inline CEventListener EventListener;
	/* entity listener implementation */
	inline CEntityListener EntityListener;
}
