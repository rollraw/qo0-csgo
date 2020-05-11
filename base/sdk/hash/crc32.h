#pragma once
// used: std::uint32_t
#include <cstdint>

using CRC32_t = std::uint32_t;

namespace CRC32
{
	void	Init(CRC32_t* pulCRC);
	void	ProcessBuffer(CRC32_t* pulCRC, const void* p, int len);
	void	Final(CRC32_t* pulCRC);
	CRC32_t	GetTableEntry(unsigned int nSlot);

	inline CRC32_t ProcessSingleBuffer(const void* p, int len)
	{
		CRC32_t crc;
		CRC32::Init(&crc);
		CRC32::ProcessBuffer(&crc, p, len);
		CRC32::Final(&crc);
		return crc;
	}
}
