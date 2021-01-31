#pragma once
// used: std::array
#include <array>

// used: winapi includes
#include "../common.h"

enum class EKeyState : int
{
	NONE,
	DOWN,
	UP,
	RELEASED
};

/*
 * INPUT SYSTEM
 * listen and handle key states
 */
namespace IPT
{
	// Values
	/* current window */
	inline HWND	hWindow = nullptr;
	/* saved window messages handler */
	inline WNDPROC pOldWndProc = nullptr;
	/* last processed key states */
	inline std::array<EKeyState, 256U> arrKeyState = { };

	// Get
	/* set our window messages proccesor */
	bool Setup();
	/* restore window messages processor and clear saved pointer */
	void Restore();
	/* process input window message and save keys states in array */
	bool Process(UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* is given key being held */
	inline bool IsKeyDown(const std::uint32_t uButtonCode)
	{
		return arrKeyState.at(uButtonCode) == EKeyState::DOWN;
	}

	/* was given key released */
	inline bool IsKeyReleased(const std::uint32_t uButtonCode)
	{
		if (arrKeyState.at(uButtonCode) == EKeyState::RELEASED)
		{
			arrKeyState.at(uButtonCode) = EKeyState::UP;
			return true;
		}

		return false;
	}
}
