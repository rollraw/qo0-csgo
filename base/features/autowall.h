#pragma once
// used: traceline
#include "../utilities.h"
// used: convar
#include "../sdk/convar.h"
// used: surfacedata
#include "../sdk/interfaces/iphysicssurfaceprops.h"
// used: tracefilter class, trace structure
#include "../sdk/interfaces/ienginetrace.h"
// used: baseentity, cliententity, baseweapon, weapondata classes
#include "../sdk/entity.h"

/* autowall objects structure */
struct FireBulletData_t
{
	Vector			vecPosition;
	Vector			vecDir;
	Trace_t			enterTrace;
	CTraceFilter	filter;

	float			flCurrentDamage;
	int				iPenetrateCount;
};

// @credits: outlassn
class CAutoWall : public CSingleton<CAutoWall>
{
public:
	// Get
	/* returns damage at point and bullet data (if given) */
	float GetDamage(CBaseEntity* pLocal, const Vector& vecPoint, FireBulletData_t& dataOut = FireBulletData_t{});

private:
	// Damage Multiplier
	void ScaleDamage(int iHitGroup, CBaseEntity* pEntity, float flWeaponArmorRatio, float& flDamage);

	// Check
	void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int fMask, ITraceFilter* pFilter, Trace_t* pTrace);
	bool IsBreakableEntity(CBaseEntity* pEntity);
	bool TraceToExit(Trace_t* pEnterTrace, Trace_t* pExitTrace, Vector vecPosition, Vector vecDirection);
	bool HandleBulletPenetration(CBaseEntity* pLocal, surfacedata_t* pEnterSurfaceData, CCSWeaponData* pWeaponData, FireBulletData_t& data);
	bool SimulateFireBullet(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, FireBulletData_t& data);
};
