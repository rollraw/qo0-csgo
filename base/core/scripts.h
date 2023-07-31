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
	/// @returns: current script's name
	[[nodiscard]] std::size_t GetCurrentScriptIndex();

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
		ScriptEventData_t(int nScriptIndex, sol::protected_function fnCallback) :
			nScriptIndex(nScriptIndex), fnCallback(fnCallback)
		{};

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

	// UI namespace table wrapper for lua
	namespace UI
	{
		class CBaseMenuObject
		{
		public:
			virtual bool Render() = 0;

			[[nodiscard]] const bool IsVisible() const
			{
				return bVisible;
			}

			void SetVisible(const bool bVisible)
			{
				this->bVisible = bVisible;
			}

			void SetCallBack(sol::function fnCallback)
			{
				this->fnCallback = fnCallback;
			}

			void Callback()
			{
				if (fnCallback != sol::nil)
					fnCallback(this);
			}

			[[nodiscard]] const std::size_t GetID() const
			{
				return nID;
			}

			void SetID(const std::size_t nID)
			{
				this->nID = nID;
			}

			[[nodiscard]] const char* GetName() const
			{
				return szName;
			}

			void SetName(const char* szName);
		private:
			char szName[MAX_PATH];
			bool bVisible = true;
			std::size_t nID = -1;
			sol::function fnCallback = sol::nil;
		};

		CBaseMenuObject* AddNewCheckbox(const char* szLabel, bool bDefaultValue);
	}

	struct ScriptUIContext_t
	{
		void Render();

		UI::CBaseMenuObject* Add(wchar_t* wszContextName, UI::CBaseMenuObject* pObject);
		void Remove(wchar_t* wszContextName);

		[[nodiscard]] const bool IsEmpty() const
		{
			return mapContexts.empty();
		}
	private:
		std::unordered_map<wchar_t*, std::vector<UI::CBaseMenuObject*>> mapContexts = {};
	};

	/* @section: values */
	// all user script filenames
	inline std::vector<UserScriptData_t> vecScriptData = {};
	inline ScriptEventHandler_t scriptEventHandler = {};
	inline ScriptUIContext_t scriptUIContext = {};
}
