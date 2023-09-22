#pragma once
// used: [stl] string, string_view, wstring, wstring_view
#include <string>
// used: [crt] va_list, va_start, va_end
#include <stdarg.h>

#include "../common.h"
// used: findpattern, getabsoluteaddress @todo: temporarily
#include "memory.h"

#ifdef Q_COMPILER_MSC
#include <intrin.h>
#endif

// add support of syntax highlight to our string formatting methods
#ifdef Q_COMPILER_CLANG
#define Q_CRT_FORMAT_STRING_ATTRIBUTE(METHOD, STRING_INDEX, FIRST_INDEX) __attribute__((format(METHOD, STRING_INDEX, FIRST_INDEX)))
#elif defined(__RESHARPER__)
#define Q_CRT_FORMAT_STRING_ATTRIBUTE(METHOD, STRING_INDEX, FIRST_INDEX) [[rscpp::format(METHOD, STRING_INDEX, FIRST_INDEX)]]
#else
#define Q_CRT_FORMAT_STRING_ATTRIBUTE(METHOD, STRING_INDEX, FIRST_INDEX)
#endif

// @todo: '__restrict' keyword to hint compiler on those
// @todo: disable msvc debug runtime checks for those

/*
 * C-RUNTIME
 * - rebuild of C standard library and partially STL
 */
namespace CRT
{
	/*
	 * - according to IEEE-754:
	 *
	 * 1. binary32 floating point
	 *           +/-  exponent (+127)            mantissa (fractional part)
	 *  *-------*---*-----------------*-----------------------------------------------*
	 *  | width | 1 |        8        |                      23                       |
	 *  *-------*---*-----------------*-----------------------------------------------*
	 *  | mask  | 0 | 0 1 1 1 1 1 0 0 | 0 0 0 1 0 0 0 1 1 1 0 1 0 0 0 1 0 1 0 0 1 1 1 |
	 *  *-------*---*-----------------*-----------------------------------------------*
	 *            ^   ^             ^   ^                                           ^
	 *           31   30           23   22                                          0
	 *
	 * 2. binary64 floating point
	 *            +/-    exponent (+1023)                 mantissa (fractional part)
	 *   *-------*---*-----------------------*---------------------------------------------------*
	 *   | width | 1 |          11           |                        52                         |
	 *   *-------*---*-----------------------*---------------------------------------------------*
	 *   | mask  | 0 | 0 1 1 1 1 1 0 0 0 0 0 | 0 0 0 1 0 0 0 1 1 1 0 1 0 0 0 1 0 1 0 0 1 1 1 ... |
	 *   *-------*---*-----------------------*---------------------------------------------------*
	 *             ^   ^                   ^   ^                                               ^
	 *            63   62                 52   51                                              0
	 */
	template <typename T> requires (std::is_floating_point_v<T>)
	struct FloatSpecification_t
	{
		static_assert(std::numeric_limits<T>::is_iec559);
		static_assert(sizeof(long double) == 8, "CRT rebuild doesn't support binary128 format");

		// bit equivalent integer type for given floating point type
		using BitEquivalent_t = std::conditional_t<std::is_same_v<T, double> || std::is_same_v<T, long double>, std::uint64_t, std::uint32_t>;

		static constexpr std::uint32_t MANTISSA_WIDTH = ((sizeof(T) == sizeof(double)) ? 52U : 23U);
		static constexpr std::uint32_t EXPONENT_WIDTH = ((sizeof(T) == sizeof(double)) ? 11U : 8U);
		static constexpr std::uint32_t SIGN_WIDTH = 1U;
		static constexpr std::uint32_t TOTAL_WIDTH = MANTISSA_WIDTH + EXPONENT_WIDTH + SIGN_WIDTH;
		static constexpr std::uint32_t EXPONENT_BIAS = (1U << (EXPONENT_WIDTH - 1U)) - 1U;

		static constexpr BitEquivalent_t MANTISSA_MASK = (BitEquivalent_t(1U) << MANTISSA_WIDTH) - 1U;
		static constexpr BitEquivalent_t SIGN_MASK = (BitEquivalent_t(1U) << (EXPONENT_WIDTH + MANTISSA_WIDTH));
		static constexpr BitEquivalent_t EXPONENT_MASK = ~(SIGN_MASK | MANTISSA_MASK);

		static constexpr BitEquivalent_t QUIET_NAN_MASK = (BitEquivalent_t(1U) << (MANTISSA_WIDTH - 1U));
	};

	/* @section: [internal] constants */
	// largest valid base
	constexpr int _NUMBER_MAX_BASE = 36;
	// every possible character to represent the number with the largest valid base
	constexpr const char* _NUMBER_ALPHA = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	// lookup table for fast integer to string conversion in range [00 .. 99]
	constexpr char _TWO_DIGITS_LUT[ ] =
		"0001020304050607080910111213141516171819"
		"2021222324252627282930313233343536373839"
		"4041424344454647484950515253545556575859"
		"6061626364656667686970717273747576777879"
		"8081828384858687888990919293949596979899";
	// lookup table for fast hex integer to string conversion in range [00 .. FF]
	constexpr char _TWO_DIGITS_HEX_LUT[ ] =
		"000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F"
		"202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F"
		"404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F"
		"606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F"
		"808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9F"
		"A0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
		"C0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
		"E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";

	template <typename T, std::size_t BASE> requires (std::is_integral_v<T> && BASE > 0U && BASE <= _NUMBER_MAX_BASE)
	struct IntegerToString_t
	{
		// maximum count of characters, including terminating null and negative sign where appropriate, needed for integer-to-string conversion
		static consteval std::size_t MaxCount()
		{
			std::size_t nDigitsCount = 0U;

			constexpr std::uint64_t ullNegativeMax = (std::is_unsigned_v<T> ? (std::numeric_limits<T>::max)() : (static_cast<std::uint64_t>((std::numeric_limits<T>::max)()) + 1ULL));

			for (std::uint64_t nShift = ullNegativeMax; nShift > 0ULL; nShift /= BASE)
				nDigitsCount++;

			return (nDigitsCount + (std::is_signed_v<T> && BASE == 10U ? 1U : 0U) + 1U);
		}
	};

	/*
	 * @section: utility
	 * - functions that provide general-purpose functionality
	 */
	#pragma region stl_utility
	// indicate that an object may be "moved from", i.e. allowing the efficient transfer of resources to another object, alternative of 'std::move'
	template <class T>
	[[nodiscard]] Q_INLINE constexpr std::remove_reference_t<T>&& Move(T&& argument) noexcept
	{
		return static_cast<std::remove_reference_t<T>&&>(argument);
	}

	// forward an lvalue as either an lvalue or an rvalue, alternative of 'std::forward'
	template <class T>
	[[nodiscard]] Q_INLINE constexpr T&& Forward(std::remove_reference_t<T>& argument) noexcept
	{
		return static_cast<T&&>(argument);
	}

	// forward an rvalue as an rvalue, alternative of 'std::forward'
	template <class T>
	[[nodiscard]] Q_INLINE constexpr T&& Forward(std::remove_reference_t<T>&& argument) noexcept
	{
		static_assert(!std::is_lvalue_reference_v<T>, "bad forward call");
		return static_cast<T&&>(argument);
	}

	/// swap value of @a'left' to @a'right' and @a'right' to @a'left', alternative of 'std::swap'
	template <typename T>
	requires (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
	Q_INLINE constexpr void Swap(T& left, T& right) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_assignable_v<T>)
	{
		T temporary = Move(left);
		left = Move(right);
		right = Move(temporary);
	}

	#pragma endregion

	/*
	 * @section: algorithm
	 * - functions for a variety of purposes (e.g. searching, sorting, counting, manipulating) that operate on ranges of elements
	 */
	#pragma region stl_algorithm
	/// alternative of 'std::min'
	/// @returns : minimal value of the given comparable values
	template <typename T>
	[[nodiscard]] Q_INLINE constexpr const T& Min(const T& left, const T& right) noexcept
	{
		return (right < left) ? right : left;
	}

	/// alternative of 'std::max'
	/// @returns : maximal value of the given comparable values
	template <typename T>
	[[nodiscard]] Q_INLINE constexpr const T& Max(const T& left, const T& right) noexcept
	{
		return (right > left) ? right : left;
	}

	/// alternative of 'std::clamp'
	/// @returns : value clamped in range ['minimal' .. 'maximal']
	template <typename T>
	[[nodiscard]] Q_INLINE constexpr const T& Clamp(const T& value, const T& minimal, const T& maximal) noexcept
	{
		return (value < minimal) ? minimal : (value > maximal) ? maximal : value;
	}
	#pragma endregion

	/* @section: memory */
	#pragma region crt_memory
	/// compare bytes in two buffers, alternative of 'memcmp()'
	/// @remarks: compares the first count bytes of @a'pFirstBuffer' and @a'pRightBuffer' and return a value that indicates their relationship, performs unsigned character comparison
	/// @returns: <0 - if @a'pFirstBuffer' less than @a'pRightBuffer', 0 - if @a'pFirstBuffer' identical to @a'pRightBuffer', >0 - if @a'pFirstBuffer' greater than @a'pRightBuffer'
	Q_INLINE int MemoryCompare(const void* pLeftBuffer, const void* pRightBuffer, std::size_t nCount)
	{
		auto pLeftByte = static_cast<const std::uint8_t*>(pLeftBuffer);
		auto pRightByte = static_cast<const std::uint8_t*>(pRightBuffer);

		while (nCount--)
		{
			if (*pLeftByte++ != *pRightByte++)
				return pLeftByte[-1] - pRightByte[-1];
		}

		return 0;
	}

	/// compare bytes in two buffers, alternative of 'wmemcmp()'
	/// @remarks: compares the first count characters of @a'pwLeftBuffer' and @a'pwRightBuffer' and return a value that indicates their relationship, performs signed character comparison
	/// @returns: <0 - if @a'pwLeftBuffer' less than @a'pwRightBuffer', 0 - if @a'pwLeftBuffer' identical to @a'pwRightBuffer', >0 - if @a'pwLeftBuffer' greater than @a'pwRightBuffer'
	Q_INLINE int MemoryCompareW(const wchar_t* pwLeftBuffer, const wchar_t* pwRightBuffer, std::size_t nCount)
	{
		while (nCount--)
		{
			if (*pwLeftBuffer++ != *pwRightBuffer++)
				return pwLeftBuffer[-1] - pwRightBuffer[-1];
		}

		return 0;
	}

	/// find character in a buffer, alternative of 'memchr()'
	/// @remarks: look for the first occurrence of @a'uSearch' character in the first @a'nCount' characters of buffer, performs unsigned comparison for elements
	/// @returns: pointer to the found character on success, null otherwise
	Q_INLINE void* MemoryChar(const void* pBuffer, const std::uint8_t uSearch, std::size_t nCount)
	{
		auto pByte = static_cast<const std::uint8_t*>(pBuffer);

		while (nCount--)
		{
			if (*pByte == uSearch)
				return const_cast<std::uint8_t*>(pByte);

			++pByte;
		}

		return nullptr;
	}

	/// find wide character in a buffer, alternative of 'wmemchr()'
	/// @remarks: look for the first occurrence of @a'wSearch' character in the first @a'nCount' wide characters of buffer, performs signed comparison for elements
	/// @returns: pointer to the found wide character on success, null otherwise
	Q_INLINE wchar_t* MemoryCharW(wchar_t* pwBuffer, const wchar_t wSearch, std::size_t nCount)
	{
		while (nCount--)
		{
			if (*pwBuffer == wSearch)
				return pwBuffer;

			++pwBuffer;
		}

		return nullptr;
	}

	/// set a buffer to a specified byte, alternative of 'memset()'
	/// @remarks: sets the first @a'nCount' bytes of @a'pDestination' to the @a'uByte'
	/// @returns: pointer to the @a'pDestination'
	Q_INLINE void* MemorySet(void* pDestination, const std::uint8_t uByte, std::size_t nCount)
	{
	#ifdef Q_COMPILER_MSC
		// @test: clang always tries to detect 'memset' like instructions and replace them with CRT's function call
		if (const std::size_t nCountAlign = (nCount & 3U); nCountAlign == 0U)
		{
			auto pDestinationLong = static_cast<unsigned long*>(pDestination);
			__stosd(pDestinationLong, static_cast<unsigned long>(uByte) * 0x01010101, nCount >> 2U);
		}
		else if (nCountAlign == 2U)
		{
			auto pDestinationWord = static_cast<unsigned short*>(pDestination);
			__stosw(pDestinationWord, static_cast<unsigned short>(uByte | (uByte << 8U)), nCount >> 1U);
		}
		else
		{
			auto pDestinationByte = static_cast<unsigned char*>(pDestination);
			__stosb(pDestinationByte, uByte, nCount);
		}
	#else
		auto pDestinationByte = static_cast<std::uint8_t*>(pDestination);

		while (nCount--)
			*pDestinationByte++ = uByte;
	#endif

		return pDestination;
	}

	/// copy one buffer to another, alternative of 'memcpy()'
	/// @remarks: copies @a'nCount' bytes from @a'pSource' to @a'pDestination'. if the source and destination regions overlap, the behavior is undefined
	/// @returns: pointer to the @a'pDestination'
	Q_INLINE void* MemoryCopy(void* pDestination, const void* pSource, std::size_t nCount)
	{
	#ifdef Q_COMPILER_MSC
		// @test: clang always tries to detect 'memcpy' like instructions and replace them with CRT's function call
		if (const std::size_t nCountAlign = (nCount & 3U); nCountAlign == 0U)
		{
			auto pDestinationLong = static_cast<unsigned long*>(pDestination);
			auto pSourceLong = static_cast<const unsigned long*>(pSource);
			__movsd(pDestinationLong, pSourceLong, nCount >> 2U);
		}
		else if (nCountAlign == 2U)
		{
			auto pDestinationWord = static_cast<unsigned short*>(pDestination);
			auto pSourceWord = static_cast<const unsigned short*>(pSource);
			__movsw(pDestinationWord, pSourceWord, nCount >> 1U);
		}
		else
		{
			auto pDestinationByte = static_cast<unsigned char*>(pDestination);
			auto pSourceByte = static_cast<const unsigned char*>(pSource);
			__movsb(pDestinationByte, pSourceByte, nCount);
		}
	#else
		auto pDestinationByte = static_cast<std::uint8_t*>(pDestination);
		auto pSourceByte = static_cast<const std::uint8_t*>(pSource);

		while (nCount--)
			*pDestinationByte++ = *pSourceByte++;
	#endif

		return pDestination;
	}

	/// move one buffer to another, alternative of 'memmove()'
	/// @remarks: copies @a'nCount' bytes from @a'pSource' to @a'pDestination'. if some portions of the source and the destination regions overlap, both functions ensure that the original source bytes in the overlapping region are copied before being overwritten
	/// @returns: pointer to the @a'pDestination'
	Q_INLINE void* MemoryMove(void* pDestination, const void* pSource, std::size_t nCount)
	{
		auto pDestinationByte = static_cast<std::uint8_t*>(pDestination);
		auto pSourceByte = static_cast<const std::uint8_t*>(pSource);

		// perform copy when source greater than destination
		if (pDestinationByte < pSourceByte)
		{
		#ifdef Q_COMPILER_MSC
			// @todo: also check for available align for given nCount
			__movsb(pDestinationByte, pSourceByte, nCount);
		#else
			while (nCount--)
				*pDestinationByte++ = *pSourceByte++;
		#endif
		}
		// inverse copy otherwise
		else
		{
			std::uint8_t* pLastDestinationByte = pDestinationByte + (nCount - 1U);
			const std::uint8_t* pLastSourceByte = pSourceByte + (nCount - 1U);

			while (nCount--)
				*pLastDestinationByte-- = *pLastSourceByte--;
		}

		return pDestination;
	}
	#pragma endregion

	/*
	 * @section: character
	 * - valid only for default C locale
	 */
	#pragma region crt_characters
	/// alternative of 'iscntrl()', @todo: 'iswcntrl()'
	/// @returns: true if given character is a control character, false otherwise
	[[nodiscard]] constexpr bool IsControl(const std::uint8_t uChar)
	{
		return (uChar <= 0x1F || uChar == 0x7F);
	}

	/// alternative of 'isdigit()', @todo: 'iswdigit()'
	/// @returns: true if given character is decimal digit, false otherwise
	[[nodiscard]] constexpr bool IsDigit(const std::uint8_t uChar)
	{
		return (uChar >= '0' && uChar <= '9');
	}

	/// alternative of 'isxdigit()', @todo: 'iswxdigit()'
	/// @returns: true if given character is hexadecimal digit, false otherwise
	[[nodiscard]] constexpr bool IsHexDigit(const std::uint8_t uChar)
	{
		return ((uChar >= '0' && uChar <= '9') || (uChar >= 'A' && uChar <= 'F') || (uChar >= 'a' && uChar <= 'f'));
	}

	/// alternative of 'isblank()', @todo: 'iswblank()'
	/// @returns: true if given character is blank, false otherwise
	[[nodiscard]] constexpr bool IsBlank(const std::uint8_t uChar)
	{
		return (uChar == '\t' || uChar == ' ');
	}

	/// alternative of 'isspace()', @todo: 'iswspace()'
	/// @returns: true if given character is whitespace, false otherwise
	[[nodiscard]] constexpr bool IsSpace(const std::uint8_t uChar)
	{
		return ((uChar >= '\t' && uChar <= '\r') || uChar == ' ');
	}

	/// alternative of 'isalpha()', @todo: 'iswalpha()'
	/// @returns: true if given character is alphabetic, false otherwise
	[[nodiscard]] constexpr bool IsAlpha(const std::uint8_t uChar)
	{
		return ((uChar >= 'A' && uChar <= 'Z') || (uChar >= 'a' && uChar <= 'z'));
	}

	/// alternative of 'isalnum()', @todo: 'iswalnum()'
	/// @returns: true if given character is alphabetic or numeric, false otherwise
	[[nodiscard]] constexpr bool IsAlphaNum(const std::uint8_t uChar)
	{
		return ((uChar >= '0' && uChar <= '9') || (uChar >= 'A' && uChar <= 'Z') || (uChar >= 'a' && uChar <= 'z'));
	}

	/// alternative of 'isprint()', @todo: 'iswprint()'
	/// @returns: true if given character is printable, false otherwise
	[[nodiscard]] constexpr bool IsPrint(const std::uint8_t uChar)
	{
		return (uChar >= ' ' && uChar <= '~');
	}

	/// alternative of 'isgraph()', @todo: 'iswgraph()'
	/// @returns: true if given character is graphic (has a graphical representation), false otherwise
	[[nodiscard]] constexpr bool IsGraph(const std::uint8_t uChar)
	{
		return (uChar >= '!' && uChar <= '~');
	}

	/// alternative of 'ispunct()', @todo: 'iswpunct()'
	/// @returns: true if given character is a punctuation character, false otherwise
	[[nodiscard]] constexpr bool IsPunct(const std::uint8_t uChar)
	{
		return ((uChar >= '!' && uChar <= '/') || (uChar >= ':' && uChar <= '@') || (uChar >= '[' && uChar <= '`') || (uChar >= '{' && uChar <= '~'));
	}

	/// alternative of 'isupper()', @todo: 'iswupper()'
	/// @returns: true if given alphabetic character is uppercase, false otherwise
	[[nodiscard]] constexpr bool IsUpper(const std::uint8_t uChar)
	{
		return (uChar >= 'A' && uChar <= 'Z');
	}

	/// alternative of 'islower()', @todo: 'iswlower()'
	/// @returns: true if given alphabetic character is lowercase, false otherwise
	[[nodiscard]] constexpr bool IsLower(const std::uint8_t uChar)
	{
		return (uChar >= 'a' && uChar <= 'z');
	}
	#pragma endregion

	/* @section: character conversion */
	#pragma region crt_character_conversion
	/// convert single digit character to integer
	/// @returns: converted value if character is digit, 0 otherwise
	[[nodiscard]] constexpr std::int32_t CharToInt(const std::uint8_t uChar)
	{
		return IsDigit(uChar) ? (uChar - '0') : 0;
	}

	/// convert single hex digit character to integer
	/// @returns: converted value if character is hex digit, 0 otherwise
	[[nodiscard]] constexpr std::uint32_t CharToHexInt(const std::uint8_t uChar)
	{
		const std::uint8_t uCharLower = (uChar | ('a' ^ 'A'));
		return ((uCharLower >= 'a' && uCharLower <= 'f') ? (uCharLower - 'a' + 0xA) : (IsDigit(uChar) ? (uChar - '0') : 0x0));
	}

	// convert single character to uppercase, alternative of 'toupper()', @todo: 'towupper()'
	[[nodiscard]] constexpr char CharToUpper(const std::uint8_t uChar)
	{
		return static_cast<char>(IsLower(uChar) ? (uChar & ~('a' ^ 'A')) : uChar);
	}

	// convert single character to lowercase, alternative of 'tolower()', @todo: 'towlower()'
	[[nodiscard]] constexpr char CharToLower(const std::uint8_t uChar)
	{
		return static_cast<char>(IsUpper(uChar) ? (uChar | ('a' ^ 'A')) : uChar);
	}
	#pragma endregion

	/*
	 * @section: string
	 * - @note: return value of some functions correspond to the POSIX standard but not C standard, it was necessary to reduce time complexity of them
	 * - valid only for default C locale
	 */
	#pragma region crt_string
	/// get the length of a string, alternative of 'strlen()', 'wcslen()'
	/// @returns: number of characters in the string, not including the terminating null character
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr std::size_t StringLength(const C* tszSource)
	{
		const C* tszSourceEnd = tszSource;

		while (*tszSourceEnd != C('\0'))
			++tszSourceEnd;

		return tszSourceEnd - tszSource;
	}

	/// get the length of a string limited by max length, alternative of 'strnlen()', 'wcsnlen()'
	/// @returns: number of characters in the string, not including the terminating null character. if there is no null terminator within the first @a'nMaxLength' bytes of the string, then @a'nMaxLength' is returned to indicate the error condition
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr std::size_t StringLengthN(const C* tszSource, const std::size_t nMaxLength)
	{
		std::size_t i = 0U;

		while (tszSource[i] != C('\0') && i < nMaxLength)
			++i;

		return i;
	}

	/// compare two strings, alternative of 'strcmp()', 'wcscmp()'
	/// @remarks: performs a signed/unsigned character comparison depending on the string type
	/// @returns: <0 - if @a'tszLeft' less than @a'tszRight', 0 - if @a'tszLeft' is identical to @a'tszRight', >0 - if @a'tszLeft' greater than @a'tszRight'
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr int StringCompare(const C* tszLeft, const C* tszRight)
	{
		using ComparisonType_t = std::conditional_t<std::is_same_v<C, char>, std::uint8_t, std::conditional_t<sizeof(wchar_t) == 2U, std::int16_t, std::int32_t>>;

		ComparisonType_t nLeft, nRight;
		do
		{
			nLeft = static_cast<ComparisonType_t>(*tszLeft++);
			nRight = static_cast<ComparisonType_t>(*tszRight++);

			if (nLeft == C('\0'))
				break;
		} while (nLeft == nRight);

		return nLeft - nRight;
	}

	/// case-insensitive compare two strings, alternative of 'stricmp()', @todo: 'wcsicmp()'
	/// @remarks: performs unsigned character comparison
	/// @returns: <0 - if @a'szLeft' less than @a'szRight', 0 - if @a'szLeft' is identical to @a'szRight', >0 - if @a'szLeft' greater than @a'szRight'
	constexpr int StringCompareI(const char* szLeft, const char* szRight)
	{
		std::uint8_t uLeft, uRight;
		do
		{
			uLeft = static_cast<std::uint8_t>(CharToLower(static_cast<std::uint8_t>(*szLeft++)));
			uRight = static_cast<std::uint8_t>(CharToLower(static_cast<std::uint8_t>(*szRight++)));

			if (uLeft == '\0')
				break;
		} while (uLeft == uRight);

		return uLeft - uRight;
	}

	/// compare two strings up to the specified count of characters, alternative of 'strncmp()', 'wcsncmp()'
	/// @remarks: performs a signed/unsigned character comparison depending on the string type
	/// @returns: <0 - if @a'tszLeft' less than @a'tszRight', 0 - if 'tszLeft' is identical to @a'tszRight', >0 - if @a'tszLeft' greater than @a'tszRight'
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr int StringCompareN(const C* tszLeft, const C* tszRight, std::size_t nCount)
	{
		using ComparisonType_t = std::conditional_t<std::is_same_v<C, char>, std::uint8_t, std::conditional_t<sizeof(wchar_t) == 2U, std::int16_t, std::int32_t>>;

		ComparisonType_t nLeft, nRight;
		while (nCount--)
		{
			nLeft = static_cast<ComparisonType_t>(*tszLeft++);
			nRight = static_cast<ComparisonType_t>(*tszRight++);

			if (nLeft != nRight)
				return nLeft - nRight;

			if (nLeft == C('\0'))
				break;
		}

		return 0;
	}

	/// find a character in a string, alternative of 'strchr()', 'wcschr()'
	/// @remarks: the null terminating character is included in the search, performs signed character comparison
	/// @returns: pointer to the first found occurrence of @a'iSearch' character in @a'tszSource' on success, null otherwise
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr C* StringChar(const C* tszSource, const int iSearch)
	{
		while (*tszSource != C('\0'))
		{
			if (*tszSource == iSearch)
				return const_cast<C*>(tszSource);

			++tszSource;
		}

		return nullptr;
	}

	/// find a last occurrence of character in a string, alternative of 'strrchr()', 'wcsrchr()'
	/// @remarks: the null terminating character is included in the search, performs signed character comparison
	/// @returns: pointer to the last found occurrence of @a'iSearch' character in @a'tszSource' on success, null otherwise
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr C* StringCharR(const C* tszSource, const int iSearch)
	{
		C* tszLastOccurrence = nullptr;

		do
		{
			if (*tszSource == iSearch)
				tszLastOccurrence = const_cast<C*>(tszSource);
		} while (*tszSource++ != C('\0'));

		return tszLastOccurrence;
	}

	/// search for one string inside another, alternative of 'strstr()', 'wcsstr()'
	/// @remarks: finds the first occurrence of @a'tszSearch' in @a'tszSource'. the search does not include terminating null character, performs signed character comparison
	/// @returns: pointer to the first found occurrence of @a'tszSearch' substring in @a'tszSource' on success, null otherwise
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr C* StringString(const C* tszSource, const C* tszSearch)
	{
		while (*tszSource != C('\0'))
		{
			while (*tszSearch != C('\0') && (*tszSource - *tszSearch) == C('\0'))
			{
				++tszSource;
				++tszSearch;
			}

			if (*tszSearch == C('\0'))
				return const_cast<C*>(tszSource);

			++tszSource;
		}

		return nullptr;
	}

	/// case-insensitive search for one string inside another, alternative of 'strcasestr()'
	/// @remarks: finds the first occurrence of @a'szSearch' in @a'szSource'. the search does not include terminating null character, performs unsigned character comparison
	/// @returns: pointer to the first found occurrence of @a'szSearch' substring in @a'szSource' on success, null otherwise
	constexpr char* StringStringI(const char* szSource, const char* szSearch)
	{
		while (*szSource != '\0')
		{
			while (*szSearch != '\0' && CharToLower(static_cast<std::uint8_t>(*szSource)) == CharToLower(static_cast<std::uint8_t>(*szSearch)))
			{
				++szSource;
				++szSearch;
			}

			if (*szSearch == '\0')
				return const_cast<char*>(szSource);

			++szSource;
		}

		return nullptr;
	}

	/// copy a one string to another, alternative of 'stpcpy()', 'wcpcpy()'
	/// @remarks: copies @a'szSource', including the terminating null character, to the location that's specified by @a'szDestination'. the behavior is undefined if the source and destination strings overlap
	/// @returns: pointer to the terminating null in @a'tszDestination'
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr C* StringCopy(C* tszDestination, const C* tszSource)
	{
		while (*tszSource != C('\0'))
			*tszDestination++ = *tszSource++;

		*tszDestination = C('\0');
		return tszDestination;
	}

	/// copy a one string to another up to the specified count of characters, alternative of 'stpncpy()', 'wcpncpy()'
	/// @remarks: copies the initial @a'nCount' characters of @a'tszSource' to @a'tszDestination'. if count is less than or equal to the length of @a'tszSource', a null character is not appended automatically to the copied string. if @a'nCount' is greater than the length of @a'tszSource', the destination string is padded with null characters up to length count. the behavior is undefined if the source and destination strings overlap
	/// @returns: pointer to @a'tszDestination' + @a'nCount'
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr C* StringCopyN(C* tszDestination, const C* tszSource, std::size_t nCount)
	{
		while (nCount--)
			*tszDestination++ = (*tszSource != C('\0') ? *tszSource++ : C('\0'));

		return tszDestination;
	}

	/// append a one string to another, alternative of 'stpcat()', 'wcpcat()'
	/// @remarks: appends @a'tszSource' to @a'tszDestination' and terminates the resulting string with a null character. the initial character of @a'tszSource' overwrites the terminating null character of @a'tszDestination'. the behavior is undefined if the source and destination strings overlap
	/// @returns: pointer to the terminating null in @a'tszDestination'
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr C* StringCat(C* tszDestination, const C* tszSource)
	{
		while (*tszDestination != C('\0'))
			++tszDestination;

		while (*tszSource != C('\0'))
			*tszDestination++ = *tszSource++;

		*tszDestination = C('\0');
		return tszDestination;
	}

	/// append a one string to another up to the specified count of characters, alternative of 'stpncat()', 'wcpncat()'
	/// @remarks: appends, at most, the first @a'nCount' characters of @a'tszSource' to @a'tszDestination'. the initial character of @a'tszSource' overwrites the terminating null character of @a'tszDestination'. if a null character appears in @a'tszSource' before @a'nCount' characters are appended, function appends all characters from @a'tszSource', up to the null character. if count is greater than the length of @a'tszSource', the length of @a'tszSource' is used in place of count. in all cases, the resulting string is terminated with a null character. if copying takes place between strings that overlap, the behavior is undefined
	/// @returns: pointer to the terminating null in @a'tszDestination'
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	constexpr C* StringCatN(C* tszDestination, const C* tszSource, std::size_t nCount)
	{
		while (*tszDestination != C('\0'))
			++tszDestination;

		while (*tszSource != C('\0') && nCount--)
			*tszDestination++ = *tszSource++;

		*tszDestination = C('\0');
		return tszDestination;
	}
	#pragma endregion

	#pragma region crt_string_format
	/// write formatted data to a string, alternative of 'sprintf'
	/// @returns: the number of characters written to the formatted data string, not including the terminating null character. a return value of -1 indicates that an encoding error has occurred
	inline Q_CRT_FORMAT_STRING_ATTRIBUTE(printf, 2, 3) int StringPrint(char* szBuffer, const char* const szFormat, ...)
	{
		// @todo: because i dont have time so yeah, will rebuild it some time
		/*
		switch ()
		{
		case 'A':
			break;
		case 'a':
			break;
		case 'c':
			break;
		case 'd':
			break;
		case 'F':
			break;
		case 'f':
			break;
		case 's':
			break;
		case 'i':
			break;
		case 'u':
			break;
		case 'o':
			break;
		case 'x':
			break;
		case 'X':
			break;
		case 'n':
			break;
		case 'p':
			break;
		case '%':
			break;
		default:
			break;
		}
		 */

		static auto fn_sprintf = reinterpret_cast<int(Q_CDECL*)(char*, const char* const, ...)>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, Q_XOR("E8 ? ? ? ? 8A 75 18")) + 0x1));

		va_list argList;
		va_start(argList, szFormat);
		const int iReturn = fn_sprintf(szBuffer, szFormat, argList);
		va_end(argList);

		return iReturn;
	}

	/// write formatted data to a string up to the specified count of characters, alternative of 'snprintf'
	/// @remarks: format and store @a'nCount' or fewer characters in @a'szBuffer'. always store a terminating null character, truncating the output if necessary. if copying occurs between strings that overlap, the behavior is undefined
	/// @returns: if the buffer size specified by @a'nCount' isn't sufficiently large to contain the output specified by @a'szFormat', the return value is the number of characters that would be written, not including the terminating null character, if @a'nCount' were sufficiently large. if the return value is greater than @a'nCount' - 1, the output has been truncated. a return value of -1 indicates that an encoding error has occurred
	inline Q_CRT_FORMAT_STRING_ATTRIBUTE(printf, 3, 4) int StringPrintN(char* szBuffer, std::size_t nCount, const char* const szFormat, ...)
	{
		// @todo: because i dont have time so yeah, will rebuild it some time
		static auto fn_snprintf = reinterpret_cast<int(Q_CDECL*)(char*, std::size_t, const char* const, ...)>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, Q_XOR("E8 ? ? ? ? C6 45 F7 00")) + 0x1));

		va_list argList;
		va_start(argList, szFormat);
		const int iReturn = fn_snprintf(szBuffer, nCount, szFormat, argList);
		va_end(argList);

		return iReturn;
	}
	#pragma endregion

	/*
	 * @section: string conversion
	 * - these functions can write past the end of a buffer that is too small.
	 *   to prevent buffer overruns, ensure that buffer is large enough to hold the converted data and the trailing null-character.
	 *   misuse of these functions can cause serious security issues in your code
	 * - valid only for default C locale
	 */
	#pragma region crt_string_conversion
	/// convert every char in the string to uppercase
	/// @returns: pointer to the @a'szDestination'
	constexpr char* StringToUpper(char* szDestination)
	{
		char* szDestinationOut = szDestination;

		while (*szDestinationOut++ != '\0')
			*szDestinationOut = CharToUpper(static_cast<std::uint8_t>(*szDestinationOut));

		return szDestination;
	}

	/// convert every char in the string to lowercase
	/// @returns: pointer to the @a'szDestination'
	constexpr char* StringToLower(char* szDestination)
	{
		char* szDestinationOut = szDestination;

		while (*szDestinationOut++ != '\0')
			*szDestinationOut = CharToLower(static_cast<std::uint8_t>(*szDestinationOut));

		return szDestination;
	}

	// @todo: rework sprintf like, with specific format right here
	/// convert the integer to a string, alternative of 'to_string', '_itoa_s', '_ltoa_s', '_ultoa_s', '_i64toa_s', '_ui64toa_s'
	/// @param[in] iBase numeric base to use to represent number in range [2 .. 36]
	/// @returns: pointer to the begin of converted integer in the buffer
	template <typename T> requires std::is_integral_v<T>
	char* IntegerToString(const T value, char* szDestination, const std::size_t nDestinationLength, int iBase = 10)
	{
		if (iBase < 0 || iBase == 1 || iBase > _NUMBER_MAX_BASE)
		{
			Q_ASSERT(false); // given number base is out of range
			return szDestination;
		}

		const bool bIsPositive = (value >= 0);
		std::make_unsigned_t<T> uValue = (bIsPositive ? static_cast<std::make_unsigned_t<T>>(value) : static_cast<std::make_unsigned_t<T>>(0 - value)); // @test: how it actually compiles, can avoid branch at compile time

		char* szDestinationEnd = szDestination + nDestinationLength;
		*--szDestinationEnd = '\0';

		if (uValue == 0U)
			*--szDestinationEnd = '0';
		// for decimal base perform fast path write by two digits in a group
		else if (iBase == 10)
		{
			while (uValue >= 100U)
			{
				const char* szTwoDigits = &_TWO_DIGITS_LUT[(uValue % 100U) * 2U];
				*--szDestinationEnd = szTwoDigits[1];
				*--szDestinationEnd = szTwoDigits[0];
				uValue /= 100U;
			}

			if (uValue < 10U)
				*--szDestinationEnd = _NUMBER_ALPHA[uValue];
			else
			{
				const char* szTwoDigits = &_TWO_DIGITS_LUT[uValue * 2U];
				*--szDestinationEnd = szTwoDigits[1];
				*--szDestinationEnd = szTwoDigits[0];
			}

			// insert negative sign, only decimal base can have it
			if (!bIsPositive)
				*--szDestinationEnd = '-';
		}
		// for hexadecimal base perform fast path write by two digits in a group
		else if (iBase == 16)
		{
			while (uValue >= 0x100)
			{
				const char* szTwoDigits = &_TWO_DIGITS_HEX_LUT[(uValue % 0x100) * 2U];
				*--szDestinationEnd = szTwoDigits[1];
				*--szDestinationEnd = szTwoDigits[0];
				uValue /= 0x100;
			}

			if (uValue < 0x10)
				*--szDestinationEnd = _NUMBER_ALPHA[uValue];
			else
			{
				const char* szTwoDigits = &_TWO_DIGITS_HEX_LUT[uValue * 2U];
				*--szDestinationEnd = szTwoDigits[1];
				*--szDestinationEnd = szTwoDigits[0];
			}
		}
		// for other bases perform write by single digit
		else
		{
			while (uValue > 0U)
			{
				*--szDestinationEnd = _NUMBER_ALPHA[uValue % iBase];
				uValue /= iBase;
			}
		}

		return szDestinationEnd;
	}

	// @todo: rework sprintf like, with specific format right here
	/// convert the floating point to a string, alternative of 'to_string'
	/// @returns:
	template <typename T> requires std::is_floating_point_v<T>
	char* FloatToString(const T value, char* szDestination, const std::size_t nDestinationSize, int iPrecision = std::numeric_limits<T>::digits10)
	{
		// @todo: because i dont have time so yeah, would rebuild it some time
		StringPrintN(szDestination, nDestinationSize, "%.*f", iPrecision, value);
		return szDestination;
	}

	/**
	 * convert the time point to a string, alternative of 'strftime', 'wcsftime'
	 * @param[in] tszFormat string specifying the format of conversion. formatting codes are:
	 * - '%a' abbreviated weekday name
	 * - '%A' full weekday name
	 * - '%b' abbreviated month name
	 * - '%B' full month name
	 * - '%c' date and time representation appropriate for locale
	 * - '%C' century as a decimal number [0 .. 99]
	 * - '%d' day of month as a decimal number [01 .. 31]
	 * - '%D' equivalent to "%m/%d/%y"
	 * - '%e' day of month as a decimal number [1 .. 31], where single digits are preceded by a space
	 * - '%F' equivalent to "%Y-%m-%d"
	 * - '%g' week-based year without century as a decimal number, the ISO 8601 time format [00 .. 99]
	 * - '%G' week-based year as a decimal number, the ISO 8601 time format [0000 .. 9999]
	 * - '%h' Abbreviated month name(equivalent to % b)
	 * - '%H' hour in 24-hour format [00 .. 23]
	 * - '%I' hour in 12-hour format [01 .. 12]
	 * - '%j' day of the year as a decimal number [001 .. 366]
	 * - '%m' month as a decimal number [01 .. 12]
	 * - '%M' minute as a decimal number [00 .. 59]
	 * - '%n' new line character escape
	 * - '%p' the locale's A.M./P.M. indicator for 12-hour clock
	 * - '%r' the locale's 12-hour clock time
	 * - '%R' equivalent to "%H:%M"
	 * - '%S' second as decimal number [00 .. 59]
	 * - '%t' tab character escape
	 * - '%T' equivalent to "%H:%M:%S", the ISO 8601 time format
	 * - '%u' weekday as a decimal number, the ISO 8601 time format [1 .. 7]
	 * - '%U' week number of the year as a decimal number, where the first Sunday is the first day of week 1 [00 .. 53]
	 * - '%V' week number as a decimal number, the ISO 8601 time format [00 .. 53]
	 * - '%w' weekday as a decimal number [0 .. 6]
	 * - '%W' week number of the year as a decimal number, where the first Monday is the first day of week 1 [00 .. 53]
	 * - '%x' date representation for the locale
	 * - '%X' time representation for the locale
	 * - '%y' year without century, as decimal number [00 .. 99]
	 * - '%Y' year with century, as decimal number [1900 .. 9999]
	 * - '%z' the offset from UTC, the ISO 8601 format
	 * - '%Z' either the locale's time-zone name or time zone abbreviation, depending on registry settings
	 * - '%%' percent character escape
	 * @returns: the number of characters placed in @a'szDestination' not including the terminating null, if the total number of characters, including the terminating null, is more than @a'nDestinationSize', returns 0 and the contents of @a'szDestination' are indeterminate
	 **/
	template <typename C> requires (std::is_same_v<C, char> || std::is_same_v<C, wchar_t>)
	std::size_t TimeToString(C* tszDestination, const std::size_t nDestinationSize, const C* tszFormat, const std::tm* pTime)
	{
		// full names of weekdays
		constexpr const char* arrWeekdayNames[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
		// full names of months
		constexpr const char* arrMonthNames[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

		C* tszDestinationOut = tszDestination;
		C* tszFormatCurrent = nullptr;

		// try to find format token, at same time copy everything until it
		while (*tszFormat != C('\0') && nDestinationSize > 0U)
		{
			if (*tszFormat == C('%'))
			{
				tszFormatCurrent = const_cast<C*>(tszFormat);
				break;
			}

			*tszDestinationOut++ = *tszFormat++;
		}

		std::size_t nWroteCount = tszDestinationOut - tszDestination;

		// check do we have nothing to format
		if (tszFormatCurrent == nullptr)
			return tszDestinationOut - tszDestination;

		while (*tszFormatCurrent != C('\0') && nWroteCount < nDestinationSize)
		{
			// check is format token begin
			if (*tszFormatCurrent == C('%'))
			{
				++tszFormatCurrent;

				// we are don't support any locale and other modifiers
				if (*tszFormatCurrent == C('#') || *tszFormatCurrent == C('0') || *tszFormatCurrent == C('E'))
					++tszFormatCurrent;

				const C* tszDestinationBefore = tszDestinationOut;

				/*
				 * handle format token
				 * @test: C standard and POSIX are very unclear and don't explicitly specify the behaviour when time components are negative
				 */
				switch (*tszFormatCurrent)
				{
				case C('a'):
				{
					const char* szWeekdayName = arrWeekdayNames[pTime->tm_wday];

					std::size_t i = 3U;
					while (i-- > 0U)
						*tszDestinationOut++ = static_cast<C>(szWeekdayName[i]);

					break;
				}
				case C('A'):
				{
					const char* szWeekdayName = arrWeekdayNames[pTime->tm_wday];

					while (*szWeekdayName != C('\0'))
						*tszDestinationOut++ = static_cast<C>(*szWeekdayName++);

					break;
				}
				case C('b'):
				case C('h'): // equivalent to "%b"
				{
					const char* szMonthName = arrMonthNames[pTime->tm_mon];

					std::size_t i = 3U;
					while (i-- > 0U)
						*tszDestinationOut++ = static_cast<C>(szMonthName[i]);

					break;
				}
				case C('B'):
				{
					const char* szMonthName = arrMonthNames[pTime->tm_mon];

					while (*szMonthName != C('\0'))
						*tszDestinationOut++ = static_cast<C>(*szMonthName++);

					break;
				}
				case C('c'):
				{
					// default locale equivalent to "%a %b %e %H:%M:%S %Y"

					// @todo: is it possible to avoid these ugly branches and make string type dependent on template
					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR(L"%a %b %e %H:%M:%S %Y"), pTime);
					else
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR("%a %b %e %H:%M:%S %Y"), pTime);

					break;
				}
				case C('C'):
				{
					const int iYear = pTime->tm_year + 1900;
					const int iCentury = iYear / 100;

					if (iYear >= -99 && iYear < 0)
					{
						*tszDestinationOut++ = C('-');
						*tszDestinationOut++ = C('0');
					}
					else if (iCentury >= 0 && iCentury < 100)
					{
						const char* szCenturyNumber = &_TWO_DIGITS_LUT[static_cast<unsigned int>(iCentury) * 2U];
						*tszDestinationOut++ = static_cast<C>(*szCenturyNumber++);
						*tszDestinationOut++ = static_cast<C>(*szCenturyNumber);
					}
					else
						*tszDestinationOut++ = static_cast<C>(iCentury % 10U + '0');

					break;
				}
				case C('d'):
				{
					const char* szMonthDayNumber = &_TWO_DIGITS_LUT[static_cast<unsigned int>(pTime->tm_mday) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szMonthDayNumber++);
					*tszDestinationOut++ = static_cast<C>(*szMonthDayNumber);
					break;
				}
				case C('D'):
					[[fallthrough]];
				case C('x'):
				{
					// default locale equivalent to "%m/%d/%y"

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR(L"%m/%d/%y"), pTime);
					else
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR("%m/%d/%y"), pTime);

					break;
				}
				case C('e'):
				{
					unsigned int uMonthDay = pTime->tm_mday;

					const char* szMonthDayNumber = &_TWO_DIGITS_LUT[uMonthDay * 2U];
					*tszDestinationOut++ = static_cast<C>(uMonthDay < 10U ? ' ' : *szMonthDayNumber++);
					*tszDestinationOut++ = static_cast<C>(*szMonthDayNumber);
					break;
				}
				case C('F'):
				{
					// equivalent to "%Y-%m-%d"

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR(L"%Y-%m-%d"), pTime);
					else
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR("%Y-%m-%d"), pTime);

					break;
				}
				case C('g'):
					[[fallthrough]];
				case C('G'):
					[[fallthrough]];
				case C('V'):
				{
					unsigned int uYear = pTime->tm_year + 1900U;
					int iDays = (pTime->tm_yday - (pTime->tm_yday - pTime->tm_wday + 382) % 7 + 3);

					if (iDays < 0)
					{
						uYear -= 1U;

						const int iPreviousYearEndDay = pTime->tm_yday + (365 + ((uYear & 3U) == 0U && (uYear % 100U != 0U || uYear % 400U == 0U)));
						iDays = (iPreviousYearEndDay - (iPreviousYearEndDay - pTime->tm_wday + 382) % 7 + 3);
					}
					else
					{
						const int iCurrentYearEndDay = pTime->tm_yday - (365 + ((uYear & 3U) == 0U && (uYear % 100U != 0U || uYear % 400U == 0U)));
						const int iNextDays = (iCurrentYearEndDay - (iCurrentYearEndDay - pTime->tm_wday + 382) % 7 + 3);

						if (iNextDays >= 0)
						{
							uYear += 1U;
							iDays = iNextDays;
						}
					}

					switch (*tszFormatCurrent)
					{
					case C('G'):
					{
						const char* szCenturyNumber = &_TWO_DIGITS_LUT[(uYear / 100U) * 2U];
						*tszDestinationOut++ = static_cast<C>(*szCenturyNumber++);
						*tszDestinationOut++ = static_cast<C>(*szCenturyNumber);
						[[fallthrough]];
					}
					case C('g'):
					{
						const char* szYearNumber = &_TWO_DIGITS_LUT[(uYear % 100U) * 2U];
						*tszDestinationOut++ = static_cast<C>(*szYearNumber++);
						*tszDestinationOut++ = static_cast<C>(*szYearNumber);
						break;
					}
					default:
					{
						const char* szWeekNumber = &_TWO_DIGITS_LUT[(iDays / 7U + 1U) * 2U];
						*tszDestinationOut++ = static_cast<C>(*szWeekNumber++);
						*tszDestinationOut++ = static_cast<C>(*szWeekNumber);
						break;
					}
					}

					break;
				}
				case C('H'):
				{
					const char* szHourNumber = &_TWO_DIGITS_LUT[static_cast<unsigned int>(pTime->tm_hour) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szHourNumber++);
					*tszDestinationOut++ = static_cast<C>(*szHourNumber);
					break;
				}
				case C('I'):
				{
					unsigned int uHourFormat = pTime->tm_hour % 12U;
					uHourFormat = ((uHourFormat == 0U) ? 12U : uHourFormat);

					const char* szHourNumber = &_TWO_DIGITS_LUT[uHourFormat * 2U];
					*tszDestinationOut++ = static_cast<C>(*szHourNumber++);
					*tszDestinationOut++ = static_cast<C>(*szHourNumber);
					break;
				}
				case C('j'):
				{
					unsigned int uYearDay = pTime->tm_yday + 1U;

					const char* szYearDayNumber = &_TWO_DIGITS_LUT[uYearDay * 2U];
					*tszDestinationOut++ = static_cast<C>((uYearDay / 100U) % 10U + '0');
					*tszDestinationOut++ = static_cast<C>(*szYearDayNumber++);
					*tszDestinationOut++ = static_cast<C>(*szYearDayNumber);
					break;
				}
				case C('m'):
				{
					const char* szMonthNumber = &_TWO_DIGITS_LUT[(pTime->tm_mon + 1U) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szMonthNumber++);
					*tszDestinationOut++ = static_cast<C>(*szMonthNumber);
					break;
				}
				case C('M'):
				{
					const char* szMinuteNumber = &_TWO_DIGITS_LUT[static_cast<unsigned int>(pTime->tm_min) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szMinuteNumber++);
					*tszDestinationOut++ = static_cast<C>(*szMinuteNumber);
					break;
				}
				case C('n'):
				{
					*tszDestinationOut++ = C('\n');
					break;
				}
				case C('p'):
				{
					*tszDestinationOut++ = ((pTime->tm_hour < 12) ? C('A') : C('P'));
					*tszDestinationOut++ = C('M');
					break;
				}
				case C('r'):
				{
					// default locale equivalent to "%I:%M:%S %p"

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR(L"%I:%M:%S %p"), pTime);
					else
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR("%I:%M:%S %p"), pTime);

					break;
				}
				case C('R'):
				{
					const char* szHourNumber = &_TWO_DIGITS_LUT[static_cast<unsigned int>(pTime->tm_hour) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szHourNumber++);
					*tszDestinationOut++ = static_cast<C>(*szHourNumber);

					const char* szMinuteNumber = &_TWO_DIGITS_LUT[static_cast<unsigned int>(pTime->tm_min) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szMinuteNumber++);
					*tszDestinationOut++ = static_cast<C>(*szMinuteNumber);
					break;
				}
				case C('S'):
				{
					const char* szSecondNumber = &_TWO_DIGITS_LUT[static_cast<unsigned int>(pTime->tm_sec) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szSecondNumber++);
					*tszDestinationOut++ = static_cast<C>(*szSecondNumber);
					break;
				}
				case C('t'):
				{
					*tszDestinationOut++ = C('\t');
					break;
				}
				case C('T'):
					[[fallthrough]];
				case C('X'):
				{
					// default locale equivalent to "%H:%M:%S"

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR(L"%H:%M:%S"), pTime);
					else
						tszDestinationOut += TimeToString(tszDestinationOut, nDestinationSize - (tszDestinationOut - tszDestination), Q_XOR("%H:%M:%S"), pTime);

					break;
				}
				case C('u'):
				{
					*tszDestinationOut++ = static_cast<C>(((pTime->tm_wday + 6U) % 7U + 1U) + '0');
					break;
				}
				case C('U'):
				{
					const char* szWeekNumber = &_TWO_DIGITS_LUT[((pTime->tm_yday - pTime->tm_wday + 7U) / 7U) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szWeekNumber++);
					*tszDestinationOut++ = static_cast<C>(*szWeekNumber);
					break;
				}
				case C('w'):
				{
					*tszDestinationOut++ = static_cast<C>(pTime->tm_wday + '0');
					break;
				}
				case C('W'):
				{
					const char* szWeekNumber = &_TWO_DIGITS_LUT[(((pTime->tm_yday - pTime->tm_wday - 6U) % 7U + 7U) / 7U) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szWeekNumber++);
					*tszDestinationOut++ = static_cast<C>(*szWeekNumber);
					break;
				}
				case C('y'):
				{
					const char* szYearNumber = &_TWO_DIGITS_LUT[((pTime->tm_year + 1900U) % 100U) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szYearNumber++);
					*tszDestinationOut++ = static_cast<C>(*szYearNumber);
					break;
				}
				case C('Y'):
				{
					unsigned int uYear = pTime->tm_year + 1900U;

					const char* szCenturyNumber = &_TWO_DIGITS_LUT[(uYear / 100U) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szCenturyNumber++);
					*tszDestinationOut++ = static_cast<C>(*szCenturyNumber);

					const char* szYearNumber = &_TWO_DIGITS_LUT[(uYear % 100U) * 2U];
					*tszDestinationOut++ = static_cast<C>(*szYearNumber++);
					*tszDestinationOut++ = static_cast<C>(*szYearNumber);
					break;
				}
				case C('z'):
					// @todo:
					break;
				case C('Z'):
				{
					// @todo:
					//if constexpr (requires { pTime->tm_zone; })
					//{
					//
					//}
					//else
					//{
					//
					//}
					break;
				}
				case C('%'): // percent escape
				{
					*tszDestinationOut++ = *tszFormatCurrent;
					break;
				}
				default:
					Q_ASSERT(false); // unknown token!
					break;
				}

				// accumulate written characters count per token
				nWroteCount += tszDestinationOut - tszDestinationBefore;

				++tszFormatCurrent;
				continue;
			}

			*tszDestinationOut++ = *tszFormatCurrent++;
			++nWroteCount;
		}

		// check is limit was reached before the entire string could be stored
		if (nWroteCount > nDestinationSize)
			return 0U;

		*tszDestinationOut = C('\0');
		return nWroteCount;
	}

	/// convert the string to an integer, @todo: no wide yet alternative of 'atoi()', '_wtoi()', '_atoi64()', '_wtoi64()', 'atol()', '_wtol()', 'atoll()', '_wtoll()', 'strtol()', 'wcstol()', '_strtoi64()', '_wcstoi64()', 'strtoul()', 'wcstoul()', 'strtoull()', 'wcstoull()'
	/// @todo: remarks about behaviour
	/// @param[in] iBase number of digits used to represent number. 0 to automatically determine number base in range [2 .. 16] or value in range [2 .. 36]
	/// @returns: integer converted from string
	template <typename T> requires (std::is_integral_v<T>)
	constexpr T StringToInteger(const char* szSourceBegin, char** pszSourceEnd = nullptr, int iBase = 0)
	{
		if (iBase < 0 || iBase == 1 || iBase > _NUMBER_MAX_BASE)
		{
			Q_ASSERT(false); // given number base is out of range
			return 0;
		}

		const char* szSourceCurrent = szSourceBegin;

		// skip to first not whitespace
		while (IsSpace(*szSourceCurrent))
			++szSourceCurrent;

		// determine the sign and strip it
		const bool bIsPositive = ((*szSourceCurrent == '+' || *szSourceCurrent == '-') ? (*szSourceCurrent++ == '+') : true);
		constexpr bool bIsUnsigned = ((std::numeric_limits<T>::min)() == 0U);

		// user provided exact number base
		if (iBase > 0)
		{
			// strip 0x or 0X
			if (iBase == 16 && *szSourceCurrent == '0' && (szSourceCurrent[1] | ('a' ^ 'A')) == 'x')
				szSourceCurrent += 2;
		}
		// otherwise try to determine base automatically
		else if (*szSourceCurrent == '0')
		{
			if (((*szSourceCurrent++) | ('a' ^ 'A')) == 'x')
			{
				// a hexadecimal number is defined as "the prefix 0x or 0X followed by a sequence of the decimal digits and the letters a (or A) through f (or F) with values 10 through 15 respectively" (C standard 6.4.4.1)
				iBase = 16;
				++szSourceCurrent;
			}
			else
				// an octal number is defined as "the prefix 0 optionally followed by a sequence of the digits 0 through 7 only" (C standard 6.4.4.1) and so any number that starts with 0, including just 0, is an octal number
				iBase = 8;
		}
		else
			// a decimal number is defined as beginning "with a nonzero digit and consisting of a sequence of decimal digits" (C standard 6.4.4.1)
			iBase = 10;

		constexpr std::uint64_t ullNegativeMax = (bIsUnsigned ? (std::numeric_limits<T>::max)() : (static_cast<std::uint64_t>((std::numeric_limits<T>::max)()) + 1ULL));
		const std::uint64_t ullAbsoluteMax = (bIsPositive ? (std::numeric_limits<T>::max)() : ullNegativeMax);
		const std::uint64_t ullAbsoluteMaxOfBase = ullAbsoluteMax / iBase;

		bool bIsNumber = false;
		std::uint64_t ullResult = 0ULL;

		for (bool bIsDigit = false, bIsAlpha = false; ((bIsDigit = IsDigit(*szSourceCurrent))) || ((bIsAlpha = IsAlpha(*szSourceCurrent))); ) // @note: looks slightly unsafe but have possibility to fast path, double parenthesis to suppress warnings
		{
			int iCurrentDigit = 0;

			if (bIsDigit)
				iCurrentDigit = *szSourceCurrent - '0';
			else if (bIsAlpha)
				iCurrentDigit = (*szSourceCurrent | ('a' ^ 'A')) - 'a' + 0xA;

			if (iCurrentDigit >= iBase)
				break;

			bIsNumber = true;
			++szSourceCurrent;

			// if the number has already hit the maximum value for the current type then the result cannot change, but we still need to advance source to the end of the number
			if (ullResult == ullAbsoluteMax)
			{
				Q_ASSERT(false); // numeric overflow
				continue;
			}

			if (ullResult <= ullAbsoluteMaxOfBase)
				ullResult *= iBase;
			else
			{
				Q_ASSERT(false); // numeric overflow
				ullResult = ullAbsoluteMax;
			}

			if (ullResult <= ullAbsoluteMax - iCurrentDigit)
				ullResult += iCurrentDigit;
			else
			{
				Q_ASSERT(false); // numeric overflow
				ullResult = ullAbsoluteMax;
			}
		}

		if (pszSourceEnd != nullptr)
			*pszSourceEnd = const_cast<char*>(bIsNumber ? szSourceCurrent : szSourceBegin);

		// clamp on overflow
		if (ullResult == ullAbsoluteMax)
			return ((bIsPositive || bIsUnsigned) ? (std::numeric_limits<T>::max)() : (std::numeric_limits<T>::min)());

		return (bIsPositive ? static_cast<T>(ullResult) : -static_cast<T>(ullResult));
	}

	// convert the string to a floating point, alternative of 'atof', 'atod', 'strtof', 'strtod'
	//
	template <typename T> requires (std::is_floating_point_v<T>)
	/*constexpr*/ T StringToFloat(const char* szSourceBegin, char** pszSourceEnd = nullptr)
	{
		// @todo: because i dont have time so yeah, will rebuild it some time
		static auto fnatof = reinterpret_cast<double(Q_CDECL*)(const char*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("8B FF 55 8B EC 6A 00 FF 75 08 E8 ? ? ? ? 59 59 5D C3 A1")));
		return static_cast<T>(fnatof(szSourceBegin));
	}
	#pragma endregion

	/*
	 * @section: string encode/decode
	 * - valid only for default C locale
	 */
	#pragma region crt_string_encode_decode
	/// convert UTF-X multibyte string to a corresponding UTF-32 character, process single character input, alternative of 'mbtowc()'
	/// @credits: github.com/skeeto/branchless-utf8
	/// @remarks: handles decoding error by skipping forward
	/// @returns: the length in bytes of the converted character
	template <typename C>
	std::ptrdiff_t CharMultiByteToUTF32(const C* tszBegin, const C* tszEnd, std::uint32_t* puOutChar)
	{
		// index from the high 5 bits of the first byte in a sequence to the length of the sequence. imperative that 0 == invalid
		constexpr std::uint8_t arrSequenceLength[32] =
		{
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	// [ 0 .. 15] [00000 .. 01111]
			0, 0, 0, 0, 0, 0, 0, 0,							// [16 .. 23] [10000 .. 10111] - 10XXX is only legal as prefixes for continuation bytes
			2, 2, 2, 2,										// [24 .. 27] [11000 .. 11011]
			3, 3,											// [28 .. 29] [11100 .. 11101]
			4, 0											// [30 .. 31] [11110 .. 11111]
		};

		constexpr std::uint8_t arrMask[ ] = { 0x00, 0x7F, 0x1F, 0x0F, 0x07 };
		constexpr std::uint32_t arrStartCodePoint[ ] = { 0x400000, 0, 0x80, 0x800, 0x10000 };
		constexpr std::uint8_t arrCharShift[ ] = { 0, 18, 12, 6, 0 };
		constexpr std::uint8_t arrErrorShift[ ] = { 0, 6, 4, 2, 0 };

		const std::uint8_t nNextLength = arrSequenceLength[*reinterpret_cast<const std::uint8_t*>(tszBegin) >> 3U];
		std::ptrdiff_t nLength = nNextLength + !nNextLength;

		unsigned char uString[4] = { };
		const std::ptrdiff_t nSourceLength = tszEnd - tszBegin;

		// copy at most 'nLength' bytes, stop copying at 0 or past end iterator. branch predictor does a good job here, so it is fast even with excessive branching
		uString[0] = nSourceLength > 0 ? tszBegin[0] : 0;
		uString[1] = nSourceLength > 1 ? tszBegin[1] : 0;
		uString[2] = nSourceLength > 2 ? tszBegin[2] : 0;
		uString[3] = nSourceLength > 3 ? tszBegin[3] : 0;

		// assume a four-byte character and load four bytes. unused bits are shifted out
		*puOutChar = static_cast<std::uint32_t>(uString[0] & arrMask[nNextLength]) << 18U;
		*puOutChar |= static_cast<std::uint32_t>(uString[1] & 0x3F) << 12U;
		*puOutChar |= static_cast<std::uint32_t>(uString[2] & 0x3F) << 6U;
		*puOutChar |= static_cast<std::uint32_t>(uString[3] & 0x3F);
		*puOutChar >>= arrCharShift[nNextLength];

		// accumulate the various error conditions
		unsigned int uError = (*puOutChar < arrStartCodePoint[nNextLength]) << 6U; // non-canonical encoding
		uError |= ((*puOutChar >> 11U) == 0x1B) << 7U; // surrogate half?
		uError |= (*puOutChar > 0x10FFFF) << 8U; // out of range?
		uError |= (uString[1] & 0xC0) >> 2U;
		uError |= (uString[2] & 0xC0) >> 4U;
		uError |= (uString[3]) >> 6U;
		uError ^= 0x2A; // top two bits of each tail byte correct?
		uError >>= arrErrorShift[nNextLength];

		if (uError > 0U)
		{
			/*
			 * no bytes are consumed when string empty
			 * one byte is consumed in case of invalid first byte of begin
			 * all available bytes (at most 'nLength' bytes) are consumed on incomplete/invalid second to last bytes
			 * invalid or incomplete input may consume less bytes than wanted, therefore every byte has to be inspected in 'uString'
			 */
			nLength = Min(nLength, static_cast<std::ptrdiff_t>(!!uString[0] + !!uString[1] + !!uString[2] + !!uString[3]));
			*puOutChar = 0xFFFD;
		}

		return nLength;
	}

	/// convert UTF-32 character to a corresponding multibyte UTF-8 string, process single character input, alternative of 'wctomb()'
	/// @credits: github.com/nothings/stb
	/// @remarks: locale-independent
	/// @returns: the length in bytes of the UTF-32 character. if UTF-32 character is invalid it returns 0
	inline std::ptrdiff_t CharMultiByteFromUTF32(char* szOutBuffer, const std::size_t nOutBufferSize, const std::uint32_t uChar)
	{
		// utf-8
		if (uChar < 0x80)
		{
			szOutBuffer[0] = static_cast<char>(uChar);
			return 1;
		}
		// utf-16
		if (uChar < 0x800 && nOutBufferSize >= 2U)
		{
			szOutBuffer[0] = static_cast<char>(0xC0 + (uChar >> 6U));
			szOutBuffer[1] = static_cast<char>(0x80 + (uChar & 0x3F));
			return 2;
		}
		// utf-16
		if (uChar < 0x10000 && nOutBufferSize >= 3U)
		{
			szOutBuffer[0] = static_cast<char>(0xE0 + (uChar >> 12U));
			szOutBuffer[1] = static_cast<char>(0x80 + ((uChar >> 6U) & 0x3F));
			szOutBuffer[2] = static_cast<char>(0x80 + (uChar & 0x3F));
			return 3;
		}
		// utf-32
		if (uChar <= 0x10FFFF && nOutBufferSize >= 4U)
		{
			szOutBuffer[0] = static_cast<char>(0xF0 + (uChar >> 18U));
			szOutBuffer[1] = static_cast<char>(0x80 + ((uChar >> 12U) & 0x3F));
			szOutBuffer[2] = static_cast<char>(0x80 + ((uChar >> 6U) & 0x3F));
			szOutBuffer[3] = static_cast<char>(0x80 + (uChar & 0x3F));
			return 4;
		}

		// invalid code point
		return 0;
	}

	/// alternative of '_mbslen()'
	/// @returns: UTF-8 characters count of UTF-16/UTF-32 string
	template <typename C>
	int StringLengthMultiByte(const C* tszBegin, const C* tszEnd = nullptr)
	{
		int nOctetCount = 0;

		// go through each character until terminating null up to end if given
		while (*tszBegin != C('\0') && (tszEnd == nullptr || tszBegin < tszEnd))
		{
			if (const std::uint32_t uChar = static_cast<std::uint32_t>(*tszBegin++); uChar < 0x80)
				nOctetCount += 1;
			else if (uChar < 0x800)
				nOctetCount += 2;
			else if (uChar < 0x10000)
				nOctetCount += 3;
			else if (uChar <= 0x7FFFFFFF)
				nOctetCount += 4;
		}

		return nOctetCount;
	}

	/// @returns: valid unicode characters count of UTF-X string
	template <typename C>
	int StringLengthUnicode(const C* tszBegin, const C* tszEnd)
	{
		int nCharCount = 0;

		std::uint32_t uChar = 0U;
		while (*tszBegin != C('\0') && tszBegin < tszEnd)
		{
			tszBegin += CharMultiByteToUTF32(tszBegin, tszEnd, &uChar);

			if (uChar == 0U)
				break;

			++nCharCount;
		}

		return nCharCount;
	}

	/// convert UTF-8 string to UTF-X string, alternative of 'MultiByteToWideChar()', 'mbstowcs()'
	/// @remarks: locale-independent
	/// @todo: param desc
	/// @returns: length of converted UTF-X string
	Q_INLINE std::ptrdiff_t StringMultiByteToUnicode(wchar_t* szOutBuffer, const std::size_t nOutBufferLength, const char* szBegin, const char* szEnd)
	{
		wchar_t* pBufferBegin = szOutBuffer;
		const wchar_t* pBufferEnd = szOutBuffer + nOutBufferLength;

		std::uint32_t uChar = 0U;
		while (pBufferBegin < pBufferEnd - 1 && szBegin < szEnd && *szBegin != '\0')
		{
			szBegin += CharMultiByteToUTF32(szBegin, szEnd, &uChar);

			if (uChar == 0U)
				break;

			*pBufferBegin++ = static_cast<wchar_t>(uChar);
		}

		*pBufferBegin = 0U;
		return pBufferBegin - szOutBuffer;
	}

	/// convert unicode string to UTF-8 string, alternative of 'WideToMultiByteChar()', 'wcstombs()'
	/// @remarks: locale-independent
	/// @todo: param desc
	/// @returns: length of converted multibyte UTF-8 string
	Q_INLINE std::ptrdiff_t StringUnicodeToMultiByte(char* szOutBuffer, const std::size_t nOutBufferLength, const wchar_t* wszBegin, const wchar_t* wszEnd = nullptr)
	{
		char* pBufferBegin = szOutBuffer;
		const char* pBufferEnd = szOutBuffer + nOutBufferLength;

		while (pBufferBegin < pBufferEnd - 1 && (wszEnd == nullptr || wszBegin < wszEnd) && *wszBegin != L'\0')
			pBufferBegin += CharMultiByteFromUTF32(pBufferBegin, pBufferEnd - pBufferBegin - 1, *wszBegin++);

		*pBufferBegin = '\0';
		return pBufferBegin - szOutBuffer;
	}

	/// @remarks: locale-independent
	/// @returns: unicode string (UTF-16 on Windows and UTF-32 on POSIX) converted from UTF-X
	Q_INLINE std::wstring StringMultiByteToUnicode(const std::string_view strMultiByte)
	{
		const std::size_t nLength = StringLengthUnicode(strMultiByte.data(), strMultiByte.data() + strMultiByte.size()) + 1U;
		std::wstring wstrUnicodeOut(nLength, L'\0');

		StringMultiByteToUnicode(wstrUnicodeOut.data(), nLength, strMultiByte.data(), strMultiByte.data() + strMultiByte.size());
		return wstrUnicodeOut;
	}

	/// @remarks: locale-independent
	/// @returns: UTF-8 string converted from UTF-X
	Q_INLINE std::string StringUnicodeToMultiByte(const std::wstring_view wstrUnicode)
	{
		const std::size_t nLength = StringLengthMultiByte(wstrUnicode.data(), wstrUnicode.data() + wstrUnicode.size()) + 1U;
		std::string strMultiByteOut(nLength, '\0');

		StringUnicodeToMultiByte(strMultiByteOut.data(), nLength, wstrUnicode.data(), wstrUnicode.data() + wstrUnicode.size());
		return strMultiByteOut;
	}
	#pragma endregion
}
