#pragma once
// used: std::array
#include <array>

struct ColorRGBExp32
{
	std::byte r, g, b;
	signed char exponent;
};

class Color
{
public:
	Color() = default;

	/* default color constructor ( 0 - 255 ) */
	constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) :
		arrColor({ r, g, b, a }) { }

	constexpr Color(int r, int g, int b, int a = 255) :
		arrColor({ static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b), static_cast<std::uint8_t>(a) }) { }

	/* float color constructor ( 0.0 - 1.0 ) */
	constexpr Color(float r, float g, float b, float a = 1.0f) :
		arrColor({ static_cast<std::uint8_t>(r * 255.f), static_cast<std::uint8_t>(g * 255.f), static_cast<std::uint8_t>(b * 255.f), static_cast<std::uint8_t>(a * 255.f) }) { }

	/* output color to given variables */
	void Get(std::uint8_t& r, std::uint8_t& g, std::uint8_t& b, std::uint8_t& a) const
	{
		r = arrColor.at(0);
		g = arrColor.at(1);
		b = arrColor.at(2);
		a = arrColor.at(3);
	}

	/* convert color to directx rgba */
	ImU32 GetU32() const
	{
		return ImGui::GetColorU32(ImVec4(this->rBase(), this->gBase(), this->bBase(), this->aBase()));
	}

	/* convert color to imgui vector */
	ImVec4 GetVec4(float flAlphaMultiplier = 1.0f) const
	{
		return ImVec4(this->rBase(), this->gBase(), this->bBase(), this->aBase() * flAlphaMultiplier);
	}

	bool operator==(const Color& colSecond) const
	{
		return *const_cast<Color*>(this) == *const_cast<Color*>(&colSecond);
	}

	bool operator!=(const Color& colSecond) const
	{
		return !(operator==(colSecond));
	}

	Color& operator=(const Color& colFrom)
	{
		arrColor.at(0) = colFrom.r();
		arrColor.at(1) = colFrom.g();
		arrColor.at(2) = colFrom.b();
		arrColor.at(3) = colFrom.a();
		return *this;
	}

	inline std::uint8_t r() const { return arrColor.at(0); }
	inline std::uint8_t g() const { return arrColor.at(1); }
	inline std::uint8_t b() const { return arrColor.at(2); }
	inline std::uint8_t a() const { return arrColor.at(3); }

	inline float rBase() const { return arrColor.at(0) / 255.f; }
	inline float gBase() const { return arrColor.at(1) / 255.f; }
	inline float bBase() const { return arrColor.at(2) / 255.f; }
	inline float aBase() const { return arrColor.at(3) / 255.f; }

	/* convert color to float array (in: 0 - 255, out: 0.0 - 1.0) */
	std::array<float, 3U> Base() const
	{
		std::array<float, 3U> arrBaseColor = { };
		arrBaseColor.at(0) = arrColor.at(0) / 255.f;
		arrBaseColor.at(1) = arrColor.at(1) / 255.f;
		arrBaseColor.at(2) = arrColor.at(2) / 255.f;
		return arrBaseColor;
	}

	/* convert color by float array (in: 0.0 - 1.0, out: 0 - 255) */
	static Color FromBase3(float arrBase[3])
	{
		return Color(arrBase[0], arrBase[1], arrBase[2]);
	}

	/* convert color to float array w/ alpha (in: 0 - 255, out: 0.0 - 1.0) */
	std::array<float, 4U> BaseAlpha()
	{
		std::array<float, 4U> arrBaseColor = { };
		arrBaseColor.at(0) = arrColor.at(0) / 255.f;
		arrBaseColor.at(1) = arrColor.at(1) / 255.f;
		arrBaseColor.at(2) = arrColor.at(2) / 255.f;
		arrBaseColor.at(3) = arrColor.at(3) / 255.f;
		return arrBaseColor;
	}

	/* convert color by float array w/ alpha (in: 0.0 - 1.0, out: 0 - 255) */
	static Color FromBase4(float arrBase[4])
	{
		return Color(arrBase[0], arrBase[1], arrBase[2], arrBase[3]);
	}

	float Hue() const
	{
		if (arrColor.at(0) == arrColor.at(1) && arrColor.at(1) == arrColor.at(2))
			return 0.f;

		const float r = this->rBase();
		const float g = this->gBase();
		const float b = this->bBase();

		const float flMax = std::max(r, std::max(g, b)), flMin = std::min(r, std::min(g, b));

		if (flMax == flMin)
			return 0.f;

		const float flDelta = flMax - flMin;
		float flHue = 0.f;

		if (flMax == r)
			flHue = (g - b) / flDelta;
		else if (flMax == g)
			flHue = 2.f + (b - r) / flDelta;
		else if (flMax == b)
			flHue = 4.f + (r - g) / flDelta;

		flHue *= 60.f;

		if (flHue < 0.f)
			flHue += 360.f;

		return flHue / 360.f;
	}

	float Saturation() const
	{
		const float r = this->rBase();
		const float g = this->gBase();
		const float b = this->bBase();

		const float flMax = std::max(r, std::max(g, b)), flMin = std::min(r, std::min(g, b));
		const float flDelta = flMax - flMin;

		if (flMax == 0.f)
			return flDelta;

		return flDelta / flMax;
	}

	float Brightness() const
	{
		const float r = this->rBase();
		const float g = this->gBase();
		const float b = this->bBase();

		return std::max(r, std::max(g, b));
	}

	/* convert HSB/HSV color to RGB color */
	static Color FromHSB(float flHue, float flSaturation, float flBrightness)
	{
		const float h = std::fmodf(flHue, 1.0f) / (60.0f / 360.0f);
		const int i = static_cast<int>(h);
		const float f = h - static_cast<float>(i);
		const float p = flBrightness * (1.0f - flSaturation);
		const float q = flBrightness * (1.0f - flSaturation * f);
		const float t = flBrightness * (1.0f - flSaturation * (1.0f - f));

		float r = 0.0f, g = 0.0f, b = 0.0f;

		switch (i)
		{
		case 0:
			r = flBrightness, g = t, b = p;
			break;
		case 1:
			r = q, g = flBrightness, b = p;
			break;
		case 2:
			r = p, g = flBrightness, b = t;
			break;
		case 3:
			r = p, g = q, b = flBrightness;
			break;
		case 4:
			r = t, g = p, b = flBrightness;
			break;
		case 5:
		default:
			r = flBrightness, g = p, b = q;
			break;
		}

		return Color(r, g, b);
	}

public:
	std::array<std::uint8_t, 4U> arrColor;
};
