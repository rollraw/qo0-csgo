#pragma once
using FNV1A_t = std::uint32_t;

/*
 * 32-BIT FNV1A HASH
 * @credits: underscorediscovery
 */
namespace FNV1A
{
	/* @section: fnv1a constants */
	constexpr std::uint32_t ullBasis = 0x811C9DC5;
	constexpr std::uint32_t ullPrime = 0x1000193;

	/* @section: get */
	/// @param szString : string to generate hash of
	/// @param uValue : key of hash generation
	/// @returns : calculated at compile-time FNV1A hash of given string
	consteval FNV1A_t HashConst(const char* szString, const FNV1A_t uValue = ullBasis) noexcept
	{
		return (szString[0] == '\0') ? uValue : HashConst(&szString[1], (uValue ^ static_cast<FNV1A_t>(szString[0])) * ullPrime);
	}

	/// @param szString : string to generate hash of
	/// @param uValue : key of hash generation
	/// @returns : calculated at run-time FNV1A hash of given string
	inline FNV1A_t Hash(const char* szString, FNV1A_t uValue = ullBasis)
	{
		for (std::size_t i = 0U; i < strlen(szString); ++i)
		{
			uValue ^= szString[i];
			uValue *= ullPrime;
		}

		return uValue;
	}
}
