#pragma once
// used: [win] getcurrentthreadid
#include <processthreadsapi.h>
// used: _mm_pause
#include <intrin0.h>

// used: getexportaddress
#include "../utilities/memory.h"

// @source: master/public/tier0/threadtools.h
// master/tier0/threadtools.cpp

#ifdef Q_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-volatile" // yeah valve, don't really want to fuck with this so
#endif

class CThreadFastMutex
{
public:
	CThreadFastMutex() :
		nOwnerID(0U), iDepth(0) { }

	void Lock(const unsigned int uSpinSleepTime = 0U) volatile
	{
		if (const std::uint32_t nThreadID = ::GetCurrentThreadId(); TryLock(nThreadID))
		{
			_mm_pause();
			Lock(nThreadID, uSpinSleepTime);
		}
	}

	void Lock(const std::uint32_t nThreadID, const unsigned int uSpinSleepTime) volatile
	{
		static auto fnThreadFastMutexLock = reinterpret_cast<void(Q_THISCALL*)(volatile void*, std::uint32_t, unsigned int)>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(TIER0_DLL), Q_XOR("?Lock@CThreadFastMutex@@ACEXII@Z"))); // @test: "55 8B EC 83 7D 0C FF"
		fnThreadFastMutexLock(this, nThreadID, uSpinSleepTime);
	}

	void Lock(const unsigned int uSpinSleepTime = 0U) const volatile
	{
		const_cast<CThreadFastMutex*>(this)->Lock(uSpinSleepTime);
	}

	void Unlock() volatile
	{
		if (--iDepth == 0)
			::_InterlockedExchange(reinterpret_cast<volatile long*>(&nOwnerID), 0L);
	}

	void Unlock() const volatile
	{
		const_cast<CThreadFastMutex*>(this)->Unlock();
	}

	[[nodiscard]] bool TryLock(const std::uint32_t nThreadID = ::GetCurrentThreadId()) volatile
	{
		if (nThreadID != nOwnerID && ::_InterlockedCompareExchange(reinterpret_cast<volatile long*>(&nOwnerID), static_cast<long>(nThreadID), 0L) != 0L)
			return false;

		::_ReadWriteBarrier(); // probably no-op
		++iDepth;
		return true;
	}

	[[nodiscard]] bool TryLock() const volatile
	{
		return const_cast<CThreadFastMutex*>(this)->TryLock();
	}

private:
	volatile std::uint32_t nOwnerID; // 0x00
	int iDepth; // 0x04
};
static_assert(sizeof(CThreadFastMutex) == 0x8);

#ifdef Q_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
