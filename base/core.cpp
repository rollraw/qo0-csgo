// used: sleep_for
#include <thread>

// used: winapi, directx, fmt includes
#include "common.h"
// used: global variables
#include "global.h"
// used: cheat variables
#include "core/variables.h"
// used: eventlistener setup/destroy
#include "utilities.h"
// used: result logging
#include "utilities/logging.h"
// used: exports setup
#include "utilities/math.h"
// used: inputsystem setup/destroy
#include "utilities/inputsystem.h"
// used: render setup/destroy
#include "utilities/draw.h"
// used: netvar manager setup
#include "core/netvar.h"
// used: configurations setup
#include "core/config.h"
// used: hooks setup/destroy
#include "core/hooks.h"

DWORD WINAPI MainThread(LPVOID lpParameter)
{
  const auto loadHelper = reinterpret_cast<CLoadHelper*>(lpParameter);

	try
	{
    // basic process check
		if (MEM::GetModuleBaseHandle(XorStr("csgo.exe")) == nullptr)
		{
			MessageBox(nullptr, XorStr("this cannot be injected in another process\nopen <csgo.exe> to inject"), XorStr("qo0 base"), MB_OK);
			return FALSE;
		}

		/*
		 * @note: serverbrowser.dll is last loaded module (u can seen it when debug)
		 * here is check for all modules loaded
		 */
		while (MEM::GetModuleBaseHandle(SERVERBROWSER_DLL) == nullptr)
			std::this_thread::sleep_for(200ms);

		#ifdef DEBUG_CONSOLE
		// console logging
		if (!L::Attach(XorStr("qo0's base developer-mode")))
			throw std::runtime_error(XorStr("failed to attach console"));

		L::Print(XorStr("console opened"));
		#else
		// file logging
		// @note: use std::ios::app instead std::ios::trunc to not clear every time
		L::ofsFile.open(C::GetWorkingPath().append(XorStr("qo0base.log")), std::ios::out | std::ios::trunc);
		#endif

		// capture interfaces from game/steam (not always) modules
		if (!I::Setup())
			throw std::runtime_error(XorStr("failed to capture interfaces"));

		L::Print(XorStr("interfaces captured"));

		// version check to know when u need to fix something
		#ifdef DEBUG_CONSOLE
		if (strcmp(I::Engine->GetProductVersionString(), XorStr("1.37.7.6")) != 0)
		{
			L::PushConsoleColor(FOREGROUND_YELLOW);
			L::Print(XorStr("[warning] version doesnt match! current cs:go version: {}"), I::Engine->GetProductVersionString());
			L::PopConsoleColor();
		}
		#endif

		/*
		 * fill networkable variables map
		 * dump received netvars to the file
		 */
		if (!CNetvarManager::Get().Setup(XorStr("netvars.qo0")))
			throw std::runtime_error(XorStr("failed to initialize netvars"));

		L::Print(XorStr("found [{:d}] props in [{:d}] tables"), CNetvarManager::Get().iStoredProps, CNetvarManager::Get().iStoredTables);

		// export completed mathematics functions from game/steam (not always) modules
		if (!M::Setup())
			throw std::runtime_error(XorStr("failed to get math exports"));

		L::Print(XorStr("math exports loaded"));

		// replace window messages processor
		if (!IPT::Setup())
			throw std::runtime_error(XorStr("failed to set window messages processor"));

		L::Print(XorStr("inputsystem setup complete"));

		#if 0
		// start tracking entities
		U::EntityListener.Setup();
		L::Print(XorStr("entity listener initialized"));
		#endif

		// start tracking specified events from vector
		// @note: all events list: https://wiki.alliedmods.net/Counter-Strike:_Global_Offensive_Events
		U::EventListener.Setup({ XorStr("player_hurt"), XorStr("round_prestart"), XorStr("round_freeze_end") });
		L::Print(XorStr("events registered"));

		// add our functionality in client functions
		if (!H::Setup())
			throw std::runtime_error(XorStr("failed initialize hooks"));

		L::Print(XorStr("hooks setup complete"));

		// add our functionality in networkable functions
		if (!P::Setup())
			throw std::runtime_error(XorStr("failed initialize proxies"));

		L::Print(XorStr("proxies applied"));

		// setup values to save/load cheat variables in/from files and load default configuration
		if (!C::Setup(XorStr("default.qo0")))
		{
			// this error is not critical, only show that
			L::PushConsoleColor(FOREGROUND_RED);
			L::Print(XorStr("[error] failed setup and/or load default configuration"));
			L::PopConsoleColor();
		}
		else
			L::Print(XorStr("default config loaded"));

		// show message about successful load in logs and in game console
		L::PushConsoleColor(FOREGROUND_MAGENTA);
		L::Print(XorStr("qo0 base successfully loaded"));
		L::PopConsoleColor();
		I::GameConsole->Clear();
		I::ConVar->ConsoleColorPrintf(Color(255, 50, 255, 255), XorStr("qo0 base successfully loaded.\nbuild date: %s / %s\n"), __DATE__, __TIME__);
	}
	catch (const std::exception& ex)
	{
		// print error message
		L::PushConsoleColor(FOREGROUND_INTENSE_RED);
		L::Print(XorStr("[error] {}"), ex.what());
		L::PopConsoleColor();

		#ifdef _DEBUG
		// show error message window (or replace to your exception handler)
		_RPT0(_CRT_ERROR, ex.what());
		#else
		// unload
		FreeLibraryAndExitThread(G::hDll, EXIT_FAILURE);
		#endif
	}

	// unload cheat if pressed specified key or if loadhelper requests it
	while (!IPT::IsKeyReleased(C::Get<int>(Vars.iPanicKey)) && !loadHelper->bUnloadRequested)
		std::this_thread::sleep_for(500ms);

	#if 0
	// destroy entity listener
	U::EntityListener.Destroy();
	#endif

	// destroy events listener
	U::EventListener.Destroy();

	// reset crosshair state
	I::ConVar->FindVar(XorStr("crosshair"))->SetValue(true);

	// restore proxies
	P::Restore();

	// restore hooks
	H::Restore();

	// set back to original window messages processor
	IPT::Restore();

	// destroy render
	D::Destroy();

	#ifdef DEBUG_CONSOLE
	// detach console
	L::Detach();
	#else
	// close logging output file
	if (L::ofsFile.is_open())
		L::ofsFile.close();
	#endif

  // We should only free the library if the loadhelper DIDNT request it. If it did, then it will do it itself.
  if (!loadHelper->bUnloadRequested)
    FreeLibraryAndExitThread(G::hDll, EXIT_SUCCESS);

  // Signal that thread has exited
  loadHelper->bThreadExited = true;

  // Return success
  return EXIT_SUCCESS;  
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
  static auto loadHelper = CLoadHelper();

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// save our module
		G::hDll = hModule;

    const auto hThread = CreateThread(nullptr, 0U, MainThread, &loadHelper, 0U, nullptr);
    if (hThread == nullptr)
      return FALSE;
  
    CloseHandle(hThread);
	} else if (dwReason == DLL_PROCESS_DETACH)
  {
    // Signal to the other thread to exit using loadhelper
    loadHelper.bUnloadRequested = true;

    // Wait for the other thread to exit
    while (!loadHelper.bThreadExited)
      Sleep(100);

    // At this point, the other thread has exited and we are free to return.
  }

	return FALSE;
}
