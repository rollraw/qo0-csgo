#pragma once
// used: [ext] json parser implementation
#include <json/json.hpp>

#define Q_JSON_EXTENSION L".json"

namespace C::JSON
{
	/* @section: [internal] */
	// write single variable to buffer
	inline void WriteBuffer(nlohmann::json& entry, VariableObject_t& variable)
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
			entry = nlohmann::json::array({ pColorValue->r, pColorValue->g, pColorValue->b, pColorValue->a });
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

			std::vector<bool> vecBools(nArraySize);
			for (std::size_t i = 0U; i < nArraySize; i++)
				vecBools[i] = arrValues[i];

			entry = vecBools;
			break;
		}
		case FNV1A::HashConst("int[]"):
		{
			const std::size_t nArraySize = variable.nStorageSize / sizeof(int);
			const auto& arrValues = *variable.GetStorage<int[], false>();

			std::vector<int> vecInts(nArraySize);
			for (std::size_t i = 0U; i < nArraySize; i++)
				vecInts[i] = arrValues[i];

			entry = vecInts;
			break;
		}
		case FNV1A::HashConst("unsigned int[]"):
		{
			const std::size_t nArraySize = variable.nStorageSize / sizeof(unsigned int);
			const auto& arrValues = *variable.GetStorage<unsigned int[], false>();

			std::vector<unsigned int> vecUInts(nArraySize);
			for (std::size_t i = 0U; i < nArraySize; i++)
				vecUInts[i] = arrValues[i];

			entry = vecUInts;
			break;
		}
		case FNV1A::HashConst("float[]"):
		{
			const std::size_t nArraySize = variable.nStorageSize / sizeof(float);
			const auto& arrValues = *variable.GetStorage<float[], false>();

			std::vector<float> vecFloats(nArraySize);
			for (std::size_t i = 0U; i < nArraySize; i++)
				vecFloats[i] = arrValues[i];

			entry = vecFloats;
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
			entry = nlohmann::json::array({ pKeyValue->uKey, static_cast<int>(pKeyValue->nMode) });
			break;
		}
		default:
		{
			bool bFoundUserType = true;
			//const std::uint8_t* pVariableStorage = variable.GetStorage<const std::uint8_t, false>();

			// lookup for user-defined data type
			for (const UserDataType_t& userType : vecUserTypes)
			{
				if (userType.uTypeHash == variable.uTypeHash)
				{
					// write size
					entry[Q_XOR("size")] = variable.GetSerializationSize();

					nlohmann::json members = { };

					// write data
					// @todo: it would be so fucking neatful if we could rework this to proceed recursive call instead
					for (const UserDataMember_t& userMember : userType.vecMembers)
					{
						nlohmann::json currentMember = { };
						currentMember[Q_XOR("name")] = userMember.uNameHash;
						currentMember[Q_XOR("type")] = userMember.uTypeHash;

						// @todo: call user defined 'to_json' callback | or again, remake to deal with recursive call instead

						members.emplace_back(std::move(currentMember));
					}

					entry[Q_XOR("members")] = members;

					bFoundUserType = true;
					break;
				}
			}

			Q_ASSERT(bFoundUserType); // value type is not defined
			break;
		}
		}
	}

	// read single variable from buffer
	inline void ReadBuffer(nlohmann::json& entry, VariableObject_t& variable)
	{
		switch (variable.uTypeHash)
		{
		case FNV1A::HashConst("bool"):
		{
			const bool bValue = entry.get<bool>();
			variable.SetStorage(&bValue);
			break;
		}
		case FNV1A::HashConst("int"):
		{
			const int iValue = entry.get<int>();
			variable.SetStorage(&iValue);
			break;
		}
		case FNV1A::HashConst("unsigned int"):
		{
			const unsigned int uValue = entry.get<unsigned int>();
			variable.SetStorage(&uValue);
			break;
		}
		case FNV1A::HashConst("float"):
		{
			const float flValue = entry.get<float>();
			variable.SetStorage(&flValue);
			break;
		}
		case FNV1A::HashConst("Color_t"):
		{
			Color_t colValue;
			for (std::uint8_t i = 0U; i < entry.size(); i++)
				colValue[i] = entry[i].get<std::uint8_t>();

			variable.SetStorage(&colValue);
			break;
		}
		case FNV1A::HashConst("char[]"):
		{
			const std::string strValue = entry.get<std::string>();
			Q_ASSERT((strValue.size() + 1U) * sizeof(char) <= variable.nStorageSize); // source size is bigger than destination size
			variable.SetStorage(strValue.c_str());
			break;
		}
		case FNV1A::HashConst("bool[]"):
		{
			const auto& vecBools = entry.get<std::vector<bool>>();
			Q_ASSERT(vecBools.size() * sizeof(bool) <= variable.nStorageSize); // source size is bigger than destination size

			bool* arrValues = *variable.GetStorage<bool*, false>();
			for (std::size_t i = 0U; i < vecBools.size(); i++)
				arrValues[i] = vecBools[i];

			break;
		}
		case FNV1A::HashConst("int[]"):
		{
			const auto& vecInts = entry.get<std::vector<int>>();
			Q_ASSERT(vecInts.size() * sizeof(int) <= variable.nStorageSize); // source size is bigger than destination size

			int* arrValues = *variable.GetStorage<int*, false>();
			for (std::size_t i = 0U; i < vecInts.size(); i++)
				arrValues[i] = vecInts[i];

			break;
		}
		case FNV1A::HashConst("unsigned int[]"):
		{
			const auto& vecUInts = entry.get<std::vector<unsigned int>>();
			Q_ASSERT(vecUInts.size() * sizeof(unsigned int) <= variable.nStorageSize); // source size is bigger than destination size

			unsigned int* arrValues = *variable.GetStorage<unsigned int*, false>();
			for (std::size_t i = 0U; i < vecUInts.size(); i++)
				arrValues[i] = vecUInts[i];

			break;
		}
		case FNV1A::HashConst("float[]"):
		{
			const auto& vecFloats = entry.get<std::vector<float>>();
			Q_ASSERT(vecFloats.size() * sizeof(float) <= variable.nStorageSize); // source size is bigger than destination size

			float* arrValues = *variable.GetStorage<float*, false>();
			for (std::size_t i = 0U; i < vecFloats.size(); i++)
				arrValues[i] = vecFloats[i];

			break;
		}
		case FNV1A::HashConst("char[][]"):
		{
			// @test: very ugh
			const std::string strValue = entry.get<std::string>();
			Q_ASSERT((strValue.size() + 1U) * sizeof(char) <= variable.nStorageSize); // source size is bigger than destination size
			variable.SetStorage(strValue.data());
			break;
		}
		// @todo: temporary while custom structs are WIP
		case FNV1A::HashConst("KeyBind_t"):
		{
			KeyBind_t* pKeyValue = variable.GetStorage<KeyBind_t>();
			pKeyValue->uKey = entry[0].get<unsigned int>();
			pKeyValue->nMode = static_cast<EKeyBindMode>(entry[1].get<int>());
			break;
		}
		default:
		{
			bool bFoundUserType = true;
			//std::uint8_t* pVariableStorage = variable.GetStorage<std::uint8_t, false>();

			// lookup for user-defined data type
			for (const UserDataType_t& userType : vecUserTypes)
			{
				if (userType.uTypeHash == variable.uTypeHash)
				{
					// read size
					const std::size_t nSourceSerializationSize = entry[Q_XOR("size")].get<std::size_t>();

					nlohmann::json members = entry[Q_XOR("members")];

					// read data
					// @todo: it would be so fucking neatful if we could rework this to proceed recursive call instead
					// @todo: instead we must loop through all "members" and them look for same in 'vecMembers'
					for (const UserDataMember_t& userMember : userType.vecMembers)
					{
						const FNV1A_t uMemberNameHash = members[Q_XOR("name")].get<FNV1A_t>();
						const FNV1A_t uMemberTypeHash = members[Q_XOR("type")].get<FNV1A_t>();

						// verify source and destination variable name and data type
						if (uMemberNameHash != userMember.uNameHash || uMemberTypeHash != userMember.uTypeHash)
						{
							// @todo: instead we can skip up to the next matched name and type hashes variable and continue read from there
							Q_ASSERT(false); // source and destination structure mismatch
							break;
						}

						// @todo: call user defined 'from_json' callback | or again, remake to deal with recursive call instead
					}

					bFoundUserType = true;
					break;
				}
			}

			Q_ASSERT(bFoundUserType); // value type is not defined
			break;
		}
		}
	}

	/* @section: main */
	inline bool SaveVariable(const wchar_t* wszFilePath, VariableObject_t& variable)
	{
		const HANDLE hFileOut = ::CreateFileW(wszFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileOut == INVALID_HANDLE_VALUE)
			return false;

		FILE* hFileStream = nullptr;
		::_wfopen_s(&hFileStream, wszFilePath, Q_XOR(L"r"));

		if (hFileStream == nullptr)
			return false;

		nlohmann::json root = nlohmann::json::parse(hFileStream);
		::fclose(hFileStream);

		char szKeyBuffer[CRT::IntegerToString_t<FNV1A_t, 16U>::MaxCount()] = { };
		const char* szVariableHash = CRT::IntegerToString(variable.uNameHash, szKeyBuffer, sizeof(szKeyBuffer), 16);

		WriteBuffer(root[szVariableHash], variable);
		
		// write re-serialized configuration to file
		const std::string strSerialized = root.dump(4);
		const BOOL bWritten = ::WriteFile(hFileOut, strSerialized.data(), strSerialized.size(), nullptr, nullptr);
		::CloseHandle(hFileOut);

		return bWritten;
	}

	inline bool LoadVariable(const wchar_t* wszFilePath, VariableObject_t& variable)
	{
		FILE* hFileStream = nullptr;
		::_wfopen_s(&hFileStream, wszFilePath, Q_XOR(L"r+"));

		if (hFileStream == nullptr)
			return false;

		nlohmann::json root = nlohmann::json::parse(hFileStream, nullptr, false);

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

		::fclose(hFileStream);
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

		nlohmann::json root;

		// put current cheat build number
		WriteBuffer(root[szVariableHash], VariableObject_t{ FNV1A::HashConst("version"), FNV1A::HashConst("int"), sizeof(int), Q_VERSION });

		for (const auto& variable : vecVariables)
		{
			szVariableHash = CRT::IntegerToString(variable.uNameHash, szHashBuffer, sizeof(szHashBuffer), 16);

			WriteBuffer(root[szVariableHash], variable);
		}

		// write serialized configuration to file
		const std::string strSerialized = root.dump(4);
		const BOOL bWritten = ::WriteFile(hFileOut, strSerialized.data(), strSerialized.size(), nullptr, nullptr);
		::CloseHandle(hFileOut);

		return bWritten;
	}

	inline bool LoadFile(const wchar_t* wszFilePath)
	{
		FILE* hFileStream = nullptr;
		::_wfopen_s(&hFileStream, wszFilePath, Q_XOR(L"r+"));

		if (hFileStream == nullptr)
			return false;

		nlohmann::json root = nlohmann::json::parse(hFileStream, nullptr, false);

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

		::fclose(hFileStream);
		return true;
	}
}
