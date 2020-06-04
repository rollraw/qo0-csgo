#include "autowall.h"

// used: angle/vector calculations
#include "../utilities/math.h"
// used: deathmatch check
#include "../core/variables.h"
// used: convar, cliententitylist, physics, trace, clients, globals interfaces
#include "../core/interfaces.h"

float CAutoWall::GetDamage(CBaseEntity* pLocal, const Vector& vecPoint, FireBulletData_t& dataOut)
{
	QAngle angView;
	Vector vecDirection;
	Vector vecDelta = vecPoint - pLocal->GetEyePosition();
	float flDamage = 0;

	// setup data
	FireBulletData_t data;
	data.vecPosition = pLocal->GetEyePosition();
	data.filter.pSkip = pLocal;

	M::VectorAngles(vecDelta, angView);
	M::AngleVectors(angView, &vecDirection);
	vecDirection.NormalizeInPlace();
	data.vecDir = vecDirection;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return -1.0f;

	if (SimulateFireBullet(pLocal, pWeapon, data))
		flDamage = data.flCurrentDamage;

	dataOut = data;
	return flDamage;
}

void CAutoWall::ScaleDamage(int iHitGroup, CBaseEntity* pEntity, float flWeaponArmorRatio, float& flDamage)
{
	bool bHeavyArmor = pEntity->HasHeavyArmor();
	int iArmor = pEntity->GetArmor();

	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
		flDamage *= bHeavyArmor ? 2.0f : 4.0f;
		break;
	case HITGROUP_STOMACH:
		flDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		flDamage *= 0.75f;
		break;
	}

	// check is armored
	if (iArmor > 0 && ((iHitGroup == HITGROUP_HEAD && pEntity->HasHelmet()) || (iHitGroup >= HITGROUP_GENERIC && iHitGroup <= HITGROUP_RIGHTARM)))
	{
		float flModifier = 1.0f, flArmorBonusRatio = 0.5f, flArmorRatio = flWeaponArmorRatio * 0.5f;

		if (bHeavyArmor)
		{
			flArmorBonusRatio = 0.33f;
			flArmorRatio *= 0.5f;
			flModifier = 0.33f;
		}

		float flNewDamage = flDamage * flArmorRatio;

		if (bHeavyArmor)
			flNewDamage *= 0.85f;

		if (((flDamage - flDamage * flArmorRatio) * (flModifier * flArmorBonusRatio)) > iArmor)
			flNewDamage = flDamage - iArmor / flArmorBonusRatio;

		flDamage = flNewDamage;
	}
}

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/util_shared.cpp#L687
void CAutoWall::ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int fMask, ITraceFilter* pFilter, Trace_t* pTrace)
{
	Trace_t trace = { };
	float flSmallestFraction = pTrace->flFraction;

	Ray_t ray = { };
	ray.Init(vecAbsStart, vecAbsEnd);

	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr || !pEntity->IsAlive() || pEntity->IsDormant())
			continue;

		if (pFilter != nullptr && pFilter->ShouldHitEntity(pEntity, fMask) == false)
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
		Vector vecDir = vecAbsEnd - vecAbsStart;
		float flLength = vecDir.NormalizeInPlace();

		const float flRangeAlong = vecDir.DotProduct(vecTo);
		float flRange = 0.0f;

		// calculate distance to ray
		// off start point
		if (flRangeAlong < 0.0f)
			flRange = -vecTo.Length();
		// off end point
		else if (flRangeAlong > flLength)
			flRange = -(vecPosition - vecAbsEnd).Length();
		// within ray bounds
		else
		{
			Vector vecRay = vecPosition - (vecDir * flRangeAlong + vecAbsStart);
			flRange = vecRay.Length();
		}

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

// tried to rebuild this but crashing with CBaseDoor???
bool CAutoWall::IsBreakableEntity(CBaseEntity* pEntity)
{
	if (pEntity == nullptr || pEntity->GetIndex() == 0)
		return false;

	// backup original takedamage value
	const int iOldTakeDamage = pEntity->GetTakeDamage();
	const CBaseClient* pClientClass = pEntity->GetClientClass();

	if (pClientClass == nullptr)
		return false;

	// force set DAMAGE_YES for certain breakable entities (as props, doors, etc)
	if (pClientClass->nClassID == EClassIndex::CBreakableSurface || pClientClass->nClassID == EClassIndex::CBaseDoor || pClientClass->nClassID == EClassIndex::CFuncBrush)
		pEntity->GetTakeDamage() = DAMAGE_YES;

	using IsBreakableEntityFn = bool(__thiscall*)(CBaseEntity*);
	static IsBreakableEntityFn oIsBreakableEntity = nullptr;

	if (oIsBreakableEntity == nullptr)
		oIsBreakableEntity = (IsBreakableEntityFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 51 56 8B F1 85 F6 74 68")));

	// restore original take damage
	pEntity->GetTakeDamage() = iOldTakeDamage;
	return oIsBreakableEntity(pEntity);
}

bool CAutoWall::TraceToExit(Trace_t* pEnterTrace, Trace_t* pExitTrace, Vector vecPosition, Vector vecDirection)
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

		if (!(iCurrentContents & MASK_SHOT_HULL) || iCurrentContents & CONTENTS_HITBOX && iCurrentContents != iStartContents)
		{
			// setup our end position by deducting the direction by the extra added distance
			const Vector vecEnd = vecStart - (vecDirection * 4.0f);
			// cast a ray from our start pos to the end pos
			U::TraceLine(vecStart, vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, pExitTrace);

			// check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (pExitTrace->bStartSolid && pExitTrace->surface.uFlags & SURF_HITBOX)
			{
				U::TraceLine(vecStart, vecPosition, MASK_SHOT_HULL, pExitTrace->pHitEntity, pExitTrace);

				if (pExitTrace->DidHit() && !pExitTrace->bStartSolid)
				{
					vecStart = pExitTrace->vecEnd;
					return true;
				}

				continue;
			}

			if (pExitTrace->DidHit() && !pExitTrace->bStartSolid)
			{
				if (IsBreakableEntity(pEnterTrace->pHitEntity) && IsBreakableEntity(pExitTrace->pHitEntity))
					return true;

				if ((!(pExitTrace->surface.uFlags & SURF_NODRAW) || (pEnterTrace->surface.uFlags & SURF_NODRAW && pExitTrace->surface.uFlags & SURF_NODRAW)) && pExitTrace->plane.vecNormal.DotProduct(vecDirection) <= 1.0f)
				{
					const float flMultiplier = pExitTrace->flFraction * 4.0f;
					vecStart -= vecDirection * flMultiplier;
					return true;
				}

				continue;
			}

			if (!pExitTrace->DidHit() || pExitTrace->bStartSolid)
			{
				if (pEnterTrace->pHitEntity != nullptr && pEnterTrace->pHitEntity->GetIndex() != 0 && IsBreakableEntity(pEnterTrace->pHitEntity))
				{
					// did hit breakable non world entity
					pExitTrace = pEnterTrace;
					pExitTrace->vecEnd = vecStart + vecDirection;
					return true;
				}

				continue;
			}
		}
	}

	return false;
}

bool CAutoWall::HandleBulletPenetration(CBaseEntity* pLocal, surfacedata_t* pEnterSurfaceData, CCSWeaponData* pWeaponData, FireBulletData_t& data)
{
	static CConVar* ff_damage_reduction_bullets = I::ConVar->FindVar(XorStr("ff_damage_reduction_bullets"));
	static CConVar* ff_damage_bullet_penetration = I::ConVar->FindVar(XorStr("ff_damage_bullet_penetration"));

	const float flReductionDamage = ff_damage_reduction_bullets->GetFloat();
	const float flPenetrateDamage = ff_damage_bullet_penetration->GetFloat();

	const MaterialHandle_t hEnterMaterial = pEnterSurfaceData->game.hMaterial;
	const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;

	Trace_t exitTrace = { };
	if ((!data.iPenetrateCount &&
		!(data.enterTrace.surface.uFlags >> 7 & SURF_LIGHT) && !((data.enterTrace.iContents >> 3) & CONTENTS_SOLID) &&
		hEnterMaterial != CHAR_TEX_GRATE && hEnterMaterial != CHAR_TEX_GLASS) ||
		pWeaponData->flPenetration <= 0.0f ||
		(!TraceToExit(&data.enterTrace, &exitTrace, data.enterTrace.vecEnd, data.vecDir) && !(I::EngineTrace->GetPointContents(data.enterTrace.vecEnd, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL)))
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
	else if ((data.enterTrace.iContents >> 3) & CONTENTS_SOLID || (data.enterTrace.surface.uFlags >> 7) & SURF_LIGHT)
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
	const float flModifier = std::max<float>(0.0f, 1.0f / flPenetrationModifier);

	// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	const float flLostDamage = (data.flCurrentDamage * flDamageLostModifier + std::max<float>(0.0f, 3.75f / pWeaponData->flPenetration) * (flModifier * 3.0f)) + ((flModifier * flTraceDistance) / 24.0f);

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
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(*pWeapon->GetItemDefinitionIndex());

	if (pWeaponData == nullptr)
		return false;

	float flMaxRange = pWeaponData->flRange;
	float flTraceLenght = 0.0f;

	// the total number of surfaces any bullet can penetrate in a single flight is capped at 4
	data.enterTrace = { };
	data.iPenetrateCount = 4;
	data.flCurrentDamage = (float)pWeaponData->iDamage;

	Trace_t trace = { };
	CTraceFilterSkipEntity filter(pLocal);

	while (data.iPenetrateCount > 0 && data.flCurrentDamage >= 1.0f)
	{
		// max bullet range
		flMaxRange -= flTraceLenght;

		// end position of bullet
		const Vector vecEnd = data.vecPosition + data.vecDir * flMaxRange;
		U::TraceLine(data.vecPosition, vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, pLocal, &data.enterTrace);

		// check for player hitboxes extending outside their collision bounds
		ClipTraceToPlayers(data.vecPosition, vecEnd + data.vecDir * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &data.filter, &data.enterTrace);

		surfacedata_t* pEnterSurfaceData = I::PhysicsProps->GetSurfaceData(data.enterTrace.surface.nSurfaceProps);
		float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;

		// we didn't hit anything, stop tracing shoot
		if (data.enterTrace.flFraction == 1.0f)
			break;

		// calculate the damage based on the distance the bullet travelled
		flTraceLenght += data.enterTrace.flFraction * flMaxRange;
		data.flCurrentDamage *= std::powf(pWeaponData->flRangeModifier, flTraceLenght / MAX_DAMAGE);

		// check is actually can shoot through
		if (flTraceLenght > 3000.0f || flEnterPenetrationModifier < 0.1f)
			break;

		// check is can do damage
		if (data.enterTrace.iHitGroup > HITGROUP_GENERIC && data.enterTrace.iHitGroup <= HITGROUP_RIGHTLEG && pLocal->IsEnemy(data.enterTrace.pHitEntity))
		{
			// we got target - scale damage
			ScaleDamage(data.enterTrace.iHitGroup, data.enterTrace.pHitEntity, pWeaponData->flArmorRatio, data.flCurrentDamage);
			return true;
		}

		// calling handlebulletpenetration here reduces our penetration ñounter, and if it returns true, we can't shoot through it
		if (!HandleBulletPenetration(pLocal, pEnterSurfaceData, pWeaponData, data))
			break;
	}

	return false;
}
