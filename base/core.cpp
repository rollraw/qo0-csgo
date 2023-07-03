#include "core.h"

// used: [win] shgetknownfolderpath
#include <shlobj_core.h>

#include "common.h"
// used: log setup/destroy
#include "utilities/log.h"
// used: inputsystem setup/destroy
#include "utilities/inputsystem.h"
// used: render setup/destroy
#include "utilities/draw.h"
// used: cheat variables
#include "core/variables.h"
// used: configurations setup
#include "core/config.h"
// used: convars setup
#include "core/convar.h"
// used: netvar manager setup
#include "core/netvar.h"
// used: interfaces setup/destroy
#include "core/interfaces.h"
// used: hooks setup/destroy
#include "core/hooks.h"
// used: proxies setup/destroy
#include "core/proxies.h"
// used: eventlistener setup/destroy
#include "core/eventlistener.h"
// used: entitylistener setup/destroy
#include "core/entitylistener.h"
// used: features setup
#include "features.h"

bool CORE::GetWorkingPath(wchar_t* wszDestination)
{
	bool bSuccess = false;
	PWSTR wszPathToDocuments = nullptr;

	// get path to user documents
	if (SUCCEEDED(::SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &wszPathToDocuments)))
	{
		CRT::StringCat(CRT::StringCopy(wszDestination, wszPathToDocuments), Q_XOR(L"\\.qo0\\"));
		bSuccess = true;

		// create directory if it doesn't exist
		if (!::CreateDirectoryW(wszDestination, nullptr))
		{
			if (::GetLastError() != ERROR_ALREADY_EXISTS)
			{
				L_PRINT(LOG_ERROR) << Q_XOR("failed to create default working directory, because one or more intermediate directories don't exist");
				bSuccess = false;
			}
		}
	}
	::CoTaskMemFree(wszPathToDocuments);

	return bSuccess;
}

static bool Setup(HMODULE hModule)
{
#ifdef Q_LOG_CONSOLE
	if (!L::AttachConsole(Q_XOR(L"qo0 base developer-mode")))
	{
		Q_ASSERT(false); // failed to attach console
		return false;
	}
#endif
#ifdef Q_LOG_FILE
	if (!L::OpenFile(Q_XOR(L"qo0base.log")))
	{
		Q_ASSERT(false); // failed to open file
		return false;
	}
#endif
	L_PRINT(LOG_NONE) << Q_XOR("logging system initialization completed");

	if (!MEM::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to initialize memory");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("memory initialization completed");

	if (!I::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to capture interfaces");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("interfaces capture completed");

	if (!CONVAR::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to initialize convars");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("convars initialization completed");

#ifdef _DEBUG
	CONVAR::Dump(L"convars.txt");
#endif

	if (!NETVAR::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to initialize netvars");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("found [") << NETVAR::GetPropertiesCount() << Q_XOR("] networkable properties in [") << NETVAR::GetTablesCount() << Q_XOR("] data tables");

#ifdef _DEBUG
	NETVAR::Dump(L"netvars.txt");
#endif

	// replace window messages processor
	if (!IPT::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to intialize input system");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("input system initialization completed");

	// start tracking events
	if (!EVENT::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to register one or more events to listener");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("event listener initialization completed");

	// start tracking entities
	ENTITY::Setup();
	L_PRINT(LOG_NONE) << Q_XOR("entity listener initialization completed");

	// initialize feature-dependent data
	if (!F::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to intialize features handler");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("features handler initialization completed");

	// add our functionality into game functions
	if (!H::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to intialize hooks");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("hooks initialization completed");

	// add our functionality into game networkable proxies
	if (!P::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to intialize proxies");
		return false;
	}
	L_PRINT(LOG_NONE) << Q_XOR("proxies initialization completed");

	// setup values to save/load cheat variables into/from files and load default configuration
	if (!C::Setup(Q_XOR(Q_CONFIGURATION_DEFAULT_FILE_NAME)))
		// this error is not critical, only show that
		L_PRINT(LOG_WARNING) << Q_XOR("failed to setup and/or load default configuration");
	else
		L_PRINT(LOG_NONE) << Q_XOR("configuration system initialization completed");

	L_PRINT(LOG_NONE) << L::SetColor(LOG_COLOR_FORE_MAGENTA) << Q_XOR("qo0-base initialization completed");
	return true;
}

// @todo: some of those may crash while closing process, because we dont have any dependencies from the game modules, it means them can be unloaded and destruct interfaces etc before our module | modify ldrlist?
static void Destroy()
{
	// restore proxies
	P::Destroy();

	// restore hooks
	H::Destroy();

	// destroy features
	F::Destroy();

	// destroy entity listener
	ENTITY::Destroy();

	// destroy event listener
	EVENT::Destroy();

	// destroy render
	D::Destroy();

	// restore window messages processor
	IPT::Destroy();

#ifdef _DEBUG
	/*
	 * decrement thread count to prevent max thread limit assertion trigger
	 * @xref: "Exceeded maximum number of unique threads (%d) attempting to access datacache.\n"
	 */
	std::uint8_t* pResourceAccessControl = *reinterpret_cast<std::uint8_t**>(MEM::FindPattern(DATACACHE_DLL, Q_XOR("83 C4 18 B8")) + 0x4);
	if (int& nThreadCount = *reinterpret_cast<int*>(pResourceAccessControl + 0x98); nThreadCount >= 15)
		--nThreadCount;
#endif

#ifdef Q_LOG_CONSOLE
	L::DetachConsole();
#endif
#ifdef Q_LOG_FILE
	L::CloseFile();
#endif
}

DWORD WINAPI PanicThread(LPVOID lpParameter)
{
	// don't let proceed unload until user press specified key
	while (!IPT::IsKeyReleased(C::Get<int>(Vars.iPanicKey)))
		::Sleep(500UL);

	// call detach code and exit this thread
	::FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), EXIT_SUCCESS);
}

extern "C" BOOL WINAPI _CRT_INIT(HMODULE hModule, DWORD dwReason, LPVOID lpReserved);

BOOL APIENTRY CoreEntryPoint(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	// process destroy of the cheat before crt calls atexit table
	if (dwReason == DLL_PROCESS_DETACH)
		Destroy();

	// dispatch reason for c-runtime, initialize/destroy static variables, TLS etc
	if (!_CRT_INIT(hModule, dwReason, lpReserved))
		return FALSE;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CORE::hProcess = MEM::GetModuleBaseHandle(nullptr);

		// basic process check
		if (CORE::hProcess == nullptr)
			return FALSE;

		/*
		 * check did all game and steam overlay (we're rendering through) modules have been loaded
		 * @note: serverbrowser.dll is the last loaded module
		 */
		if (MEM::GetModuleBaseHandle(SERVERBROWSER_DLL) == nullptr || MEM::GetModuleBaseHandle(GAMEOVERLAYRENDERER_DLL) == nullptr)
			return FALSE;

		// save our module handle
		CORE::hDll = hModule;

		// check did we perform main initialization successfully
		if (!Setup(hModule))
		{
			// undo the things we've done
			Destroy();
			return FALSE;
		}

		// create panic thread, it isn't critical error if it fails
		if (const HANDLE hThread = ::CreateThread(nullptr, 0U, &PanicThread, hModule, 0UL, nullptr); hThread != nullptr)
			::CloseHandle(hThread);
	}

	return TRUE;
}
