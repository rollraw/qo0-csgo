#include "entity.h"

// used: vector/angle calculations
#include "../utilities/math.h"
// used: get localplayer, traceline, linegoesthroughsmoke
#include "../utilities.h"
// used: cliententitylist, modelinfo, engine, globals, mdlcache
#include "../core/interfaces.h"

#pragma region entity_baseentity
int CBaseEntity::GetSequenceActivity(int iSequence)
{
	studiohdr_t* pStudioHdr = I::ModelInfo->GetStudioModel(this->GetModel());
	if (pStudioHdr == nullptr)
		return -1;

	using GetSequenceActivityFn = int(__fastcall*)(void*, void*, int);
	static auto oGetSequenceActivity = (GetSequenceActivityFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 53 8B 5D 08 56 8B F1 83"))); // @xref: "Need to handle the activity %d\n"
	return oGetSequenceActivity(this, pStudioHdr, iSequence);
}

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	return I::ClientEntityList->Get<CBaseCombatWeapon>(this->GetActiveWeapon());
}

matrix3x4_t* CBaseEntity::GetBoneMatrix(bool bSingleBone, int nSingleBone)
{
	matrix3x4_t* matBoneToWorld = nullptr;

	if (const auto pBoneMatrix = *(matrix3x4_t**)((std::uintptr_t)this + CNetvarManager::Get().dwBoneMatrix); pBoneMatrix != nullptr)
	{
		if (bSingleBone && nSingleBone != BONE_INVALID)
			matBoneToWorld = *(matrix3x4_t**)((std::uintptr_t)pBoneMatrix + 0x30 * nSingleBone);
		else
		{
			if (const auto pModel = this->GetModel(); pModel != nullptr)
			{
				if (auto pStudioHdr = I::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
				{
					for (int i = 0; i < pStudioHdr->nBones; i++)
						memcpy(matBoneToWorld + i, pBoneMatrix + i, sizeof(matrix3x4_t));
				}
			}
		}
	}

	return matBoneToWorld;
}

Vector CBaseEntity::GetBonePosition(int iBone)
{
	std::array<matrix3x4_t, MAXSTUDIOBONES> arrBonesToWorld;

	if (this->SetupBones(arrBonesToWorld.data(), MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.f))
		return arrBonesToWorld.at(iBone).at(3);

	return Vector{ };
}

int CBaseEntity::GetBoneByHash(const FNV1A_t uBoneHash)
{
	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (auto pStudioHdr = I::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
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

Vector CBaseEntity::GetHitboxPosition(int iHitbox)
{
	if (iHitbox <= HITBOX_INVALID || iHitbox >= HITBOX_MAX)
		throw std::out_of_range(XorStr("given invalid hitbox index for gethitboxposition"));

	std::array<matrix3x4_t, MAXSTUDIOBONES> arrBonesToWorld;

	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (auto pStudioModel = I::ModelInfo->GetStudioModel(pModel); pStudioModel != nullptr)
		{
			if (auto pHitbox = pStudioModel->GetHitbox(iHitbox, 0); pHitbox != nullptr)
			{
				if (this->SetupBones(arrBonesToWorld.data(), MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f))
				{
					Vector vecMin = { }, vecMax = { };

					// get mins/maxs by bone
					vecMin = M::VectorTransform(pHitbox->vecBBMin, arrBonesToWorld.at(pHitbox->iBone));
					vecMax = M::VectorTransform(pHitbox->vecBBMax, arrBonesToWorld.at(pHitbox->iBone));

					// get center
					return (vecMin + vecMax) * 0.5f;
				}
			}
		}
	}

	return Vector{ };
}

Vector CBaseEntity::GetHitGroupPosition(int iHitGroup)
{
	std::array<matrix3x4_t, MAXSTUDIOBONES> arrBonesToWorld;

	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (auto pStudioModel = I::ModelInfo->GetStudioModel(pModel); pStudioModel != nullptr)
		{
			if (auto pHitboxSet = pStudioModel->GetHitboxSet(this->GetHitboxSet()); pHitboxSet != nullptr)
			{
				if (this->SetupBones(arrBonesToWorld.data(), MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f))
				{
					mstudiobbox_t* pHitbox = nullptr;
					for (int i = 0; i < pHitboxSet->nHitboxes; i++)
					{
						pHitbox = pHitboxSet->GetHitbox(i);
						if (pHitbox->iGroup == iHitGroup)
							break;
					}

					if (pHitbox != nullptr)
					{
						Vector vecMin{ }, vecMax{ };

						// get mins/maxs by bone
						vecMin = M::VectorTransform(pHitbox->vecBBMin, arrBonesToWorld.at(pHitbox->iBone));
						vecMax = M::VectorTransform(pHitbox->vecBBMax, arrBonesToWorld.at(pHitbox->iBone));

						// get center
						return (vecMin + vecMax) * 0.5f;
					}
				}
			}
		}
	}

	return Vector{ };
}

void CBaseEntity::ModifyEyePosition(CBasePlayerAnimState* pAnimState, Vector* vecPosition)
{
	if (I::Engine->IsHLTV() || I::Engine->IsPlayingDemo())
		return;

	if (pAnimState->bHitGroundAnimation && pAnimState->flDuckAmount != 0.f)
	{
		CBaseEntity* pBaseEntity = pAnimState->pEntity;

		if (pBaseEntity != nullptr && I::ClientEntityList->GetClientEntityFromHandle(pAnimState->pEntity->GetGroundEntity()))
		{
			Vector vecBonePos = pBaseEntity->GetBonePosition(pBaseEntity->GetBoneByHash(FNV1A::HashConst("head_0")));
			vecBonePos.z += 1.7f;

			if ((*vecPosition).z > vecBonePos.z)
			{
				float flFactor = 0.0f;
				float flDelta = (*vecPosition).z - vecBonePos.z;
				float flOffset = (flDelta - 4.0f) / 6.0f;

				if (flOffset >= 0.f)
					flFactor = std::min<float>(flOffset, 1.0f);

				(*vecPosition).z += ((vecBonePos.z - (*vecPosition).z) * (((flFactor * flFactor) * 3.0f) - (((flFactor * flFactor) * 2.0f) * flFactor)));
			}
		}
	}
}

int CBaseEntity::PostThink()
{
	// @ida postthink: 56 8B 35 ? ? ? ? 57 8B F9 8B CE 8B 06 FF 90 ? ? ? ? 8B 07

	using PostThinkVPhysicsFn = bool(__thiscall*)(CBaseEntity*);
	static auto oPostThinkVPhysics = (PostThinkVPhysicsFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9")));

	using SimulatePlayerSimulatedEntitiesFn = void(__thiscall*)(CBaseEntity*);
	static auto oSimulatePlayerSimulatedEntities = (SimulatePlayerSimulatedEntitiesFn)(MEM::FindPattern(CLIENT_DLL, XorStr("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 72")));

	// begin lock
	MEM::CallVFunc<void>(I::MDLCache, 33);

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
	// end lock
	return MEM::CallVFunc<int>(I::MDLCache, 34);
}

bool CBaseEntity::IsEnemy(CBaseEntity* pEntity)
{
	static CConVar* mp_teammates_are_enemies = I::ConVar->FindVar(XorStr("mp_teammates_are_enemies"));
	
	if (mp_teammates_are_enemies != nullptr && mp_teammates_are_enemies->GetBool() && this->GetTeam() == pEntity->GetTeam() && this != pEntity)
		return true;

	if (this->GetTeam() != pEntity->GetTeam())
		return true;

	return false;
}

bool CBaseEntity::IsTargetingLocal(CBaseEntity* pLocal)
{
	Vector vecForward = { };
	QAngle angView = this->GetEyeAngles();
	M::AngleVectors(angView, &vecForward);
	vecForward *= MAX_DISTANCE;

	Vector vecStart = this->GetEyePosition();
	Vector vecEnd = vecStart + vecForward;

	Trace_t trace;
	U::TraceLine(vecStart, vecEnd, MASK_SHOT, this, &trace);

	if (trace.pHitEntity == pLocal)
		return true;

	return false;
}

bool CBaseEntity::IsCanShoot(CBaseCombatWeapon* pWeapon)
{
	float flServerTime = TICKS_TO_TIME(this->GetTickBase());

	// check is have ammo
	if (pWeapon->GetAmmo() <= 0)
		return false;

	// is player ready to shoot
	if (this->GetNextAttack() > flServerTime)
		return false;

	short nDefinitionIndex = *pWeapon->GetItemDefinitionIndex();

	// check is weapon with burst mode
	if ((nDefinitionIndex == WEAPON_FAMAS || nDefinitionIndex == WEAPON_GLOCK) &&
		// check is burst mode
		pWeapon->GetBurstMode() && pWeapon->GetBurstShotsRemaining() > 0)
		return true;

	// is weapon ready to shoot
	if (pWeapon->GetNextPrimaryAttack() > flServerTime)
		return false;

	// check for revolver cocking ready
	if (nDefinitionIndex == WEAPON_REVOLVER && pWeapon->GetFireReadyTime() > flServerTime)
		return false;

	return true;
}

bool CBaseEntity::IsVisible(CBaseEntity* pEntity, const Vector& vecEnd, bool bSmokeCheck)
{
	const Vector vecStart = this->GetEyePosition();

	Ray_t ray;
	ray.Init(vecStart, vecEnd);

	Trace_t trace;
	CTraceFilterSkipTwoEntities filter(this, pEntity);
	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	// trace check
	if ((trace.IsVisible() || trace.pHitEntity == this) &&
		// smoke check
		!(bSmokeCheck && U::LineGoesThroughSmoke(vecStart, vecEnd)))
		return true;

	return false;
}
#pragma endregion

#pragma region entity_plantedc4
float CPlantedC4::GetTimer()
{
	float flBlowTime = *(float*)((std::uintptr_t)this + CNetvarManager::Get().flC4Blow);
	float flTimer = flBlowTime - I::Globals->flCurrentTime;
	return std::max<float>(0.f, flTimer);
}

float CPlantedC4::GetDefuseTimer()
{
	float flDefuseCountDown = *(float*)((std::uintptr_t)this + CNetvarManager::Get().flDefuseCountDown);
	return flDefuseCountDown - I::Globals->flCurrentTime;
}
#pragma endregion
