#pragma once
// used: surfacedata
#include "../sdk/interfaces/iphysicssurfaceprops.h"
// used: baseentity, cliententity, baseweapon, weapondata classes
#include "../sdk/entity.h"

/* autowall objects structure */
struct FireBulletData_t
{
	Vector			vecPosition = { };
	Vector			vecDirection = { };
	Trace_t			enterTrace = { };
	float			flCurrentDamage = 0.0f;
	int				iPenetrateCount = 0;
};

// @credits: outlassn
class CAutoWall : public CSingleton<CAutoWall>
{
public:
	// Get
	/* returns damage at point and bullet data (if given) */
	float GetDamage(CBaseEntity* pLocal, const Vector& vecPoint, FireBulletData_t& dataOut);
	/* calculates damage factor */
	void ScaleDamage(int iHitGroup, CBaseEntity* pEntity, float flWeaponArmorRatio, float& flDamage);
	/* returns true if trace entity breakable */
	bool IsBreakableEntity(CBaseEntity* pEntity) const;

private:
	// Main
	void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int fMask, ITraceFilter* pFilter, Trace_t* pTrace);
	bool TraceToExit(Trace_t& enterTrace, Trace_t& exitTrace, Vector vecPosition, Vector vecDirection) const;
	bool HandleBulletPenetration(CBaseEntity* pLocal, CCSWeaponData* pWeaponData, surfacedata_t* pEnterSurfaceData, FireBulletData_t& data) const;
	bool SimulateFireBullet(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, FireBulletData_t& data);
};
