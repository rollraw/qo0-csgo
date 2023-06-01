#pragma once
// used: iappsystem
#include "iclientmode.h"

// @source: master/public/filesystem.h

using FileHandle_t = void*;
using FSAllocFunc_t = void*(Q_CDECL*)(const char*, unsigned int);

#define FILESYSTEM_INVALID_HANDLE nullptr

// forward declarations
class CUtlBuffer;

class IBaseFileSystem : protected ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	int Read(void* pOutput, int nSize, FileHandle_t hFile)
	{
		return CallVFunc<int, 0U>(this, pOutput, nSize, hFile);
	}

	int Write(void const* pInput, int nSize, FileHandle_t hFile)
	{
		return CallVFunc<int, 1U>(this, pInput, nSize, hFile);
	}

	FileHandle_t Open(const char* szFileName, const char* szOptions, const char* szPathID = nullptr)
	{
		return CallVFunc<FileHandle_t, 2U>(this, szFileName, szOptions, szPathID);
	}

	void Close(FileHandle_t hFile)
	{
		CallVFunc<void, 3U>(this, hFile);
	}

	void Seek(FileHandle_t hFile, int iPosition, int nSeekType)
	{
		CallVFunc<void, 4U>(this, hFile, iPosition, nSeekType);
	}

	unsigned int Tell(FileHandle_t hFile)
	{
		return CallVFunc<unsigned int, 5U>(this, hFile);
	}

	unsigned int Size(const char* szFileName, const char* szPathID = nullptr)
	{
		return CallVFunc<unsigned int, 6U>(this, szFileName, szPathID);
	}

	unsigned int Size(FileHandle_t hFile)
	{
		return CallVFunc<unsigned int, 7U>(this, hFile);
	}

	void Flush(FileHandle_t hFile)
	{
		CallVFunc<void, 8U>(this, hFile);
	}

	bool Precache(const char* szFileName, const char* szPathID = nullptr)
	{
		return CallVFunc<bool, 9U>(this, szFileName, szPathID);
	}

	bool FileExists(const char* szFileName, const char* szPathID = nullptr)
	{
		return CallVFunc<bool, 10U>(this, szFileName, szPathID);
	}

	bool IsFileWritable(char const* szFileName, const char* szPathID = nullptr)
	{
		return CallVFunc<bool, 11U>(this, szFileName, szPathID);
	}

	bool SetFileWritable(char const* szFileName, bool bWritable, const char* szPathID = nullptr)
	{
		return CallVFunc<bool, 12U>(this, szFileName, bWritable, szPathID);
	}

	long GetFileTime(const char* szFileName, const char* szPathID = nullptr)
	{
		return CallVFunc<long, 13U>(this, szFileName, szPathID);
	}

	bool ReadFile(const char* szFileName, const char* szPath, CUtlBuffer& buffer, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = nullptr)
	{
		return CallVFunc<bool, 14U>(this, szFileName, szPath, &buffer, nMaxBytes, nStartingByte, pfnAlloc);
	}

	bool WriteFile(const char* szFileName, const char* szPath, CUtlBuffer& buffer)
	{
		return CallVFunc<bool, 15U>(this, szFileName, szPath, &buffer);
	}

	bool UnzipFile(const char* szFileName, const char* szPath, const char* szDestination)
	{
		return CallVFunc<bool, 16U>(this, szFileName, szPath, szDestination);
	}

private:
	void* pVTable; // 0x00
};
static_assert(sizeof(IBaseFileSystem) == 0x4);

class IFileSystem : public IAppSystem, public IBaseFileSystem
{
public:
	FileHandle_t OpenEx(const char* szFileName, const char* szOptions, unsigned int uFlags = 0U, const char* szPathID = nullptr, char** ppszResolvedFilename = nullptr)
	{
		return CallVFunc<FileHandle_t, 73U>(this, szFileName, szOptions, uFlags, szPathID, ppszResolvedFilename);
	}

	int ReadEx(void* pBuffer, int iDestinationSize, int iSize, FileHandle_t hFile)
	{
		return CallVFunc<int, 74U>(this, pBuffer, iDestinationSize, iSize, hFile);
	}

	int ReadFileEx(const char* szFileName, const char* szPath, void** ppBuffer, bool bNullTerminate = false, bool bOptimalAlloc = false, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = nullptr)
	{
		return CallVFunc<int, 75U>(this, szFileName, szPath, ppBuffer, bNullTerminate, bOptimalAlloc, nMaxBytes, nStartingByte, pfnAlloc);
	}

	bool GetOptimalIOConstraints(FileHandle_t hFile, unsigned int* pOffsetAlign, unsigned int* pSizeAlign, unsigned int* pBufferAlign)
	{
		return CallVFunc<bool, 87U>(this, hFile, pOffsetAlign, pSizeAlign, pBufferAlign);
	}

	void* AllocOptimalReadBuffer(FileHandle_t hFile, unsigned int nSize = 0U, unsigned int nOffset = 0U)
	{
		return CallVFunc<void*, 88U>(this, hFile, nSize, nOffset);
	}

	void FreeOptimalReadBuffer(void* pBuffer)
	{
		CallVFunc<void, 89U>(this, pBuffer);
	}

	Q_INLINE unsigned int GetOptimalReadSize(FileHandle_t hFile, const unsigned int nLogicalSize)
	{
		unsigned int uAlign = 0U;
		if (GetOptimalIOConstraints(hFile, &uAlign, nullptr, nullptr))
			return (static_cast<unsigned int>(nLogicalSize) + uAlign - 1U) & ~(uAlign - 1U);

		return nLogicalSize;
	}
};
static_assert(sizeof(IFileSystem) == 0x8);
