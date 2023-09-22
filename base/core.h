#pragma once

/*
 * CORE
 * - global variables and functions for various purposes
 */
namespace CORE
{
	/* @section: get */
	/// @param[out] wszDestination output for working path where files will be saved (default: "%userprofile%\documents\.qo0")
	/// @returns: true if successfully got the path, false otherwise
	bool GetWorkingPath(wchar_t* wszDestination);

	/* @section: values */
	// handle of process
	inline void* hProcess = nullptr;
	// handle of self module
	inline void* hDll = nullptr;
}
