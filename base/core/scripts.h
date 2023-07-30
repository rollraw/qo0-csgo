#pragma once
#include "../common.h"

// used: std::unordered_map
#include <unordered_map>

// @note: should make a macro for this eg: Q_USE_LUAJIT...
// used: sol2 & luajit
#define SOL_USE_CXX_LUAJIT
#include "../dependencies/sol2.h"

#pragma region script_definitions
#define LUA_INVALID_INDEX static_cast<std::size_t>(-1)
#pragma endregion

namespace LUA
{
	// create directories and our lua state
	bool Setup();
	void SetupLua();
	/// release our lua state
	void Destroy();

	/* @section: main */
	/// loop through directory content and store all user scripts filenames
	void Refresh();
	/// @param[in] nFileIndex index of the exist script file name
	/// @returns: true if we successfully loaded the script, false otherwise
	bool Load(const std::size_t nFileIndex);
	/// @param[in] nFileIndex index of the exist script file name
	/// @returns: true if we successfully unloaded the script, false otherwise
	bool Unload(const std::size_t nFileIndex);
	/// @param[in] wszScriptName name of the script
	/// @returns: script's data index if found, LUA_INVALID_INDEX otherwise
	[[nodiscard]] std::size_t GetScriptIndex(wchar_t* wszScriptName);

	// script info
	struct UserScriptData_t
	{
		constexpr UserScriptData_t(wchar_t* wszScriptName) :
			wszScriptName(wszScriptName), bLoaded(false)
		{ }

		// script's name
		wchar_t* wszScriptName = nullptr;
		// script's loaded state
		bool bLoaded = false;
		// @note: not used yet, but will be used for script's load on start
		// bool bLoadOnStart = false;
	};


	// @section: script events
	// event data
	struct ScriptEventData_t
	{
		int nScriptIndex = -1;
		sol::protected_function fnCallback;
	};

	// event manager
	struct ScriptEventHandler_t
	{
		/// @param[in] szEventName name of the event
		/// @param[in] nScriptIndex index of the exist script file name
		/// @param[in] fnCallback function to call when event is triggered
		void Add(const char* szEventName, const int nScriptIndex, sol::protected_function fnCallback);
		/// @param[in] nScriptIndex index of the exist script file name
		void Remove(const int nScriptIndex);

		/// @param[in] szEventName name of the event
		/// @returns: vector of all scripts that are listening to this event
		std::vector<ScriptEventData_t>& GetEvent(const char* szEventName);
	private:
		std::unordered_map<const char*, std::vector<ScriptEventData_t>> mapEvents = {};
	};

	/* @section: values */
	// all user script filenames
	inline std::vector<UserScriptData_t> vecScriptData = {};
	inline ScriptEventHandler_t scriptEventHandler = {};
}
