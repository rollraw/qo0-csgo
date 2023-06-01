#pragma once
#include "../datatypes/qangle.h"
#include "../datatypes/vector.h"

// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/game/shared/IEffects.h

// forward declarations
class CBaseEntity;

class IPredictionSystem
{
public:
	virtual ~IPredictionSystem() { }

	IPredictionSystem* pNextSystem; // 0x04
	bool bSuppressEvent; // 0x08
	CBaseEntity* pSuppressHost; // 0x0C
	int nStatusPushed; // 0x10
};
static_assert(sizeof(IPredictionSystem) == 0x14);

class IEffects : public IPredictionSystem, ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	void Beam(const Vector_t& vecStart, const Vector_t& vecEnd, int nModelIndex, int nHaloIndex, unsigned char uFrameStart, unsigned char uFrameRate, float flLife, unsigned char uWidth, unsigned char uEndWidth, unsigned char uFadeLength, unsigned char uNoise, unsigned char uRed, unsigned char uGreen, unsigned char uBlue, unsigned char uBrightness, unsigned char uSpeed)
	{
		CallVFunc<void, 1U>(this, &vecStart, &vecEnd, nModelIndex, nHaloIndex, uFrameStart, uFrameRate, flLife, uWidth, uEndWidth, uFadeLength, uNoise, uRed, uGreen, uBlue, uBrightness, uSpeed);
	}

	void Smoke(const Vector_t& vecOrigin, int nModelIndex, float flScale, float flFrameRate)
	{
		CallVFunc<void, 2U>(this, &vecOrigin, nModelIndex, flScale, flFrameRate);
	}

	void Sparks(const Vector_t& vecOrigin, int nMagnitude = 1, int nTrailLength = 1, const Vector_t* pvecDirection = nullptr)
	{
		CallVFunc<void, 3U>(this, &vecOrigin, nMagnitude, nTrailLength, pvecDirection);
	}

	void Dust(const Vector_t& vecOrigin, const Vector_t& vecDirection, float flSize, float flSpeed)
	{
		CallVFunc<void, 4U>(this, &vecOrigin, &vecDirection, flSize, flSpeed);
	}

	void MuzzleFlash(const Vector_t& vecOrigin, const QAngle_t& angView, float flScale, int nType)
	{
		CallVFunc<void, 5U>(this, &vecOrigin, &angView, flScale, nType);
	}

	void MetalSparks(const Vector_t& vecOrigin, const Vector_t& vecDirection)
	{
		CallVFunc<void, 6U>(this, &vecOrigin, &vecDirection);
	}

	void EnergySplash(const Vector_t& vecOrigin, const Vector_t& vecDirection, bool bExplosive = false)
	{
		CallVFunc<void, 7U>(this, &vecOrigin, &vecDirection, bExplosive);
	}

	void Ricochet(const Vector_t& vecOrigin, const Vector_t& vecDirection)
	{
		CallVFunc<void, 8U>(this, &vecOrigin, &vecDirection);
	}
};
