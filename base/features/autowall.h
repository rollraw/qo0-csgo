#pragma once
// used: ccsweapondata
#include "../sdk/interfaces/iweaponsystem.h"
// used: surfacedata_t
#include "../sdk/interfaces/iphysicssurfaceprops.h"
// used: ccsplayer, cbasecombatweapon
#include "../sdk/entity.h"

struct SimulateBulletObject_t
{
	Vector_t vecPosition = { };
	Vector_t vecDirection = { };
	Trace_t enterTrace = { };
	float flCurrentDamage = 0.0f;
	int iPenetrateCount = 0;
};

// @credits: outlassn
namespace F::AUTOWALL
{
	/* @section: get */
	/// @param[in] vecPoint another player's point to get damage on
	/// @param[out] pDataOut [optional] simulated fire bullet data output
	/// @returns: damage at given point from given player shoot position
	float GetDamage(CCSPlayer* pAttacker, const Vector_t& vecPoint, SimulateBulletObject_t* pDataOut = nullptr);
	/// scales given damage by various dependent factors
	/// @param[in,out] pflDamageToScale damage value that being scaled
	void ScaleDamage(const int iHitGroup, CCSPlayer* pCSPlayer, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float* pflDamageToScale);
	/// simulates fire bullet to penetrate up to 4 walls
	/// @returns: true if simulated bullet hit the player, false otherwise
	bool SimulateFireBullet(CCSPlayer* pAttacker, CBaseCombatWeapon* pWeapon, SimulateBulletObject_t& data);

	/* @section: main */
	/// find exact penetration exit
	/// @returns: true if successfully got penetration exit for current object, false otherwise
	bool TraceToExit(const Trace_t& enterTrace, Trace_t& exitTrace, const Vector_t& vecPosition, const Vector_t& vecDirection, const IHandleEntity* pClipPlayer);
	/// process bullet penetration to count penetrated objects it hits
	/// @returns: true if bullet stopped and we should stop processing penetration, false otherwise
	bool HandleBulletPenetration(CCSPlayer* pLocal, const CCSWeaponData* pWeaponData, const surfacedata_t* pEnterSurfaceData, SimulateBulletObject_t& data);
}
