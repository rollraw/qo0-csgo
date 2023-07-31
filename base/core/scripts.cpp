// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "scripts.h"

// used: getworkingpath
#include "../core.h"
// used: config add var
#include "config.h"
// used: l_print
#include "../utilities/log.h"
// used: integertostring, strcat...
#include "../utilities/crt.h"

#pragma comment(lib, "lua51.lib")
#pragma comment(lib, "luajit.lib")

// default scripts working path
static wchar_t wszScriptPath[MAX_PATH];
// sol2's lua state
static lua_State* pLuaState;

#pragma region script_context

void LUA::ScriptUIContext_t::Render()
{
	for (auto& [wszFileName, ctx] : this->mapContexts)
	{
		for (auto& pObject : ctx)
		{
			// skip invisible objects
			if (!pObject->bVisible)
				continue;

			if (pObject->Render())
				pObject->Callback();
		}
	}
}

LUA::UI::CBaseMenuObject* LUA::ScriptUIContext_t::Add(wchar_t* wszContextName, UI::CBaseMenuObject* pObject)
{
	for (auto& ctx : this->mapContexts[wszContextName])
	{
		if (ctx == pObject)
			return ctx;
	}

	this->mapContexts[wszContextName].emplace_back(pObject);
	return pObject;
}

void LUA::ScriptUIContext_t::Remove(wchar_t* wszContextName)
{
	this->mapContexts[wszContextName].clear();
}

#pragma endregion

#pragma region script_functions

namespace LUA
{
	// ui table
	namespace UI
	{
		// return lua object for value
		sol::object LUA::UI::CBaseMenuObject::GetValue(sol::this_state state)
		{
			// not configurable object aka Button, Label, etc...
			if (nID == C_INVALID_VARIABLE)
				return sol::nil;

			/// we already have the config ID, just search for its type and return the value as lua object
			switch (C::vecVariables[nID].uTypeHash)
			{
			case FNV1A::HashConst("bool"):
			{
				return sol::make_object(state, C::Get<bool>(nID));
			}
			case FNV1A::HashConst("int"):
			{
				return sol::make_object(state, C::Get<int>(nID));
			}
			case FNV1A::HashConst("unsigned int"):
			{
				return sol::make_object(state, C::Get<unsigned int>(nID));
			}
			case FNV1A::HashConst("float"):
			{
				return sol::make_object(state, C::Get<float>(nID));
			}
			case FNV1A::HashConst("Color_t"):
			{
				return sol::make_object(state, C::Get<Color_t>(nID));
			}
			case FNV1A::HashConst("char[]"):
			{
				return sol::make_object(state, C::Get<const char*>(nID));
			}
			default:
				// if we didn't find the type, return nil
				// shouldnt be a case
				return sol::nil;
			}
		}

		/// putting here in cpp as we don't use this class outside of this scope...
		class CCheckboxObject : public CBaseMenuObject
		{
		public:
			CCheckboxObject(const char* szName, bool bDefaultValue)
			{
				CRT::StringCopy(this->szName, szName);
				this->nID = C::AddVariable<bool>(FNV1A::Hash(szName), FNV1A::HashConst("bool"), bDefaultValue);
			}

			bool Render() final
			{
				return ImGui::Checkbox(szName, &C::Get<bool>(nID));
			}
		};

		CBaseMenuObject* AddNewCheckbox(const char* szName, bool bDefaultValue)
		{
			auto nIndex = LUA::GetCurrentScriptIndex();
			auto wszFileName = LUA::vecScriptData[nIndex].wszScriptName;

			char szFileName[MAX_PATH] = {};
			CRT::StringUnicodeToMultiByte(szFileName, Q_ARRAYSIZE(szFileName), wszFileName);

			char szLuaLabel[MAX_PATH];
			CRT::StringCat(CRT::StringCopy(szLuaLabel, szName), Q_XOR("##"));
			CRT::StringCat(szLuaLabel, szFileName);
			return LUA::scriptUIContext.Add(LUA::vecScriptData[nIndex].wszScriptName, new CCheckboxObject(szLuaLabel, bDefaultValue));
		}

		template<typename T>
		class CSliderObject : public CBaseMenuObject
		{
		public:
			CSliderObject(const char* szName, T valueDefault, T valueMin, T valueMax, const char* szFormat = nullptr)
			{
				static_assert(std::is_same<T, int>::value || std::is_same<T, float>::value && "type must be either int or float.");

				CRT::StringCopy(this->szName, szName);
				CRT::StringCopy(this->szFormat, szFormat);

				this->nID = C::AddVariable<T>(FNV1A::Hash(szName), FNV1A::Hash(typeid(T).name()), valueDefault);
				this->valueMax = valueMax;
				this->valueMin = valueMin;
			}

			bool Render() final
			{
				if (std::is_same<T, int>::value)
					return ImGui::SliderInt(szName, &C::Get<int>(nID), valueMin, valueMax, szFormat[0] == '\0' ? Q_XOR("%d") : szFormat);
				else
					return ImGui::SliderFloat(szName, &C::Get<float>(nID), valueMin, valueMax, szFormat[0] == '\0' ? Q_XOR("%.2f") : szFormat);
			}

			T valueMin;
			T valueMax;
			char szFormat[32];
		};

		CBaseMenuObject* AddNewSliderInt(const char* szName, int nValueDefault, int nValueMin, int nValueMax, sol::optional<const char*> szFormat)
		{
			auto nIndex = LUA::GetCurrentScriptIndex();
			auto wszFileName = LUA::vecScriptData[nIndex].wszScriptName;

			char szFileName[MAX_PATH] = {};
			CRT::StringUnicodeToMultiByte(szFileName, Q_ARRAYSIZE(szFileName), wszFileName);

			char szLuaLabel[MAX_PATH];
			CRT::StringCat(CRT::StringCopy(szLuaLabel, szName), Q_XOR("##"));
			CRT::StringCat(szLuaLabel, szFileName);

			return LUA::scriptUIContext.Add(LUA::vecScriptData[nIndex].wszScriptName, new CSliderObject<int>(szLuaLabel, nValueDefault, nValueMin, nValueMax, szFormat.value_or(Q_XOR("%d"))));
		}

		CBaseMenuObject* AddNewSliderFloat(const char* szName, float flvalueDefault, float flValueMin, float flValueMax, sol::optional<const char*> szFormat)
		{
			auto nIndex = LUA::GetCurrentScriptIndex();
			auto wszFileName = LUA::vecScriptData[nIndex].wszScriptName;

			char szFileName[MAX_PATH] = {};
			CRT::StringUnicodeToMultiByte(szFileName, Q_ARRAYSIZE(szFileName), wszFileName);

			char szLuaLabel[MAX_PATH];
			CRT::StringCat(CRT::StringCopy(szLuaLabel, szName), Q_XOR("##"));
			CRT::StringCat(szLuaLabel, szFileName);

			return LUA::scriptUIContext.Add(LUA::vecScriptData[nIndex].wszScriptName, new CSliderObject<float>(szLuaLabel, flvalueDefault, flValueMin, flValueMax, szFormat.value_or(Q_XOR("%.2f"))));
		}

		class CButtonObject : public CBaseMenuObject
		{
		public:
			CButtonObject(const char* szName, sol::function fnCallback, const ImVec2& vecSize) :
				vecSize(vecSize)
			{
				CRT::StringCopy(this->szName, szName);
				this->fnCallback = fnCallback;
			}

			bool Render() final
			{
				return ImGui::Button(szName, vecSize);
			}

			ImVec2 vecSize;
		};

		CBaseMenuObject* AddNewButton(const char* szName, sol::function fnCallback, sol::optional<ImVec2> vecSize)
		{
			auto nIndex = LUA::GetCurrentScriptIndex();
			auto wszFileName = LUA::vecScriptData[nIndex].wszScriptName;

			char szFileName[MAX_PATH] = {};
			CRT::StringUnicodeToMultiByte(szFileName, Q_ARRAYSIZE(szFileName), wszFileName);

			char szLuaLabel[MAX_PATH];
			CRT::StringCat(CRT::StringCopy(szLuaLabel, szName), Q_XOR("##"));
			CRT::StringCat(szLuaLabel, szFileName);

			return LUA::scriptUIContext.Add(LUA::vecScriptData[nIndex].wszScriptName, new CButtonObject(szLuaLabel, fnCallback, vecSize.value_or(ImVec2(0, 0))));
		}

	}
}

#pragma endregion

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
	if (pLuaState != nullptr)
		return;

	// create lua state
	pLuaState = luaL_newstate();
	luaL_openlibs(pLuaState);

	// open sol2's libraries
	sol::state_view luaState(pLuaState);
	luaState.open_libraries(sol::lib::bit32, sol::lib::jit, sol::lib::ffi, sol::lib::math, sol::lib::string, sol::lib::package, sol::lib::table, sol::lib::debug);

	// override print function
	luaState[Q_XOR("print")] = [](const char* szText) -> void
	{
		L_PRINT(LOG_INFO) << szText;
	};
	// override error function
	luaState[Q_XOR("error")] = [](const char* szText) -> void
	{
		L_PRINT(LOG_ERROR) << szText;
	};

	luaState.new_usertype<UI::CBaseMenuObject>(Q_XOR("menu_object_t"),
	Q_XOR("name"), sol::readonly(&UI::CBaseMenuObject::szName),
	Q_XOR("id"), sol::readonly(&UI::CBaseMenuObject::nID),
	Q_XOR("is_visible"), &UI::CBaseMenuObject::bVisible,
	Q_XOR("set_visible"), &UI::CBaseMenuObject::SetVisible,
	Q_XOR("set_callback"), &UI::CBaseMenuObject::SetCallBack,
	Q_XOR("get"), &UI::CBaseMenuObject::GetValue
	);

	auto ui = luaState.create_table();
	ui[Q_XOR("add_new_checkbox")] = UI::AddNewCheckbox;
	ui[Q_XOR("add_new_slider")] = sol::overload(UI::AddNewSliderInt, UI::AddNewSliderFloat);
	luaState[Q_XOR("ui")] = ui;

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

	// change script loaded state
	vecScriptData[nFileIndex].bLoaded = true;

	const wchar_t* wszFileName = vecScriptData[nFileIndex].wszScriptName;
	L_PRINT(LOG_INFO) << Q_XOR("loaded script file: \"") << wszFileName << Q_XOR("\"");

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszScriptPath), wszFileName);
	std::string szUnicodePath = CRT::StringUnicodeToMultiByte(wszFilePath);
	sol::state_view lua(pLuaState);

	lua.script_file(szUnicodePath, [&nFileIndex, &szUnicodePath](lua_State* state, sol::protected_function_result result)
	{
		if (!result.valid())
		{
			sol::error err = result;
			L_PRINT(LOG_ERROR) << Q_XOR("failed to load script file: \"") << szUnicodePath.c_str() << Q_XOR("\"") << Q_XOR("\n") << err.what();
			LUA::Unload(nFileIndex);
		}

		return result;
	});


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
	// remove ui context
	scriptUIContext.Remove(vecScriptData[nFileIndex].wszScriptName);

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

std::size_t LUA::GetCurrentScriptIndex()
{
	sol::state_view state_view(pLuaState);
	sol::optional<sol::table> info = state_view["debug"]["getinfo"](2, "S");

	if (info && info->valid())
	{
		sol::optional<std::wstring> source = (*info)["short_src"];
		if (source && !source->empty())
		{
			auto str = source.value();

			for (size_t i = 0; i < vecScriptData.size(); i++)
			{
				if (CRT::StringString(vecScriptData[i].wszScriptName, str.data()) == 0)
				{
					L_PRINT(LOG_INFO) << Q_XOR("current script index: ") << i << Q_XOR(" source: \"") << str.data() << Q_XOR("\"");
					return i;
				}
			}
		}
		else
		{
			L_PRINT(LOG_ERROR) << "debug library returned invalid source.";
		}
	}
	else
	{
		L_PRINT(LOG_ERROR) << Q_XOR("debug library is not available.");
	}

	L_PRINT(LOG_WARNING) << Q_XOR("failed to get current script index");
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
