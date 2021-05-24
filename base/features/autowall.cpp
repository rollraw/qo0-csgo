#include "autowall.h"

// used: angle/vector calculations
#include "../utilities/math.h"
// used: convar, cliententitylist, physics, trace, clients, globals interfaces
#include "../core/interfaces.h"

float CAutoWall::GetDamage(CBaseEntity* pLocal, const Vector& vecPoint, FireBulletData_t& dataOut)
{
	const Vector vecPosition = pLocal->GetEyePosition();

	// setup data
	FireBulletData_t data = { };
	data.vecPosition = vecPosition;
	data.vecDirection = (vecPoint - vecPosition).Normalized();

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return -1.0f;

	if (!SimulateFireBullet(pLocal, pWeapon, data))
		return -1.0f;

	dataOut = data;
	return data.flCurrentDamage;
}

void CAutoWall::ScaleDamage(int iHitGroup, CBaseEntity* pEntity, float flWeaponArmorRatio, float& flDamage)
{
	const bool bHeavyArmor = pEntity->HasHeavyArmor();
	const int iArmor = pEntity->GetArmor();

	static CConVar* mp_damage_scale_ct_head = I::ConVar->FindVar(XorStr("mp_damage_scale_ct_head"));
	static CConVar* mp_damage_scale_t_head = I::ConVar->FindVar(XorStr("mp_damage_scale_t_head"));

	static CConVar* mp_damage_scale_ct_body = I::ConVar->FindVar(XorStr("mp_damage_scale_ct_body"));
	static CConVar* mp_damage_scale_t_body = I::ConVar->FindVar(XorStr("mp_damage_scale_t_body"));

	const float flHeadScale = pEntity->GetTeam() == TEAM_CT ? mp_damage_scale_ct_head->GetFloat() : pEntity->GetTeam() == TEAM_TT ? mp_damage_scale_t_head->GetFloat() : 1.0f;
	const float flBodyScale = pEntity->GetTeam() == TEAM_CT ? mp_damage_scale_ct_body->GetFloat() : pEntity->GetTeam() == TEAM_TT ? mp_damage_scale_t_body->GetFloat() : 1.0f;

	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
		flDamage *= (bHeavyArmor ? 2.0f : 4.0f) * flHeadScale;
		break;
	case HITGROUP_STOMACH:
		flDamage *= 1.25f * flBodyScale;
		break;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		flDamage *= flBodyScale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		flDamage *= 0.75f * flBodyScale;
		break;
	default:
		break;
	}

	// check is armored
	if (iArmor > 0 && ((iHitGroup == HITGROUP_HEAD && pEntity->HasHelmet()) || (iHitGroup >= HITGROUP_GENERIC && iHitGroup <= HITGROUP_RIGHTARM)))
	{
		float flArmorScale = 1.0f, flArmorBonusRatio = 0.5f, flArmorRatio = flWeaponArmorRatio * 0.5f;

		if (bHeavyArmor)
		{
			flArmorScale = 0.33f;
			flArmorBonusRatio = 0.33f;
			flArmorRatio *= 0.5f;
		}

		float flNewDamage = flDamage * flArmorRatio;

		if (bHeavyArmor)
			flNewDamage *= 0.85f;

		if (((flDamage - flDamage * flArmorRatio) * (flArmorScale * flArmorBonusRatio)) > iArmor)
			flNewDamage = flDamage - iArmor / flArmorBonusRatio;

		flDamage = flNewDamage;
	}
}

bool CAutoWall::IsBreakableEntity(CBaseEntity* pEntity) const
{
	// @ida isbreakableentity: 55 8B EC 51 56 8B F1 85 F6 74 68

	// skip invalid entities
	if (pEntity == nullptr)
		return false;

	const int iHealth = pEntity->GetHealth();

	// first check to see if it's already broken
	if (iHealth < 0 && pEntity->IsMaxHealth() > 0)
		return true;

	if (pEntity->GetTakeDamage() != DAMAGE_YES)
	{
		const EClassIndex nClassIndex = pEntity->GetClientClass()->nClassID;

		// force pass cfuncbrush
		if (nClassIndex != EClassIndex::CFuncBrush)
			return false;
	}

	const int nCollisionGroup = pEntity->GetCollisionGroup();

	if (nCollisionGroup != COLLISION_GROUP_PUSHAWAY && nCollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && nCollisionGroup != COLLISION_GROUP_NONE)
		return false;

	if (iHealth > 200)
		return false;

	IMultiplayerPhysics* pPhysicsInterface = dynamic_cast<IMultiplayerPhysics*>(pEntity);
	if (pPhysicsInterface != nullptr)
	{
		if (pPhysicsInterface->GetMultiplayerPhysicsMode() != PHYSICS_MULTIPLAYER_SOLID)
			return false;
	}
	else
	{
		const char* szClassName = pEntity->GetClassname();

		if (!strcmp(szClassName, XorStr("func_breakable")) || !strcmp(szClassName, XorStr("func_breakable_surf")))
		{
			if (!strcmp(szClassName, XorStr("func_breakable_surf")))
			{
				CBreakableSurface* pSurface = static_cast<CBreakableSurface*>(pEntity);

				// don't try to break it if it has already been broken
				if (pSurface->IsBroken())
					return false;
			}
		}
		else if (pEntity->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
		{
			// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
			return false;
		}
	}

	IBreakableWithPropData* pBreakableInterface = dynamic_cast<IBreakableWithPropData*>(pEntity);
	if (pBreakableInterface != nullptr)
	{
		// bullets don't damage it - ignore
		if (pBreakableInterface->GetDmgModBullet() <= 0.0f)
			return false;
	}

	return true;
}

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/util_shared.cpp#L687
void CAutoWall::ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int fMask, ITraceFilter* pFilter, Trace_t* pTrace)
{
	Trace_t trace = { };
	float flSmallestFraction = pTrace->flFraction;

	const Ray_t ray(vecAbsStart, vecAbsEnd);

	for (int i = 1; i < I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr || !pEntity->IsAlive() || pEntity->IsDormant())
			continue;

		if (pFilter != nullptr && !pFilter->ShouldHitEntity(pEntity, fMask))
			continue;

		const ICollideable* pCollideable = pEntity->GetCollideable();

		if (pCollideable == nullptr)
			continue;

		// get bounding box
		const Vector vecMin = pCollideable->OBBMins();
		const Vector vecMax = pCollideable->OBBMaxs();

		// calculate world space center
		const Vector vecCenter = (vecMax + vecMin) * 0.5f;
		const Vector vecPosition = vecCenter + pEntity->GetOrigin();

		Vector vecTo = vecPosition - vecAbsStart;
		Vector vecDirection = vecAbsEnd - vecAbsStart;
		const float flLength = vecDirection.NormalizeInPlace();

		const float flRangeAlong = vecDirection.DotProduct(vecTo);
		float flRange = 0.0f;

		// calculate distance to ray
		if (flRangeAlong < 0.0f)
			// off start point
			flRange = -vecTo.Length();
		else if (flRangeAlong > flLength)
			// off end point
			flRange = -(vecPosition - vecAbsEnd).Length();
		else
			// within ray bounds
			flRange = (vecPosition - (vecDirection * flRangeAlong + vecAbsStart)).Length();

		if (flRange < 0.0f || flRange > 60.0f)
			continue;

		I::EngineTrace->ClipRayToEntity(ray, fMask | CONTENTS_HITBOX, pEntity, &trace);

		if (trace.flFraction < flSmallestFraction)
		{
			// we shortened the ray - save off the trace
			*pTrace = trace;
			flSmallestFraction = trace.flFraction;
		}
	}
}

bool CAutoWall::TraceToExit(Trace_t& enterTrace, Trace_t& exitTrace, Vector vecPosition, Vector vecDirection) const
{
	float flDistance = 0.0f;
	int iStartContents = 0;

	while (flDistance <= 90.0f)
	{
		// add extra distance to our ray
		flDistance += 4.0f;

		// multiply the direction vector to the distance so we go outwards, add our position to it
		Vector vecStart = vecPosition + vecDirection * flDistance;

		if (!iStartContents)
			iStartContents = I::EngineTrace->GetPointContents(vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		const int iCurrentContents = I::EngineTrace->GetPointContents(vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(iCurrentContents & MASK_SHOT_HULL) || (iCurrentContents & CONTENTS_HITBOX && iCurrentContents != iStartContents))
		{
			// setup our end position by deducting the direction by the extra added distance
			const Vector vecEnd = vecStart - (vecDirection * 4.0f);

			// trace ray to world
			Ray_t rayWorld(vecStart, vecEnd);
			I::EngineTrace->TraceRay(rayWorld, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			// check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.bStartSolid && exitTrace.surface.uFlags & SURF_HITBOX)
			{
				// trace ray to entity
				Ray_t ray(vecStart, vecPosition);
				CTraceFilter filter(exitTrace.pHitEntity);

				I::EngineTrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.bStartSolid)
				{
					vecStart = exitTrace.vecEnd;
					return true;
				}

				continue;
			}

			if (exitTrace.DidHit() && !exitTrace.bStartSolid)
			{
				if (IsBreakableEntity(enterTrace.pHitEntity) && IsBreakableEntity(exitTrace.pHitEntity))
					return true;

				if (enterTrace.surface.uFlags & SURF_NODRAW || (!(exitTrace.surface.uFlags & SURF_NODRAW) && exitTrace.plane.vecNormal.DotProduct(vecDirection) <= 1.0f))
				{
					const float flMultiplier = exitTrace.flFraction * 4.0f;
					vecStart -= vecDirection * flMultiplier;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.bStartSolid)
			{
				if (enterTrace.pHitEntity != nullptr && enterTrace.pHitEntity->GetIndex() != 0 && IsBreakableEntity(enterTrace.pHitEntity))
				{
					// did hit breakable non world entity
					exitTrace = enterTrace;
					exitTrace.vecEnd = vecStart + vecDirection;
					return true;
				}

				continue;
			}
		}
	}

	return false;
}

bool CAutoWall::HandleBulletPenetration(CBaseEntity* pLocal, CCSWeaponData* pWeaponData, surfacedata_t* pEnterSurfaceData, FireBulletData_t& data) const
{
	static CConVar* ff_damage_reduction_bullets = I::ConVar->FindVar(XorStr("ff_damage_reduction_bullets"));
	static CConVar* ff_damage_bullet_penetration = I::ConVar->FindVar(XorStr("ff_damage_bullet_penetration"));

	const float flReductionDamage = ff_damage_reduction_bullets->GetFloat();
	const float flPenetrateDamage = ff_damage_bullet_penetration->GetFloat();

	const MaterialHandle_t hEnterMaterial = pEnterSurfaceData->game.hMaterial;
	const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;
	const bool bIsSolidSurf = ((data.enterTrace.iContents >> 3) & CONTENTS_SOLID);
	const bool bIsLightSurf = ((data.enterTrace.surface.uFlags >> 7) & SURF_LIGHT);

	Trace_t exitTrace = { };

	if (data.iPenetrateCount <= 0 ||
		(!data.iPenetrateCount && !bIsLightSurf && !bIsSolidSurf && hEnterMaterial != CHAR_TEX_GRATE && hEnterMaterial != CHAR_TEX_GLASS) ||
		(pWeaponData->flPenetration <= 0.0f) ||
		(!TraceToExit(data.enterTrace, exitTrace, data.enterTrace.vecEnd, data.vecDirection) && !(I::EngineTrace->GetPointContents(data.enterTrace.vecEnd, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL)))
		return false;

	const surfacedata_t* pExitSurfaceData = I::PhysicsProps->GetSurfaceData(exitTrace.surface.nSurfaceProps);
	const MaterialHandle_t hExitMaterial = pExitSurfaceData->game.hMaterial;
	const float flExitPenetrationModifier = pExitSurfaceData->game.flPenetrationModifier;

	float flDamageLostModifier = 0.16f;
	float flPenetrationModifier = 0.0f;

	if (hEnterMaterial == CHAR_TEX_GRATE || hEnterMaterial == CHAR_TEX_GLASS)
	{
		flDamageLostModifier = 0.05f;
		flPenetrationModifier = 3.0f;
	}
	else if (bIsSolidSurf || bIsLightSurf)
	{
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = 1.0f;
	}
	else if (hEnterMaterial == CHAR_TEX_FLESH && (pLocal->GetTeam() == data.enterTrace.pHitEntity->GetTeam() && flReductionDamage == 0.0f))
	{
		if (flPenetrateDamage == 0.0f)
			return false;

		// shoot through teammates
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = flPenetrateDamage;
	}
	else
	{
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = (flEnterPenetrationModifier + flExitPenetrationModifier) * 0.5f;
	}

	if (hEnterMaterial == hExitMaterial)
	{
		if (hExitMaterial == CHAR_TEX_CARDBOARD || hExitMaterial == CHAR_TEX_WOOD)
			flPenetrationModifier = 3.0f;
		else if (hExitMaterial == CHAR_TEX_PLASTIC)
			flPenetrationModifier = 2.0f;
	}

	const float flTraceDistance = (exitTrace.vecEnd - data.enterTrace.vecEnd).LengthSqr();

	// penetration modifier
	const float flModifier = std::max(0.0f, 1.0f / flPenetrationModifier);

	// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	const float flLostDamage = (data.flCurrentDamage * flDamageLostModifier + std::max(0.0f, 3.75f / pWeaponData->flPenetration) * (flModifier * 3.0f)) + ((flModifier * flTraceDistance) / 24.0f);

	// did we loose too much damage?
	if (flLostDamage > data.flCurrentDamage)
		return false;

	// we can't use any of the damage that we've lost
	if (flLostDamage > 0.0f)
		data.flCurrentDamage -= flLostDamage;

	// do we still have enough damage to deal?
	if (data.flCurrentDamage < 1.0f)
		return false;

	data.vecPosition = exitTrace.vecEnd;
	--data.iPenetrateCount;
	return true;
}

bool CAutoWall::SimulateFireBullet(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, FireBulletData_t& data)
{
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeapon->GetItemDefinitionIndex());

	if (pWeaponData == nullptr)
		return false;

	float flMaxRange = pWeaponData->flRange;

	// the total number of surfaces any bullet can penetrate in a single flight is capped at 4
	data.iPenetrateCount = 4;
	// set our current damage to what our gun's initial damage reports it will do
	data.flCurrentDamage = static_cast<float>(pWeaponData->iDamage);

	float flTraceLenght = 0.0f;
	CTraceFilter filter(pLocal);

	while (data.iPenetrateCount > 0 && data.flCurrentDamage >= 1.0f)
	{
		// max bullet range
		flMaxRange -= flTraceLenght;

		// end position of bullet
		const Vector vecEnd = data.vecPosition + data.vecDirection * flMaxRange;

		Ray_t ray(data.vecPosition, vecEnd);
		I::EngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);

		// check for player hitboxes extending outside their collision bounds
		ClipTraceToPlayers(data.vecPosition, vecEnd + data.vecDirection * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);

		surfacedata_t* pEnterSurfaceData = I::PhysicsProps->GetSurfaceData(data.enterTrace.surface.nSurfaceProps);
		const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;

		// we didn't hit anything, stop tracing shoot
		if (data.enterTrace.flFraction == 1.0f)
			break;

		// calculate the damage based on the distance the bullet traveled
		flTraceLenght += data.enterTrace.flFraction * flMaxRange;
		data.flCurrentDamage *= std::powf(pWeaponData->flRangeModifier, flTraceLenght / MAX_DAMAGE);

		// check is actually can shoot through
		if (flTraceLenght > 3000.0f || flEnterPenetrationModifier < 0.1f)
			break;

		// check is can do damage
		if (data.enterTrace.iHitGroup != HITGROUP_GENERIC && data.enterTrace.iHitGroup != HITGROUP_GEAR && pLocal->IsEnemy(data.enterTrace.pHitEntity))
		{
			// we got target - scale damage
			ScaleDamage(data.enterTrace.iHitGroup, data.enterTrace.pHitEntity, pWeaponData->flArmorRatio, data.flCurrentDamage);
			return true;
		}

		// calling handlebulletpenetration here reduces our penetration ñounter, and if it returns true, we can't shoot through it
		if (!HandleBulletPenetration(pLocal, pWeaponData, pEnterSurfaceData, data))
			break;
	}

	return false;
}
