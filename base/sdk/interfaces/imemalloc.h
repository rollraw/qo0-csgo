#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier0/memalloc.h

using MemAllocFailHandler_t = std::size_t(__cdecl*)(std::size_t);

// @note: not full class
class IMemAlloc
{
public:
	// Release versions
	virtual void* Alloc(std::size_t nSize) = 0;
	virtual void* Realloc(void* pMemory, std::size_t nSize) = 0;
	virtual void  Free(void* pMemory) = 0;
	virtual void* Expand(void* pMemory, std::size_t nSize) = 0; // no longer supported

	// Debug versions
	virtual void* Alloc(std::size_t nSize, const char* szFileName, int nLine) = 0;
	virtual void* Realloc(void* pMemory, std::size_t nSize, const char* szFileName, int nLine) = 0;
	virtual void  Free(void* pMemory, const char* szFileName, int nLine) = 0;
	virtual void* Expand(void* pMemory, std::size_t nSize, const char* szFileName, int nLine) = 0; // no longer supported

	// Returns size of a particular allocation
	virtual std::size_t GetSize(void* pMemory) = 0;
};
