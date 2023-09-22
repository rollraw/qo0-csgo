#pragma once
// used: [crt] fmodf
#include <cmath>
// used: [stl] bit_cast
#include <bit>

#include "../../common.h"
// used: swap
#include "../../utilities/crt.h"
// used: [ext] imu32
#include "../../../dependencies/imgui/imgui.h"

enum
{
	COLOR_R = 0,
	COLOR_G = 1,
	COLOR_B = 2,
	COLOR_A = 3
};

struct ColorRGBExp32
{
	std::uint8_t r, g, b;
	std::int8_t iExponent;
};
static_assert(sizeof(ColorRGBExp32) == 0x4);

struct Color_t
{
	Color_t() = default;

	// 8-bit color constructor (in: [0 .. 255])
	constexpr Color_t(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a = 255) :
		r(r), g(g), b(b), a(a) { }

	// 8-bit color constructor (in: [0 .. 255])
	constexpr Color_t(const int r, const int g, const int b, const int a = 255) :
		r(static_cast<std::uint8_t>(r)), g(static_cast<std::uint8_t>(g)), b(static_cast<std::uint8_t>(b)), a(static_cast<std::uint8_t>(a)) { }

	// 8-bit array color constructor (in: [0.0 .. 1.0])
	explicit constexpr Color_t(const std::uint8_t arrColor[4]) :
		r(arrColor[COLOR_R]), g(arrColor[COLOR_G]), b(arrColor[COLOR_B]), a(arrColor[COLOR_A]) { }

	// 32-bit packed color constructor (in: 0x00000000 - 0xFFFFFFFF)
	explicit constexpr Color_t(const ImU32 uPackedColor) :
		r(static_cast<std::uint8_t>((uPackedColor >> IM_COL32_R_SHIFT) & 0xFF)), g(static_cast<std::uint8_t>((uPackedColor >> IM_COL32_G_SHIFT) & 0xFF)), b(static_cast<std::uint8_t>((uPackedColor >> IM_COL32_B_SHIFT) & 0xFF)), a(static_cast<std::uint8_t>((uPackedColor >> IM_COL32_A_SHIFT) & 0xFF)) { }

	// 32-bit color constructor (in: [0.0 .. 1.0])
	constexpr Color_t(const float r, const float g, const float b, const float a = 1.0f) :
		r(static_cast<std::uint8_t>(r * 255.f)), g(static_cast<std::uint8_t>(g * 255.f)), b(static_cast<std::uint8_t>(b * 255.f)), a(static_cast<std::uint8_t>(a * 255.f)) { }

	/// @returns: 32-bit packed integer representation of color
	[[nodiscard]] constexpr ImU32 GetU32(const float flAlphaMultiplier = 1.0f) const
	{
		return IM_COL32(r, g, b, a * flAlphaMultiplier);
	}

	std::uint8_t& operator[](const std::uint8_t nIndex)
	{
		Q_ASSERT(nIndex <= COLOR_A); // given index is out of range
		return reinterpret_cast<std::uint8_t*>(this)[nIndex];
	}

	const std::uint8_t& operator[](const std::uint8_t nIndex) const
	{
		Q_ASSERT(nIndex <= COLOR_A); // given index is out of range
		return reinterpret_cast<const std::uint8_t*>(this)[nIndex];
	}

	bool operator==(const Color_t& colSecond) const
	{
		return (std::bit_cast<std::uint32_t>(*this) == std::bit_cast<std::uint32_t>(colSecond));
	}

	bool operator!=(const Color_t& colSecond) const
	{
		return (std::bit_cast<std::uint32_t>(*this) != std::bit_cast<std::uint32_t>(colSecond));
	}

	/// @returns: copy of color with certain R/G/B/A component changed to given value
	template <std::size_t N>
	[[nodiscard]] Color_t Set(const std::uint8_t nValue) const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "color component index is out of range");

		Color_t colCopy = *this;
		colCopy[N] = nValue;
		return colCopy;
	}

	/// @returns: copy of color with certain R/G/B/A component multiplied by given value
	template <std::size_t N>
	[[nodiscard]] Color_t Multiplier(const float flValue) const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "color component index is out of range");

		Color_t colCopy = *this;
		colCopy[N] = static_cast<std::uint8_t>(static_cast<float>(colCopy[N]) * flValue);
		return colCopy;
	}

	/// @returns: copy of color with certain R/G/B/A component divided by given value
	template <std::size_t N>
	[[nodiscard]] Color_t Divider(const int iValue) const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "color component index is out of range");

		Color_t colCopy = *this;
		colCopy[N] /= iValue;
		return colCopy;
	}

	/// @returns: certain R/G/B/A float value (in: [0 .. 255], out: [0.0 .. 1.0])
	template <std::size_t N>
	[[nodiscard]] float Base() const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "color component index is out of range");
		return reinterpret_cast<const std::uint8_t*>(this)[N] / 255.f;
	}

	/// @param[out] arrBase output array of R/G/B color components converted to float (in: [0 .. 255], out: [0.0 .. 1.0])
	constexpr void Base(float (&arrBase)[3]) const
	{
		arrBase[COLOR_R] = static_cast<float>(r) / 255.f;
		arrBase[COLOR_G] = static_cast<float>(g) / 255.f;
		arrBase[COLOR_B] = static_cast<float>(b) / 255.f;
	}

	/// @returns: color created from float[3] array (in: [0.0 .. 1.0], out: [0 .. 255])
	static Color_t FromBase3(const float arrBase[3])
	{
		return { arrBase[0], arrBase[1], arrBase[2] };
	}

	/// @param[out] arrBase output array of R/G/B/A color components converted to float (in: [0 .. 255], out: [0.0 .. 1.0])
	constexpr void BaseAlpha(float (&arrBase)[4]) const
	{
		arrBase[COLOR_R] = static_cast<float>(r) / 255.f;
		arrBase[COLOR_G] = static_cast<float>(g) / 255.f;
		arrBase[COLOR_B] = static_cast<float>(b) / 255.f;
		arrBase[COLOR_A] = static_cast<float>(a) / 255.f;
	}

	/// @returns : color created from float[3] array (in: [0.0 .. 1.0], out: [0 .. 255])
	static Color_t FromBase4(const float arrBase[4])
	{
		return { arrBase[COLOR_R], arrBase[COLOR_G], arrBase[COLOR_B], arrBase[COLOR_A] };
	}

	/// @param[out] arrHSB output array of HSB/HSV color converted from RGB color
	void ToHSB(float (&arrHSB)[3]) const
	{
		float arrBase[3] = { };
		Base(arrBase);

		float flKernel = 0.0f;
		if (arrBase[COLOR_G] < arrBase[COLOR_B])
		{
			CRT::Swap(arrBase[COLOR_G], arrBase[COLOR_B]);
			flKernel = -1.0f;
		}
		if (arrBase[COLOR_R] < arrBase[COLOR_G])
		{
			CRT::Swap(arrBase[COLOR_R], arrBase[COLOR_G]);
			flKernel = -2.0f / 6.0f - flKernel;
		}

		const float flChroma = arrBase[COLOR_R] - CRT::Min(arrBase[COLOR_G], arrBase[COLOR_B]);
		arrHSB[COLOR_R] = std::fabsf(flKernel + (arrBase[COLOR_G] - arrBase[COLOR_B]) / (6.0f * flChroma + std::numeric_limits<float>::epsilon()));
		arrHSB[COLOR_G] = flChroma / (arrBase[COLOR_R] + std::numeric_limits<float>::epsilon());
		arrHSB[COLOR_G] = arrBase[COLOR_R];
	}

	/// @returns: RGB color converted from HSB/HSV color
	static Color_t FromHSB(const float flHue, const float flSaturation, const float flBrightness, const float flAlpha = 1.0f)
	{
		constexpr float flHueRange = (60.0f / 360.0f);
		const float flHuePrime = std::fmodf(flHue, 1.0f) / flHueRange;
		const int iRoundHuePrime = static_cast<int>(flHuePrime);
		const float flDelta = flHuePrime - static_cast<float>(iRoundHuePrime);

		const float p = flBrightness * (1.0f - flSaturation);
		const float q = flBrightness * (1.0f - flSaturation * flDelta);
		const float t = flBrightness * (1.0f - flSaturation * (1.0f - flDelta));

		float flRed, flGreen, flBlue;
		switch (iRoundHuePrime)
		{
		case 0:
			flRed = flBrightness; flGreen = t; flBlue = p;
			break;
		case 1:
			flRed = q; flGreen = flBrightness; flBlue = p;
			break;
		case 2:
			flRed = p; flGreen = flBrightness; flBlue = t;
			break;
		case 3:
			flRed = p; flGreen = q; flBlue = flBrightness;
			break;
		case 4:
			flRed = t; flGreen = p; flBlue = flBrightness;
			break;
		default:
			flRed = flBrightness; flGreen = p; flBlue = q;
			break;
		}

		return { flRed, flGreen, flBlue, flAlpha };
	}

	std::uint8_t r = 0U, g = 0U, b = 0U, a = 0U;
};
