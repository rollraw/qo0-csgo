#pragma once
// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "../common.h"

// using: stringcopy, stringcat, timetostring
#include "crt.h"

// @todo: poorly designed in case we don't need logging at all, xor continues wasteful compilation without any references | add smth like dummystream_t and unreference macro params?

#pragma region log_definitions
#ifdef _DEBUG
#if defined(Q_COMPILER_CLANG)
#define L_PRINT(LEVEL) L::stream(LEVEL, "[" __FILE_NAME__ ":" Q_STRINGIFY(__LINE__) "] ")
#else
#define L_PRINT(LEVEL) L::stream(LEVEL, L::DETAIL::MakeFileBlock<CRT::StringLength(L::DETAIL::GetFileName(__FILE__)), CRT::StringLength(Q_STRINGIFY(__LINE__))>(L::DETAIL::GetFileName(__FILE__), Q_STRINGIFY(__LINE__)).Get())
#endif
#else
#define L_PRINT(LEVEL) L::stream(LEVEL)
#endif
#pragma endregion

#pragma region log_enumerations
enum ELogLevel : std::uint8_t
{
	LOG_NONE = 0,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

using LogModeFlags_t = std::uint16_t;
enum ELogModeFlags : LogModeFlags_t
{
	LOG_MODE_NONE = 0U,

	// boolean formatting
	LOG_MODE_BOOL_ALPHA = (1U << 0U), // switches between textual and numeric representation of booleans

	// integer formatting
	LOG_MODE_INT_SHOWBASE = (1U << 1U), // switches display of number base prefixes used by C++ literal constants
	LOG_MODE_INT_FORMAT_HEX = (1U << 2U), // switches integer numbers hexadecimal format
	LOG_MODE_INT_FORMAT_DEC = (1U << 3U), // switches integer numbers decimal format
	LOG_MODE_INT_FORMAT_OCT = (1U << 4U), // switches integer numbers octal format
	LOG_MODE_INT_FORMAT_BIN = (1U << 5U), // switches integer numbers binary format
	LOG_MODE_INT_FORMAT_MASK = (LOG_MODE_INT_FORMAT_HEX | LOG_MODE_INT_FORMAT_DEC | LOG_MODE_INT_FORMAT_OCT | LOG_MODE_INT_FORMAT_BIN),

	// floating-point formatting
	LOG_MODE_FLOAT_SHOWPOINT = (1U << 6U), // switches decimal point for those numbers whose decimal part is zero
	LOG_MODE_FLOAT_FORMAT_HEX = (1U << 7U), // switches floating-point numbers hexadecimal format
	LOG_MODE_FLOAT_FORMAT_FIXED = (1U << 8U), // switches floating-point numbers formatting in fixed-point notation
	LOG_MODE_FLOAT_FORMAT_SCIENTIFIC = (1U << 9U), // switches floating-point numbers formatting in scientific notation
	LOG_MODE_FLOAT_FORMAT_MASK = (LOG_MODE_FLOAT_FORMAT_HEX | LOG_MODE_FLOAT_FORMAT_FIXED | LOG_MODE_FLOAT_FORMAT_SCIENTIFIC),

	// numerical formatting
	LOG_MODE_NUM_SHOWPOSITIVE = (1U << 10U), // switches display of plus sign '+' in non-negative numbers
	LOG_MODE_NUM_UPPERCASE = (1U << 11U), // switches uppercase characters in numbers

	/* [internal] */
	LOG_MODE_REMOVE = (1U << 15U)
};

using LogColorFlags_t = std::uint16_t;
enum ELogColorFlags : LogColorFlags_t
{
	LOG_COLOR_FORE_BLUE = FOREGROUND_BLUE,
	LOG_COLOR_FORE_GREEN = FOREGROUND_GREEN,
	LOG_COLOR_FORE_RED = FOREGROUND_RED,
	LOG_COLOR_FORE_INTENSITY = FOREGROUND_INTENSITY,
	LOG_COLOR_FORE_GRAY = FOREGROUND_INTENSITY,
	LOG_COLOR_FORE_CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN,
	LOG_COLOR_FORE_MAGENTA = FOREGROUND_BLUE | FOREGROUND_RED,
	LOG_COLOR_FORE_YELLOW = FOREGROUND_GREEN | FOREGROUND_RED,
	LOG_COLOR_FORE_BLACK = 0U,
	LOG_COLOR_FORE_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

	LOG_COLOR_BACK_BLUE = BACKGROUND_BLUE,
	LOG_COLOR_BACK_GREEN = BACKGROUND_GREEN,
	LOG_COLOR_BACK_RED = BACKGROUND_RED,
	LOG_COLOR_BACK_INTENSITY = BACKGROUND_INTENSITY,
	LOG_COLOR_BACK_GRAY = BACKGROUND_INTENSITY,
	LOG_COLOR_BACK_CYAN = BACKGROUND_BLUE | BACKGROUND_GREEN,
	LOG_COLOR_BACK_MAGENTA = BACKGROUND_BLUE | BACKGROUND_RED,
	LOG_COLOR_BACK_YELLOW = BACKGROUND_GREEN | BACKGROUND_RED,
	LOG_COLOR_BACK_BLACK = 0U,
	LOG_COLOR_BACK_WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,

	/* [internal] */
	LOG_COLOR_DEFAULT = LOG_COLOR_FORE_WHITE | LOG_COLOR_BACK_BLACK
};
#pragma endregion

/*
 * LOGGING
 * - simple logging system with file and console output
 *   used for debugging and fetching values/errors at run-time
 * @todo: currently not thread safe and can mess messages when used from different threads
 */
namespace L
{
	namespace DETAIL
	{
		// @todo: constructs string per-byte in the stack, how do we can optimize this?
		template <std::size_t N>
		struct FileBlockStorage_t
		{
			template <std::size_t... I1, std::size_t... I2>
			consteval explicit FileBlockStorage_t(const char* szFileName, const char* szLineNumber, std::index_sequence<I1...>, std::index_sequence<I2...>) :
				szStorage{ '[', szFileName[I1]..., ':', szLineNumber[I2]..., ']', ' ', '\0' } { }

			[[nodiscard]] constexpr const char* Get() const
			{
				return szStorage;
			}

			const char szStorage[N + 5U];
		};

		// fail-free version of 'StringCharR'
		consteval const char* GetFileName(const char* szFilePath)
		{
			const char* szLastPath = szFilePath;

			do
			{
				if (*szFilePath == '\\')
					szLastPath = szFilePath + 1U;
			} while (*szFilePath++ != '\0');

			return szLastPath;
		}

		// helper to generate file info block for logging message at compile-time
		template <std::size_t N1, std::size_t N2>
		consteval auto MakeFileBlock(const char* szFileName, const char* szFileNumber) noexcept
		{
			return FileBlockStorage_t<N1 + N2>(szFileName, szFileNumber, std::make_index_sequence<N1>{ }, std::make_index_sequence<N2>{ });
		}
	}

	/* @section: main */
	// attach console to current window with write permission and given title
	bool AttachConsole(const wchar_t* wszWindowTitle);
	// close write streams and detach console from current window
	void DetachConsole();
	// open logging output file
	bool OpenFile(const wchar_t* wszFileName);
	// close logging output file
	void CloseFile();
	// write message to the file or/and console
	void WriteMessage(const char* szMessage, const std::size_t nMessageLength);

	// alternative of C++ 'std::cout' and other STL-like streams logging scheme
	// @todo: is it faster to constantly call 'WriteMessage' instead of concatenating all of the output and print once? i dont think so, due to additional allocations, thread-safe requirements, conditions when we still should call print due to color/etc changes | but generally this should lead to better inlining and less complicated compiled code
	struct Stream_t
	{
		// special unique return type markers to determine and handle change of those flags, just a snap for compile-time, inlined as underlying types at run-time
		struct ColorMarker_t
		{
			LogColorFlags_t nColorFlags;
		};

		struct PrecisionMarker_t
		{
			int iPrecision;
		};

		struct ModeMarker_t
		{
			LogModeFlags_t nModeFlags;
		};

		// begin of each log message, puts time, file & line, level blocks
		Stream_t& operator()(const ELogLevel nLevel, const char* szFileBlock = nullptr);

		// manipulators
		Stream_t& operator<<(ColorMarker_t);
		Stream_t& operator<<(PrecisionMarker_t);
		Stream_t& operator<<(ModeMarker_t);
		// message
		Stream_t& operator<<(const char* szMessage);
		Stream_t& operator<<(const wchar_t* wszMessage);
		// conversion
		Stream_t& operator<<(const bool bValue);

		template <typename T> requires std::is_integral_v<T>
		Stream_t& operator<<(const T value)
		{
		#if defined(Q_LOG_CONSOLE) || defined(Q_LOG_FILE)
			int iBase = 10;
			const char* szPrefix = nullptr;

			if (this->nModeFlags & LOG_MODE_INT_FORMAT_HEX)
			{
				iBase = 16;
				szPrefix = "0x";
			}
			else if (this->nModeFlags & LOG_MODE_INT_FORMAT_OCT)
				iBase = 8;
			else if (this->nModeFlags & LOG_MODE_INT_FORMAT_BIN)
			{
				iBase = 2;
				szPrefix = "0b";
			}

			// @todo: LOG_MODE_NUM_UPPERCASE not handled
			char szIntegerBuffer[CRT::IntegerToString_t<std::int64_t, 2U>::MaxCount() + 2U];
			char* szInteger = CRT::IntegerToString(value, szIntegerBuffer + 2U, sizeof(szIntegerBuffer) - 2U, iBase);

			// @todo: after int2str rework could be simplified | or completely replaced with strformat
			if (szPrefix != nullptr && (this->nModeFlags & LOG_MODE_INT_SHOWBASE))
			{
				*--szInteger = szPrefix[1];
				*--szInteger = szPrefix[0];
			}

			if constexpr (std::is_signed_v<T>)
			{
				if (value >= 0 && (this->nModeFlags & LOG_MODE_NUM_SHOWPOSITIVE))
					*--szInteger = '+';
			}

			const std::size_t nIntegerLength = szIntegerBuffer + sizeof(szIntegerBuffer) - szInteger - 1;
			WriteMessage(szInteger, nIntegerLength);
		#endif
			return *this;
		}

		template <typename T> requires std::is_floating_point_v<T>
		Stream_t& operator<<(const T value)
		{
		#if defined(Q_LOG_CONSOLE) || defined(Q_LOG_FILE)
			static_assert((nModeFlags & (LOG_MODE_FLOAT_FORMAT_FIXED | LOG_MODE_FLOAT_FORMAT_SCIENTIFIC)) && std::is_same_v<T, float>); // expected 'double' or 'long double'
			int iDesiredPrecision = ((nModeFlags & (LOG_MODE_FLOAT_FORMAT_FIXED | LOG_MODE_FLOAT_FORMAT_SCIENTIFIC)) ? -1 : (iPrecision > 0 ? iPrecision : FLT_DIG));

			char szFormatBuffer[8];
			char* szFormat = szFormatBuffer;
			*szFormat++ = '%';

			if (nModeFlags & LOG_MODE_NUM_SHOWPOSITIVE)
				*szFormat++ = '+';

			if (nModeFlags & LOG_MODE_FLOAT_SHOWPOINT)
				*szFormat++ = '#';

			*szFormat++ = '.';
			*szFormat++ = '*';
			if constexpr (std::is_same_v<T, long double>)
				*szFormat++ = 'L';

			if (nModeFlags & LOG_MODE_FLOAT_FORMAT_FIXED)
				*szFormat++ = 'f';
			else
			{
				const bool bIsUpperCase = (nModeFlags & LOG_MODE_NUM_UPPERCASE);

				if (nModeFlags & LOG_MODE_FLOAT_FORMAT_HEX)
					*szFormat++ = bIsUpperCase ? 'A' : 'a';
				else if (nModeFlags & LOG_MODE_FLOAT_FORMAT_SCIENTIFIC)
					*szFormat++ = bIsUpperCase ? 'E' : 'e';
				else
					*szFormat++ = bIsUpperCase ? 'G' : 'g';
			}
			*szFormat = '\0';

			char szFloatBuffer[96];
			const int nFloatLength = CRT::StringPrintN(szFloatBuffer, sizeof(szFloatBuffer), szFormatBuffer, iDesiredPrecision, value);

			WriteMessage(szFloatBuffer, nFloatLength);
		#endif
			return *this;
		}

		bool bFirstPrint = true;
		int iPrecision = 0;
		LogModeFlags_t nModeFlags = LOG_MODE_NONE;
	};

	/* @section: stream control */
	// set console color flags for current stream, will reset on next message
	Stream_t::ColorMarker_t SetColor(const LogColorFlags_t nColorFlags);
	// set the decimal precision to be used to format floating-point values for current stream, will reset on next message
	Stream_t::PrecisionMarker_t SetPrecision(const int iPrecision);
	// add logging mode flags for current stream, will reset on next message
	Stream_t::ModeMarker_t AddFlags(const LogModeFlags_t nModeFlags);
	// remove logging mode flags for current stream
	Stream_t::ModeMarker_t RemoveFlags(const LogModeFlags_t nModeFlags);

	/* @section: values */
	// primary logging stream
	inline Stream_t stream;
}
