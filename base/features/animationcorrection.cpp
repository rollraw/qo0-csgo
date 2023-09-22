#include "animationcorrection.h"

// used: memorycopy
#include "../utilities/crt.h"
// used: getclientangles, getserverangles
#include "../features.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/iclientstate.h"
#include "../sdk/interfaces/icliententitylist.h"
#include "../sdk/interfaces/ivdebugoverlay.h"

#include "../utilities/inputsystem.h"
#include "../core/variables.h"

using namespace F;

struct AnimationUpdateObject_t
{
	// last spawn time index, used to detect player re-spawn and reset needed data
	float flSpawnTimeIndex = 0.0f;
	// cache of animation layers since last animation update with server data
	CAnimationLayer arrAnimationLayers[MAX_OVERLAYS] = { };
	// cache of pose parameters since last animation update with server data
	float arrPoseParameters[MAXSTUDIOPOSEPARAM] = { };
	// bones built with adjusted server-side data, origins should be always stored in the local space
	Matrix3x4a_t arrBonesToLocal[MAXSTUDIOBONES] = { };
	// state of game setup bones prohibition for this player
	bool bDisableSetupBones = false;
	// state of game animation update prohibition for this player
	bool bDisableAnimationUpdate = false;
	// state of bones origins space, used for debugging
	bool bIsBonesInLocalSpace = false;
};

// separate animation state for local player animation update based on client-side data
static CCSGOPlayerAnimState* pClientAnimationState = nullptr;
// bones of local player built with adjusted client-side data, origins should be always stored in the local space
static Matrix3x4a_t arrClientBonesToLocal[MAXSTUDIOBONES] = { };
// state of local player client-side bones origins space, used for debugging
static bool bIsClientBonesInLocalSpace = false;
// cache of all players animations update with adjusted server-side data
static AnimationUpdateObject_t arrUpdatePlayers[MAX_PLAYERS] = { };
// tick count of the last animation update
static int nLastServerUpdateTick = 0;

static void DrawServerHitboxes()
{
	static auto fnUTIL_GetPlayerByIndex = reinterpret_cast<void*(Q_FASTCALL*)(int)>(MEM::FindPattern(SERVER_DLL, "85 C9 7E 32 A1"));

	if (fnUTIL_GetPlayerByIndex == nullptr)
		return;

	static auto fnDrawServerHitboxes = reinterpret_cast<void(Q_FASTCALL*)(void*, float, bool)>(MEM::FindPattern(SERVER_DLL, "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE"));

	if (fnDrawServerHitboxes == nullptr)
		return;

	void* pLocal = fnUTIL_GetPlayerByIndex(I::Engine->GetLocalPlayer());
	float flDuration = -1.0f;

	if (pLocal == nullptr)
		return;

#ifdef Q_COMPILER_CLANG
	fnDrawServerHitboxes(pLocal, flDuration, false);
#else
	__asm
	{
		push 0 // 0 - color, 1 - monochrome (blue)
		movss xmm1, flDuration
		mov ecx, pLocal
		call fnDrawServerHitboxes
	}
#endif
}

static void DrawClientHitboxes()
{
	CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer();

	const CStudioHdr* pStudioHdr = pLocal->GetModelPtr();
	if (pStudioHdr == nullptr)
		return;

	const mstudiohitboxset_t* pHitboxSet = pStudioHdr->GetHitboxSet(pLocal->GetHitboxSet());
	if (pHitboxSet == nullptr)
		return;

	constexpr int r = 180, g = 85, b = 0;

	if (!pLocal->IsBoneCacheValid())
		pLocal->SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, I::Globals->flCurrentTime);

	const CBoneAccessor& boneAccessor = pLocal->GetBoneAccessor();

	for (int i = 0; i < pHitboxSet->nHitboxCount; i++)
	{
		const mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);
		const Matrix3x4a_t& matBoneTransform = boneAccessor.matBones[pHitbox->iBone];

		Matrix3x4a_t matBoneOriented = pHitbox->angOffsetOrientation.ToMatrix();
		matBoneOriented = matBoneTransform.ConcatTransforms(matBoneOriented);

		if (pHitbox->flCapsuleRadius > 0.0f)
		{
			const Vector_t vecHitboxMin = pHitbox->vecBBMin.Transform(matBoneOriented);
			const Vector_t vecHitboxMax = pHitbox->vecBBMax.Transform(matBoneOriented);
			I::DebugOverlay->AddCapsuleOverlay(vecHitboxMin, vecHitboxMax, pHitbox->flCapsuleRadius, r, g, b, 255, -1.0f);
		}
		else
		{
			const QAngle_t angRotation = matBoneOriented.ToAngles();
			I::DebugOverlay->AddBoxOverlay(matBoneOriented.GetOrigin(), pHitbox->vecBBMin, pHitbox->vecBBMax, angRotation, r, g, b, 0, -1.0f);
		}
	}
}

/*
 * update player animations when client receives data from the server and cache it once per tick
 * @note: you should also collect and process the lag compensation info and adjust the data using it here
 */
static void UpdatePlayer(CCSPlayer* pPlayer, AnimationUpdateObject_t& updateObject)
{
	// get the active animation state of the player
	CCSGOPlayerAnimState* pAnimationState = pPlayer->GetAnimationState();

	if (pAnimationState == nullptr)
		return;

	// current player origin that being used to convert bone matrices to local space
	const Vector_t& vecAbsOrigin = pPlayer->GetAbsOrigin();

	// get the active animation layers and pose parameters of the player
	CUtlVector<CAnimationLayer>& vecAnimationLayers = pPlayer->GetAnimationOverlays();
	const int nAnimationLayersCount = vecAnimationLayers.Count();
	float (&arrPoseParameters)[MAXSTUDIOPOSEPARAM] = pPlayer->GetPoseParameterArray();

	// check do we need to reset (on respawn)
	if (const float flSpawnTimeIndex = pPlayer->GetLastSpawnTimeIndex(); flSpawnTimeIndex != updateObject.flSpawnTimeIndex)
	{
		pAnimationState->Reset();

		CRT::MemoryCopy(updateObject.arrAnimationLayers, vecAnimationLayers.Base(), sizeof(CAnimationLayer) * nAnimationLayersCount);
		CRT::MemoryCopy(updateObject.arrPoseParameters, arrPoseParameters, sizeof(float[MAXSTUDIOPOSEPARAM]));

		updateObject.flSpawnTimeIndex = flSpawnTimeIndex;
	}

	// save original time measurement values
	const float flOldCurrentTime = I::Globals->flCurrentTime;
	const float flOldFrameTime = I::Globals->flFrameTime;

	// time when this player received update from server on client
	const float flSimulationTime = pPlayer->GetSimulationTime();

	// perform update when client had just received data from the server
	if (nLastServerUpdateTick != I::Globals->nTickCount && flSimulationTime != pPlayer->GetOldSimulationTime())
	{
		// make sure it will guaranteed process update whenever we want
		// can only happen on new game / inject a cheat while in game
		if (pAnimationState->nLastUpdateFrame == I::Globals->nFrameCount)
		{
			pAnimationState->nLastUpdateFrame -= 1;
			pAnimationState->flLastUpdateTime -= I::Globals->flIntervalPerTick;
		}

		// @todo: recheck everything and if it's not used somewhere in those calls, just set this directly through 'SetupBones()' call argument, since it's rebuilt now, it should work just fine
		// force server update based time measurement values
		I::Globals->flCurrentTime = flSimulationTime;
		I::Globals->flFrameTime = I::Globals->flIntervalPerTick;

		// force 'C_CSPlayer::UpdateClientSideAnimation()' to call 'CCSGOPlayerAnimState::Update()'
		const bool bOldIsClientSideAnimation = pPlayer->IsClientSideAnimation();
		pPlayer->IsClientSideAnimation() = true;

		updateObject.bDisableAnimationUpdate = false;
		pPlayer->UpdateClientSideAnimation();
		updateObject.bDisableAnimationUpdate = true;

		// restore original values
		pPlayer->IsClientSideAnimation() = bOldIsClientSideAnimation;

		// force game to rebuild bones
		pPlayer->InvalidateBoneCache();
		updateObject.bDisableSetupBones = false;
		pPlayer->SetupBones(updateObject.arrBonesToLocal, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, flSimulationTime);
		updateObject.bDisableSetupBones = true;

		updateObject.bIsBonesInLocalSpace = false;
		ANIMATION::ConvertBonesPositionToLocalSpace(updateObject.arrBonesToLocal, pPlayer->GetIndex(), vecAbsOrigin);

		// store cache of server data
		CRT::MemoryCopy(updateObject.arrAnimationLayers, vecAnimationLayers.Base(), sizeof(CAnimationLayer) * nAnimationLayersCount);
		CRT::MemoryCopy(updateObject.arrPoseParameters, arrPoseParameters, sizeof(float[MAXSTUDIOPOSEPARAM]));
	}

	// force last data that client received from server to line up hitboxes
	CRT::MemoryCopy(vecAnimationLayers.Base(), updateObject.arrAnimationLayers, sizeof(CAnimationLayer) * nAnimationLayersCount);
	CRT::MemoryCopy(arrPoseParameters, updateObject.arrPoseParameters, sizeof(float[MAXSTUDIOPOSEPARAM]));

	// restore original time measurement values
	I::Globals->flCurrentTime = flOldCurrentTime;
	I::Globals->flFrameTime = flOldFrameTime;
}

/*
 * update local player animations when client receives data from the server and cache it once per tick,
 * local player is handled different way to other players, because we also need to emulate adjusted client-side animations and store both data
 */
static void UpdateLocal(CCSPlayer* pLocal, AnimationUpdateObject_t& updateObject)
{
	// get the active animation state of the player
	CCSGOPlayerAnimState* pAnimationState = pLocal->GetAnimationState();

	if (pAnimationState == nullptr)
		return;

	// current local player origin that being used to convert bone matrices to local space
	const Vector_t& vecAbsOrigin = pLocal->GetAbsOrigin();

	// get the active animation layers and pose parameters of the player
	CUtlVector<CAnimationLayer>& vecAnimationLayers = pLocal->GetAnimationOverlays();
	const int nAnimationLayersCount = vecAnimationLayers.Count();
	float (&arrPoseParameters)[MAXSTUDIOPOSEPARAM] = pLocal->GetPoseParameterArray();

	// check do we need to reset (on respawn)
	if (const float flSpawnTimeIndex = pLocal->GetLastSpawnTimeIndex(); flSpawnTimeIndex != updateObject.flSpawnTimeIndex)
	{
		// re-copy client-side animation state, source has been already initialized by the game
		CRT::MemoryCopy(pClientAnimationState, pAnimationState, sizeof(CCSGOPlayerAnimState));
		Q_ASSERT(pClientAnimationState->pPlayer == pLocal); // should never trigger, animation state player is different to local

		CRT::MemoryCopy(updateObject.arrAnimationLayers, vecAnimationLayers.Base(), sizeof(CAnimationLayer) * nAnimationLayersCount);
		CRT::MemoryCopy(updateObject.arrPoseParameters, arrPoseParameters, sizeof(float[MAXSTUDIOPOSEPARAM]));

		updateObject.flSpawnTimeIndex = flSpawnTimeIndex;
	}

	/*
	 * now we're ready to update our animations
	 * update order of the server/client animations doesn't matter
	 * but we should always force server data last because game gonna use it
	 */

	QAngle_t& angLocalView = pLocal->GetPlayerState()->GetViewAngles();
	const QAngle_t angOldLocalView = angLocalView;

	// backup original time measurement values
	const float flOldCurrentTime = I::Globals->flCurrentTime;
	const float flOldFrameTime = I::Globals->flFrameTime;

	// get server based time measurement values
	const int iServerTickCount = pLocal->GetTickBase();
	const float flServerTime = TICKS_TO_TIME(iServerTickCount);

	// cached server rotation
	static float flServerFootYaw = 0.0f;

	// perform update only once per tick as server does
	if (nLastServerUpdateTick != I::Globals->nTickCount)
	{
		#pragma region animation_update_local_server
		// make sure it will guaranteed process update whenever we want
		// can only happen on new game / inject a cheat while in game
		if (pAnimationState->nLastUpdateFrame == I::Globals->nFrameCount)
		{
			pAnimationState->nLastUpdateFrame -= 1;
			pAnimationState->flLastUpdateTime -= I::Globals->flIntervalPerTick;
		}

		// @todo: recheck everything and if it's not used somewhere in those calls, just set this directly through 'SetupBones()' call argument, since it's rebuilt now, it should work just fine
		// force server based time measurement values
		I::Globals->flCurrentTime = flServerTime;
		I::Globals->flFrameTime = I::Globals->flIntervalPerTick;

		// perform animation update every frame
		#if 0
		/*
		 * if you're not lazy you can also rebuild it and then you doesn't need to force any of this shit (IsClientSideAnimation, GetViewAngles)
		 * note that you'll also never let the game to call 'C_CSPlayer::UpdateClientSideAnimation()' at least for the local player
		 */
		pAnimationState->Update(GetServerAngles());
		#else
		// force 'C_CSPlayer::UpdateClientSideAnimation()' to call 'CCSGOPlayerAnimState::Update()'
		const bool bOldIsClientSideAnimation = pLocal->IsClientSideAnimation();
		pLocal->IsClientSideAnimation() = true;

		// force 'C_BasePlayer::EyeAngles' to use our angles for 'CCSGOPlayerAnimState::Update()' and 'C_CSPlayer::AdjustBonesToBBox()' calls
		CPlayerState* pLocalPlayerState = pLocal->GetPlayerState();
		pLocalPlayerState->GetViewAngles() = GetServerAngles();

		updateObject.bDisableAnimationUpdate = false;
		pLocal->UpdateClientSideAnimation(); // @test: crash on InvalidatePhysicsRecursive on loss connection / check with fullupdate
		updateObject.bDisableAnimationUpdate = true;

		// restore original values
		pLocal->IsClientSideAnimation() = bOldIsClientSideAnimation;
		#endif

		/*
		 * predict next lower body yaw update time
		 * @ida: server.dll -> "F3 0F 11 86 ? ? ? ? F3 0F 10 01"
		 */
		// check is on ground
		if (pAnimationState->bOnGround)
		{
			// check is walking or leaving the ladder
			if (pAnimationState->flVelocityLength2D > 0.1f || std::fabsf(pAnimationState->flVelocityLengthZ) > 100.f)
				ANIMATION::flNextLowerBodyYawUpdateTime = flServerTime + 0.22f;
			// check is standing and lower body yaw had updated
			else if (flServerTime > ANIMATION::flNextLowerBodyYawUpdateTime && std::fabsf(std::remainderf(pAnimationState->flFootYaw - pAnimationState->flEyeYaw, 360.f)) > 35.f)
				ANIMATION::flNextLowerBodyYawUpdateTime = flServerTime + 1.10f;
		}

		// check that choke cycle reset and server received update
		if (I::ClientState->nChokedCommands == 0)
		{
			// force to rebuild bones
			pLocal->InvalidateBoneCache();
			updateObject.bDisableSetupBones = false;
			pLocal->SetupBones(updateObject.arrBonesToLocal, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, flServerTime);
			updateObject.bDisableSetupBones = true;

			updateObject.bIsBonesInLocalSpace = false;
			ANIMATION::ConvertBonesPositionToLocalSpace(updateObject.arrBonesToLocal, pLocal->GetIndex(), vecAbsOrigin);

			// store cache of server data
			flServerFootYaw = pAnimationState->flFootYaw;
			CRT::MemoryCopy(updateObject.arrAnimationLayers, vecAnimationLayers.Base(), sizeof(CAnimationLayer) * nAnimationLayersCount);
			CRT::MemoryCopy(updateObject.arrPoseParameters, arrPoseParameters, sizeof(float[MAXSTUDIOPOSEPARAM]));
		}

		// restore forced angles back to not affect other functions
		pLocalPlayerState->GetViewAngles() = angOldLocalView;
		#pragma endregion

		#pragma region animation_update_local_client
		const int iSimulationTick = iServerTickCount - I::ClientState->nChokedCommands + 1;
		const float flSimulationTime = TICKS_TO_TIME(iSimulationTick);

		// @todo: recheck everything and if it's not used somewhere in those calls, just set this directly through 'SetupBones()' call argument, since it's rebuilt now, it should work just fine
		// force client to use time measurements based on server's last choke tick (1 tick before update)
		I::Globals->flCurrentTime = flSimulationTime;

		// check that choke cycle reset and server received update
		if (I::ClientState->nChokedCommands == 0)
		{
			Q_ASSERT(pClientAnimationState->nLastUpdateFrame != I::Globals->nFrameCount); // should never trigger, otherwise you've broken something

			// force 'C_BasePlayer::EyeAngles()' to use our view angles for 'C_CSPlayer::AdjustBonesToBBox()' call
			angLocalView = GetClientAngles();

			pClientAnimationState->Update(GetClientAngles());

			// force to rebuild bones
			pLocal->InvalidateBoneCache();
			updateObject.bDisableSetupBones = false;
			pLocal->SetupBones(arrClientBonesToLocal, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, flSimulationTime);
			updateObject.bDisableSetupBones = true;

			bIsClientBonesInLocalSpace = false;
			ANIMATION::ConvertBonesPositionToLocalSpace(arrClientBonesToLocal, pLocal->GetIndex(), vecAbsOrigin);

			// restore forced angles back to not affect other functions
			pLocalPlayerState->GetViewAngles() = angOldLocalView;
		}
		#pragma endregion
	}

	// force last data that client received from server to line up hitboxes
	pLocal->SetAbsAngles(QAngle_t(0.0f, flServerFootYaw, 0.0f));
	CRT::MemoryCopy(vecAnimationLayers.Base(), updateObject.arrAnimationLayers, sizeof(CAnimationLayer) * nAnimationLayersCount);
	CRT::MemoryCopy(arrPoseParameters, updateObject.arrPoseParameters, sizeof(float[MAXSTUDIOPOSEPARAM]));

	// restore original time measurement values
	I::Globals->flCurrentTime = flOldCurrentTime;
	I::Globals->flFrameTime = flOldFrameTime;
}

#pragma region animationcorrection_callbacks
void ANIMATION::OnFrame(const EClientFrameStage nStage)
{
	if (nStage != FRAME_RENDER_START)
		return;

	const bool bIsHLTV = I::Engine->IsHLTV();
	const int nLocalPlayerIndex = I::Engine->GetLocalPlayer();

	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CCSPlayer* pPlayer = I::ClientEntityList->Get<CCSPlayer>(i);

		if (pPlayer == nullptr)
			continue;

		AnimationUpdateObject_t& updateObject = arrUpdatePlayers[i - 1];

		if (bIsHLTV || !pPlayer->IsAlive()) // @todo: quickfix for playdemo crashes, need a better way
		{
			updateObject.bDisableSetupBones = false;
			updateObject.bDisableAnimationUpdate = false;
			continue;
		}

		if (i == nLocalPlayerIndex)
		{
			if (pPlayer->ShouldDraw())
			{
				DrawServerHitboxes();
				DrawClientHitboxes();
			}

			UpdateLocal(pPlayer, updateObject);
			continue;
		}

		UpdatePlayer(pPlayer, updateObject);
	}

	nLastServerUpdateTick = I::Globals->nTickCount;
}

void ANIMATION::OnPlayerCreated(CCSPlayer* pPlayer)
{
	if (pPlayer->GetIndex() == I::Engine->GetLocalPlayer())
	{
		// allocate separate animation state, but don't initialize with constructor, because it will be overwritten by game's anyway
		pClientAnimationState = static_cast<CCSGOPlayerAnimState*>(CCSGOPlayerAnimState::operator new(sizeof(CCSGOPlayerAnimState)));
	}
}

void ANIMATION::OnPlayerDeleted(CCSPlayer* pPlayer)
{
	if (pClientAnimationState != nullptr && pPlayer->GetIndex() == I::Engine->GetLocalPlayer())
	{
		delete pClientAnimationState;
		pClientAnimationState = nullptr;
	}
}
#pragma endregion

#pragma region animationcorrection_get
bool ANIMATION::IsGameAllowedToUpdateAnimations(const int nPlayerIndex)
{
	return !arrUpdatePlayers[nPlayerIndex - 1].bDisableAnimationUpdate;
}

bool ANIMATION::IsGameAllowedToSetupBones(const int nPlayerIndex)
{
	return !arrUpdatePlayers[nPlayerIndex - 1].bDisableSetupBones;
}

std::add_lvalue_reference_t<Matrix3x4a_t[MAXSTUDIOBONES]> ANIMATION::GetClientBoneMatrices()
{
	return arrClientBonesToLocal;
}

std::add_lvalue_reference_t<Matrix3x4a_t[MAXSTUDIOBONES]> ANIMATION::GetPlayerBoneMatrices(const int nIndex)
{
	return arrUpdatePlayers[nIndex - 1].arrBonesToLocal;
}
#pragma endregion

#pragma region animationcorrection_extra
void ANIMATION::ConvertBonesPositionToLocalSpace(Matrix3x4a_t (&arrBonesToWorld)[MAXSTUDIOBONES], const int nPlayerIndex, const Vector_t& vecPlayerOrigin)
{
#ifdef _DEBUG
	const bool bIsClientBoneMatrices = (&arrBonesToWorld[0] == &arrClientBonesToLocal[0]);
	const bool bIsPlayerBoneMatrices = (&arrBonesToWorld[0] == &arrUpdatePlayers[nPlayerIndex - 1].arrBonesToLocal[0]);

	Q_ASSERT((!bIsClientBoneMatrices || !bIsClientBonesInLocalSpace) && (!bIsPlayerBoneMatrices || !arrUpdatePlayers[nPlayerIndex - 1].bIsBonesInLocalSpace)); // given bone matrices position are already in local space, this will lead to invalid position of the bones
#endif

	for (auto& matBoneToLocal : arrBonesToWorld)
	{
		matBoneToLocal[0][3] -= vecPlayerOrigin.x;
		matBoneToLocal[1][3] -= vecPlayerOrigin.y;
		matBoneToLocal[2][3] -= vecPlayerOrigin.z;
	}

#ifdef _DEBUG
	if (bIsClientBoneMatrices)
		bIsClientBonesInLocalSpace = true;
	else if (bIsPlayerBoneMatrices)
		arrUpdatePlayers[nPlayerIndex - 1].bIsBonesInLocalSpace = true;
#endif
}

void ANIMATION::ConvertBonesPositionToWorldSpace(Matrix3x4a_t (&arrBonesToLocal)[MAXSTUDIOBONES], const int nPlayerIndex, const Vector_t& vecPlayerOrigin)
{
#ifdef _DEBUG
	const bool bIsClientBoneMatrices = (&arrBonesToLocal[0] == &arrClientBonesToLocal[0]);
	const bool bIsPlayerBoneMatrices = (&arrBonesToLocal[0] == &arrUpdatePlayers[nPlayerIndex - 1].arrBonesToLocal[0]);

	Q_ASSERT((bIsClientBoneMatrices && bIsClientBonesInLocalSpace) || (bIsPlayerBoneMatrices && arrUpdatePlayers[nPlayerIndex - 1].bIsBonesInLocalSpace)); // given bone matrices position are already in world space, this will lead to invalid position of the bones
#endif

	for (auto& matBoneToWorld : arrBonesToLocal)
	{
		matBoneToWorld[0][3] += vecPlayerOrigin.x;
		matBoneToWorld[1][3] += vecPlayerOrigin.y;
		matBoneToWorld[2][3] += vecPlayerOrigin.z;
	}

#ifdef _DEBUG
	if (bIsClientBoneMatrices)
		bIsClientBonesInLocalSpace = false;
	else if (bIsPlayerBoneMatrices)
		arrUpdatePlayers[nPlayerIndex - 1].bIsBonesInLocalSpace = false;
#endif
}
#pragma endregion
