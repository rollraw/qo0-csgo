#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/mathlib/vector.h

// used: isfinite, fmodf, sqrtf
#include <cmath>
// used: numeric_limits
#include <limits>

class Vector2D
{
public:
	constexpr Vector2D(float x = 0.f, float y = 0.f) :
		x(x), y(y) { }

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO);
	}

public:
	float x, y;
};

class Vector
{
public:
	constexpr Vector(float x = 0.f, float y = 0.f, float z = 0.f) :
		x(x), y(y), z(z) { }

	constexpr Vector(const float* arrVector) :
		x(arrVector[0]), y(arrVector[1]), z(arrVector[2]) { }

	constexpr Vector(const Vector2D& vecBase2D) :
		x(vecBase2D.x), y(vecBase2D.y), z(0.0f) { }

	[[nodiscard]] bool IsValid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z);
	}

	constexpr void Invalidate()
	{
		this->x = this->y = this->z = std::numeric_limits<float>::infinity();
	}

	[[nodiscard]] float* data()
	{
		return reinterpret_cast<float*>(this);
	}

	[[nodiscard]] const float* data() const
	{
		return reinterpret_cast<float*>(const_cast<Vector*>(this));
	}

	float& operator[](const std::size_t i)
	{
		return this->data()[i];
	}

	const float& operator[](const std::size_t i) const
	{
		return this->data()[i];
	}

	bool operator==(const Vector& vecBase) const
	{
		return this->IsEqual(vecBase);
	}

	bool operator!=(const Vector& vecBase) const
	{
		return !this->IsEqual(vecBase);
	}

	constexpr Vector& operator=(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z;
		return *this;
	}

	constexpr Vector& operator=(const Vector2D& vecBase2D)
	{
		this->x = vecBase2D.x; this->y = vecBase2D.y; this->z = 0.0f;
		return *this;
	}

	constexpr Vector& operator+=(const Vector& vecBase)
	{
		this->x += vecBase.x; this->y += vecBase.y; this->z += vecBase.z;
		return *this;
	}

	constexpr Vector& operator-=(const Vector& vecBase)
	{
		this->x -= vecBase.x; this->y -= vecBase.y; this->z -= vecBase.z;
		return *this;
	}

	constexpr Vector& operator*=(const Vector& vecBase)
	{
		this->x *= vecBase.x; this->y *= vecBase.y; this->z *= vecBase.z;
		return *this;
	}

	constexpr Vector& operator/=(const Vector& vecBase)
	{
		this->x /= vecBase.x; this->y /= vecBase.y; this->z /= vecBase.z;
		return *this;
	}

	constexpr Vector& operator+=(const float flAdd)
	{
		this->x += flAdd; this->y += flAdd; this->z += flAdd;
		return *this;
	}

	constexpr Vector& operator-=(const float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract; this->z -= flSubtract;
		return *this;
	}

	constexpr Vector& operator*=(const float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply; this->z *= flMultiply;
		return *this;
	}

	constexpr Vector& operator/=(const float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide; this->z /= flDivide;
		return *this;
	}

	Vector operator+(const Vector& vecAdd) const
	{
		return Vector(this->x + vecAdd.x, this->y + vecAdd.y, this->z + vecAdd.z);
	}

	Vector operator-(const Vector& vecSubtract) const
	{
		return Vector(this->x - vecSubtract.x, this->y - vecSubtract.y, this->z - vecSubtract.z);
	}

	Vector operator*(const Vector& vecMultiply) const
	{
		return Vector(this->x * vecMultiply.x, this->y * vecMultiply.y, this->z * vecMultiply.z);
	}

	Vector operator/(const Vector& vecDivide) const
	{
		return Vector(this->x / vecDivide.x, this->y / vecDivide.y, this->z / vecDivide.z);
	}

	Vector operator+(const float flAdd) const
	{
		return Vector(this->x + flAdd, this->y + flAdd, this->z + flAdd);
	}

	Vector operator-(const float flSubtract) const
	{
		return Vector(this->x - flSubtract, this->y - flSubtract, this->z - flSubtract);
	}

	Vector operator*(const float flMultiply) const
	{
		return Vector(this->x * flMultiply, this->y * flMultiply, this->z * flMultiply);
	}

	Vector operator/(const float flDivide) const
	{
		return Vector(this->x / flDivide, this->y / flDivide, this->z / flDivide);
	}

	[[nodiscard]] bool IsEqual(const Vector& vecEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const
	{
		return (std::fabsf(this->x - vecEqual.x) < flErrorMargin &&
			std::fabsf(this->y - vecEqual.y) < flErrorMargin &&
			std::fabsf(this->z - vecEqual.z) < flErrorMargin);
	}

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO &&
			std::fpclassify(this->z) == FP_ZERO);
	}

	[[nodiscard]] Vector2D ToVector2D() const
	{
		return Vector2D(this->x, this->y);
	}

	[[nodiscard]] float Length() const
	{
		return std::sqrtf(this->LengthSqr());
	}

	[[nodiscard]] constexpr float LengthSqr() const
	{
		return DotProduct(*this);
	}

	[[nodiscard]] float Length2D() const
	{
		return std::sqrtf(this->Length2DSqr());
	}

	[[nodiscard]] constexpr float Length2DSqr() const
	{
		return (this->x * this->x + this->y * this->y);
	}

	[[nodiscard]] float DistTo(const Vector& vecEnd) const
	{
		return (*this - vecEnd).Length();
	}

	[[nodiscard]] constexpr float DistToSqr(const Vector& vecEnd) const
	{
		return (*this - vecEnd).LengthSqr();
	}

	[[nodiscard]] Vector Normalized() const
	{
		Vector vecOut = *this;
		vecOut.NormalizeInPlace();
		return vecOut;
	}

	float NormalizeInPlace()
	{
		const float flLength = this->Length();
		const float flRadius = 1.0f / (flLength + std::numeric_limits<float>::epsilon());

		this->x *= flRadius;
		this->y *= flRadius;
		this->z *= flRadius;

		return flLength;
	}

	[[nodiscard]] constexpr float DotProduct(const Vector& vecDot) const
	{
		return (this->x * vecDot.x + this->y * vecDot.y + this->z * vecDot.z);
	}

	[[nodiscard]] constexpr Vector CrossProduct(const Vector& vecCross) const
	{
		return Vector(this->y * vecCross.z - this->z * vecCross.y, this->z * vecCross.x - this->x * vecCross.z, this->x * vecCross.y - this->y * vecCross.x);
	}

public:
	float x, y, z;
};

class Vector4D
{
public:
	constexpr Vector4D(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) :
		x(x), y(y), z(z), w(w) { }

public:
	float x, y, z, w;
};

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	VectorAligned() = default;

	explicit VectorAligned(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
	}

	constexpr VectorAligned& operator=(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
		return *this;
	}

public:
	float w;
};
