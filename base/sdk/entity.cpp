#include "entity.h"

// used: stringcompare
#include "../utilities/crt.h"
// used: game_mode, game_type, inferno_flame_lifetime, mp_teammates_are_enemies, sv_coaching_enabled
#include "../core/convar.h"
// used: vecbreakableentities
#include "../core/entitylistener.h"
// used: linegoesthroughsmoke
#include "../sdk.h"
// used: cbonemergecache
#include "bonemergecache.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface definitions
#include "interfaces/iglobalvars.h"
#include "interfaces/ibaseclientdll.h"
#include "interfaces/icliententitylist.h"
#include "interfaces/iengineclient.h"
#include "interfaces/imdlcache.h"
#include "interfaces/iweaponsystem.h"
#include "interfaces/ibreakablewithpropdata.h"

#pragma region entity_baseentity
int CBaseEntity::GetMaxHealth()
{
	// check for dangerzone @todo: find a proper way which game doing this
	if (this->IsPlayer() && CONVAR::game_type->GetInt() == GAMETYPE_FREEFORALL)
		return 120;

	// @ida: client.dll -> ["8B 80 ? ? ? ? FF D0 66 0F 6E C0 0F 5B C0 0F" + 0x2] / sizeof(std::uintptr_t)
	return CallVFunc<int, 123U>(this);
}

bool CBaseEntity::IsBreakable()
{
	// @ida IsBreakableEntity(): client.dll -> "55 8B EC 51 56 8B F1 85 F6 74 68 83"

	const int iHealth = static_cast<CCSPlayer*>(this)->GetHealth();

	// entity is already broken
	if (iHealth < 0 && this->GetMaxHealth() > 0)
		return true;

	const ENTITY::BreakableObject_t* pBreakableObject = nullptr;
	if (const Model_t* pModel = this->GetModel(); pModel != nullptr)
	{
		for (const ENTITY::BreakableObject_t& breakableObject : ENTITY::vecBreakableEntities)
		{
			if (CRT::StringCompare(pModel->szPathName, breakableObject.szModelPath) == 0)
			{
				pBreakableObject = &breakableObject;
				break;
			}
		}
	}

	// [side change] force pass indexes that game doesn't mark as breakable for some reason
	if (this->GetTakeDamage() != DAMAGE_YES && pBreakableObject == nullptr)
		return false;

	if (const int nCollisionGroup = this->GetCollisionGroup(); nCollisionGroup != COLLISION_GROUP_PUSHAWAY && nCollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && nCollisionGroup != COLLISION_GROUP_NONE)
		return false;

	if (iHealth > 200)
		return false;

#ifdef Q_NO_RTTI
	static RTTITypeDescriptor_t* pIMultiplayerPhysicsTD = MEM::FindClassTypeDescriptor(MEM::GetModuleBaseHandle(CLIENT_DLL), Q_XOR("IMultiplayerPhysics"));
	static RTTITypeDescriptor_t* pC_BaseEntityTD = MEM::FindClassTypeDescriptor(MEM::GetModuleBaseHandle(CLIENT_DLL), Q_XOR("C_BaseEntity"));

	const auto pPhysicsInterface = static_cast<IMultiplayerPhysics*>(MEM::fnRTDynamicCast(this, 0, pC_BaseEntityTD, pIMultiplayerPhysicsTD, false));
#else
	const auto pPhysicsInterface = dynamic_cast<IMultiplayerPhysics*>(this);
#endif

	if (pPhysicsInterface != nullptr)
	{
		if (pPhysicsInterface->GetMultiplayerPhysicsMode() != PHYSICS_MULTIPLAYER_SOLID)
			return false;
	}
	else
	{
		// [side change] used class name of breakable entities we've parsed instead of 'CBaseEntity::GetClassname()' because only those will correspond to server class names
		if (bool bIsFuncBreakableSurf; pBreakableObject != nullptr && ((bIsFuncBreakableSurf = (CRT::StringCompare(pBreakableObject->szClassName, Q_XOR("func_breakable_surf")) == 0)) || CRT::StringCompare(pBreakableObject->szClassName, Q_XOR("func_breakable")) == 0))
		{
			if (bIsFuncBreakableSurf)
			{
				// don't try to break it if it has already been broken
				if (const auto pSurface = static_cast<CBreakableSurface*>(this); pSurface->IsBroken())
					return false;
			}
		}
		else if (this->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
			// hostages and players use 'CONTENTS_PLAYERCLIP', so we can use it to ignore them
			return false;
	}

#ifdef Q_NO_RTTI
	static RTTITypeDescriptor_t* pIBreakableWithPropDataTD = MEM::FindClassTypeDescriptor(MEM::GetModuleBaseHandle(CLIENT_DLL), Q_XOR("IBreakableWithPropData"));
	const auto pBreakableInterface = static_cast<IBreakableWithPropData*>(MEM::fnRTDynamicCast(this, 0, pC_BaseEntityTD, pIBreakableWithPropDataTD, false));
#else
	const auto pBreakableInterface = dynamic_cast<IBreakableWithPropData*>(this);
#endif

	if (pBreakableInterface != nullptr)
	{
		// bullets don't damage it - ignore
		if (pBreakableInterface->GetDmgModBullet() <= 0.0f)
			return false;
	}

	return true;
}
#pragma endregion

#pragma region entity_baseanimating
[[nodiscard]] float CBaseAnimating::GetPoseParameter(const CStudioHdr* pStudioHdr, const int iParameter)
{
	// @ida CBaseAnimating::GetPoseParameter(): server.dll -> ABS["E8 ? ? ? ? 8B 44 24 10 8B CB" + 0x1]

	if (iParameter >= 0)
		return Studio_GetPoseParameter(pStudioHdr, iParameter, this->GetPoseParameterArray()[iParameter]);

	return 0.0f;
}

float CBaseAnimating::SetPoseParameter(const CStudioHdr* pStudioHdr, const int iParameter, float flValue)
{
	// @ida C_BaseAnimating::SetPoseParameter(): client.dll -> "55 8B EC 51 56 8B F1 0F 28 C2"
	// @ida CBaseAnimating::SetPoseParameter(): server.dll -> ABS["E8 ? ? ? ? D9 45 08" + 0x1]

	if (iParameter >= 0)
	{
		float flNewValue;
		flValue = Studio_SetPoseParameter(pStudioHdr, iParameter, flValue, flNewValue);
		this->GetPoseParameterArray()[iParameter] = flNewValue;
	}

	return flValue;
}

int CBaseAnimating::GetBoneByHash(const FNV1A_t uBoneHash)
{
	// @ida C_BaseAnimating::LookupBone(): client.dll -> "55 8B EC 53 56 8B F1 57 83 BE ? ? ? ? ? 75"

	if (const CStudioHdr* pStudioHdr = this->GetModelPtr(); pStudioHdr != nullptr)
	{
		for (int i = 0; i < pStudioHdr->GetBoneCount(); i++)
		{
			if (const mstudiobone_t* pBone = pStudioHdr->GetBone(i); pBone != nullptr && FNV1A::Hash(pBone->GetName()) == uBoneHash)
				return i;
		}
	}

	Q_ASSERT(false); // given invalid bone hash or model header is corrupted
	return BONE_INVALID;
}

Vector_t CBaseAnimating::GetBonePosition(const int nBoneIndex)
{
	// @ida C_BaseAnimating::GetBonePosition(): client.dll -> "55 8B EC 83 E4 F8 56 8B F1 57 83"

	if (const CStudioHdr* pStudioHdr = this->GetModelPtr(); pStudioHdr != nullptr && nBoneIndex >= 0 && nBoneIndex < pStudioHdr->GetBoneCount())
	{
		if (!this->IsBoneCacheValid())
			this->SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, I::Globals->flCurrentTime);

		const CBoneAccessor& boneAccessor = this->GetBoneAccessor();
		return boneAccessor.matBones[nBoneIndex].GetOrigin();
	}

	Q_ASSERT(false); // given invalid bone index or model header is corrupted
	return { };
}

Vector_t CBaseAnimating::GetHitboxPosition(const int nHitboxIndex)
{
	if (const CStudioHdr* pStudioHdr = this->GetModelPtr(); pStudioHdr != nullptr && nHitboxIndex > HITBOX_INVALID && nHitboxIndex < HITBOX_MAX)
	{
		if (const mstudiobbox_t* pHitbox = pStudioHdr->GetHitbox(nHitboxIndex, this->GetHitboxSet()); pHitbox != nullptr)
		{
			if (!IsBoneCacheValid())
				SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, I::Globals->flCurrentTime);

			const CBoneAccessor& boneAccessor = this->GetBoneAccessor();
			const Matrix3x4a_t& matBoneTransform = boneAccessor.matBones[pHitbox->iBone];

			Matrix3x4a_t matBoneOriented = pHitbox->angOffsetOrientation.ToMatrix();
			matBoneOriented = matBoneTransform.ConcatTransforms(matBoneOriented);

			const Vector_t vecHitboxMin = pHitbox->vecBBMin.Transform(matBoneOriented);
			const Vector_t vecHitboxMax = pHitbox->vecBBMax.Transform(matBoneOriented);

			// get center
			return (vecHitboxMin + vecHitboxMax) * 0.5f;
		}
	}

	Q_ASSERT(false); // given invalid hitbox index or model header is corrupted
	return { };
}

Vector_t CBaseAnimating::GetHitGroupPosition(const int iHitGroup)
{
	if (iHitGroup == HITGROUP_GENERIC || iHitGroup == HITGROUP_GEAR)
		return this->WorldSpaceCenter();

	if (const CStudioHdr* pStudioHdr = this->GetModelPtr(); pStudioHdr != nullptr && iHitGroup > HITGROUP_GENERIC && iHitGroup < HITGROUP_GEAR)
	{
		if (const mstudiohitboxset_t* pHitboxSet = pStudioHdr->GetHitboxSet(this->GetHitboxSet()); pHitboxSet != nullptr)
		{
			if (!this->IsBoneCacheValid())
				this->SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, I::Globals->flCurrentTime);

			for (int i = 0; i < pHitboxSet->nHitboxCount; i++)
			{
				const mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);

				// check is reached needed group
				if (pHitbox->iGroup == iHitGroup)
				{
					const CBoneAccessor& boneAccessor = this->GetBoneAccessor();
					const Matrix3x4a_t& matBoneTransform = boneAccessor.matBones[pHitbox->iBone];

					Matrix3x4a_t matBoneOriented = pHitbox->angOffsetOrientation.ToMatrix();
					matBoneOriented = matBoneTransform.ConcatTransforms(matBoneOriented);

					const Vector_t vecHitboxMin = pHitbox->vecBBMin.Transform(matBoneOriented);
					const Vector_t vecHitboxMax = pHitbox->vecBBMax.Transform(matBoneOriented);

					// get center
					return (vecHitboxMin + vecHitboxMax) * 0.5f;
				}
			}
		}
	}

	Q_ASSERT(false); // given invalid hit group or model header is corrupted
	return { };
}

void CBaseAnimating::GetSkeleton(CStudioHdr* pStudioHdr, Vector_t* arrBonesPosition, QuaternionAligned_t* arrBonesRotation, int nBoneMask)
{
	// @ida CBaseAnimating::GetSkeleton(): server.dll -> "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 83"

	CIKContext* pIKContext = this->GetIKContext();

	// [side change] allocating in stack instead of heap
	CBoneSetup boneSetup(pStudioHdr, nBoneMask, this->GetPoseParameterArray());
	boneSetup.InitPose(arrBonesPosition, arrBonesRotation);
	boneSetup.AccumulatePose(arrBonesPosition, arrBonesRotation, this->GetSequence(), this->GetCycle(), 1.0f, I::Globals->flCurrentTime, pIKContext);

	if (pIKContext != nullptr)
	{
		CIKContext* pAutoIK = new CIKContext;
		pAutoIK->Init(pStudioHdr, this->GetAbsAngles(), this->GetAbsOrigin(), I::Globals->flCurrentTime, 0, nBoneMask);
		boneSetup.CalcAutoplaySequences(arrBonesPosition, arrBonesRotation, I::Globals->flCurrentTime, pAutoIK);
		delete pAutoIK;
	}
	else
		boneSetup.CalcAutoplaySequences(arrBonesPosition, arrBonesRotation, I::Globals->flCurrentTime, nullptr);

	boneSetup.CalcBoneAdjustment(arrBonesPosition, arrBonesRotation, this->GetEncodedControllerArray());
}
#pragma endregion

#pragma region entity_baseanimatingoverlay
void CBaseAnimatingOverlay::GetSkeleton(CStudioHdr* pStudioHdr, BoneVector_t* arrBonesPosition, BoneQuaternionAligned_t* arrBonesRotation, int nBoneMask)
{
	// @ida CBaseAnimatingOverlay::GetSkeleton(): server.dll -> "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 8B 45 08 56 57 8B F9"

	CIKContext* pIKContext = this->GetIKContext();
	if (pIKContext == nullptr)
	{
		pIKContext = new CIKContext;
		pIKContext->Init(pStudioHdr, this->GetAbsAngles(), this->GetAbsOrigin(), I::Globals->flCurrentTime, 0, BONE_USED_BY_BONE_MERGE);

		this->GetIKContext() = pIKContext;
	}

	// [side change] allocating in stack instead of heap
	CBoneSetup boneSetup(pStudioHdr, nBoneMask, this->GetPoseParameterArray());
	boneSetup.InitPose(arrBonesPosition, arrBonesRotation);
	boneSetup.AccumulatePose(arrBonesPosition, arrBonesRotation, this->GetSequence(), this->GetCycle(), 1.0f, I::Globals->flCurrentTime, pIKContext);

	bool bDoWeaponSetup = false;
	CBaseWeaponWorldModel* pWeaponWorldModel = nullptr;

	// check if this is a player with a valid weapon
	if (this->IsPlayer())
	{
		if (CCSPlayer* pPlayer = static_cast<CCSPlayer*>(this); pPlayer != nullptr && pPlayer->IsUsingNewAnimState())
		{
			if (CBaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon(); pWeapon != nullptr)
			{
				// [side change] removed second 'GetModelPtr()' check and just checking for 'IsHoldPlayerAnimations()' return instead, added weapon's bone merge cache sanity check
				if (pWeaponWorldModel = I::ClientEntityList->Get<CBaseWeaponWorldModel>(pWeapon->GetWorldModelHandle()); pWeaponWorldModel != nullptr && pWeaponWorldModel->IsHoldPlayerAnimations() && pWeaponWorldModel->GetBoneMergeCache() != nullptr)
					// [side change] don't initialize weapon's bone merge cache because we don't rebuild 'C_BaseAnimating::SetupBones()', so it should be already initialized in 'C_BaseAnimating::BuildTransformations()' -> 'C_BaseAnimating::CalcBoneMerge()', otherwise it will cause memory leak
					bDoWeaponSetup = true;
			}
		}
	}

	CUtlVector<CAnimationLayer>& vecAnimationLayers = this->GetAnimationOverlays();

	// look for weapon, pull layer animations from it if/when they exist
	if (bDoWeaponSetup)
	{
		CStudioHdr* pWeaponStudioHdr = pWeaponWorldModel->GetModelPtr();

		// copy matching player pose parameters to weapon pose parameters
		CBoneMergeCache* pBoneMergeCache = pWeaponWorldModel->GetBoneMergeCache();
		pBoneMergeCache->MergeMatchingPoseParams();

		// build a temporary setup for the weapon
		CIKContext* pWeaponIK = new CIKContext;
		pWeaponIK->Init(pWeaponStudioHdr, this->GetAbsAngles(), this->GetAbsOrigin(), I::Globals->flCurrentTime, 0, BONE_USED_BY_BONE_MERGE);

		BoneVector_t arrWeaponPositions[MAXSTUDIOBONES];
		BoneQuaternionAligned_t arrWeaponRotations[MAXSTUDIOBONES];

		CBoneSetup weaponSetup(pWeaponStudioHdr, BONE_USED_BY_BONE_MERGE, pWeaponWorldModel->GetPoseParameterArray());
		weaponSetup.InitPose(arrWeaponPositions, arrWeaponRotations);

		for (int i = 0; i < vecAnimationLayers.Count(); i++)
		{
			CAnimationLayer* pLayer = &vecAnimationLayers[i];

			if (pLayer->nSequence <= 1 || pLayer->flWeight <= 0.0f)
				continue;

			this->UpdateDispatchLayer(pLayer, pWeaponStudioHdr, pLayer->nSequence); // @note: this assumes we call setupbones once per tick (as we should do and ensure)

			if (pLayer->nDispatchedDst <= 0 || pLayer->nDispatchedDst >= pStudioHdr->GetSequenceCount())
				boneSetup.AccumulatePose(arrBonesPosition, arrBonesRotation, pLayer->nSequence, pLayer->flCycle, pLayer->flWeight, I::Globals->flCurrentTime, pIKContext);
			else
			{
				// copy player bones to weapon setup bones
				pBoneMergeCache->CopyFromFollow(arrBonesPosition, arrBonesRotation, BONE_USED_BY_BONE_MERGE, arrWeaponPositions, arrWeaponRotations);

				// [side change] removed null check for 'pIkContext', because it will never happen
				// respect ik rules on archetypal sequence, even if we're not playing it
				mstudioseqdesc_t& sequenceDescription = pStudioHdr->GetSequenceDescription(pLayer->nSequence);
				pIKContext->AddDependencies(sequenceDescription, pLayer->nSequence, pLayer->flCycle, this->GetPoseParameterArray(), pLayer->flWeight);

				weaponSetup.AccumulatePose(arrWeaponPositions, arrWeaponRotations, pLayer->nDispatchedDst, pLayer->flCycle, pLayer->flWeight, I::Globals->flCurrentTime, pWeaponIK);
				pBoneMergeCache->CopyToFollow(arrWeaponPositions, arrWeaponRotations, BONE_USED_BY_BONE_MERGE, arrBonesPosition, arrBonesRotation);

				pWeaponIK->CopyTo(pIKContext, pBoneMergeCache->arrRawIndexMapping);
			}
		}

		delete pWeaponIK;
	}
	else
	{
		// [side change] moved whole ordering under check, changed initial/error value to minimize checks count
		int arrLayers[MAX_OVERLAYS];
		const int nLayersCount = vecAnimationLayers.Count();

		for (int i = 0; i < MAX_OVERLAYS; i++)
			arrLayers[i] = -1;

		// sort the layers
		for (int i = 0; i < nLayersCount; i++)
		{
			if (const CAnimationLayer& layer = vecAnimationLayers[i]; layer.flWeight > 0.0f && layer.nSequence != -1 && layer.iOrder >= 0 && layer.iOrder < nLayersCount)
				arrLayers[layer.iOrder] = i;
		}

		for (int i = 0; i < nLayersCount; i++)
		{
			if (const int nIndex = arrLayers[i]; nIndex != -1)
			{
				const CAnimationLayer& layer = vecAnimationLayers[nIndex];
				boneSetup.AccumulatePose(arrBonesPosition, arrBonesRotation, layer.nSequence, layer.flCycle, layer.flWeight, I::Globals->flCurrentTime, pIKContext);
			}
		}
	}

	// [side change] removed branch when 'pIKContext' is null, because it will never happen
	CIKContext* pAutoIK = new CIKContext;
	pAutoIK->Init(pStudioHdr, this->GetAbsAngles(), this->GetAbsOrigin(), I::Globals->flCurrentTime, 0, nBoneMask);
	boneSetup.CalcAutoplaySequences(arrBonesPosition, arrBonesRotation, I::Globals->flCurrentTime, pAutoIK);
	delete pAutoIK;

	boneSetup.CalcBoneAdjustment(arrBonesPosition, arrBonesRotation, this->GetEncodedControllerArray());
}
#pragma endregion

#pragma region entity_basecombatcharacter
CBaseCombatWeapon* CBaseCombatCharacter::GetActiveWeapon()
{
	return I::ClientEntityList->Get<CBaseCombatWeapon>(this->GetActiveWeaponHandle());
}
#pragma endregion

#pragma region entity_baseplayer
void CBasePlayer::PostThink()
{
	// @ida C_BasePlayer::PostThink(): client.dll -> "56 8B 35 ? ? ? ? 57 8B F9 8B CE 8B 06 FF 90 ? ? ? ? 8B 07"

	static auto fnPostThinkVPhysics = reinterpret_cast<bool(Q_THISCALL*)(CBaseEntity*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB")));
	static auto fnSimulatePlayerSimulatedEntities = reinterpret_cast<void(Q_THISCALL*)(CBaseEntity*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74")));

	I::MDLCache->BeginLock();

	if (this->IsAlive() || static_cast<CCSPlayer*>(this)->IsGhost())
	{
		this->UpdateCollisionBounds();

		/* skipped footsteps and weapons sound parts to prevent double sound */

		if (this->GetFlags() & FL_ONGROUND)
			this->GetLocalData()->GetFallVelocity() = 0.0f;

		if (this->GetSequence() == -1)
			this->SetSequence(0);

		this->StudioFrameAdvance();
		fnPostThinkVPhysics(this);
	}

	fnSimulatePlayerSimulatedEntities(this);

	I::MDLCache->EndLock();
}

int CBasePlayer::GetAssociatedTeam()
{
	// @ida C_BasePlayer::GetAssociatedTeamNumber(): client.dll -> "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 34 85 C0 74 44"

	const int nTeam = this->GetTeam();

	// check is coaching
	if (CONVAR::sv_coaching_enabled->GetBool() && nTeam == TEAM_SPECTATOR)
		return this->GetCoachingTeam();

	return nTeam;
}

bool CBasePlayer::IsOtherVisible(const CBasePlayer* pOtherPlayer, const Vector_t& vecEnd, const bool bSmokeCheck)
{
	const Vector_t vecStart = this->GetEyePosition();

	const Ray_t ray(vecStart, vecEnd);
	CTraceFilterSimple filter(this);

	Trace_t trace = { };
	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	// trace check
	if ((trace.IsVisible() || trace.pHitEntity == pOtherPlayer) &&
		// smoke check
		!(bSmokeCheck && SDK::LineGoesThroughSmoke(vecStart, vecEnd)))
		return true;

	return false;
}
#pragma endregion

#pragma region entity_csplayer
CCSPlayer* CCSPlayer::GetLocalPlayer()
{
	return I::ClientEntityList->Get<CCSPlayer>(I::Engine->GetLocalPlayer());
}

Vector_t CCSPlayer::GetWeaponShootPosition()
{
	// @ida C_CSPlayer::Weapon_ShootPos(): client.dll, server.dll -> "55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90"

	Vector_t vecPosition = this->GetEyePosition();

	if (this->IsUsingNewAnimState())
	{
		if (const CCSGOPlayerAnimState* pAnimationState = this->GetAnimationState(); pAnimationState != nullptr)
			pAnimationState->ModifyEyePosition(vecPosition);
	}

	return vecPosition;
}

bool CCSPlayer::IsArmored(const int iHitGroup)
{
	// @ida CCSPlayer::IsArmored(): server.dll -> "55 8B EC 32 D2"

	bool bIsArmored = false;

	if (this->GetArmor() > 0)
	{
		switch (iHitGroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
		case HITGROUP_NECK:
			bIsArmored = true;
			break;
		case HITGROUP_HEAD:
			if (this->HasHelmet())
				bIsArmored = true;
			[[fallthrough]];
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			if (this->HasHeavyArmor())
				bIsArmored = true;
			break;
		default:
			break;
		}
	}

	return bIsArmored;
}

bool CCSPlayer::IsOtherEnemy(CCSPlayer* pOtherPlayer)
{
	// @source: master/game/shared/cstrike15/cs_player_shared.cpp#L682
	// @ida C_CSPlayer::IsOtherEnemy(): client.dll -> "55 8B EC 53 57 8B 7D 08 8B D9 80"

	// check are other player is invalid or we're comparing against ourselves
	if (pOtherPlayer == nullptr || this == pOtherPlayer)
		return false;

	// check is dangerzone
	if (CONVAR::game_type->GetInt() == GAMETYPE_FREEFORALL && CONVAR::game_mode->GetInt() == GAMEMODE_FREEFORALL_SURVIVAL)
		// check is not teammate
		return (this->GetSurvivalTeam() != pOtherPlayer->GetSurvivalTeam());

	// check is deathmatch
	if (CONVAR::mp_teammates_are_enemies->GetBool())
		return true;

	return (this->GetAssociatedTeam() != pOtherPlayer->GetAssociatedTeam());
}

bool CCSPlayer::CanAttack(const float flServerTime)
{
	// check is player ready to attack
	if (this->IsWaitForNoAttack() || this->GetNextAttack() > flServerTime)
		return false;

	return true;
}
#pragma endregion

#pragma region entity_weaponcsbasegun
bool CWeaponCSBaseGun::CanPrimaryAttack(const int nWeaponType, const float flServerTime)
{
	// check are weapon support burst mode and it's ready to attack
	if (this->IsBurstMode())
	{
		// check is it ready to attack
		if (this->GetBurstShotsRemaining() > 0 && this->GetNextBurstShotTime() <= flServerTime) // @todo: this shit doesnt work
			return true;
	}

	// check is weapon ready to attack
	if (this->GetNextPrimaryAttack() > flServerTime)
		return false;

	// we doesn't need additional checks for knives
	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	// check do weapon have ammo
	if (this->GetAmmo() <= 0)
		return false;

	const ItemDefinitionIndex_t nDefinitionIndex = this->GetEconItemView()->GetItemDefinitionIndex();

	// check is revolver cocking ready
	if (nDefinitionIndex == WEAPON_REVOLVER && this->GetPostponeFireReadyTime() > flServerTime)
		return false;

	return true;
}

bool CWeaponCSBaseGun::CanSecondaryAttack(const int nWeaponType, const float flServerTime)
{
	// check is weapon ready to attack
	if (this->GetNextSecondaryAttack() > flServerTime)
		return false;

	// we doesn't need additional checks for knives
	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	// check do weapon have ammo
	if (this->GetAmmo() <= 0)
		return false;

	// only revolver is allowed weapon for secondary attack
	if (this->GetEconItemView()->GetItemDefinitionIndex() != WEAPON_REVOLVER)
		return false;

	return true;
}
#pragma endregion

#pragma region entity_inferno
float CInferno::GetMaxTime()
{
	return CONVAR::inferno_flame_lifetime->GetFloat();
}
#pragma endregion
