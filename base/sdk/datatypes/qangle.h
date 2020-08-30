#pragma once
// used: clamp
#include <algorithm>
// used: isfinite, fmodf, remainderf
#include <cmath>

class QAngle
{
public:
	constexpr QAngle(float x = 0.f, float y = 0.f, float z = 0.f) :
		x(x), y(y), z(z) { }

	constexpr QAngle(const float* arrAngles) :
		x(arrAngles[0]), y(arrAngles[1]), z(arrAngles[2]) { }

	bool operator==(const QAngle& angBase) const
	{
		return this->IsEqual(angBase);
	}

	bool operator!=(const QAngle& angBase) const
	{
		return !this->IsEqual(angBase);
	}

	constexpr QAngle& operator=(const QAngle& angBase) = default;

	constexpr QAngle& operator+=(const QAngle& angBase)
	{
		this->x += angBase.x; this->y += angBase.y; this->z += angBase.z;
		return *this;
	}

	constexpr QAngle& operator-=(const QAngle& angBase)
	{
		this->x -= angBase.x; this->y -= angBase.y; this->z -= angBase.z;
		return *this;
	}

	constexpr QAngle& operator*=(const QAngle& angBase)
	{
		this->x *= angBase.x; this->y *= angBase.y; this->z *= angBase.z;
		return *this;
	}

	constexpr QAngle& operator/=(const QAngle& angBase)
	{
		this->x /= angBase.x; this->y /= angBase.y; this->z /= angBase.z;
		return *this;
	}

	constexpr QAngle& operator+=(float flAdd)
	{
		this->x += flAdd; this->y += flAdd; this->z += flAdd;
		return *this;
	}

	constexpr QAngle& operator-=(float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract; this->z -= flSubtract;
		return *this;
	}

	constexpr QAngle& operator*=(float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply; this->z *= flMultiply;
		return *this;
	}

	constexpr QAngle& operator/=(float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide; this->z /= flDivide;
		return *this;
	}

	QAngle operator+(const QAngle& angAdd) const
	{
		return QAngle(this->x + angAdd.x, this->y + angAdd.y, this->z + angAdd.z);
	}

	QAngle operator-(const QAngle& angSubtract) const
	{
		return QAngle(this->x - angSubtract.x, this->y - angSubtract.y, this->z - angSubtract.z);
	}

	QAngle operator*(const QAngle& angMultiply) const
	{
		return QAngle(this->x * angMultiply.x, this->y * angMultiply.y, this->z * angMultiply.z);
	}

	QAngle operator/(const QAngle& angDivide) const
	{
		return QAngle(this->x / angDivide.x, this->y / angDivide.y, this->z / angDivide.z);
	}

	QAngle operator+(float flAdd) const
	{
		return QAngle(this->x + flAdd, this->y + flAdd, this->z + flAdd);
	}

	QAngle operator-(float flSubtract) const
	{
		return QAngle(this->x - flSubtract, this->y - flSubtract, this->z - flSubtract);
	}

	QAngle operator*(float flMultiply) const
	{
		return QAngle(this->x * flMultiply, this->y * flMultiply, this->z * flMultiply);
	}

	QAngle operator/(float flDivide) const
	{
		return QAngle(this->x / flDivide, this->y / flDivide, this->z / flDivide);
	}

	bool IsEqual(const QAngle& angEqual) const
	{
		return (std::fabsf(this->x - angEqual.x) < std::numeric_limits<float>::epsilon() &&
				std::fabsf(this->y - angEqual.y) < std::numeric_limits<float>::epsilon() &&
				std::fabsf(this->z - angEqual.z) < std::numeric_limits<float>::epsilon());
	}

	bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
				std::fpclassify(this->y) == FP_ZERO &&
				std::fpclassify(this->z) == FP_ZERO);
	}

	float Length() const
	{
		return std::sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	constexpr float LengthSqr() const
	{
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}

	void Clamp()
	{
		this->x = std::clamp(this->x, -89.f, 89.f);
		this->y = std::clamp(this->y, -180.f, 180.f);
		this->z = std::clamp(this->z, -50.f, 50.f);
	}

	QAngle Normalize()
	{
		this->x = std::isfinite(this->x) ? std::remainderf(this->x, 360.f) : 0.f;
		this->y = std::isfinite(this->y) ? std::remainderf(this->y, 360.f) : 0.f;
		this->z = std::clamp(this->z, -50.f, 50.f);
		return *this;
	}

	QAngle Mod(float flValue)
	{
		this->x = std::fmodf(this->x, flValue);
		this->y = std::fmodf(this->y, flValue);
		this->z = std::fmodf(this->z, flValue);
		return *this;
	}

public:
	float x, y, z;
};
