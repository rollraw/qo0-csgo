// used: std::this_thread
#include <thread>

#include "inputsystem.h"
// used: wndproc hook, inputsystem interface
#include "../core/hooks.h"
// used: menu open/panic keys
#include "../core/variables.h"
// used: get variable
#include "../core/config.h"
// used: menu open state
#include "../core/menu.h"

bool IPT::Setup()
{
	D3DDEVICE_CREATION_PARAMETERS creationParameters;
	while (FAILED(I::DirectDevice->GetCreationParameters(&creationParameters)))
		std::this_thread::sleep_for(200ms);

	hWindow = creationParameters.hFocusWindow;

	if (hWindow == nullptr)
		return false;

	pOldWndProc = (WNDPROC)SetWindowLongPtrW(hWindow, GWLP_WNDPROC, (LONG_PTR)H::hkWndProc);

	if (pOldWndProc == nullptr)
		return false;

	return true;
}

void IPT::Restore()
{
	if (pOldWndProc != nullptr)
	{
		SetWindowLongPtrW(hWindow, GWLP_WNDPROC, (LONG_PTR)pOldWndProc);
		pOldWndProc = nullptr;
	}

	// reset input state
	I::InputSystem->EnableInput(true);
}

bool IPT::Process(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// prevent process when e.g. binding something in-menu
	if (W::bMainOpened && wParam != C::Get<int>(Vars.iMenuKey) && wParam != C::Get<int>(Vars.iPanicKey))
		return false;

	// current active key
	int nKey = 0;
	// current active key state
	EKeyState state = EKeyState::NONE;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam >= 0U && wParam < 256U)
		{
			nKey = wParam;
			state = EKeyState::DOWN;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam >= 0U && wParam < 256U)
		{
			nKey = wParam;
			state = EKeyState::UP;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		nKey = VK_LBUTTON;
		state = uMsg == WM_LBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		nKey = VK_RBUTTON;
		state = uMsg == WM_RBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		nKey = VK_MBUTTON;
		state = uMsg == WM_MBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		nKey = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		state = uMsg == WM_XBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	default:
		return false;
	}

	// save key states
	if (state == EKeyState::UP && arrKeyState.at(nKey) == EKeyState::DOWN) // if swap states it will be pressed state
		arrKeyState.at(nKey) = EKeyState::RELEASED;
	else
		arrKeyState.at(nKey) = state;

	return true;
}
