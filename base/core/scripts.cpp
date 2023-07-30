// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "scripts.h"

// used: getworkingpath
#include "../core.h"
// used: l_print
#include "../utilities/log.h"
// used: integertostring, strcat...
#include "../utilities/crt.h"

#pragma comment(lib, "lua51.lib")
#pragma comment(lib, "luajit.lib")

// default scripts working path
static wchar_t wszScriptPath[MAX_PATH];
// sol2's lua state
static lua_State* pLuaState = nullptr;

bool LUA::Setup()
{
	if (!CORE::GetWorkingPath(wszScriptPath))
		return false;

	CRT::StringCat(wszScriptPath, Q_XOR(L"scripts\\"));

	// create directory if it doesn't exist
	if (!::CreateDirectoryW(wszScriptPath, nullptr))
	{
		if (::GetLastError() != ERROR_ALREADY_EXISTS)
		{
			L_PRINT(LOG_ERROR) << Q_XOR("failed to create scripts directory, because one or more intermediate directories don't exist");
			return false;
		}
	}

	// store existing scripts list
	Refresh();

	// setup our lua state and its tables, usertype...
	SetupLua();

	// @todo: add script's load on start

	// return true if our lua state is valid
	return pLuaState != nullptr;
}

void LUA::SetupLua()
{
	if (pLuaState!= nullptr)
		return;

	// create lua state
	pLuaState = luaL_newstate();
	luaL_openlibs(pLuaState);

	// open sol2's libraries
	sol::state_view lua(pLuaState);
	lua.open_libraries(sol::lib::bit32, sol::lib::jit, sol::lib::ffi, sol::lib::math, sol::lib::string, sol::lib::package, sol::lib::table, sol::lib::debug);

	// override print function
	lua[Q_XOR("print")] = [](const char* szText)
	{
		L_PRINT(LOG_INFO) << szText;
	};
	// override error function
	lua[Q_XOR("error")] = [ ](const char* szText)
	{
		L_PRINT(LOG_ERROR) << szText;
	};

	// setup our table here
}

void LUA::Destroy()
{
	if (pLuaState)
	{
		lua_close(pLuaState);
		pLuaState = nullptr;
	}
}

#pragma region script_main
void LUA::Refresh()
{
	// store previous loaded scripts
	auto oldVecScriptData = vecScriptData;
	// clear and free previous stored file names
	vecScriptData.clear();

	// make configuration files path filter
	wchar_t wszPathFilter[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszPathFilter, wszScriptPath), Q_XOR(L"*.lua"));

	// iterate through all files with our filter
	WIN32_FIND_DATAW findData;
	if (const HANDLE hFindFile = ::FindFirstFileW(wszPathFilter, &findData); hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			vecScriptData.push_back(new wchar_t[CRT::StringLength(findData.cFileName) + 1U]);
			CRT::StringCopy(vecScriptData.back().wszScriptName, findData.cFileName);

			L_PRINT(LOG_INFO) << Q_XOR("found script file: \"") << findData.cFileName << Q_XOR("\"");
		} while (::FindNextFileW(hFindFile, &findData));

		::FindClose(hFindFile);
	}

	// adding back our previous loaded scripts
	for (auto& oldScriptData : oldVecScriptData)
	{
		for (auto& newScriptData : vecScriptData)
		{
			// if same script name appear in new scripts list
			if (CRT::StringCompare(oldScriptData.wszScriptName, newScriptData.wszScriptName) == 0)
			{
				// copy old script loaded data to new script data
				newScriptData.bLoaded = oldScriptData.bLoaded;
			}
		}
	}
}

bool LUA::Load(const std::size_t nFileIndex)
{
	if (nFileIndex == LUA_INVALID_INDEX)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("invalid script file index");
		return false;
	}

	// already loaded
	if (vecScriptData[nFileIndex].bLoaded)
	{
		L_PRINT(LOG_WARNING) << Q_XOR("script file: \"") << vecScriptData[nFileIndex].wszScriptName << Q_XOR("\" already loaded");
		return false;
	}

	const wchar_t* wszFileName = vecScriptData[nFileIndex].wszScriptName;

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszScriptPath), wszFileName);

	try
	{
		// convert path to unicode
		std::string szUnicodePath = CRT::StringUnicodeToMultiByte(wszFilePath);
		sol::state_view lua(pLuaState);
		lua.script_file(szUnicodePath);
	}
	catch (const sol::error& error)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to load script file: \"") << wszFileName << Q_XOR("\"") << Q_XOR("\n") << error.what();
		return false;
	}

	// change script loaded state
	vecScriptData[nFileIndex].bLoaded = true;

	L_PRINT(LOG_INFO) << Q_XOR("loaded script file: \"") << wszFileName << Q_XOR("\"");

	return true;
}

bool LUA::Unload(const std::size_t nFileIndex)
{
	if (nFileIndex == LUA_INVALID_INDEX)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("invalid script file index");
		return false;
	}

	// it's not loaded yet...
	if (!vecScriptData[nFileIndex].bLoaded)
	{
		L_PRINT(LOG_WARNING) << Q_XOR("script file: \"") << vecScriptData[nFileIndex].wszScriptName << Q_XOR("\" is not loaded");
		return false;
	}

	// remove event listener
	scriptEventHandler.Remove(nFileIndex);
	// change script loaded state
	vecScriptData[nFileIndex].bLoaded = false;

	L_PRINT(LOG_INFO) << Q_XOR("unloaded script file: \"") << vecScriptData[nFileIndex].wszScriptName << Q_XOR("\"");
	return true;
}

std::size_t LUA::GetScriptIndex(wchar_t* wszScriptName)
{
	for (std::size_t i = 0U; i < vecScriptData.size(); i++)
	{
		if (CRT::StringCompare(vecScriptData[i].wszScriptName, wszScriptName) == 0)
			return i;
	}

	return LUA_INVALID_INDEX;
}

#pragma endregion

#pragma region script_events
void LUA::ScriptEventHandler_t::Add(const char* szEventName, const int nScriptIndex, sol::protected_function fnCallback)
{
	this->mapEvents[szEventName].emplace_back(nScriptIndex, fnCallback);
}

void LUA::ScriptEventHandler_t::Remove(const int nScriptIndex)
{
	for (auto& [szScriptName, vecScriptEvents] : this->mapEvents)
	{
		if (vecScriptEvents[nScriptIndex].nScriptIndex == nScriptIndex)
			// erase the script event from the vector
			vecScriptEvents.erase(vecScriptEvents.begin() + vecScriptEvents[nScriptIndex].nScriptIndex);
	}
}

std::vector<LUA::ScriptEventData_t>& LUA::ScriptEventHandler_t::GetEvent(const char* szEventName)
{
	return this->mapEvents[szEventName];
}
#pragma endregion
