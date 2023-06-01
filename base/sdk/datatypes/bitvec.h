#pragma once
// @source: master/public/bitvec.h

template <std::size_t N>
class CBitVec
{
	enum : std::uint32_t
	{
		BITS_PER_INT_LOG2 = 5U,
		BITS_PER_INT = 32U
	};

public:

	[[nodiscard]] std::uint32_t operator[](std::size_t nIndex) const
	{
		Q_ASSERT(nIndex < N); // bit index is out of range
		return (arrUInts[nIndex >> BITS_PER_INT_LOG2] & (1U << (nIndex & (BITS_PER_INT - 1U))));
	}

	[[nodiscard]] Q_INLINE std::uint32_t* Base()
	{
		return arrUInts;
	}

	[[nodiscard]] Q_INLINE const std::uint32_t* Base() const
	{
		return arrUInts;
	}

	Q_INLINE void Set(const std::size_t nBit)
	{
		Q_ASSERT(nBit < N); // bit index is out of range
		arrUInts[nBit >> BITS_PER_INT_LOG2] |= (1U << (nBit & (BITS_PER_INT - 1U)));
	}

	Q_INLINE void Clear(const std::size_t nBit)
	{
		Q_ASSERT(nBit < N); // bit index is out of range
		arrUInts[nBit >> BITS_PER_INT_LOG2] &= ~(1U << (nBit & (BITS_PER_INT - 1U)));
	}

	[[nodiscard]] Q_INLINE std::uint32_t Get(const std::size_t nBit) const
	{
		Q_ASSERT(nBit < N); // bit index is out of range
		return (arrUInts[nBit >> BITS_PER_INT_LOG2] & (1U << (nBit & (BITS_PER_INT - 1U))));
	}

	[[nodiscard]] Q_INLINE bool IsBitSet(const std::size_t nBit) const
	{
		Q_ASSERT(nBit < N); // bit index is out of range
		return (arrUInts[nBit >> BITS_PER_INT_LOG2] & (1U << (nBit & (BITS_PER_INT - 1U)))) != 0U;
	}

private:
	std::uint32_t arrUInts[(N + (BITS_PER_INT - 1U)) / BITS_PER_INT] = { };
};
