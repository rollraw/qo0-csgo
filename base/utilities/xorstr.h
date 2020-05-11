#pragma once

namespace XorCompileTime
{
	constexpr auto time = __TIME__;
	constexpr auto seed = static_cast<int>(time[7]) +
						  static_cast<int>(time[6]) * 10 +
						  static_cast<int>(time[4]) * 60 +
						  static_cast<int>(time[3]) * 600 +
						  static_cast<int>(time[1]) * 3600 +
						  static_cast<int>(time[0]) * 36000;

	template <int N>
	struct RandomGenerator
	{
	private:
		static constexpr unsigned a = 16807; // 7^5
		static constexpr unsigned m = 2147483647; // 2^31 - 1

		static constexpr unsigned s = RandomGenerator<N - 1>::value;
		static constexpr unsigned lo = a * (s & 0xFFFF); // Multiply lower 16 bits by 16807
		static constexpr unsigned hi = a * (s >> 16); // Multiply higher 16 bits by 16807
		static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16); // Combine lower 15 bits of hi with lo's upper bits
		static constexpr unsigned hi2 = hi >> 15; // Discard lower 15 bits of hi
		static constexpr unsigned lo3 = lo2 + hi;

	public:
		static constexpr unsigned max = m;
		static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
	};

	template <>
	struct RandomGenerator<0>
	{
		static constexpr unsigned value = seed;
	};

	template <int N, int M>
	struct RandomInt
	{
		static constexpr auto value = RandomGenerator<N + 1>::value % M;
	};

	template <int N>
	struct RandomChar
	{
		static const char value = static_cast<char>(1 + RandomInt<N, 0x7F - 1>::value);
	};

	template <const std::size_t N, const int K>
	class CXorStr
	{
	private:
		const char chKey;
		std::array<char, N + 1> arrBuffer;

		constexpr char encrypt(char c) const
		{
			return c ^ chKey;
		}

		char decrypt(char c) const
		{
			return c ^ chKey;
		}

	public:
		template <std::size_t... I>
		constexpr __forceinline CXorStr(const char* str, std::index_sequence<I...>) : chKey(RandomChar<K>::value), arrBuffer{ encrypt(str[I])... } { }

		__forceinline decltype(auto) data(void)
		{
			for (std::size_t i = 0; i < N; ++i)
				arrBuffer[i] = decrypt(arrBuffer[i]);

			arrBuffer[N] = '\0';
			return arrBuffer.data();
		}

		char* get(void)
		{
			return arrBuffer.data();
		}
	};

	/* @note:
	 * xorstr not work directly with functions like 'printf'
	 * to work with them you need a wrapper function that takes a 'const char*'
	 * as argument and passes it to 'printf' and alike.
	 * 
	 * msvc is not working correctly with variadic templates
	 * use the functions below or use std::cout (and similar)!
	 */

	static auto w_printf = [](const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	};

	static auto w_printf_s = [](const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vprintf_s(fmt, args);
		va_end(args);
	};

	static auto w_sprintf = [](char* buf, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vsprintf(buf, fmt, args);
		va_end(args);
	};

	static auto w_sprintf_s = [](char* buf, std::size_t buf_size, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vsprintf_s(buf, buf_size, fmt, args);
		va_end(args);
	};

	static bool w_strcmp(const char* str1, const char* str2)
	{
		return strcmp(str1, str2);
	};
}

#if NDEBUG
#define XorStr( s ) XorCompileTime::CXorStr<sizeof( s ) - 1, __COUNTER__>( s, std::make_index_sequence<sizeof( s ) - 1>() ).data()
#else
#define XorStr( s ) s
#endif
