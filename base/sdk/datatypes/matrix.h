#pragma once
// used: vector
#include "vector.h"

struct ViewMatrix_t
{
	ViewMatrix_t() = default;

	float* operator[](int nIndex)
	{
		return flData[nIndex];
	}

	const float* operator[](int nIndex) const
	{
		return flData[nIndex];
	}

	float flData[4][4] = { };
};

using matrix3x3_t = float[3][3];
struct matrix3x4_t
{
	matrix3x4_t() = default;

	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		flData[0][0] = m00; flData[0][1] = m01; flData[0][2] = m02; flData[0][3] = m03;
		flData[1][0] = m10; flData[1][1] = m11; flData[1][2] = m12; flData[1][3] = m13;
		flData[2][0] = m20; flData[2][1] = m21; flData[2][2] = m22; flData[2][3] = m23;
	}

	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	// create matrix where the X axis = forward, the Y axis = left, the Z axis = up
	void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin)
	{
		flData[0][0] = xAxis.x; flData[0][1] = yAxis.x; flData[0][2] = zAxis.x; flData[0][3] = vecOrigin.x;
		flData[1][0] = xAxis.y; flData[1][1] = yAxis.y; flData[1][2] = zAxis.y; flData[1][3] = vecOrigin.y;
		flData[2][0] = xAxis.z; flData[2][1] = yAxis.z; flData[2][2] = zAxis.z; flData[2][3] = vecOrigin.z;
	}

	void SetOrigin(const Vector& p)
	{
		flData[0][3] = p.x;
		flData[1][3] = p.y;
		flData[2][3] = p.z;
	}

	constexpr void Invalidate()
	{
		for (int i = 0; i < 3; i++)
		{
			for (int k = 0; k < 4; k++)
				flData[i][k] = std::numeric_limits<float>::infinity();
		}
	}

	float*			operator[](int i) { return flData[i]; }
	const float*	operator[](int i) const { return flData[i]; }

	Vector			at(int i) const { return Vector{ flData[0][i], flData[1][i], flData[2][i] }; }
	float*			base() { return &flData[0][0]; }
	const float*	base() const { return &flData[0][0]; }

	float flData[3][4] = { };
};

__declspec(align(16)) class matrix3x4a_t : public matrix3x4_t
{
public:
	matrix3x4a_t& operator=(const matrix3x4_t & matSource)
	{
		memcpy(this->base(), matSource.base(), sizeof(float) * 3U * 4U);
		return *this;
	};
};
