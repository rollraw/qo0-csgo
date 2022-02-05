#pragma once
// used: std::vector for getfilledvector
#include <vector>
// used: winapi, fmt includes
#include "../common.h"

#pragma region memory_modules_definitions
#define ENGINE_DLL				XorStr("engine.dll")
#define CLIENT_DLL				XorStr("client.dll")
#define LOCALIZE_DLL			XorStr("localize.dll")
#define MATERIALSYSTEM_DLL		XorStr("materialsystem.dll")
#define VGUI_DLL				XorStr("vguimatsurface.dll")
#define VGUI2_DLL				XorStr("vgui2.dll")
#define SHADERPIDX9_DLL			XorStr("shaderapidx9.dll")
#define GAMEOVERLAYRENDERER_DLL XorStr("gameoverlayrenderer.dll")
#define PHYSICS_DLL				XorStr("vphysics.dll")
#define VSTDLIB_DLL				XorStr("vstdlib.dll")
#define TIER0_DLL				XorStr("tier0.dll")
#define INPUTSYSTEM_DLL			XorStr("inputsystem.dll")
#define STUDIORENDER_DLL		XorStr("studiorender.dll")
#define DATACACHE_DLL			XorStr("datacache.dll")
#define STEAM_API_DLL			XorStr("steam_api.dll")
#define MATCHMAKING_DLL			XorStr("matchmaking.dll")
#define SERVER_DLL				XorStr("server.dll")
#define SERVERBROWSER_DLL		XorStr("serverbrowser.dll")
#pragma endregion

/*
 * singleton implementation
 * restricts the instantiation of a class to one single class instance
 */
template <typename T>
class CSingleton
{
protected:
	CSingleton() { }
	~CSingleton() { }

	CSingleton(const CSingleton&) = delete;
	CSingleton& operator=(const CSingleton&) = delete;

	CSingleton(CSingleton&&) = delete;
	CSingleton& operator=(CSingleton&&) = delete;
public:
	static T& Get()
	{
		static T pInstance{ };
		return pInstance;
	}
};

/* virtualprotect raii wrapper */
class CWrappedProtect
{
public:
	CWrappedProtect(void* pBaseAddress, const std::size_t uLength, const DWORD dwFlags) :
		pBaseAddress(pBaseAddress), uLength(uLength)
	{
		if (!VirtualProtect(pBaseAddress, uLength, dwFlags, &dwOldFlags))
			throw std::system_error(GetLastError(), std::system_category(), XorStr("failed to protect region"));
	}

	~CWrappedProtect()
	{
		VirtualProtect(pBaseAddress, uLength, dwOldFlags, &dwOldFlags);
	}
private:
	void*			pBaseAddress;
	std::size_t		uLength;
	DWORD			dwOldFlags;
};

/*
 * MEMORY
 * memory management functions
 */
namespace MEM
{
	/*
	 * @section: get
	 */
	/// ida style pattern byte comparison in specific module
	/// @param szModuleName : module name where search for pattern
	/// @param szPattern : ida style pattern, e.g. "55 8B 40 ? 30", wildcard can be either '?' or "??", blank delimiters are ignored
	std::uintptr_t FindPattern(const std::string_view szModuleName, const std::string_view szPattern);
	/// ida style pattern byte comparison in specific region
	/// @param szPattern : ida style pattern, e.g. "55 8B 40 ? 30", wildcard can be either '?' or "??", blank delimiters are ignored
	std::uintptr_t FindPattern(const std::uint8_t* uRegionStart, const std::uintptr_t uRegionSize, const std::string_view szPattern);
	/// @param szModuleName : module name to search base handle for, empty means current module
	/// @returns : base handle of given module name
	void* GetModuleBaseHandle(const std::string_view szModuleName);
	/// alternative of 'GetProcAddress()'
	/// @returns : export procedure address
	void* GetExportAddress(const void* pModuleBase, const std::string_view szProcedureName);
	/// @returns : all cross-references (x-refs) addresses in given range
	std::vector<std::uintptr_t> GetCrossReferences(const std::uintptr_t uAddress, std::uintptr_t uRegionStart, const std::size_t uRegionSize);
	/// @param szSectionName : section to get info of (e.g. ".rdata", ".text", etc)
	/// @param puSectionStart : output for section start address
	/// @param puSectionSize : output for section size
	/// @returns : true if section has been found
	bool GetSectionInfo(const std::uintptr_t uBaseAddress, const std::string_view szSectionName, std::uintptr_t* puSectionStart, std::uintptr_t* puSectionSize);
	/// @returns : type descriptor address of given vtable name in given module
	std::uintptr_t GetVTableTypeDescriptor(const std::string_view szModuleName, const std::string_view szTableName);
	/// @returns : pointer of given vtable name in given module @credits: hinnie
	std::uintptr_t* GetVTablePointer(const std::string_view szModuleName, const std::string_view szTableName);
	/// @returns : vector filled by given value
	// @todo: move it outside of mem
	template <typename T, std::size_t S>
	std::vector<T> GetFilledVector(const T& fill)
	{
		std::vector<T> vecTemp(S);
		std::fill(vecTemp.begin(), vecTemp.begin() + S, fill);
		return vecTemp;
	}
	/// @returns : virtual function pointer of specified class at given index
	template <typename T = void*>
	constexpr T GetVFunc(void* thisptr, std::size_t nIndex)
	{
		return (*static_cast<T**>(thisptr))[nIndex];
	}
	// call virtual function of specified class at given index
	// @note: references should be wrapped with std::ref call!
	template <typename T, typename ... Args_t>
	constexpr T CallVFunc(void* thisptr, std::size_t nIndex, Args_t... argList)
	{
		using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<VirtualFn**>(thisptr))[nIndex](thisptr, argList...);
	}

	/*
	 * @section: convert
	 */
	std::vector<std::optional<std::uint8_t>> PatternToBytes(const std::string_view szPattern);
	std::string BytesToPattern(const std::uint8_t* arrBytes, const std::size_t uSize);

	/*
	 * @section: check
	 */
	// checks do we can we read/readwrite given memory region
	bool IsValidCodePtr(const void* pPointer);
}
