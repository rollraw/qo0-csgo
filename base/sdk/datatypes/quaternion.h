#pragma once

struct Quaternion_t
{
	constexpr Quaternion_t(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f, const float w = 0.0f) :
		x(x), y(y), z(z), w(w) { }

	[[nodiscard]] bool IsValid() const
	{
		return (std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w));
	}

	/// @param[in] vecOrigin [optional] translation for converted matrix
	/// @returns: matrix converted from quaternion
	[[nodiscard]] Matrix3x4_t ToMatrix(const Vector_t& vecOrigin = { }) const
	{
		Q_ASSERT(this->IsValid());

		Matrix3x4_t matOut;

#ifdef _DEBUG // precalculate common multiplications
		const float x2 = this->x + this->x, y2 = this->y + this->y, z2 = this->z + this->z;
		const float xx = this->x * x2, xy = this->x * y2, xz = this->x * z2;
		const float yy = this->y * y2, yz = this->y * z2;
		const float zz = this->z * z2;
		const float wx = this->w * x2, wy = this->w * y2, wz = this->w * z2;

		matOut[0][0] = 1.0f - (yy + zz);
		matOut[1][0] = xy + wz;
		matOut[2][0] = xz - wy;

		matOut[0][1] = xy - wz;
		matOut[1][1] = 1.0f - (xx + zz);
		matOut[2][1] = yz + wx;

		matOut[0][2] = xz + wy;
		matOut[1][2] = yz - wx;
		matOut[2][2] = 1.0f - (xx + yy);
#else // let the compiler optimize calculations itself
		matOut[0][0] = 1.0f - 2.0f * this->y * this->y - 2.0f * this->z * this->z;
		matOut[1][0] = 2.0f * this->x * this->y + 2.0f * this->w * this->z;
		matOut[2][0] = 2.0f * this->x * this->z - 2.0f * this->w * this->y;

		matOut[0][1] = 2.0f * this->x * this->y - 2.0f * this->w * this->z;
		matOut[1][1] = 1.0f - 2.0f * this->x * this->x - 2.0f * this->z * this->z;
		matOut[2][1] = 2.0f * this->y * this->z + 2.0f * this->w * this->x;

		matOut[0][2] = 2.0f * this->x * this->z + 2.0f * this->w * this->y;
		matOut[1][2] = 2.0f * this->y * this->z - 2.0f * this->w * this->x;
		matOut[2][2] = 1.0f - 2.0f * this->x * this->x - 2.0f * this->y * this->y;
#endif

		matOut[0][3] = vecOrigin.x;
		matOut[1][3] = vecOrigin.y;
		matOut[2][3] = vecOrigin.z;
		return matOut;
	}

	float x, y, z, w;
};

struct alignas(16) QuaternionAligned_t : Quaternion_t
{
	QuaternionAligned_t& operator=(const Quaternion_t& quatOther)
	{
		this->x = quatOther.x;
		this->y = quatOther.y;
		this->z = quatOther.z;
		this->w = quatOther.w;
		return *this;
	}
};
static_assert(alignof(QuaternionAligned_t) == 16);
