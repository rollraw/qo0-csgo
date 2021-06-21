#include "entity.h"

// used: vector/angle calculations
#include "../utilities/math.h"
// used: linegoesthroughsmoke
#include "../utilities.h"
// used: cliententitylist, modelinfo, engine, globals, mdlcache
#include "../core/interfaces.h"

#pragma region entity_baseentity
CBaseEntity* CBaseEntity::GetLocalPlayer()
{
	return I::ClientEntityList->Get<CBaseEntity>(I::Engine->GetLocalPlayer());
}

int CBaseEntity::GetSequenceActivity(int iSequence)
{
	studiohdr_t* pStudioHdr = I::ModelInfo->GetStudioModel(this->GetModel());

	if (pStudioHdr == nullptr)
		return -1;

	using GetSequenceActivityFn = int(__fastcall*)(void*, void*, int);
	static auto oGetSequenceActivity = reinterpret_cast<GetSequenceActivityFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 53 8B 5D 08 56 8B F1 83"))); // @xref: "Need to handle the activity %d\n"
	return oGetSequenceActivity(this, pStudioHdr, iSequence);
}

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	return I::ClientEntityList->Get<CBaseCombatWeapon>(this->GetActiveWeaponHandle());
}

int CBaseEntity::GetMaxHealth()
{
	// check is dangerzone
	if (I::GameTypes->GetCurrentGameType() == GAMETYPE_FREEFORALL)
		return 120;

	return 100;
}

std::optional<Vector> CBaseEntity::GetBonePosition(int iBone)
{
	assert(iBone > BONE_INVALID && iBone < MAXSTUDIOBONES); // given invalid bone index for getboneposition

	std::array<matrix3x4_t, MAXSTUDIOBONES> arrBonesToWorld = { };

	if (this->SetupBones(arrBonesToWorld.data(), arrBonesToWorld.size(), BONE_USED_BY_ANYTHING, 0.f))
		return arrBonesToWorld.at(iBone).at(3);

	return std::nullopt;
}

int CBaseEntity::GetBoneByHash(const FNV1A_t uBoneHash) const
{
	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = I::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			for (int i = 0; i < pStudioHdr->nBones; i++)
			{
				if (auto pBone = pStudioHdr->GetBone(i); pBone != nullptr && pBone->iFlags & BONE_USED_BY_HITBOX && FNV1A::Hash(pBone->GetName()) == uBoneHash)
					return i;
			}
		}
	}

	return BONE_INVALID;
}

std::optional<Vector> CBaseEntity::GetHitboxPosition(int iHitbox)
{
	assert(iHitbox > HITBOX_INVALID && iHitbox < HITBOX_MAX); // given invalid hitbox index for gethitboxposition

	std::array<matrix3x4_t, MAXSTUDIOBONES> arrBonesToWorld = { };

	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = I::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			if (const auto pHitbox = pStudioHdr->GetHitbox(iHitbox, 0); pHitbox != nullptr)
			{
				if (this->SetupBones(arrBonesToWorld.data(), arrBonesToWorld.size(), BONE_USED_BY_HITBOX, 0.f))
				{
					// get mins/maxs by bone
					const Vector vecMin = M::VectorTransform(pHitbox->vecBBMin, arrBonesToWorld.at(pHitbox->iBone));
					const Vector vecMax = M::VectorTransform(pHitbox->vecBBMax, arrBonesToWorld.at(pHitbox->iBone));

					// get center
					return (vecMin + vecMax) * 0.5f;
				}
			}
		}
	}

	return std::nullopt;
}

std::optional<Vector> CBaseEntity::GetHitGroupPosition(int iHitGroup)
{
	assert(iHitGroup >= HITGROUP_GENERIC && iHitGroup <= HITGROUP_GEAR); // given invalid hitbox index for gethitgroupposition

	std::array<matrix3x4_t, MAXSTUDIOBONES> arrBonesToWorld = { };

	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = I::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			if (const auto pHitboxSet = pStudioHdr->GetHitboxSet(this->GetHitboxSet()); pHitboxSet != nullptr)
			{
				if (this->SetupBones(arrBonesToWorld.data(), arrBonesToWorld.size(), BONE_USED_BY_HITBOX, 0.f))
				{
					mstudiobbox_t* pHitbox = nullptr;
					for (int i = 0; i < pHitboxSet->nHitboxes; i++)
					{
						pHitbox = pHitboxSet->GetHitbox(i);

						// check is reached needed group
						if (pHitbox->iGroup == iHitGroup)
							break;
					}

					if (pHitbox != nullptr)
					{
						// get mins/maxs by bone
						const Vector vecMin = M::VectorTransform(pHitbox->vecBBMin, arrBonesToWorld.at(pHitbox->iBone));
						const Vector vecMax = M::VectorTransform(pHitbox->vecBBMax, arrBonesToWorld.at(pHitbox->iBone));

						// get center
						return (vecMin + vecMax) * 0.5f;
					}
				}
			}
		}
	}

	return std::nullopt;
}

void CBaseEntity::ModifyEyePosition(CCSGOPlayerAnimState* pAnimState, Vector* vecPosition) const
{
	if (I::Engine->IsHLTV() || I::Engine->IsPlayingDemo())
		return;

	CBaseEntity* pBaseEntity = pAnimState->pEntity;

	if (pBaseEntity == nullptr)
		return;

	IClientEntity* pGroundEntity = I::ClientEntityList->GetClientEntityFromHandle(pBaseEntity->GetGroundEntityHandle());

	if (!pAnimState->bHitGroundAnimation && pAnimState->flDuckAmount == 0.f && pGroundEntity != nullptr)
		return;

	if (const auto headPosition = pBaseEntity->GetBonePosition(pBaseEntity->GetBoneByHash(FNV1A::HashConst("head_0"))); headPosition.has_value())
	{
		Vector vecBone = headPosition.value();
		vecBone.z += 1.7f;

		if (vecPosition->z > vecBone.z)
		{
			float flFactor = 0.f;
			const float flDelta = vecPosition->z - vecBone.z;
			const float flOffset = (flDelta - 4.0f) / 6.0f;

			if (flOffset >= 0.f)
				flFactor = std::min(flOffset, 1.0f);

			vecPosition->z += ((vecBone.z - vecPosition->z) * (((flFactor * flFactor) * 3.0f) - (((flFactor * flFactor) * 2.0f) * flFactor)));
		}
	}
}

void CBaseEntity::PostThink()
{
	// @ida postthink: 56 8B 35 ? ? ? ? 57 8B F9 8B CE 8B 06 FF 90 ? ? ? ? 8B 07

	using PostThinkVPhysicsFn = bool(__thiscall*)(CBaseEntity*);
	static auto oPostThinkVPhysics = reinterpret_cast<PostThinkVPhysicsFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB")));

	using SimulatePlayerSimulatedEntitiesFn = void(__thiscall*)(CBaseEntity*);
	static auto oSimulatePlayerSimulatedEntities = reinterpret_cast<SimulatePlayerSimulatedEntitiesFn>(MEM::FindPattern(CLIENT_DLL, XorStr("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74")));

	I::MDLCache->BeginLock();

	if (this->IsAlive())
	{
		this->UpdateCollisionBounds();

		if (this->GetFlags() & FL_ONGROUND)
			*this->GetFallVelocity() = 0.f;

		if (this->GetSequence() == -1)
			this->SetSequence(0);

		this->StudioFrameAdvance();
		oPostThinkVPhysics(this);
	}

	oSimulatePlayerSimulatedEntities(this);

	I::MDLCache->EndLock();
}

bool CBaseEntity::IsEnemy(CBaseEntity* pEntity)
{
	// check is dangerzone
	if (I::GameTypes->GetCurrentGameType() == GAMETYPE_FREEFORALL)
		// check is not teammate
		return (this->GetSurvivalTeam() != pEntity->GetSurvivalTeam());

	static CConVar* mp_teammates_are_enemies = I::ConVar->FindVar(XorStr("mp_teammates_are_enemies"));
	
	// check is deathmatch
	if (mp_teammates_are_enemies != nullptr && mp_teammates_are_enemies->GetBool() && this->GetTeam() == pEntity->GetTeam() && this != pEntity)
		return true;

	if (this->GetTeam() != pEntity->GetTeam())
		return true;

	return false;
}

bool CBaseEntity::IsTargetingLocal(CBaseEntity* pLocal)
{
	Vector vecForward = { };
	const QAngle angView = this->GetEyeAngles();
	M::AngleVectors(angView, &vecForward);
	vecForward *= MAX_DISTANCE;

	const Vector vecStart = this->GetEyePosition();
	const Vector vecEnd = vecStart + vecForward;

	const Ray_t ray(vecStart, vecEnd);
	CTraceFilter filter(this);

	Trace_t trace = { };
	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	if (trace.pHitEntity == pLocal)
		return true;

	return false;
}

bool CBaseEntity::CanShoot(CWeaponCSBase* pBaseWeapon)
{
	const float flServerTime = TICKS_TO_TIME(this->GetTickBase());

	// check is have ammo
	if (pBaseWeapon->GetAmmo() <= 0)
		return false;

	// is player ready to shoot
	if (this->GetNextAttack() > flServerTime)
		return false;

	const short nDefinitionIndex = pBaseWeapon->GetItemDefinitionIndex();

	// check is weapon with burst mode
	if ((nDefinitionIndex == WEAPON_FAMAS || nDefinitionIndex == WEAPON_GLOCK) &&
		// check is burst mode
		pBaseWeapon->IsBurstMode() && pBaseWeapon->GetBurstShotsRemaining() > 0)
		return true;

	// is weapon ready to shoot
	if (pBaseWeapon->GetNextPrimaryAttack() > flServerTime)
		return false;

	// check for revolver cocking ready
	if (nDefinitionIndex == WEAPON_REVOLVER && pBaseWeapon->GetFireReadyTime() > flServerTime)
		return false;

	return true;
}

bool CBaseEntity::IsVisible(CBaseEntity* pEntity, const Vector& vecEnd, bool bSmokeCheck)
{
	const Vector vecStart = this->GetEyePosition(false);

	const Ray_t ray(vecStart, vecEnd);
	CTraceFilter filter(this);

	Trace_t trace = { };
	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	// trace check
	if ((trace.IsVisible() || trace.pHitEntity == pEntity) &&
		// smoke check
		!(bSmokeCheck && U::LineGoesThroughSmoke(vecStart, vecEnd)))
		return true;

	return false;
}
#pragma endregion
