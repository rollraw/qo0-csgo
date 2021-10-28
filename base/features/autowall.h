#pragma once
// used: surfacedata
#include "../sdk/interfaces/iphysicssurfaceprops.h"
// used: baseentity, cliententity, baseweapon, weapondata classes
#include "../sdk/entity.h"

struct FireBulletData_t
{
	Vector			vecPosition = { };
	Vector			vecDirection = { };
	Trace_t			enterTrace = { };
	float			flCurrentDamage = 0.0f;
	int				iPenetrateCount = 0;
};

// @credits: outlassn
class CAutoWall
{
public:
	// Get
	/* returns damage at point and simulated bullet data (if given) */
	static float GetDamage(CBaseEntity* pLocal, const Vector& vecPoint, FireBulletData_t* pDataOut = nullptr);
	/* calculates damage factor */
	static void ScaleDamage(const int iHitGroup, CBaseEntity* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float& flDamage);
	/* simulates fire bullet to penetrate up to 4 walls, return true when hitting player */
	static bool SimulateFireBullet(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, FireBulletData_t& data);

private:
	// Main
	static void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, const unsigned int fMask, ITraceFilter* pFilter, Trace_t* pTrace, const float flMinRange = 0.0f);
	static bool TraceToExit(Trace_t& enterTrace, Trace_t& exitTrace, const Vector& vecPosition, const Vector& vecDirection, const CBaseEntity* pClipPlayer);
	static bool HandleBulletPenetration(CBaseEntity* pLocal, const CCSWeaponData* pWeaponData, const surfacedata_t* pEnterSurfaceData, FireBulletData_t& data);
};
