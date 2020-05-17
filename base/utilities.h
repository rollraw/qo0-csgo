#pragma once
// used: winapi, fmt includes
#include "common.h"
// used: color
#include "sdk/datatypes/color.h"
// used: vector
#include "sdk/datatypes/vector.h"
// used: keyvalues
#include "sdk/datatypes/keyvalues.h"
// used: baseentity, handleentity
#include "sdk/entity.h"
// used: trace
#include "sdk/interfaces/ienginetrace.h"
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

	/* returns elapsed time between last time point and now */
	long long Elapsed() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timePoint).count();
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
	template <class C>
	C* FindHudElement(const char* szName);

	// Game
	/* returns localplayer pointer */
	CBaseEntity* GetLocalPlayer();
	/* trace ray implementation, returns modified trace */
	void TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int fMask, IHandleEntity* pSkip, Trace_t* pTrace);
	/* updates hud, clears delta tick */
	void ForceFullUpdate();
	/* return true if ray goes through smoke */
	bool LineGoesThroughSmoke(Vector vecStartPos, Vector vecEndPos);
	/* set localplayer accepted competitive game */
	void SetLocalPlayerReady(); // @credits: oneshotgh
	/* set given string to nickname */
	void SendName(const char* szName);
	/* set given string to clantag */
	void SendClanTag(const char* szClanTag, const char* szIdentifier);
	/* calculate ~chocked ticks num for given entity */
	int GetChokedTicks(CBaseEntity* pLocal, CBaseEntity* pEntity);
	/* load specified model before you will be in-game */
	bool PrecacheModel(const char* szModelName);
	/* returns icon of given index for custom font */
	const char* GetWeaponIcon(short nItemDefinitionIndex);

	// Extra
	/* flashes the specified window */
	void FlashWindow(HWND pWindow);

	// String
	/* converts from unicode to ascii string */
	std::string UnicodeAscii(const std::wstring& wszUnicode);
	/* converts from ascii to unicode string */
	std::wstring AsciiUnicode(const std::string& szAscii);

	// Links
	/* event listener implementation */
	inline CEventListener EventListener;
	/* entity listener implementation */
	inline CEntityListener EntityListener;
}
