#pragma once
// used: [stl] uint32_t
#include <cstdint>

// used: stringcompare
#include "../../utilities/crt.h"

using FNV1A_t = std::uint32_t;

/*
 * 32-BIT FNV1A HASH
 */
namespace FNV1A
{
	/* @section: [internal] constants */
	constexpr FNV1A_t ullBasis = 0x811C9DC5;
	constexpr FNV1A_t ullPrime = 0x1000193;

	/* @section: get */
	/// @param[in] szString string for which you want to generate a hash
	/// @param[in] uKey key of hash generation
	/// @returns: calculated at compile-time hash of given string
	consteval FNV1A_t HashConst(const char* szString, const FNV1A_t uKey = ullBasis) noexcept
	{
		return (szString[0] == '\0') ? uKey : HashConst(&szString[1], (uKey ^ static_cast<FNV1A_t>(szString[0])) * ullPrime);
	}

	/// @param[in] szString string for which you want to generate a hash
	/// @param[in] uKey key of hash generation
	/// @returns: calculated at run-time hash of given string
	inline FNV1A_t Hash(const char* szString, FNV1A_t uKey = ullBasis) noexcept
	{
		const std::size_t nLength = CRT::StringLength(szString);

		for (std::size_t i = 0U; i < nLength; ++i)
		{
			uKey ^= szString[i];
			uKey *= ullPrime;
		}

		return uKey;
	}
}
