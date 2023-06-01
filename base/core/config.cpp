// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "config.h"
// used: getworkingpath
#include "../core.h"
// used: l_print
#include "../utilities/log.h"
// used: integertostring
#include "../utilities/crt.h"
// used: heapalloc, heapfree
#include "../utilities/memory.h"

// used: formatter implementation
#ifdef Q_CONFIGURATION_INCLUDE
#include Q_CONFIGURATION_INCLUDE
#else
#include "../../extensions/binary.h"
#endif

// determine file extension for selected formatter
#if defined(Q_BINARY_EXTENSION)
#define C_EXTENSION Q_BINARY_EXTENSION
#elif defined(Q_JSON_EXTENSION)
#define C_EXTENSION Q_JSON_EXTENSION
#elif defined(Q_TOML_EXTENSION)
#define C_EXTENSION Q_TOML_EXTENSION
#endif

// default configurations working path
static wchar_t wszConfigurationsPath[MAX_PATH];

bool C::Setup(const wchar_t* wszDefaultFileName)
{
	if (!CORE::GetWorkingPath(wszConfigurationsPath))
		return false;

	CRT::StringCat(wszConfigurationsPath, Q_XOR(L"settings\\"));

	// create directory if it doesn't exist
	if (!::CreateDirectoryW(wszConfigurationsPath, nullptr))
	{
		if (::GetLastError() != ERROR_ALREADY_EXISTS)
		{
			L_PRINT(LOG_ERROR) << Q_XOR("failed to create configurations directory, because one or more intermediate directories don't exist");
			return false;
		}
	}

	// define custom data types we want to serialize
	AddUserType(FNV1A::HashConst("KeyBind_t"),
		{
			UserDataMember_t{ FNV1A::HashConst("uKey"), FNV1A::HashConst("unsigned int"), &KeyBind_t::uKey },
			UserDataMember_t{ FNV1A::HashConst("nMode"), FNV1A::HashConst("int"), &KeyBind_t::nMode }
		});

	// store existing configurations list
	Refresh();

	// create default configuration
	if (!CreateFile(wszDefaultFileName))
		return false;

	return true;
}

#pragma region config_main
void C::Refresh()
{
	// clear previous stored file names
	for (wchar_t* wszFileName : vecFileNames)
		MEM::HeapFree(wszFileName);
	vecFileNames.clear();

	// make configuration files path filter
	wchar_t wszPathFilter[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszPathFilter, wszConfigurationsPath), Q_XOR(L"*" C_EXTENSION));

	// iterate through all files with our filter
	WIN32_FIND_DATAW findData;
	if (const HANDLE hFindFile = ::FindFirstFileW(wszPathFilter, &findData); hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			vecFileNames.push_back(static_cast<wchar_t*>(MEM::HeapAlloc((CRT::StringLength(findData.cFileName) + 1U) * sizeof(wchar_t))));
			CRT::StringCopy(vecFileNames.back(), findData.cFileName);

			L_PRINT(LOG_INFO) << Q_XOR("found configuration file: \"") << findData.cFileName << Q_XOR("\"");
		} while (::FindNextFileW(hFindFile, &findData));

		::FindClose(hFindFile);
	}
}

void C::AddUserType(const FNV1A_t uTypeHash, const std::initializer_list<UserDataMember_t> vecUserMembers)
{
	if (vecUserMembers.size() == 0U)
		return;

	UserDataType_t userDataType;
	userDataType.uTypeHash = uTypeHash;

	for (const auto& userDataMember : vecUserMembers)
		userDataType.vecMembers.push_back(userDataMember);

	vecUserTypes.emplace_back(CRT::Move(userDataType));
}

bool C::SaveFileVariable(const std::size_t nFileIndex, const VariableObject_t& variable)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

#if defined(Q_BINARY_EXTENSION)
	if (BIN::SaveVariable(wszFilePath, variable))
#elif defined(Q_JSON_EXTENSION)
	if (JSON::SaveVariable(wszFilePath, variable))
#elif defined(Q_TOML_EXTENSION)
	if (TOML::SaveVariable(wszFilePath, variable))
#endif
	{
		return true;
	}

	return false;
}

bool C::LoadFileVariable(const std::size_t nFileIndex, VariableObject_t& variable)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

#if defined(Q_BINARY_EXTENSION)
	if (BIN::LoadVariable(wszFilePath, variable))
#elif defined(Q_JSON_EXTENSION)
	if (JSON::LoadVariable(wszFilePath, variable))
#elif defined(Q_TOML_EXTENSION)
	if (TOML::LoadVariable(wszFilePath, variable))
#endif
	{
		return true;
	}

	return false;
}

bool C::RemoveFileVariable(const std::size_t nFileIndex, const VariableObject_t& variable)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

#if defined(Q_BINARY_EXTENSION)
	if (BIN::RemoveVariable(wszFilePath, variable))
#elif defined(Q_JSON_EXTENSION)
	if (JSON::RemoveVariable(wszFilePath, variable))
#elif defined(Q_TOML_EXTENSION)
	if (TOML::RemoveVariable(wszFilePath, variable))
#endif
	{
		return true;
	}

	return false;
}

bool C::CreateFile(const wchar_t* wszFileName)
{
	wchar_t wszFilePath[MAX_PATH];
	wchar_t* wszFilePathEnd = CRT::StringCopy(wszFilePath, wszConfigurationsPath);

	// check is file extension already correct
	if (const wchar_t* wszFileExtension = CRT::StringCharR(wszFileName, L'.'); wszFileExtension != nullptr && CRT::StringCompare(wszFileExtension, Q_XOR(C_EXTENSION)) == 0)
		wszFilePathEnd = CRT::StringCat(wszFilePathEnd, wszFileName);
	// file extension is either not set or incorrect
	else
	{
		if (wszFileExtension != nullptr)
			wszFilePathEnd = CRT::StringCatN(wszFilePathEnd, wszFileName, wszFileExtension - wszFileName);
		else
			wszFilePathEnd = CRT::StringCat(wszFilePathEnd, wszFileName);

		wszFilePathEnd = CRT::StringCat(wszFilePathEnd, Q_XOR(C_EXTENSION));
	}

	// add file to the list
	const wchar_t* wszFileNameStripped = CRT::StringCharR(wszFilePath, '\\') + 1U;
	wchar_t* wszFullFileName = static_cast<wchar_t*>(MEM::HeapAlloc((wszFilePathEnd - wszFileNameStripped + 1U) * sizeof(wchar_t)));
	CRT::StringCopy(wszFullFileName, wszFileNameStripped);
	vecFileNames.emplace_back(wszFullFileName);

	// create and save it by the index
	if (SaveFile(vecFileNames.size() - 1U))
	{
		L_PRINT(LOG_INFO) << Q_XOR("created configuration file: \"") << wszFullFileName << Q_XOR("\"");
		return true;
	}

	L_PRINT(LOG_WARNING) << Q_XOR("failed to create configuration file: \"") << wszFullFileName << Q_XOR("\"");
	return false;
}

bool C::SaveFile(const std::size_t nFileIndex)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

#if defined(Q_BINARY_EXTENSION)
	if (BIN::SaveFile(wszFilePath))
#elif defined(Q_JSON_EXTENSION)
	if (JSON::SaveFile(wszFilePath))
#elif defined(Q_TOML_EXTENSION)
	if (TOML::SaveFile(wszFilePath))
#endif
	{
		L_PRINT(LOG_INFO) << Q_XOR("saved configuration file: \"") << wszFileName << Q_XOR("\"");
		return true;
	}

	L_PRINT(LOG_WARNING) << Q_XOR("failed to save configuration file: \"") << wszFileName << Q_XOR("\"");
	return false;
}

bool C::LoadFile(const std::size_t nFileIndex)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

#if defined(Q_BINARY_EXTENSION)
	if (BIN::LoadFile(wszFilePath))
#elif defined(Q_JSON_EXTENSION)
	if (JSON::LoadFile(wszFilePath))
#elif defined(Q_TOML_EXTENSION)
	if (TOML::LoadFile(wszFilePath))
#endif
	{
		L_PRINT(LOG_INFO) << Q_XOR("loaded configuration file: \"") << wszFileName << Q_XOR("\"");
		return true;
	}

	L_PRINT(LOG_WARNING) << Q_XOR("failed to load configuration file: \"") << wszFileName << Q_XOR("\"");
	return false;
}

void C::RemoveFile(const std::size_t nFileIndex)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	// unable to delete default config
	if (CRT::StringCompare(wszFileName, Q_XOR(L"default" C_EXTENSION)) == 0)
		return;

	wchar_t wszFilePath[MAX_PATH];
	CRT::StringCat(CRT::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

	if (::DeleteFileW(wszFilePath))
	{
		vecFileNames.erase(vecFileNames.cbegin() + nFileIndex);
		L_PRINT(LOG_INFO) << Q_XOR("removed configuration file: \"") << wszFileName << Q_XOR("\"");
	}
}
#pragma endregion

#pragma region config_get
std::size_t C::GetVariableIndex(const FNV1A_t uNameHash)
{
	for (std::size_t i = 0U; i < vecVariables.size(); i++)
	{
		if (vecVariables[i].uNameHash == uNameHash)
			return i;
	}

	return C_INVALID_VARIABLE;
}
#pragma endregion
