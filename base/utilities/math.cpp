#include "math.h"

// used: modules definitons, convar, globals interfaces
#include "../core/interfaces.h"

bool M::Setup()
{
	RandomSeed = reinterpret_cast<RandomSeedFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomSeed")));
	if (RandomSeed == nullptr)
		return false;

	RandomFloat = reinterpret_cast<RandomFloatFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomFloat")));
	if (RandomFloat == nullptr)
		return false;

	RandomFloatExp = reinterpret_cast<RandomFloatExpFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomFloatExp")));
	if (RandomFloatExp == nullptr)
		return false;

	RandomInt = reinterpret_cast<RandomIntFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomInt")));
	if (RandomInt == nullptr)
		return false;

	RandomGaussianFloat = reinterpret_cast<RandomGaussianFloatFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomGaussianFloat")));
	if (RandomGaussianFloat == nullptr)
		return false;

	return true;
}

void M::VectorAngles(const Vector& vecForward, QAngle& angView)
{
	float flPitch, flYaw;

	if (vecForward.x == 0.f && vecForward.y == 0.f)
	{
		flPitch = (vecForward.z > 0.f) ? 270.f : 90.f;
		flYaw = 0.f;
	}
	else
	{
		flPitch = std::atan2f(-vecForward.z, vecForward.Length2D()) * 180.f / M_PI;

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = std::atan2f(vecForward.y, vecForward.x) * 180.f / M_PI;

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	angView.x = flPitch;
	angView.y = flYaw;
	angView.z = 0.f;
}

void M::AngleVectors(const QAngle& angView, Vector* pForward, Vector* pRight, Vector* pUp)
{
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angView.z));

	if (pForward != nullptr)
	{
		pForward->x = cp * cy;
		pForward->y = cp * sy;
		pForward->z = -sp;
	}

	if (pRight != nullptr)
	{
		pRight->x = -1 * sr * sp * cy + -1 * cr * -sy;
		pRight->y = -1 * sr * sp * sy + -1 * cr * cy;
		pRight->z = -1 * sr * cp;
	}

	if (pUp != nullptr)
	{
		pUp->x = cr * sp * cy + -sr * -sy;
		pUp->y = cr * sp * sy + -sr * cy;
		pUp->z = cr * cp;
	}
}

void M::AngleMatrix(const QAngle& angView, matrix3x4_t& matrix)
{
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angView.z));

	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	const float crcy = cr * cy;
	const float crsy = cr * sy;
	const float srcy = sr * cy;
	const float srsy = sr * sy;

	matrix[0][1] = sp * srcy - crsy;
	matrix[1][1] = sp * srsy + crcy;
	matrix[2][1] = sr * cp;

	matrix[0][2] = (sp * crcy + srsy);
	matrix[1][2] = (sp * crsy - srcy);
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.f;
	matrix[1][3] = 0.f;
	matrix[2][3] = 0.f;
}

Vector2D M::AnglePixels(const float flSensitivity, const float flPitch, const float flYaw, const QAngle& angBegin, const QAngle& angEnd)
{
	QAngle angDelta = angBegin - angEnd;
	angDelta.Normalize();

	const float flPixelMovePitch = (-angDelta.x) / (flYaw * flSensitivity);
	const float flPixelMoveYaw = (angDelta.y) / (flPitch * flSensitivity);

	return Vector2D(flPixelMoveYaw, flPixelMovePitch);
}

QAngle M::PixelsAngle(const float flSensitivity, const float flPitch, const float flYaw, const Vector2D& vecPixels)
{
	const float flAngleMovePitch = (-vecPixels.x) * (flYaw * flSensitivity);
	const float flAngleMoveYaw = (vecPixels.y) * (flPitch * flSensitivity);

	return QAngle(flAngleMoveYaw, flAngleMovePitch, 0.f);
}

QAngle M::CalcAngle(const Vector& vecStart, const Vector& vecEnd)
{
	QAngle angView;
	const Vector vecDelta = vecEnd - vecStart;
	VectorAngles(vecDelta, angView);
	angView.Normalize();

	return angView;
}

Vector M::VectorTransform(const Vector& vecTransform, const matrix3x4_t& matrix)
{
	return Vector(vecTransform.DotProduct(matrix[0]) + matrix[0][3],
		vecTransform.DotProduct(matrix[1]) + matrix[1][3],
		vecTransform.DotProduct(matrix[2]) + matrix[2][3]);
}

Vector M::ExtrapolateTick(const Vector& p0, const Vector& v0)
{
	// position formula: p0 + v0t
	return p0 + (v0 * I::Globals->flIntervalPerTick);
}
