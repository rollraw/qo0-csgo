#pragma once
// used: [ext] toml parser implementation
#include <toml/toml.hpp>

#define Q_TOML_EXTENSION L".toml"

namespace C::TOML
{
	/* @section: [internal] */
	// write single variable to buffer
	inline void WriteBuffer(toml::value& entry, const VariableObject_t& variable)
	{
		// write is different for variable types
		switch (variable.uTypeHash)
		{
		case FNV1A::HashConst("bool"):
		{
			entry = *variable.GetStorage<bool>();
			break;
		}
		case FNV1A::HashConst("int"):
		{
			entry = *variable.GetStorage<int>();
			break;
		}
		case FNV1A::HashConst("unsigned int"):
		{
			entry = *variable.GetStorage<unsigned int>();
			break;
		}
		case FNV1A::HashConst("float"):
		{
			entry = *variable.GetStorage<float>();
			break;
		}
		case FNV1A::HashConst("Color_t"):
		{
			const Color_t* pColorValue = variable.GetStorage<Color_t>();
			entry = toml::array{ pColorValue->r, pColorValue->g, pColorValue->b, pColorValue->a };
			break;
		}
		case FNV1A::HashConst("char[]"):
		{
			entry = std::string(*variable.GetStorage<char[], false>());
			break;
		}
		case FNV1A::HashConst("bool[]"):
		{
			const std::size_t nArraySize = variable.nStorageSize / sizeof(bool);
			const auto& arrValues = *variable.GetStorage<bool[], false>();

			toml::array arrBools;
			for (std::size_t i = 0U; i < nArraySize; i++)
				arrBools.push_back(arrValues[i]);

			entry = arrBools;
			break;
		}
		case FNV1A::HashConst("int[]"):
		{
			const std::size_t nArraySize = variable.nStorageSize / sizeof(int);
			const auto& arrValues = *variable.GetStorage<int[], false>();

			toml::array arrInts;
			for (std::size_t i = 0U; i < nArraySize; i++)
				arrInts.push_back(arrValues[i]);

			entry = arrInts;
			break;
		}
		case FNV1A::HashConst("unsigned int[]"):
		{
			const std::size_t nArraySize = variable.nStorageSize / sizeof(unsigned int);
			const auto& arrValues = *variable.GetStorage<unsigned int[], false>();

			toml::array arrUInts;
			for (std::size_t i = 0U; i < nArraySize; i++)
				arrUInts.push_back(arrValues[i]);

			entry = arrUInts;
			break;
		}
		case FNV1A::HashConst("float[]"):
		{
			const std::size_t nArraySize = variable.nStorageSize / sizeof(float);
			const auto& arrValues = *variable.GetStorage<float[], false>();

			toml::array arrFloats;
			for (std::size_t i = 0U; i < nArraySize; i++)
				arrFloats.push_back(arrValues[i]);

			entry = arrFloats;
			break;
		}
		case FNV1A::HashConst("char[][]"):
		{
			// @test: very ugh
			std::string strValue(variable.nStorageSize, '\0');
			CRT::MemoryCopy(strValue.data(), variable.GetStorage<char*[], false>(), variable.nStorageSize);
			entry = strValue;
			break;
		}
		// @todo: temporary while custom structs are WIP
		case FNV1A::HashConst("KeyBind_t"):
		{
			const KeyBind_t* pKeyValue = variable.GetStorage<KeyBind_t>();
			entry = toml::array{ pKeyValue->uKey, static_cast<int>(pKeyValue->nMode) };
			break;
		}
		default:
			break;
		}
	}

	// read single variable from buffer
	inline void ReadBuffer(toml::value& entry, VariableObject_t& variable)
	{
		switch (variable.uTypeHash)
		{
		case FNV1A::HashConst("bool"):
		{
			const bool bValue = toml::get<bool>(entry);
			variable.SetStorage(&bValue);
			break;
		}
		case FNV1A::HashConst("int"):
		{
			const int iValue = toml::get<int>(entry);
			variable.SetStorage(&iValue);
			break;
		}
		case FNV1A::HashConst("unsigned int"):
		{
			const unsigned int uValue = toml::get<unsigned int>(entry);
			variable.SetStorage(&uValue);
			break;
		}
		case FNV1A::HashConst("float"):
		{
			const float flValue = toml::get<float>(entry);
			variable.SetStorage(&flValue);
			break;
		}
		case FNV1A::HashConst("Color_t"):
		{
			const auto& arrColor = entry.as_array();

			Color_t colValue;
			for (std::uint8_t i = 0U; i < arrColor.size(); i++)
				colValue[i] = toml::get<std::uint8_t>(arrColor[i]);

			variable.SetStorage(&colValue);
			break;
		}
		case FNV1A::HashConst("char[]"):
		{
			const std::string strValue = toml::get<std::string>(entry);
			Q_ASSERT((strValue.size() + 1U) * sizeof(char) <= variable.nStorageSize); // source size is bigger than destination size
			variable.SetStorage(strValue.c_str());
			break;
		}
		case FNV1A::HashConst("bool[]"):
		{
			const auto& arrBools = entry.as_array();
			Q_ASSERT(arrBools.size() * sizeof(bool) <= variable.nStorageSize); // source size is bigger than destination size

			bool* arrValues = *variable.GetStorage<bool*, false>();
			for (std::size_t i = 0U; i < arrBools.size(); i++)
				arrValues[i] = toml::get<bool>(arrBools[i]);

			break;
		}
		case FNV1A::HashConst("int[]"):
		{
			const auto& arrInts = entry.as_array();
			Q_ASSERT(arrInts.size() * sizeof(int) <= variable.nStorageSize); // source size is bigger than destination size

			int* arrValues = *variable.GetStorage<int*, false>();
			for (std::size_t i = 0U; i < arrInts.size(); i++)
				arrValues[i] = toml::get<int>(arrInts[i]);

			break;
		}
		case FNV1A::HashConst("unsigned int[]"):
		{
			const auto& arrUInts = entry.as_array();
			Q_ASSERT(arrUInts.size() * sizeof(unsigned int) <= variable.nStorageSize); // source size is bigger than destination size

			unsigned int* arrValues = *variable.GetStorage<unsigned int*, false>();
			for (std::size_t i = 0U; i < arrUInts.size(); i++)
				arrValues[i] = toml::get<unsigned int>(arrUInts[i]);

			break;
		}
		case FNV1A::HashConst("float[]"):
		{
			const auto& arrFloats = entry.as_array();
			Q_ASSERT(arrFloats.size() * sizeof(float) <= variable.nStorageSize); // source size is bigger than destination size

			float* arrValues = *variable.GetStorage<float*, false>();
			for (std::size_t i = 0U; i < arrFloats.size(); i++)
				arrValues[i] = toml::get<float>(arrFloats[i]);

			break;
		}
		case FNV1A::HashConst("char[][]"):
		{
			// @test: very ugh
			const std::string strValue = toml::get<std::string>(entry);
			Q_ASSERT((strValue.size() + 1U) * sizeof(char) <= variable.nStorageSize); // source size is bigger than destination size
			variable.SetStorage(strValue.data());
			break;
		}
		// @todo: temporary while custom structs are WIP
		case FNV1A::HashConst("KeyBind_t"):
		{
			const auto& arrKeyBind = entry.as_array();

			KeyBind_t* pKeyValue = variable.GetStorage<KeyBind_t>();
			pKeyValue->uKey = toml::get<unsigned int>(arrKeyBind[0]);
			pKeyValue->nMode = static_cast<EKeyBindMode>(toml::get<int>(arrKeyBind[1]));
			break;
		}
		default:
			break;
		}
	}

	/* @section: main */
	inline bool SaveVariable(const wchar_t* wszFilePath, const VariableObject_t& variable)
	{
		const HANDLE hFileOut = ::CreateFileW(wszFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileOut == INVALID_HANDLE_VALUE)
			return false;

		toml::value root = toml::parse(wszFilePath);

		char szKeyBuffer[CRT::IntegerToString_t<FNV1A_t, 16U>::MaxCount()] = { };
		const char* szVariableHash = CRT::IntegerToString(variable.uNameHash, szKeyBuffer, sizeof(szKeyBuffer), 16);

		WriteBuffer(root[szVariableHash], variable);
		
		// write re-serialized configuration to file
		const std::string strSerialized = toml::format(root);
		const BOOL bWritten = ::WriteFile(hFileOut, strSerialized.data(), strSerialized.size(), nullptr, nullptr);
		::CloseHandle(hFileOut);

		return bWritten;
	}

	inline bool LoadVariable(const wchar_t* wszFilePath, VariableObject_t& variable)
	{
		toml::value root = toml::parse(wszFilePath);

		char szHashBuffer[CRT::IntegerToString_t<FNV1A_t, 16U>::MaxCount()] = { };
		const char* szVariableHash = CRT::IntegerToString(variable.uNameHash, szHashBuffer, sizeof(szHashBuffer), 16);

		if (root.contains(szVariableHash))
			ReadBuffer(root[szVariableHash], variable);
		else // @todo: we should check for version at first
		{
			WriteBuffer(root[szVariableHash], variable);

			// overwrite version
			szVariableHash = CRT::IntegerToString(FNV1A::HashConst("version"), szHashBuffer, sizeof(szHashBuffer), 16);
			root[szVariableHash] = Q_VERSION;

			// @todo: reserialize and write to file
		}

		return true;
	}

	inline bool RemoveVariable(const wchar_t* wszFilePath, const VariableObject_t& variable)
	{
		// @todo:
		return false;
	}

	inline bool SaveFile(const wchar_t* wszFilePath)
	{
		const HANDLE hFileOut = ::CreateFileW(wszFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileOut == INVALID_HANDLE_VALUE)
			return false;

		char szHashBuffer[_MAX_ULTOSTR_BASE16_COUNT] = { };
		const char* szVariableHash = CRT::IntegerToString(FNV1A::HashConst("version"), szHashBuffer, sizeof(szHashBuffer), 16);

		toml::value root;

		// put current cheat build number
		WriteBuffer(root[szVariableHash], VariableObject_t{ FNV1A::HashConst("version"), FNV1A::HashConst("int"), sizeof(int), Q_VERSION });

		for (const auto& variable : vecVariables)
		{
			szVariableHash = CRT::IntegerToString(variable.uNameHash, szHashBuffer, sizeof(szHashBuffer), 16);

			WriteBuffer(root[szVariableHash], variable);
		}

		// write serialized configuration to file
		const std::string strSerialized = toml::format(root);
		const BOOL bWritten = ::WriteFile(hFileOut, strSerialized.data(), strSerialized.size(), nullptr, nullptr);
		::CloseHandle(hFileOut);

		return bWritten;
	}

	inline bool LoadFile(const wchar_t* wszFilePath)
	{
		toml::value root = toml::parse(wszFilePath);

		// @todo: implement version adaptation mechanism like so: if file has variable but src doesn't - remove from file, if src has variable but file doesn't - add it to file + probably with menu notification and ask for this

		char szHashBuffer[CRT::IntegerToString_t<FNV1A_t, 16U>::MaxCount()] = { };
		const char* szVariableHash = CRT::IntegerToString(FNV1A::HashConst("version"), szHashBuffer, sizeof(szHashBuffer), 16);

		// get cheat version at time when configuration has been saved
		auto& version = root[szVariableHash];

		for (auto& variable : vecVariables)
		{
			szVariableHash = CRT::IntegerToString(variable.uNameHash, szHashBuffer, sizeof(szHashBuffer), 16);

			// check is variable not found
			if (!root.contains(szVariableHash))
			{
				// add variable to save | assert if version matches to current
				continue;
			}

			ReadBuffer(root[szVariableHash], variable);
		}

		// @todo: check is configuration version older than current cheat version
		//if (toml::get<int>(version) != Q_VERSION)
		//{
		//	// manually update version number
		//	version = Q_VERSION;
		//
		//	// re-serialize toml configuration and write to file
		//	const std::string strSerialized = toml::format(root);
		//	if (!::WriteFile(hFileOut, strSerialized.data(), strSerialized.size(), nullptr, nullptr))
		//		L_PRINT(LOG_WARNING) << Q_XOR("failed to re-serialize configuration file: \"") << wszFileName << Q_XOR("\"");
		//
		//	::CloseHandle(hFileOut);
		//}

		return true;
	}
}
