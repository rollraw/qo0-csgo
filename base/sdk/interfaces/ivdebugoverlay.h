#pragma once

// used: callvfunc
#include "../../utilities/memory.h"

struct Color_t;
struct QAngle_t;
struct Vector_t;
struct Matrix3x4_t;

class IVDebugOverlay : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void AddBoxOverlay(const Vector_t& vecOrigin, const Vector_t& vecAbsMin, const Vector_t& vecAbsMax, const QAngle_t& angOrientation, int r, int g, int b, int a, float flDuration)
	{
		CallVFunc<void, 1U>(this, &vecOrigin, &vecAbsMin, &vecAbsMax, &angOrientation, r, g, b, a, flDuration);
	}

	void AddSphereOverlay(const Vector_t& vecOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration)
	{
		CallVFunc<void, 2U>(this, &vecOrigin, flRadius, nTheta, nPhi, r, g, b, a, flDuration);
	}

	void AddTriangleOverlay(const Vector_t& vecFirst, const Vector_t& vecSecond, const Vector_t& vecThird, int r, int g, int b, int a, bool bNoDepthTest, float flDuration)
	{
		CallVFunc<void, 3U>(this, &vecFirst, &vecSecond, &vecThird, r, g, b, a, bNoDepthTest, flDuration);
	}

	void AddLineOverlay(const Vector_t& vecFirst, const Vector_t& vecSecond, int r, int g, int b, int a, float flThickness, float flDuration)
	{
		CallVFunc<void, 4U>(this, &vecFirst, &vecSecond, r, g, b, a, flThickness, flDuration);
	}

	void AddLineOverlay(const Vector_t& vecFirst, const Vector_t& vecSecond, int r, int g, int b, bool bNoDepthTest, float flDuration)
	{
		CallVFunc<void, 5U>(this, &vecFirst, &vecSecond, r, g, b, bNoDepthTest, flDuration);
	}

	/// @returns: true if converted 2D point is behind screen, false otherwise
	int ScreenPosition(const Vector_t& vecPoint, Vector_t& vecScreen)
	{
		return CallVFunc<int, 13U>(this, &vecPoint, &vecScreen);
	}

	void AddLineOverlayAlpha(const Vector_t& vecFirst, const Vector_t& vecSecond, int r, int g, int b, int a, bool bNoDepthTest, float flDuration)
	{
		CallVFunc<void, 20U>(this, &vecFirst, &vecSecond, r, g, b, a, bNoDepthTest, flDuration);
	}

	void AddBoxOverlayTwoColor(const Vector_t& vecOrigin, const Vector_t& vecAbsMin, const Vector_t& vecAbsMax, const QAngle_t& angOrientation, const Color_t& colBoxFace, const Color_t& colBoxEdge, float flDuration)
	{
		CallVFunc<void, 21U>(this, &vecOrigin, &vecAbsMin, &vecAbsMax, &angOrientation, &colBoxFace, &colBoxEdge, flDuration);
	}

	void AddCapsuleOverlay(const Vector_t& vecAbsMin, const Vector_t& vecAbsMax, const float& flRadius, int r, int g, int b, int a, float flDuration)
	{
		CallVFunc<void, 24U>(this, &vecAbsMin, &vecAbsMax, &flRadius, r, g, b, a, flDuration);
	}
};
