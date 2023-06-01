#include "matrix.h"

#include "qangle.h"

// used: m_rad2deg
#include "../../utilities/math.h"

[[nodiscard]] QAngle_t Matrix3x4_t::ToAngles() const
{
	// extract the basis vectors from the matrix. since we only need the z component of the up vector, we don't get x and y
	const Vector_t vecForward = this->GetForward();
	const Vector_t vecLeft = this->GetLeft();
	const float flUpZ = this->arrData[2][2];

	const float flLength2D = vecForward.Length2D();
	const float flPitch = M_RAD2DEG(std::atan2f(-vecForward.z, flLength2D));

	// check is enough here to get angles
	if (flLength2D > 0.001f)
		return { flPitch, M_RAD2DEG(std::atan2f(vecForward.y, vecForward.x)), M_RAD2DEG(std::atan2f(vecLeft.z, flUpZ)) };

	// forward is mostly Z, gimbal lock
	// assume no roll in this case as one degree of freedom has been lost (i.e. yaw equals roll)
	return { flPitch, M_RAD2DEG(std::atan2f(-vecLeft.x, vecLeft.y)), 0.0f };
}
