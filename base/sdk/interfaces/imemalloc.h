#pragma once
// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/public/tier0/memalloc.h

using MemAllocFailHandler_t = std::size_t(Q_CDECL*)(std::size_t);

class IMemAlloc : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	virtual ~IMemAlloc() { }

	void* Alloc(std::size_t nSize, const char* szFileName, int nLine)
	{
		// @ida: tier0.dll -> "FF 10 85 C0 74"
		return CallVFunc<void*, 0U>(this, nSize, szFileName, nLine);
	}

	void* Alloc(std::size_t nSize)
	{
		// @ida: tier0.dll -> U8["50 FF 52 ? 8B D0" + 0x3] / sizeof(std::uintptr_t)
		return CallVFunc<void*, 1U>(this, nSize);
	}

	void* Realloc(void* pMemory, std::size_t nSize, const char* szFileName, int nLine)
	{
		return CallVFunc<void*, 2U>(this, pMemory, nSize, szFileName, nLine);
	}

	void* Realloc(void* pMemory, std::size_t nSize)
	{
		// @ida: tier0.dll -> U8["FF 52 ? 8B 56" + 0x2]
		return CallVFunc<void*, 3U>(this, pMemory, nSize);
	}

	void Free(void* pMemory, const char* szFileName, int nLine)
	{
		CallVFunc<void, 4U>(this, pMemory, szFileName, nLine);
	}

	void Free(void* pMemory)
	{
		// @ida: tier0.dll -> U8["FF 52 ? 8B C6 85 F6 75 E7 5E 68" + 0x2] / sizeof(std::uintptr_t)
		CallVFunc<void, 5U>(this, pMemory);
	}

	// returns size of a particular allocation
	std::size_t GetSize(void* pMemory)
	{
		// @ida: client.dll -> U8["A1 ? ? ? ? 83 C4 04 8B 08 6A 00 8B 01 FF 50" + 0x10] / sizeof(std::uintptr_t) @xref: "Heap Used:\n"
		return CallVFunc<std::size_t, 8U>(this, pMemory);
	}
};
