#pragma once
// used: min
#include "../utilities/crt.h"

// @source: master/public/tier1/bitbuf.h
// master/tier1/newbitbuf.cpp

constexpr std::uint32_t arrBitMaskTable[33] =
{
	0U,
	(1U << 1U) - 1U, (1U << 2U) - 1U, (1U << 3U) - 1U, (1U << 4U) - 1U,
	(1U << 5U) - 1U, (1U << 6U) - 1U, (1U << 7U) - 1U, (1U << 8U) - 1U,
	(1U << 9U) - 1U, (1U << 10U) - 1U, (1U << 11U) - 1U, (1U << 12U) - 1U,
	(1U << 13U) - 1U, (1U << 14U) - 1U, (1U << 15U) - 1U, (1U << 16U) - 1U,
	(1U << 17U) - 1U, (1U << 18U) - 1U, (1U << 19U) - 1U, (1U << 20U) - 1U,
	(1U << 21U) - 1U, (1U << 22U) - 1U, (1U << 23U) - 1U, (1U << 24U) - 1U,
	(1U << 25U) - 1U, (1U << 26U) - 1U, (1U << 27U) - 1U, (1U << 28U) - 1U,
	(1U << 29U) - 1U, (1U << 30U) - 1U, (1U << 31U) - 1U, 0xFFFFFFFF
};

class CBitBuffer
{
public:
	CBitBuffer() = default;

	Q_INLINE void SetOverflowFlag()
	{
		bOverflow = true;
	}

	Q_INLINE bool IsOverflowed() const
	{
		return bOverflow;
	}

public:
	const char* szDebugName = nullptr; // 0x00
	bool bOverflow = false; // 0x04
	int nDataBits = -1; // 0x08
	std::size_t nDataBytes = 0; // 0x0C
};
static_assert(sizeof(CBitBuffer) == 0x10);

class CBitWrite : public CBitBuffer
{
public:
	CBitWrite(void* pData, const int nBytes, const int nBits = -1)
	{
		StartWriting(pData, nBytes, 0, nBits);
	}

	~CBitWrite()
	{
		TempFlush();
		Q_ASSERT(pData == nullptr || bFlushed);
	}

	void StartWriting(void* pWriteData, const int nBytes, const int iStartBit = 0, const int nBits = -1)
	{
		Q_ASSERT((nBytes & 3) == 0);
		Q_ASSERT((reinterpret_cast<std::uintptr_t>(pWriteData) & 3U) == 0U);
		Q_ASSERT(iStartBit == 0);

		pData = static_cast<std::uint32_t*>(pWriteData);
		pDataOut = pData;
		nDataBytes = nBytes;

		if (nBits == -1)
			nDataBits = nBytes << 3;
		else
		{
			Q_ASSERT(nBits <= (nBytes << 3));
			nDataBits = nBits;
		}

		bOverflow = false;
		uOutBufferWord = 0U;
		nOutBitsAvail = 32;
		pBufferEnd = pDataOut + (nBytes >> 2);
	}

	Q_INLINE void Reset()
	{
		bOverflow = false;
		uOutBufferWord = 0U;
		nOutBitsAvail = 32;
		pDataOut = pData;
	}

	Q_INLINE void TempFlush()
	{
		static_assert(std::endian::native == std::endian::little); // following code assume little-endian

		if (nOutBitsAvail != 32)
		{
			if (pDataOut == pBufferEnd)
				SetOverflowFlag();
			else
				*pDataOut = (*pDataOut & ~arrBitMaskTable[32 - nOutBitsAvail]) | uOutBufferWord;
		}

		bFlushed = true;
	}

	void Seek(const int nPosition)
	{
		TempFlush();
		pDataOut = pData + (nPosition / 32);
		uOutBufferWord = *pDataOut;
		nOutBitsAvail = 32 - (nPosition & 31);
	}

	[[nodiscard]] Q_INLINE unsigned char* GetBasePointer()
	{
		TempFlush();
		return reinterpret_cast<unsigned char*>(pData);
	}

	[[nodiscard]] Q_INLINE unsigned char* GetData()
	{
		return GetBasePointer();
	}

	[[nodiscard]] Q_INLINE int GetNumBitsLeft() const
	{
		return nOutBitsAvail + (32 * (pBufferEnd - pDataOut - 1));
	}

	[[nodiscard]] Q_INLINE int GetNumBitsWritten() const
	{
		return (32 - nOutBitsAvail) + (32 * (pDataOut - pData));
	}

	[[nodiscard]] Q_INLINE int GetNumBytesWritten() const
	{
		return (GetNumBitsWritten() + 7) >> 3;
	}

	Q_INLINE void Flush()
	{
		if (pDataOut == pBufferEnd)
			SetOverflowFlag();
		else
			*pDataOut++ = uOutBufferWord;

		uOutBufferWord = 0U; // we need this because of 32 bit writes. "a <<= 32" is a nop
		nOutBitsAvail = 32;
	}

	Q_INLINE void WriteUBitLong(const std::uint32_t nData, const int nBits, const bool bCheckRange = true)
	{
	#ifdef _DEBUG
		if (bCheckRange && nBits < 32)
			Q_ASSERT(nData <= static_cast<std::uint32_t>(1 << nBits));

		Q_ASSERT(nBits >= 0 && nBits <= 32);
	#endif

		if (nBits <= nOutBitsAvail)
		{
			uOutBufferWord |= bCheckRange ? (nData << (32 - nOutBitsAvail)) : ((nData & arrBitMaskTable[nBits]) << (32 - nOutBitsAvail));
			nOutBitsAvail -= nBits;

			if (nOutBitsAvail == 0)
				Flush();
		}
		else
		{
			// split dwords case
			const int nOverflowBits = nBits - nOutBitsAvail;
			uOutBufferWord |= (nData & arrBitMaskTable[nOutBitsAvail]) << (32 - nOutBitsAvail);

			Flush();

			uOutBufferWord = (nData >> (nBits - nOverflowBits));
			nOutBitsAvail = 32 - nOverflowBits;
		}
	}

	Q_INLINE void WriteSBitLong(const std::int32_t nData, const int nBits)
	{
		WriteUBitLong(static_cast<std::uint32_t>(nData), nBits, false);
	}

	Q_INLINE void WriteFloat(const float flValue)
	{
		WriteUBitLong(std::bit_cast<std::uint32_t>(flValue), 32);
	}

	//bool WriteBits(const void* pInData, int nBits);
	//void WriteBytes(const void* pBuf, int nBytes);

	Q_INLINE void WriteLong(const std::int32_t iValue)
	{
		WriteSBitLong(iValue, 32);
	}

	Q_INLINE void WriteChar(const char chValue)
	{
		WriteSBitLong(chValue, sizeof(char) << 3);
	}

	Q_INLINE void WriteByte(const std::uint8_t uValue)
	{
		WriteUBitLong(uValue, sizeof(std::uint8_t) << 3, false);
	}

	Q_INLINE void WriteShort(const short shValue)
	{
		WriteSBitLong(shValue, sizeof(short) << 3);
	}

	Q_INLINE void WriteWord(const std::uint16_t uValue)
	{
		WriteUBitLong(uValue, sizeof(std::uint16_t) << 3);
	}

	Q_INLINE bool WriteString(const char* szString)
	{
		if (szString != nullptr)
		{
			while (*szString)
				WriteChar(*szString++);
		}

		WriteChar(0);
		return !IsOverflowed();
	}

	//bool WriteWString(const wchar_t* wszString);
	//void WriteLongLong(std::int64_t llValue);

public:
	std::uint32_t uOutBufferWord = 0U; // 0x10
	int nOutBitsAvail = 0U; // 0x14
	std::uint32_t* pDataOut = nullptr; // 0x18
	std::uint32_t* pBufferEnd = nullptr; // 0x1C
	std::uint32_t* pData = nullptr; // 0x20
	bool bFlushed = false; // 0x24
};
static_assert(sizeof(CBitWrite) == 0x28);

class CBitRead : public CBitBuffer
{
	CBitRead(const void* pData, const int nBytes, const int nBits = -1)
	{
		StartReading(pData, nBytes, 0, nBits);
	}

	void StartReading(const void* pReadData, const int nBytes, const int iStartBit, const int nBits = -1)
	{
		Q_ASSERT((reinterpret_cast<std::uintptr_t>(pReadData) & 3U) == 0U);

		pData = static_cast<const std::uint32_t*>(pReadData);
		pDataIn = pData;
		nDataBytes = nBytes;

		if (nBits == -1)
			nDataBits = nBytes << 3;
		else
		{
			Q_ASSERT(nBits <= (nBytes << 3));
			nDataBits = nBits;
		}

		bOverflow = false;
		pBufferEnd = reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const std::uint8_t*>(pData) + nBytes);

		if (pData)
			Seek(iStartBit);
	}

	Q_INLINE void GrabNextDWord(const bool bOverflowImmediately = false)
	{
		static_assert(std::endian::native == std::endian::little); // following code assume little-endian

		if (pDataIn == pBufferEnd)
		{
			nInBitsAvail = 1;
			uInBufferWord = 0U;
			++pDataIn;

			if (bOverflowImmediately)
				SetOverflowFlag();
		}
		else if (pDataIn > pBufferEnd)
		{
			SetOverflowFlag();
			uInBufferWord = 0U;
		}
		else
		{
			Q_ASSERT(reinterpret_cast<std::intptr_t>(pDataIn) + 3 < reinterpret_cast<std::intptr_t>(pBufferEnd));
			uInBufferWord = *pDataIn++;
		}
	}

	Q_INLINE void FetchNext()
	{
		nInBitsAvail = 32;
		GrabNextDWord(false);
	}

	bool Seek(int nPosition)
	{
		// @ida CBitRead::Seek(): engine.dll -> "55 8B EC 51 53 56 57 8B 7D 08 8B F1 C6 45 FF 01"

		bool bSuccess = true;

		if (nPosition < 0 || nPosition > nDataBits)
		{
			SetOverflowFlag();
			nPosition = nDataBits;

			bSuccess = false;
		}

		if (const int nHead = static_cast<int>(nDataBytes & 3U); (nDataBytes < 4) || (nHead > 0 && (nPosition / 8) < nHead))
		{
			// partial first dword
			const std::uint8_t* pPartial = reinterpret_cast<const std::uint8_t*>(pData);

			if (pData != nullptr)
			{
				uInBufferWord = *(pPartial++);
				if (nHead > 1)
					uInBufferWord |= (*pPartial++) << 8;
				if (nHead > 2)
					uInBufferWord |= (*pPartial++) << 16;
			}

			pDataIn = reinterpret_cast<const std::uint32_t*>(pPartial);
			uInBufferWord >>= (nPosition & 31);
			nInBitsAvail = (nHead << 3) - (nPosition & 31);
		}
		else
		{
			const int nAdjustPosition = nPosition - (nHead << 3);
			pDataIn = reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const std::uint8_t*>(pData) + ((nAdjustPosition / 32) << 2) + nHead);

			if (pData != nullptr)
			{
				nInBitsAvail = 32;
				GrabNextDWord();
			}
			else
			{
				uInBufferWord = 0U;
				nInBitsAvail = 1;
			}

			uInBufferWord >>= (nAdjustPosition & 31);
			nInBitsAvail = CRT::Min(nInBitsAvail, 32 - (nAdjustPosition & 31));
		}

		return bSuccess;
	}

	// master/tier1/newbitbuf.cpp#L604
	// @ida CBitRead::ReadBits(): engine.dll -> "55 8B EC 83 EC 08 8B C1 53 8B"

	[[nodiscard]] Q_INLINE int GetNumBitsRead() const
	{
		if (pData == nullptr)
			return 0;

		int nCurrentOffset = (reinterpret_cast<std::intptr_t>(pDataIn) - reinterpret_cast<std::intptr_t>(pData)) / 4 - 1;
		nCurrentOffset *= 32;
		nCurrentOffset += (32 - nInBitsAvail);

		const int nAdjust = static_cast<int>(8U * (nDataBytes & 3U));
		return CRT::Min(nDataBits, nCurrentOffset + nAdjust);
	}

	[[nodiscard]] Q_INLINE int GetNumBytesRead() const
	{
		return ((GetNumBitsRead() + 7) >> 3);
	}

	[[nodiscard]] Q_INLINE std::uint32_t ReadUBitLong(int nBits)
	{
		if (nInBitsAvail >= nBits)
		{
			const std::uint32_t nReturn = uInBufferWord & arrBitMaskTable[nBits];
			nInBitsAvail -= nBits;

			if (nInBitsAvail > 0)
				uInBufferWord >>= nBits;
			else
				FetchNext();

			return nReturn;
		}

		// need to merge words
		std::uint32_t nReturn = uInBufferWord;
		nBits -= nInBitsAvail;
		GrabNextDWord(true);

		if (bOverflow)
			return 0;

		nReturn |= ((uInBufferWord & arrBitMaskTable[nBits]) << nInBitsAvail);
		nInBitsAvail = 32 - nBits;
		uInBufferWord >>= nBits;

		return nReturn;
	}

	[[nodiscard]] Q_INLINE int ReadSBitLong(const int nBits)
	{
		const int nReturn = static_cast<int>(ReadUBitLong(nBits));
		return (nReturn << (32 - nBits)) >> (32 - nBits); // sign extend
	}

	[[nodiscard]] Q_INLINE std::int32_t ReadLong()
	{
		return static_cast<int>(ReadUBitLong(sizeof(std::int32_t) << 3));
	}

	[[nodiscard]] Q_INLINE float ReadFloat()
	{
		const std::uint32_t uValue = ReadUBitLong(sizeof(std::int32_t) << 3);
		return std::bit_cast<float>(uValue);
	}

	[[nodiscard]] Q_INLINE char ReadChar()
	{
		return static_cast<char>(ReadSBitLong(sizeof(char) << 3));
	}

	[[nodiscard]] Q_INLINE std::uint8_t ReadByte()
	{
		return static_cast<std::uint8_t>(ReadUBitLong(sizeof(unsigned char) << 3));
	}

	[[nodiscard]] Q_INLINE short ReadShort()
	{
		return static_cast<short>(ReadSBitLong(sizeof(short) << 3));
	}

	[[nodiscard]] Q_INLINE std::uint16_t ReadWord()
	{
		return static_cast<std::uint16_t>(ReadUBitLong(sizeof(unsigned short) << 3));
	}

	[[nodiscard]] Q_INLINE bool ReadString(char* szStringOut, const int nMaxLength, const bool bLine, int* pOutCharCount = nullptr)
	{
		Q_ASSERT(nMaxLength != 0);

		bool bTooSmall = false;
		int iCurrentChar = 0;

		while (true)
		{
			const char chValue = ReadChar();

			if (chValue == '\0' || (bLine && chValue == '\n'))
				break;

			if (iCurrentChar < nMaxLength - 1)
			{
				szStringOut[iCurrentChar] = chValue;
				++iCurrentChar;
			}
			else
				bTooSmall = true;
		}

		// make sure it's null-terminated
		Q_ASSERT(iCurrentChar < nMaxLength);
		szStringOut[nMaxLength] = '\0';

		if (pOutCharCount != nullptr)
			*pOutCharCount = iCurrentChar;

		return !IsOverflowed() && !bTooSmall;
	}

public:
	std::uint32_t uInBufferWord = 0U; // 0x10
	int nInBitsAvail = 0; // 0x14
	const std::uint32_t* pDataIn = nullptr; // 0x18
	const std::uint32_t* pBufferEnd = nullptr; // 0x1C
	const std::uint32_t* pData = nullptr; // 0x20
};
static_assert(sizeof(CBitRead) == 0x24);

// @note: valve had completely moved to the new bitbuffer structures after leaked source
using bf_write = CBitWrite;
using bf_read = CBitRead;
