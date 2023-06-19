#pragma once

#define Q_BINARY_EXTENSION L".bin"

namespace C::BIN
{
	/* @section: [internal] */
	/// write single variable to the buffer
	/// @returns: number of written bytes
	inline std::size_t WriteBuffer(std::uint8_t* pBuffer, const VariableObject_t& variable)
	{
		std::uint8_t* pBufferCurrent = pBuffer;

		*reinterpret_cast<FNV1A_t*>(pBufferCurrent) = variable.uNameHash;
		pBufferCurrent += sizeof(FNV1A_t);

		*reinterpret_cast<FNV1A_t*>(pBufferCurrent) = variable.uTypeHash;
		pBufferCurrent += sizeof(FNV1A_t);

		switch (variable.uTypeHash)
		{
		case FNV1A::HashConst("bool"):
		case FNV1A::HashConst("int"):
		case FNV1A::HashConst("unsigned int"):
		case FNV1A::HashConst("float"):
		case FNV1A::HashConst("Color_t"):
		case FNV1A::HashConst("char[]"):
		{
			CRT::MemoryCopy(pBufferCurrent, variable.GetStorage<const std::uint8_t, false>(), variable.nStorageSize);
			pBufferCurrent += variable.nStorageSize;
			break;
		}
		case FNV1A::HashConst("bool[]"):
		case FNV1A::HashConst("int[]"):
		case FNV1A::HashConst("unsigned int[]"):
		case FNV1A::HashConst("float[]"):
		case FNV1A::HashConst("char[][]"):
		{
			// write size
			*reinterpret_cast<std::size_t*>(pBufferCurrent) = variable.nStorageSize;
			pBufferCurrent += sizeof(std::size_t);

			// write data
			CRT::MemoryCopy(pBufferCurrent, variable.GetStorage<const std::uint8_t, false>(), variable.nStorageSize);
			pBufferCurrent += variable.nStorageSize;
			break;
		}
		default:
		{
			[[maybe_unused]] bool bFoundUserType = false;
			const std::uint8_t* pVariableStorage = variable.GetStorage<const std::uint8_t, false>();

			// lookup for user-defined data type
			for (const UserDataType_t& userType : vecUserTypes)
			{
				if (userType.uTypeHash == variable.uTypeHash)
				{
					// write size
					*reinterpret_cast<std::size_t*>(pBufferCurrent) = variable.GetSerializationSize();
					pBufferCurrent += sizeof(std::size_t);

					// write data
					// @test: it would be so fucking neatful if we could rework this to proceed recursive call instead
					for (const UserDataMember_t& userMember : userType.vecMembers)
					{
						*reinterpret_cast<FNV1A_t*>(pBufferCurrent) = userMember.uNameHash;
						pBufferCurrent += sizeof(FNV1A_t);

						*reinterpret_cast<FNV1A_t*>(pBufferCurrent) = userMember.uTypeHash;
						pBufferCurrent += sizeof(FNV1A_t);

						CRT::MemoryCopy(pBufferCurrent, pVariableStorage + userMember.uBaseOffset, userMember.nDataSize);
						pBufferCurrent += userMember.nDataSize;
					}

					bFoundUserType = true;
					break;
				}
			}

			Q_ASSERT(bFoundUserType); // value type is not defined
			break;
		}
		}

		const std::size_t nWriteCount = (pBufferCurrent - pBuffer);
		Q_ASSERT(nWriteCount == sizeof(FNV1A_t[2]) + variable.GetSerializationSize()); // count of actually written bytes mismatch to serialization size
		return nWriteCount;
	}

	/// read single variable from the buffer
	/// @returns: number of read bytes
	inline std::size_t ReadBuffer(std::uint8_t* pBuffer, VariableObject_t& variable)
	{
		std::uint8_t* pBufferCurrent = pBuffer;

		// @todo: instead of overwriting those, check are them same? or alternatively, make caller of this method do not initialize variable
		variable.uNameHash = *reinterpret_cast<FNV1A_t*>(pBufferCurrent);
		pBufferCurrent += sizeof(FNV1A_t);

		variable.uTypeHash = *reinterpret_cast<FNV1A_t*>(pBufferCurrent);
		pBufferCurrent += sizeof(FNV1A_t);

		switch (variable.uTypeHash)
		{
		case FNV1A::HashConst("char[]"):
			Q_ASSERT((CRT::StringLength(reinterpret_cast<const char*>(pBufferCurrent)) + 1U) * sizeof(char) <= variable.nStorageSize); // string length mismatched
			[[fallthrough]];
		case FNV1A::HashConst("bool"):
		case FNV1A::HashConst("int"):
		case FNV1A::HashConst("unsigned int"):
		case FNV1A::HashConst("float"):
		case FNV1A::HashConst("Color_t"):
		{
			variable.SetStorage(pBufferCurrent);
			pBufferCurrent += variable.nStorageSize;
			break;
		}
		case FNV1A::HashConst("bool[]"):
		case FNV1A::HashConst("int[]"):
		case FNV1A::HashConst("unsigned int[]"):
		case FNV1A::HashConst("float[]"):
		case FNV1A::HashConst("char[][]"):
		{
			// read size
			[[maybe_unused]] const std::size_t nSourceStorageSize = *reinterpret_cast<std::size_t*>(pBufferCurrent);
			pBufferCurrent += sizeof(std::size_t);

			// read data
			Q_ASSERT(nSourceStorageSize <= variable.nStorageSize); // source size is bigger than destination size
			variable.SetStorage(pBufferCurrent);
			pBufferCurrent += nSourceStorageSize;
			break;
		}
		default:
		{
			[[maybe_unused]] bool bFoundUserType = false;
			std::uint8_t* pVariableStorage = variable.GetStorage<std::uint8_t, false>();

			// lookup for user-defined data type
			for (const UserDataType_t& userType : vecUserTypes)
			{
				if (userType.uTypeHash == variable.uTypeHash)
				{
					// read size
					const std::size_t nSourceSerializationSize = *reinterpret_cast<std::size_t*>(pBufferCurrent);
					pBufferCurrent += sizeof(std::size_t);

					const std::uint8_t* pBufferFirstMember = pBufferCurrent;

					// read data
					// @test: it would be so fucking neatful if we could rework this to proceed recursive call instead
					for (const UserDataMember_t& userMember : userType.vecMembers)
					{
						const FNV1A_t uMemberNameHash = *reinterpret_cast<FNV1A_t*>(pBufferCurrent);
						pBufferCurrent += sizeof(FNV1A_t);

						const FNV1A_t uMemberTypeHash = *reinterpret_cast<FNV1A_t*>(pBufferCurrent);
						pBufferCurrent += sizeof(FNV1A_t);

						// verify source and destination variable name and data type
						if (uMemberNameHash != userMember.uNameHash || uMemberTypeHash != userMember.uTypeHash)
						{
							// @todo: instead we can skip up to the next matched name and type hashes variable and continue read from there
							Q_ASSERT(false); // source and destination structure mismatch

							// skip rest of the variables due to the impossibility of determining their size
							pBufferCurrent += nSourceSerializationSize - (pBufferCurrent - pBufferFirstMember);
							break;
						}

						CRT::MemoryCopy(pVariableStorage + userMember.uBaseOffset, pBufferCurrent, userMember.nDataSize);
						pBufferCurrent += userMember.nDataSize;
					}

					bFoundUserType = true;
					break;
				}
			}

			Q_ASSERT(bFoundUserType); // value type is not defined
			break;
		}
		}

		const std::size_t nReadCount = (pBufferCurrent - pBuffer);
		Q_ASSERT(nReadCount == sizeof(FNV1A_t[2]) + variable.GetSerializationSize()); // count of actually read bytes mismatch to serialization size
		return nReadCount;
	}

	/// search for single variable in the buffer
	/// @returns: pointer to the buffer at position of found variable on success, null otherwise
	inline std::uint8_t* FindBuffer(const std::uint8_t* pBufferStart, const std::size_t nBufferSize, const VariableObject_t& variable)
	{
		// convert hashes to bytes
		const FNV1A_t uNameTypeHash[2] = { variable.uNameHash, variable.uTypeHash };

		// @todo: do we need to always go from the start of file and parse contents until needed variable due to binary format? if so then rework it and do not store type hash (currently it just used to have more explicit search)
	#ifdef Q_PARANOID
		const std::vector<std::uint8_t*> vecVariableHeaders = MEM::FindPatternAllOccurrencesEx(pBufferStart, nBufferSize, reinterpret_cast<const std::uint8_t*>(uNameTypeHash), sizeof(FNV1A_t[2]));

		if (!vecVariableHeaders.empty())
		{
			// notify user about multiple variables with same name
			// @note: this also could happen due to FNV1A hash collision, open an issue on github if you experiencing this
			if (vecVariableHeaders.size() > 1U)
				L_PRINT(LOG_WARNING) << Q_XOR("found more than one variable that matches [NAME: ") << L::AddFlags(LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX) << variable.uNameHash << Q_XOR(", TYPE: ") << variable.uTypeHash << Q_XOR("] hashes");

			// return first found occurrence
			return vecVariableHeaders[0];
		}

		return nullptr;
	#else
		return MEM::FindPatternEx(pBufferStart, nBufferSize, reinterpret_cast<const std::uint8_t*>(uNameTypeHash), sizeof(FNV1A_t[2]));
	#endif
	}

	/* @section: main */
	inline bool SaveVariable(const wchar_t* wszFilePath, const VariableObject_t& variable)
	{
		const HANDLE hFileInOut = ::CreateFileW(wszFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileInOut == INVALID_HANDLE_VALUE)
			return false;

		DWORD dwFileSize;
		if (dwFileSize = ::GetFileSize(hFileInOut, nullptr); dwFileSize == INVALID_FILE_SIZE)
		{
			::CloseHandle(hFileInOut);
			return false;
		}

		std::uint8_t* pBuffer = static_cast<std::uint8_t*>(MEM::HeapAlloc(dwFileSize));
		if (!::ReadFile(hFileInOut, pBuffer, dwFileSize, nullptr, nullptr))
		{
			::CloseHandle(hFileInOut);

			MEM::HeapFree(pBuffer);
			return false;
		}

		VariableObject_t version = { FNV1A::HashConst("version"), FNV1A::HashConst("int"), sizeof(int), Q_VERSION };
		ReadBuffer(pBuffer, version);

		BOOL bWritten = FALSE;

		// check have we found a variable
		if (std::uint8_t* pVariableData = FindBuffer(pBuffer, dwFileSize, variable); pVariableData != nullptr)
		{
			const std::size_t nOverwriteBytesCount = WriteBuffer(pVariableData, variable);

			// overwrite variable in the file
			if (::SetFilePointer(hFileInOut, pVariableData - pBuffer, nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
				bWritten = ::WriteFile(hFileInOut, pVariableData, nOverwriteBytesCount, nullptr, nullptr);
		}
		// or we need to create new
		else
		{
			// write missing variable to the end of file
			std::uint8_t* pTemporaryBuffer = static_cast<std::uint8_t*>(MEM_STACKALLOC(sizeof(FNV1A_t[2]) + variable.GetSerializationSize()));
			const std::size_t nWriteBytesCount = WriteBuffer(pTemporaryBuffer, variable);

			bWritten = ::WriteFile(hFileInOut, pTemporaryBuffer, nWriteBytesCount, nullptr, nullptr);
			MEM_STACKFREE(pTemporaryBuffer);
		}
		::CloseHandle(hFileInOut);

		MEM::HeapFree(pBuffer);
		return bWritten;
	}

	inline bool LoadVariable(const wchar_t* wszFilePath, VariableObject_t& variable)
	{
		const HANDLE hFileInOut = ::CreateFileW(wszFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileInOut == INVALID_HANDLE_VALUE)
			return false;

		DWORD dwFileSize;
		if (dwFileSize = ::GetFileSize(hFileInOut, nullptr); dwFileSize == INVALID_FILE_SIZE)
		{
			::CloseHandle(hFileInOut);
			return false;
		}

		std::uint8_t* pBuffer = static_cast<std::uint8_t*>(MEM::HeapAlloc(dwFileSize));
		if (!::ReadFile(hFileInOut, pBuffer, dwFileSize, nullptr, nullptr))
		{
			::CloseHandle(hFileInOut);

			MEM::HeapFree(pBuffer);
			return false;
		}

		VariableObject_t version = { FNV1A::HashConst("version"), FNV1A::HashConst("int"), sizeof(int), Q_VERSION };
		ReadBuffer(pBuffer, version);

		// search a variable in the file
		std::uint8_t* pVariableData = FindBuffer(pBuffer, dwFileSize, variable);
		const bool bFoundVariable = (pVariableData != nullptr);

		// check have we found the variable
		if (bFoundVariable)
			ReadBuffer(pVariableData, variable);
		// otherwise serialize it
		else
		{
			if (*version.GetStorage<int>() < Q_VERSION)
			{
				// write missing variable to the end of file
				std::uint8_t* pTemporaryBuffer = static_cast<std::uint8_t*>(MEM_STACKALLOC(sizeof(FNV1A_t[2]) + variable.GetSerializationSize()));
				const std::size_t nWriteBytesCount = WriteBuffer(pTemporaryBuffer, variable);

				::WriteFile(hFileInOut, pTemporaryBuffer, nWriteBytesCount, nullptr, nullptr);
				MEM_STACKFREE(pTemporaryBuffer);

				// overwrite version
				if (::SetFilePointer(hFileInOut, sizeof(FNV1A_t[2]), nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
				{
					constexpr int iLastVersion = Q_VERSION;
					::WriteFile(hFileInOut, &iLastVersion, version.GetSerializationSize(), nullptr, nullptr);
				}
			}
			else
				Q_ASSERT(false); // version of configuration is greater than cheat, or version is same but configuration missing variable, consider update 'Q_VERSION' cheat version
		}
		::CloseHandle(hFileInOut);

		MEM::HeapFree(pBuffer);
		return bFoundVariable;
	}

	inline bool RemoveVariable(const wchar_t* wszFilePath, const VariableObject_t& variable)
	{
		const HANDLE hFileInOut = ::CreateFileW(wszFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileInOut == INVALID_HANDLE_VALUE)
			return false;

		DWORD dwFileSize;
		if (dwFileSize = ::GetFileSize(hFileInOut, nullptr); dwFileSize == INVALID_FILE_SIZE)
		{
			::CloseHandle(hFileInOut);
			return false;
		}

		std::uint8_t* pBuffer = static_cast<std::uint8_t*>(MEM::HeapAlloc(dwFileSize));
		if (!::ReadFile(hFileInOut, pBuffer, dwFileSize, nullptr, nullptr))
		{
			::CloseHandle(hFileInOut);

			MEM::HeapFree(pBuffer);
			return false;
		}
		const std::uint8_t* pBufferEnd = pBuffer + dwFileSize;

		VariableObject_t version = { FNV1A::HashConst("version"), FNV1A::HashConst("int"), sizeof(int), Q_VERSION };
		ReadBuffer(pBuffer, version);

		// search a variable in the file
		std::uint8_t* pVariableData = FindBuffer(pBuffer, dwFileSize, variable);
		bool bRemovedVariable = false;

		// check have we found the variable
		if (pVariableData != nullptr)
		{
			const std::size_t nVariableDataLength = sizeof(FNV1A_t[2]) + variable.GetSerializationSize();
			const std::uint8_t* pVariableDataEnd = pVariableData + nVariableDataLength;

			// shift bytes left with overlapping data of variable to remove
			CRT::MemoryMove(pVariableData, pVariableDataEnd, pBufferEnd - pVariableDataEnd);

			// truncate file size
			if (::SetFilePointer(hFileInOut, -static_cast<LONG>(nVariableDataLength), nullptr, FILE_END) != INVALID_SET_FILE_POINTER)
				bRemovedVariable = true;

		}
		::CloseHandle(hFileInOut);

		MEM::HeapFree(pBuffer);
		return bRemovedVariable;
	}

	inline bool SaveFile(const wchar_t* wszFilePath)
	{
		const HANDLE hFileOut = ::CreateFileW(wszFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileOut == INVALID_HANDLE_VALUE)
			return false;

		VariableObject_t version = { FNV1A::HashConst("version"), FNV1A::HashConst("int"), sizeof(int), Q_VERSION };

		// pre-calculate buffer size for all variables to avoid reallocation
		std::size_t nBufferSize = sizeof(FNV1A_t[2]) + version.GetSerializationSize();
		for (const auto& variable : vecVariables)
			nBufferSize += sizeof(FNV1A_t[2]) + variable.GetSerializationSize();

		// since we know final size, reserve disk space for it
		if (::SetFilePointer(hFileOut, nBufferSize, nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
		{
			::SetEndOfFile(hFileOut);

			if (::SetFilePointer(hFileOut, 0L, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
				return false;
		}

		std::uint8_t* pBuffer = static_cast<std::uint8_t*>(MEM::HeapAlloc(nBufferSize));
		const std::uint8_t* pBufferEnd = pBuffer + nBufferSize;

		std::uint8_t* pCurrentBuffer = pBuffer;
		
		// put current cheat build number
		pCurrentBuffer += WriteBuffer(pBuffer, version);

		for (auto& variable : vecVariables)
		{
			Q_ASSERT(pCurrentBuffer <= pBufferEnd); // allocated buffer can't hold more variables
			pCurrentBuffer += WriteBuffer(pCurrentBuffer, variable);
		}

		// write serialized configuration to file
		const BOOL bWritten = ::WriteFile(hFileOut, pBuffer, nBufferSize, nullptr, nullptr);
		::CloseHandle(hFileOut);

		MEM::HeapFree(pBuffer);
		return bWritten;
	}

	inline bool LoadFile(const wchar_t* wszFilePath)
	{
		const HANDLE hFileInOut = ::CreateFileW(wszFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFileInOut == INVALID_HANDLE_VALUE)
			return false;

		DWORD dwFileSize;
		if (dwFileSize = ::GetFileSize(hFileInOut, nullptr); dwFileSize == INVALID_FILE_SIZE)
		{
			::CloseHandle(hFileInOut);
			return false;
		}

		std::uint8_t* pBuffer = static_cast<std::uint8_t*>(MEM::HeapAlloc(dwFileSize));
		if (!::ReadFile(hFileInOut, pBuffer, dwFileSize, nullptr, nullptr))
		{
			::CloseHandle(hFileInOut);

			MEM::HeapFree(pBuffer);
			return false;
		}

		VariableObject_t version = { FNV1A::HashConst("version"), FNV1A::HashConst("int"), sizeof(int), Q_VERSION };
		ReadBuffer(pBuffer, version);

		for (auto& variable : vecVariables)
		{
			std::uint8_t* pVariableData = FindBuffer(pBuffer, dwFileSize, variable);

			// check is variable not found
			if (pVariableData == nullptr)
			{
				if (*version.GetStorage<int>() < Q_VERSION)
				{
					// write missing variable to the end of file
					if (::SetFilePointer(hFileInOut, 0L, nullptr, FILE_END) != INVALID_SET_FILE_POINTER)
					{
						std::uint8_t* pTemporaryBuffer = static_cast<std::uint8_t*>(MEM_STACKALLOC(sizeof(FNV1A_t[2]) + variable.GetSerializationSize()));
						const std::size_t nWriteBytesCount = WriteBuffer(pTemporaryBuffer, variable);

						::WriteFile(hFileInOut, pTemporaryBuffer, nWriteBytesCount, nullptr, nullptr);
						MEM_STACKFREE(pTemporaryBuffer);

						// overwrite version
						if (::SetFilePointer(hFileInOut, sizeof(FNV1A_t[2]), nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
						{
							constexpr int iLastVersion = Q_VERSION;
							::WriteFile(hFileInOut, &iLastVersion, version.GetSerializationSize(), nullptr, nullptr);
						}
					}
				}
				else
					Q_ASSERT(false); // version of configuration is greater than cheat, or version is same but configuration missing variable, consider update 'Q_VERSION' cheat version

				continue;
			}

			ReadBuffer(pVariableData, variable);
		}
		::CloseHandle(hFileInOut);

		MEM::HeapFree(pBuffer);
		return true;
	}
}
