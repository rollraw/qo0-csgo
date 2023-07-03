#pragma once
#include "../common.h"

// convert angle in degrees to radians
#define M_DEG2RAD(DEGREES) ((DEGREES) * (M::_PI / 180.f))
// convert angle in radians to degrees
#define M_RAD2DEG(RADIANS) ((RADIANS) * (180.f / M::_PI))
/// linearly interpolate the value between @a'X0' and @a'X1' by @a'FACTOR'
#define M_LERP(X0, X1, FACTOR) ((X0) + ((X1) - (X0)) * (FACTOR))

/*
 * MATHEMATICS
 * - basic trigonometry, algebraic mathematical functions and constants
 */
namespace M
{
	/* @section: constants */
	// pi value
	inline constexpr float _PI = 3.141592654f;
	// double of pi
	inline constexpr float _2PI = 6.283185307f;
	// half of pi
	inline constexpr float _HPI = 1.570796327f;
	// quarter of pi
	inline constexpr float _QPI = 0.785398163f;
	// reciprocal of double of pi
	inline constexpr float _1DIV2PI = 0.159154943f;
	// golden ratio
	inline constexpr float _PHI = 1.618033988f;

	/* @section: exponential */
	/// @returns: true if given number is power of two, false otherwise
	template <typename T> requires (std::is_integral_v<T>)
	[[nodiscard]] Q_INLINE constexpr bool IsPowerOfTwo(const T value) noexcept
	{
		return value != 0 && (value & (value - 1)) == 0;
	}
}
