#pragma once
// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "../common.h"
// used: keybind_t
#include "../core/config.h"

/*
 * INPUT SYSTEM
 * listen and handle key states
 */
namespace IPT
{
	using KeyState_t = std::uint8_t;

	enum EKeyState : KeyState_t
	{
		KEY_STATE_NONE,
		KEY_STATE_DOWN,
		KEY_STATE_UP,
		KEY_STATE_RELEASED
	};

	/* @section: values */
	// current window
	inline HWND	hWindow = nullptr;
	// saved window messages handler
	inline WNDPROC pOldWndProc = nullptr;
	// last processed key states
	inline KeyState_t arrKeyState[256] = { };

	// replace game window messages processor with our
	bool Setup();
	// restore window messages processor to original
	void Destroy();

	/* @section: callbacks */
	// process input window message and save keys states in array
	bool OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* @section: get */
	/// @returns: true if keybind is active, false otherwise
	bool GetBindState(KeyBind_t& keyBind);
	/// @returns: true if key is being held, false otherwise
	inline bool IsKeyDown(const std::uint32_t uButtonCode)
	{
		return arrKeyState[uButtonCode] == KEY_STATE_DOWN;
	}
	/// @returns: true if key has been just released, false otherwise
	inline bool IsKeyReleased(const std::uint32_t uButtonCode)
	{
		if (arrKeyState[uButtonCode] == KEY_STATE_RELEASED)
		{
			arrKeyState[uButtonCode] = KEY_STATE_UP;
			return true;
		}

		return false;
	}
}
