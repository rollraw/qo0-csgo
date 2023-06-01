#pragma once
// used: [stl] uint32_t
#include <cstdint>

#include "../../common.h"

using CRC32_t = std::uint32_t;

namespace CRC32
{
	void Init(CRC32_t* pulCRC);
	void Final(CRC32_t* pulCRC);
	CRC32_t	GetTableEntry(const unsigned int nSlot);
	void ProcessBuffer(CRC32_t* pulCRC, const void* pBuffer, int nBufferSize);

	Q_INLINE CRC32_t ProcessSingleBuffer(const void* pBuffer, const int nBufferSize)
	{
		CRC32_t uCRC;
		Init(&uCRC);
		ProcessBuffer(&uCRC, pBuffer, nBufferSize);
		Final(&uCRC);
		return uCRC;
	}
}
