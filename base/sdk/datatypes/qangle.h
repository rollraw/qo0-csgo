#pragma once
// used: [crt] isfinite, fmodf, remainderf
#include <cmath>

#include "vector.h"

// used: clamp
#include "../../utilities/crt.h"

// forward declarations
struct Matrix3x4_t;

struct QAngle_t
{
	constexpr QAngle_t(float x = 0.f, float y = 0.f, float z = 0.f) :
		x(x), y(y), z(z) { }

	constexpr QAngle_t(const float* arrAngles) :
		x(arrAngles[0]), y(arrAngles[1]), z(arrAngles[2]) { }

	#pragma region qangle_array_operators
	[[nodiscard]] float& operator[](const int nIndex)
	{
		return reinterpret_cast<float*>(this)[nIndex];
	}

	[[nodiscard]] const float& operator[](const int nIndex) const
	{
		return reinterpret_cast<const float*>(this)[nIndex];
	}
	#pragma endregion

	#pragma region qangle_relational_operators
	bool operator==(const QAngle_t& angBase) const
	{
		return this->IsEqual(angBase);
	}

	bool operator!=(const QAngle_t& angBase) const
	{
		return !this->IsEqual(angBase);
	}
	#pragma endregion

	#pragma region qangle_assignment_operators
	constexpr QAngle_t& operator=(const QAngle_t& angBase)
	{
		this->x = angBase.x; this->y = angBase.y; this->z = angBase.z;
		return *this;
	}
	#pragma endregion

	#pragma region qangle_arithmetic_assignment_operators
	constexpr QAngle_t& operator+=(const QAngle_t& angBase)
	{
		this->x += angBase.x; this->y += angBase.y; this->z += angBase.z;
		return *this;
	}

	constexpr QAngle_t& operator-=(const QAngle_t& angBase)
	{
		this->x -= angBase.x; this->y -= angBase.y; this->z -= angBase.z;
		return *this;
	}

	constexpr QAngle_t& operator*=(const QAngle_t& angBase)
	{
		this->x *= angBase.x; this->y *= angBase.y; this->z *= angBase.z;
		return *this;
	}

	constexpr QAngle_t& operator/=(const QAngle_t& angBase)
	{
		this->x /= angBase.x; this->y /= angBase.y; this->z /= angBase.z;
		return *this;
	}

	constexpr QAngle_t& operator+=(const float flAdd)
	{
		this->x += flAdd; this->y += flAdd; this->z += flAdd;
		return *this;
	}

	constexpr QAngle_t& operator-=(const float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract; this->z -= flSubtract;
		return *this;
	}

	constexpr QAngle_t& operator*=(const float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply; this->z *= flMultiply;
		return *this;
	}

	constexpr QAngle_t& operator/=(const float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide; this->z /= flDivide;
		return *this;
	}
	#pragma endregion

	#pragma region qangle_arithmetic_unary_operators
	constexpr QAngle_t& operator-()
	{
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
		return *this;
	}

	constexpr QAngle_t operator-() const
	{
		return { -this->x, -this->y, -this->z };
	}
	#pragma endregion

	#pragma region qangle_arithmetic_ternary_operators
	constexpr QAngle_t operator+(const QAngle_t& angAdd) const
	{
		return { this->x + angAdd.x, this->y + angAdd.y, this->z + angAdd.z };
	}

	constexpr QAngle_t operator-(const QAngle_t& angSubtract) const
	{
		return { this->x - angSubtract.x, this->y - angSubtract.y, this->z - angSubtract.z };
	}

	constexpr QAngle_t operator*(const QAngle_t& angMultiply) const
	{
		return { this->x * angMultiply.x, this->y * angMultiply.y, this->z * angMultiply.z };
	}

	constexpr QAngle_t operator/(const QAngle_t& angDivide) const
	{
		return { this->x / angDivide.x, this->y / angDivide.y, this->z / angDivide.z };
	}

	constexpr QAngle_t operator+(const float flAdd) const
	{
		return { this->x + flAdd, this->y + flAdd, this->z + flAdd };
	}

	constexpr QAngle_t operator-(const float flSubtract) const
	{
		return { this->x - flSubtract, this->y - flSubtract, this->z - flSubtract };
	}

	constexpr QAngle_t operator*(const float flMultiply) const
	{
		return { this->x * flMultiply, this->y * flMultiply, this->z * flMultiply };
	}

	constexpr QAngle_t operator/(const float flDivide) const
	{
		return { this->x / flDivide, this->y / flDivide, this->z / flDivide };
	}
	#pragma endregion

	// @returns : true if each component of angle is finite, false otherwise
	[[nodiscard]] bool IsValid() const
	{
		return (std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z));
	}

	/// @returns: true if each component of angle equals to another, false otherwise
	[[nodiscard]] bool IsEqual(const QAngle_t& angEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const
	{
		return (std::fabsf(this->x - angEqual.x) < flErrorMargin && std::fabsf(this->y - angEqual.y) < flErrorMargin && std::fabsf(this->z - angEqual.z) < flErrorMargin);
	}

	/// @returns: true if each component of angle equals zero, false otherwise
	[[nodiscard]] bool IsZero() const
	{
		// @test: to make this implementation right, we should use fpclassify here, but game aren't doing same, probably it's better to keep this same, just ensure that it will be compiled same
		return (this->x == 0.0f && this->y == 0.0f && this->z == 0.0f);
	}

	/// @returns: length of hypotenuse
	[[nodiscard]] float Length2D() const
	{
		return std::sqrtf(x * x + y * y);
	}

	/// clamp each angle component by minimal/maximal allowed value for source sdk games
	/// @returns: clamped angle
	constexpr QAngle_t& Clamp()
	{
		this->x = CRT::Clamp(this->x, -89.f, 89.f);
		this->y = CRT::Clamp(this->y, -180.f, 180.f);
		this->z = CRT::Clamp(this->z, -45.f, 45.f);
		return *this;
	}

	/// map polar angles to the range of [-180, 180] degrees
	/// @returns: normalized angle
	QAngle_t& Normalize()
	{
		this->x = std::remainderf(this->x, 360.f);
		this->y = std::remainderf(this->y, 360.f);
		this->z = std::remainderf(this->z, 360.f);
		return *this;
	}

	/// convert angle to direction vectors
	/// @param[out] pvecForward [optional] output for converted forward vector
	/// @param[out] pvecRight [optional] output for converted right vector
	/// @param[out] pvecUp [optional] output for converted up vector
	void ToDirections(Vector_t* pvecForward, Vector_t* pvecRight = nullptr, Vector_t* pvecUp = nullptr) const;

	/// @param[in] vecOrigin [optional] origin for converted matrix
	/// @returns: matrix converted from angle
	[[nodiscard]] Matrix3x4_t ToMatrix(const Vector_t& vecOrigin = { }) const;

public:
	float x = 0.0f, y = 0.0f, z = 0.0f;
};
