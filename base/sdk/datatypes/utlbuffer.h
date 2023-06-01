#pragma once
#include "utlmemory.h"

class CByteSwap
{
public:
	unsigned int bSwapBytes : 1; // does valve know it's even worse than just two bools? the only hope is that the compiler will fix this shit (because even it is smarter than valve)
	unsigned int bBigEndian : 1;
};
static_assert(sizeof(CByteSwap) == 0x4);

// @source: master/public/tier1/utlbuffer.h
// master/tier1/utlbuffer.cpp

class CUtlBuffer
{
public:
	// overflow functions when a get or put overflows
	using UtlBufferOverflowFunc_t = bool(Q_CDECL*)(int nSize);

	enum ESeekType : int
	{
		SEEK_HEAD = 0,
		SEEK_CURRENT,
		SEEK_TAIL
	};

	enum EBufferFlags : unsigned char
	{
		TEXT_BUFFER = 0x1, // describes how get + put work (as strings, or binary)
		EXTERNAL_GROWABLE = 0x2, // this is used w/ external buffers and causes the utlbuf to switch to reallocatable memory if an overflow happens when Putting
		CONTAINS_CRLF = 0x4, // for text buffers only, does this contain \n or \n\r?
		READ_ONLY = 0x8, // for external buffers; prevents null termination from happening
		AUTO_TABS_DISABLED = 0x10 // used to disable/enable push/pop tabs
	};

	Q_INLINE unsigned char GetFlags() const
	{
		return nFlags;
	}

	Q_INLINE bool IsExternallyAllocated() const
	{
		return memory.IsExternallyAllocated();
	}

	/// @returns: current write position
	Q_INLINE int TellPut() const
	{
		return iPut;
	}

	/// @returns: the most written position ever
	Q_INLINE int TellMaxPut() const
	{
		return iMaxPut;
	}

	/// @returns: current read position
	Q_INLINE void* PeekPut(int iPeekOffset)
	{
		return &memory[iPut + iPeekOffset - iOffset];
	}

	Q_INLINE void SeekPut(const ESeekType nType, const int iSeekOffset)
	{
		int iNextPut = iPut;
		switch (nType)
		{
		case SEEK_HEAD:
			iNextPut = iSeekOffset;
			break;
		case SEEK_CURRENT:
			iNextPut += iSeekOffset;
			break;
		case SEEK_TAIL:
			iNextPut = iMaxPut - iSeekOffset;
			break;
		}

		// force a write of the data
		// FIXME: we could make this more optimal potentially by writing out the entire buffer if you seek outside the current range

		// this call will write and will also seek the file to 'iNextPut'
		OnPutOverflow(-iNextPut - 1);
		iPut = iNextPut;

		AddNullTermination(iPut);
	}

	Q_INLINE const void* Base() const
	{
		return memory.Base();
	}

	Q_INLINE void* Base()
	{
		return memory.Base();
	}

	// memory allocation size, does *not* reflect size written or read,
	//	use TellPut or TellGet for that
	Q_INLINE int Size() const
	{
		return memory.AllocationCount();
	}

	/// @returns: true if buffer is a text buffer, false otherwise
	Q_INLINE bool IsText() const
	{
		return (nFlags & TEXT_BUFFER);
	}

	/// @returns: true if buffer can grow if it is externally allocated, false otherwise
	Q_INLINE bool IsGrowable() const
	{
		return (nFlags & EXTERNAL_GROWABLE);
	}

	/// @returns: true if buffer is valid, means didn't ever caused overflow/underflow error, false otherwise
	Q_INLINE bool IsValid() const
	{
		return (nError == 0U);
	}

	/// @returns: true if buffer contains carriage-return linefeeds, false otherwise
	Q_INLINE bool ContainsCRLF() const
	{
		return (IsText() && (nFlags & CONTAINS_CRLF));
	}

	/// @returns: true if buffer is read-only, false otherwise
	Q_INLINE bool IsReadOnly() const
	{
		return (nFlags & READ_ONLY);
	}

	// null terminate the buffer
	// NOTE: Pass in nPut here even though it is just a copy of m_Put.  This is almost always called immediately 
	// after modifying m_Put and this lets it stay in a register and avoid LHS on PPC.
	void AddNullTermination(const int iCurrentPut)
	{
		if (iCurrentPut > iMaxPut)
		{
			if (!IsReadOnly() && (nError & PUT_OVERFLOW) == 0U)
			{
				// add null termination value
				if (CheckPut(1))
					memory[iCurrentPut - iOffset] = 0;
				else
					// restore the overflow state, it was valid before
					nError &= ~PUT_OVERFLOW;
			}

			iMaxPut = iCurrentPut;
		}
	}

protected:
	enum : unsigned char
	{
		PUT_OVERFLOW = 0x1,
		GET_OVERFLOW = 0x2,
		MAX_ERROR_FLAG = GET_OVERFLOW,
	};

	Q_INLINE bool OnPutOverflow(int nSize)
	{
		return (*fnPutOverflow)(nSize);
	}

	Q_INLINE bool OnGetOverflow(int nSize)
	{
		return (*fnGetOverflow)(nSize);
	}

	bool CheckPut(const int nSize)
	{
		if ((nError & PUT_OVERFLOW) || IsReadOnly())
			return false;

		if ((iPut < iOffset) || (memory.AllocationCount() < iPut - iOffset + nSize))
		{
			if (!OnPutOverflow(nSize))
			{
				nError |= PUT_OVERFLOW;
				return false;
			}
		}
		return true;
	}

	bool CheckGet(const int nSize)
	{
		if (nError & GET_OVERFLOW)
			return false;

		if (TellMaxPut() < iGet + nSize)
		{
			nError |= GET_OVERFLOW;
			return false;
		}

		if ((iGet < iOffset) || (memory.AllocationCount() < iGet - iOffset + nSize))
		{
			if (!OnGetOverflow(nSize))
			{
				nError |= GET_OVERFLOW;
				return false;
			}
		}

		return true;
	}

protected:
	CUtlMemory<unsigned char> memory; // 0x00
	int iGet; // 0x0C
	int iPut; // 0x10
	unsigned char nError; // 0x14
	unsigned char nFlags; // 0x15
	unsigned char nReserved; // 0x16
	int iTab; // 0x18
	int iMaxPut; // 0x1C
	int iOffset; // 0x20
	UtlBufferOverflowFunc_t fnGetOverflow; // 0x24
	UtlBufferOverflowFunc_t fnPutOverflow; // 0x28
	CByteSwap byteSwap; // 0x2C
};
static_assert(sizeof(CUtlBuffer) == 0x30);
