#include "qangle.h"

// used: [d3d] xmscalarsincos
#include <directxmath.h>

#include "matrix.h"

// used: m_deg2rad
#include "../../utilities/math.h"

void QAngle_t::ToDirections(Vector_t* pvecForward, Vector_t* pvecRight, Vector_t* pvecUp) const
{
	float flPitchSin, flPitchCos, flYawSin, flYawCos, flRollSin, flRollCos;
	DirectX::XMScalarSinCos(&flPitchSin, &flPitchCos, M_DEG2RAD(this->x));
	DirectX::XMScalarSinCos(&flYawSin, &flYawCos, M_DEG2RAD(this->y));
	DirectX::XMScalarSinCos(&flRollSin, &flRollCos, M_DEG2RAD(this->z));

	if (pvecForward != nullptr)
	{
		pvecForward->x = flPitchCos * flYawCos;
		pvecForward->y = flPitchCos * flYawSin;
		pvecForward->z = -flPitchSin;
	}

	if (pvecRight != nullptr)
	{
		pvecRight->x = (-flRollSin * flPitchSin * flYawCos) + (-flRollCos * -flYawSin);
		pvecRight->y = (-flRollSin * flPitchSin * flYawSin) + (-flRollCos * flYawCos);
		pvecRight->z = (-flRollSin * flPitchCos);
	}

	if (pvecUp != nullptr)
	{
		pvecUp->x = (flRollCos * flPitchSin * flYawCos) + (-flRollSin * -flYawSin);
		pvecUp->y = (flRollCos * flPitchSin * flYawSin) + (-flRollSin * flYawCos);
		pvecUp->z = (flRollCos * flPitchCos);
	}
}

Matrix3x4_t QAngle_t::ToMatrix(const Vector_t& vecOrigin) const
{
	float flPitchSin, flPitchCos, flYawSin, flYawCos, flRollSin, flRollCos;
	DirectX::XMScalarSinCos(&flPitchSin, &flPitchCos, M_DEG2RAD(this->x));
	DirectX::XMScalarSinCos(&flYawSin, &flYawCos, M_DEG2RAD(this->y));
	DirectX::XMScalarSinCos(&flRollSin, &flRollCos, M_DEG2RAD(this->z));

	return
	{
		(flPitchCos * flYawCos), (flRollSin * flPitchSin * flYawCos + flRollCos * -flYawSin), (flRollCos * flPitchSin * flYawCos + -flRollSin * -flYawSin), vecOrigin.x,
		(flPitchCos * flYawSin), (flRollSin * flPitchSin * flYawSin + flRollCos * flYawCos), (flRollCos * flPitchSin * flYawSin + -flRollSin * flYawCos), vecOrigin.y,
		(-flPitchSin), (flRollSin * flPitchCos), (flRollCos * flPitchCos), vecOrigin.z
	};
}
