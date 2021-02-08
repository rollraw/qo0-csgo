#pragma once
using FNV1A_t = std::uint32_t;

/*
 * 32-BIT FNV1A HASH
 * much proper to use fnv1a instead crc32 with randomly access ~1kb 
 * @credits: underscorediscovery
 */
namespace FNV1A
{
	/* fnv1a constants */
	inline constexpr std::uint32_t ullBasis = 0x811C9DC5;
	inline constexpr std::uint32_t ullPrime = 0x1000193;

	/* create compile time hash */
	constexpr FNV1A_t HashConst(const char* szString, const FNV1A_t uValue = ullBasis) noexcept
	{
		return (szString[0] == '\0') ? uValue : HashConst(&szString[1], (uValue ^ FNV1A_t(szString[0])) * ullPrime);
	}

	/* create runtime hash */
	inline FNV1A_t Hash(const char* szString)
	{
		FNV1A_t uHashed = ullBasis;

		for (std::size_t i = 0U; i < strlen(szString); ++i)
		{
			uHashed ^= szString[i];
			uHashed *= ullPrime;
		}

		return uHashed;
	}
}
