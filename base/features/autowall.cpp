#include "autowall.h"

// used: mp_teammates_are_enemies, mp_damage_headshot_only, mp_damage_scale_ct_head, mp_damage_scale_ct_body, mp_damage_scale_t_head, mp_damage_scale_t_body, sv_clip_penetration_traces_to_players, ff_damage_reduction_bullets, ff_damage_bullet_penetration
#include "../core/convar.h"
// used: cliptracetoplayers
#include "../sdk.h"

using namespace F;

#pragma region autowall_get
float AUTOWALL::GetDamage(CCSPlayer* pAttacker, const Vector_t& vecPoint, SimulateBulletObject_t* pDataOut)
{
	const Vector_t vecPosition = pAttacker->GetWeaponShootPosition();

	// setup initial data
	SimulateBulletObject_t data = { };
	data.vecPosition = vecPosition;
	data.vecDirection = (vecPoint - vecPosition).Normalized();

	if (CBaseCombatWeapon* pWeapon = pAttacker->GetActiveWeapon(); pWeapon == nullptr || !SimulateFireBullet(pAttacker, pWeapon, data))
		return -1.0f;

	if (pDataOut != nullptr)
		*pDataOut = data;

	return data.flCurrentDamage;
}

void AUTOWALL::ScaleDamage(const int iHitGroup, CCSPlayer* pCSPlayer, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float* pflDamageToScale)
{
	// @ida CCSPlayer::TraceAttack(): server.dll -> "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 8B 75 08 57 8B F9 C6"

	if (iHitGroup != HITGROUP_HEAD && CONVAR::mp_damage_headshot_only->GetBool())
	{
		*pflDamageToScale = 0.0f;
		return;
	}

	const bool bHeavyArmor = pCSPlayer->HasHeavyArmor();

	float flHeadDamageScale = (pCSPlayer->GetTeam() == TEAM_CT ? CONVAR::mp_damage_scale_ct_head->GetFloat() : CONVAR::mp_damage_scale_t_head->GetFloat());
	const float flBodyDamageScale = (pCSPlayer->GetTeam() == TEAM_CT ? CONVAR::mp_damage_scale_ct_body->GetFloat() : CONVAR::mp_damage_scale_t_body->GetFloat());

	if (bHeavyArmor)
		flHeadDamageScale *= 0.5f;

	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
		*pflDamageToScale *= flWeaponHeadShotMultiplier * flHeadDamageScale;
		break;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
	case HITGROUP_NECK:
		*pflDamageToScale *= flBodyDamageScale;
		break;
	case HITGROUP_STOMACH:
		*pflDamageToScale *= 1.25f * flBodyDamageScale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		*pflDamageToScale *= 0.75f * flBodyDamageScale;
		break;
	default:
		break;
	}

	if (pCSPlayer->IsArmored(iHitGroup))
	{
		// @ida CCSPlayer::OnTakeDamage(): server.dll -> "80 BF ? ? ? ? ? F3 0F 10 5C 24 ? F3 0F 10 35"

		const int iArmor = pCSPlayer->GetArmor();
		float flHeavyArmorBonus = 1.0f, flArmorBonus = 0.5f, flArmorRatio = flWeaponArmorRatio * 0.5f;

		if (bHeavyArmor)
		{
			flHeavyArmorBonus = 0.25f;
			flArmorBonus = 0.33f;
			flArmorRatio *= 0.20f;
		}

		float flDamageToHealth = *pflDamageToScale * flArmorRatio;
		if (const float flDamageToArmor = (*pflDamageToScale - flDamageToHealth) * (flHeavyArmorBonus * flArmorBonus); flDamageToArmor > static_cast<float>(iArmor))
			flDamageToHealth = *pflDamageToScale - static_cast<float>(iArmor) / flArmorBonus;

		*pflDamageToScale = flDamageToHealth;
	}
}

bool AUTOWALL::SimulateFireBullet(CCSPlayer* pAttacker, CBaseCombatWeapon* pWeapon, SimulateBulletObject_t& data)
{
	// @ida C_CSPlayer::FireBullet(): client.dll -> "55 8B EC 83 E4 F0 81 EC ? ? ? ? F3 0F 7E"

	const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeapon->GetEconItemView()->GetItemDefinitionIndex());

	if (pWeaponData == nullptr)
		return false;

	float flMaxRange = pWeaponData->flRange;

	// the total number of surfaces any bullet can penetrate in a single flight is capped at 4
	data.iPenetrateCount = 4;
	// set our current damage to what our gun's initial damage reports it will do
	data.flCurrentDamage = static_cast<float>(pWeaponData->iDamage);

	float flTraceLength = 0.0f;
	CTraceFilterSimple filter(pAttacker);

	while (data.iPenetrateCount > 0 && data.flCurrentDamage >= 1.0f)
	{
		// max bullet range
		flMaxRange -= flTraceLength;

		// end position of bullet
		const Vector_t vecEnd = data.vecPosition + data.vecDirection * flMaxRange;

		Ray_t ray(data.vecPosition, vecEnd);
		I::EngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);

		// check for player hitboxes extending outside their collision bounds
		SDK::ClipTraceToPlayers(data.vecPosition, vecEnd + data.vecDirection * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);

		const surfacedata_t* pEnterSurfaceData = I::PhysicsProps->GetSurfaceData(data.enterTrace.surface.nSurfaceProps);
		const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;

		// we didn't hit anything, stop tracing shoot
		if (data.enterTrace.flFraction == 1.0f)
			break;

		// calculate the damage based on the distance the bullet traveled
		flTraceLength += data.enterTrace.flFraction * flMaxRange;
		data.flCurrentDamage *= std::powf(pWeaponData->flRangeModifier, flTraceLength / MAX_DAMAGE);

		// check is actually can shoot through
		if (flTraceLength > 3000.f || flEnterPenetrationModifier < 0.1f)
			break;

		// check did we hit the player
		// @todo: here shouldn't be enemy check?
		if (data.enterTrace.iHitGroup != HITGROUP_GENERIC && data.enterTrace.iHitGroup != HITGROUP_GEAR && pAttacker->IsOtherEnemy(static_cast<CCSPlayer*>(data.enterTrace.pHitEntity)))
		{
			ScaleDamage(data.enterTrace.iHitGroup, static_cast<CCSPlayer*>(data.enterTrace.pHitEntity), pWeaponData->flArmorRatio, pWeaponData->flHeadShotMultiplier, &data.flCurrentDamage);
			return true;
		}

		// check if the bullet can no longer continue penetrating materials
		if (HandleBulletPenetration(pAttacker, pWeaponData, pEnterSurfaceData, data))
			break;
	}

	return false;
}
#pragma endregion

#pragma region autowall_main
bool AUTOWALL::TraceToExit(const Trace_t& enterTrace, Trace_t& exitTrace, const Vector_t& vecPosition, const Vector_t& vecDirection, const IHandleEntity* pClipPlayer)
{
	// @ida TraceToExit(): client.dll | server.dll -> "55 8B EC 83 EC 4C F3 0F 10 75"

	float flDistance = 0.0f;
	int iStartContents = 0;

	while (flDistance <= 90.0f)
	{
		// add extra distance to our ray
		flDistance += 4.0f;

		// multiply the direction vector to the distance so we go outwards, add our position to it
		Vector_t vecEnd = vecPosition + vecDirection * flDistance;

		if (iStartContents == 0)
			iStartContents = I::EngineTrace->GetPointContents(vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (const int iCurrentContents = I::EngineTrace->GetPointContents(vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr); !(iCurrentContents & MASK_SHOT_HULL) || ((iCurrentContents & CONTENTS_HITBOX) && iCurrentContents != iStartContents))
		{
			// setup our end position by deducting the direction by the extra added distance
			const Vector_t vecStart = vecEnd - (vecDirection * 4.0f);

			// trace ray to world
			Ray_t rayWorld(vecEnd, vecStart);
			I::EngineTrace->TraceRay(rayWorld, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			if (CONVAR::sv_clip_penetration_traces_to_players->GetBool()) // @note: convar check was added since 07.07.2021 (version 1.37.9.5, build 1304)
			{
				CTraceFilterSimple filter(pClipPlayer);
				SDK::ClipTraceToPlayers(vecStart, vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &exitTrace, -60.f);
			}

			// check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.bStartSolid && (exitTrace.surface.uFlags & SURF_HITBOX))
			{
				// trace ray to entity @note: added since 2021Q4-2022Q1 updates, also skip one more specific entity
				Ray_t ray(vecEnd, vecPosition);
				CTraceFilterSkipTwoEntities filter(exitTrace.pHitEntity, pClipPlayer);

				I::EngineTrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.bStartSolid)
				{
					vecEnd = exitTrace.vecEnd;
					return true;
				}
			}
			else if (!exitTrace.DidHit() || exitTrace.bStartSolid)
			{
				// check did hit non world entity
				if (enterTrace.pHitEntity != nullptr && enterTrace.pHitEntity->GetIndex() != 0 &&
					// and it is breakable
					enterTrace.pHitEntity->IsBreakable())
				{
					exitTrace = enterTrace;
					exitTrace.vecEnd = vecEnd + vecDirection;
					return true;
				}
			}
			else
			{
				// check did hit breakable two-way entity (barrel, box, etc)
				if (enterTrace.pHitEntity->IsBreakable() && exitTrace.pHitEntity->IsBreakable())
					return true;

				if ((enterTrace.surface.uFlags & SURF_NODRAW) || (!(exitTrace.surface.uFlags & SURF_NODRAW) && exitTrace.plane.vecNormal.DotProduct(vecDirection) <= 1.0f))
				{
					vecEnd -= vecDirection * (exitTrace.flFraction * 4.0f);
					return true;
				}
			}
		}
	}

	return false;
}

bool AUTOWALL::HandleBulletPenetration(CCSPlayer* pLocal, const CCSWeaponData* pWeaponData, const surfacedata_t* pEnterSurfaceData, SimulateBulletObject_t& data)
{
	// @ida CCSPlayer::HandleBulletPenetration(): client.dll | server.dll -> ABS["E8 ? ? ? ? 83 C4 40 84 C0" + 0x1]

	const MaterialHandle_t hEnterMaterial = pEnterSurfaceData->game.hMaterial;

	// check is not penetrable material
	if (data.iPenetrateCount == 0 && hEnterMaterial != CHAR_TEX_GRATE && hEnterMaterial != CHAR_TEX_GLASS && !(data.enterTrace.surface.uFlags & SURF_NODRAW))
		return true;

	// check is weapon can't penetrate
	if (pWeaponData->flPenetration <= 0.0f || data.iPenetrateCount <= 0)
		return true;

	Trace_t exitTrace = { };
	if (!TraceToExit(data.enterTrace, exitTrace, data.enterTrace.vecEnd, data.vecDirection, pLocal) && !(I::EngineTrace->GetPointContents(data.enterTrace.vecEnd, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return true;

	const surfacedata_t* pExitSurfaceData = I::PhysicsProps->GetSurfaceData(exitTrace.surface.nSurfaceProps);
	const MaterialHandle_t hExitMaterial = pExitSurfaceData->game.hMaterial;

	const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;
	const float flExitPenetrationModifier = pExitSurfaceData->game.flPenetrationModifier;

	float flDamageLostModifier = 0.16f;
	float flPenetrationModifier = 0.0f;

	// [side change] we only handle new penetration method and expect that 'sv_penetration_type' is 1

	if (hEnterMaterial == CHAR_TEX_GRATE || hEnterMaterial == CHAR_TEX_GLASS)
	{
		flDamageLostModifier = 0.05f;
		flPenetrationModifier = 3.0f;
	}
	else if (((data.enterTrace.iContents >> 3) & CONTENTS_SOLID) || ((data.enterTrace.surface.uFlags >> 7) & SURF_LIGHT))
		flPenetrationModifier = 1.0f;
	else if (hEnterMaterial == CHAR_TEX_FLESH && CONVAR::ff_damage_reduction_bullets->GetFloat() == 0.0f && data.enterTrace.pHitEntity != nullptr && data.enterTrace.pHitEntity->IsPlayer() && !CONVAR::mp_teammates_are_enemies->GetBool() && pLocal->GetTeam() == data.enterTrace.pHitEntity->GetTeam())
	{
		const float flPenetrateDamage = CONVAR::ff_damage_bullet_penetration->GetFloat();

		if (flPenetrateDamage == 0.0f)
			return true;

		// shoot through teammates
		flPenetrationModifier = flPenetrateDamage;
	}
	else
		flPenetrationModifier = (flEnterPenetrationModifier + flExitPenetrationModifier) * 0.5f;

	if (hEnterMaterial == hExitMaterial)
	{
		if (hExitMaterial == CHAR_TEX_CARDBOARD || hExitMaterial == CHAR_TEX_WOOD)
			flPenetrationModifier = 3.0f;
		else if (hExitMaterial == CHAR_TEX_PLASTIC)
			flPenetrationModifier = 2.0f;
	}

	// [side change] used squared length, because game wastefully converts it back to squared
	const float flTraceDistanceSqr = (exitTrace.vecEnd - data.enterTrace.vecEnd).LengthSqr();

	// penetration modifier
	const float flModifier = (flPenetrationModifier > 0.0f ? 1.0f / flPenetrationModifier : 0.0f);

	// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	const float flLostDamage = (data.flCurrentDamage * flDamageLostModifier + (pWeaponData->flPenetration > 0.0f ? 3.75f / pWeaponData->flPenetration : 0.0f) * (flModifier * 3.0f)) + ((flModifier * flTraceDistanceSqr) / 24.0f);

	// reduce damage power each time we hit something other than a grate
	data.flCurrentDamage -= CRT::Max(flLostDamage, 0.0f);

	// check do we still have enough damage to deal?
	if (data.flCurrentDamage < 1.0f)
		return true;

	data.vecPosition = exitTrace.vecEnd;
	--data.iPenetrateCount;
	return false;
}
#pragma endregion
