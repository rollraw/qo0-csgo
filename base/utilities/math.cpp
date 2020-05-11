#include "math.h"

// used: modules definitons, convar, globals interfaces
#include "../core/interfaces.h"

bool M::Setup()
{
	RandomSeed = (RandomSeedFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomSeed"));
	if (RandomSeed == nullptr)
		return false;

	RandomFloat = (RandomFloatFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomFloat"));
	if (RandomFloat == nullptr)
		return false;

	RandomFloatExp = (RandomFloatExpFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomFloatExp"));
	if (RandomFloatExp == nullptr)
		return false;

	RandomInt = (RandomIntFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomInt"));
	if (RandomInt == nullptr)
		return false;

	RandomGaussianFloat = (RandomGaussianFloatFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("RandomGaussianFloat"));
	if (RandomGaussianFloat == nullptr)
		return false;

	return true;
}

void M::VectorAngles(const Vector& forward, QAngle& angles)
{
	if (forward[PITCH] == 0.f && forward[YAW] == 0.f)
	{
		angles[PITCH] = (forward[ROLL] > 0.f) ? 270.f : 90.f;
		angles[YAW] = 0.f;
	}
	else
	{
		angles[PITCH] = std::atan2f(-forward[ROLL], forward.Length2D()) * -180.f / M_PI;
		angles[YAW] = std::atan2f(forward[YAW], forward[PITCH]) * 180.f / M_PI;

		if (angles[YAW] > 90.f) angles[YAW] -= 180.f;
		else if (angles[YAW] < 90.f) angles[YAW] += 180.f;
		else if (angles[YAW] == 90.f) angles[YAW] = 0.f;
	}

	angles[ROLL] = 0.f;
}

void M::AngleVectors(const QAngle& angles, Vector* pForward, Vector* pRight, Vector* pUp)
{
	float sr, sp, sy, cr, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angles[PITCH]));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angles[YAW]));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angles[ROLL]));

	if (pForward != nullptr)
	{
		pForward->x = cp * cy;
		pForward->y = cp * sy;
		pForward->z = -sp;
	}

	if (pRight != nullptr)
	{
		pRight->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		pRight->y = (-1 * sr * sp * sy + -1 * cr * cy);
		pRight->z = -1 * sr * cp;
	}

	if (pUp != nullptr)
	{
		pUp->x = (cr * sp * cy + -sr * -sy);
		pUp->y = (cr * sp * sy + -sr * cy);
		pUp->z = cr * cp;
	}
}

void M::MatrixGetColumn(const matrix3x4_t& matIn, int nColumn, Vector& vecOut)
{
	vecOut.x = matIn[0][nColumn];
	vecOut.y = matIn[1][nColumn];
	vecOut.z = matIn[2][nColumn];
}

void M::MatrixSetColumn(const Vector& vecIn, int nColumn, matrix3x4_t& matOut)
{
	matOut[0][nColumn] = vecIn.x;
	matOut[1][nColumn] = vecIn.y;
	matOut[2][nColumn] = vecIn.z;
}

void M::MatrixSetOrigin(matrix3x4_t& matrix, const Vector vecOrigin, const Vector vecNewOrigin)
{
	Vector vecMatrixOrigin(matrix[0][3], matrix[1][3], matrix[2][3]);
	M::MatrixSetColumn(vecNewOrigin + (vecMatrixOrigin - vecOrigin), 3, matrix);
}

void M::AngleMatrix(const QAngle& angles, matrix3x4_t& matrix)
{
	float sr, sp, sy, cr, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angles[PITCH]));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angles[YAW]));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angles[ROLL]));

	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;

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

Vector2D M::AnglePixels(float flSensitivity, float flPitch, float flYaw, const QAngle& angBegin, const QAngle& angEnd)
{
	QAngle angDelta = angBegin - angEnd;
	angDelta.Normalize();

	float flPixelMovePitch = (-angDelta.x) / (flYaw * flSensitivity);
	float flPixelMoveYaw = (angDelta.y) / (flPitch * flSensitivity);

	return Vector2D(flPixelMoveYaw, flPixelMovePitch);
}

QAngle M::PixelsAngle(float flSensitivity, float flPitch, float flYaw, const Vector2D& vecPixels)
{
	float flAngleMovePitch = (-vecPixels.x) * (flYaw * flSensitivity);
	float flAngleMoveYaw = (vecPixels.y) * (flPitch * flSensitivity);

	return QAngle(flAngleMoveYaw, flAngleMovePitch, 0.f);
}

float M::GetFov(const QAngle& angView, const QAngle& angAimPoint)
{
	Vector vecView, vecAim;
	AngleVectors(angView, &vecView);
	AngleVectors(angAimPoint, &vecAim);
	return M_RAD2DEG(std::acosf(vecView.DotProduct(vecAim) / vecView.LengthSqr()));
}

QAngle M::CalcAngle(Vector src, Vector dst)
{
	QAngle angles;
	Vector vecDelta = src - dst;
	VectorAngles(vecDelta, angles);
	vecDelta.Normalize();
	return angles;
}

float M::VectorDistance(Vector src, Vector dst)
{
	return std::sqrtf(std::pow(src.x - dst.x, 2) + std::pow(src.y - dst.y, 2) + std::pow(src.z - dst.z, 2));
}

Vector M::VectorTransform(const Vector& vecIn, const matrix3x4_t& matrix)
{
	return Vector(vecIn.DotProduct(matrix[0]) + matrix[0][3], vecIn.DotProduct(matrix[1]) + matrix[1][3], vecIn.DotProduct(matrix[2]) + matrix[2][3]);
}

Vector M::ExtrapolateTick(Vector p0, Vector v0)
{
	// position formula: p0 + v0t
	return p0 + (v0 * I::Globals->flIntervalPerTick);
}

float M::FloatNormalize(float flValue)
{
	while (flValue > 180.f)
		flValue -= 360.f;

	while (flValue < -180.f)
		flValue += 360.f;

	return flValue;
}
