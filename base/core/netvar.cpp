#include "netvar.h"

// used: working path
#include "config.h"
// used: client interface
#include "interfaces.h"

constexpr std::array<std::string_view, DPT_SENDPROPTYPEMAX> arrPropTypes =
{
	XorStr("int"),
	XorStr("float"),
	XorStr("vector"),
	XorStr("vector2d"),
	XorStr("const char*"),
	XorStr("std::array"),
	XorStr("void*"),
	XorStr("std::int64_t")
};

bool CNetvarManager::Setup(std::string_view szDumpFileName)
{
	// clear values
	mapProps.clear();
	iStoredProps = 0;
	iStoredTables = 0;

	// get current time
	tm time{};
	const std::chrono::system_clock::time_point systemNow = std::chrono::system_clock::now();
	const std::time_t timeNow = std::chrono::system_clock::to_time_t(systemNow);
	localtime_s(&time, &timeNow);

	// format time
	std::string szTime = fmt::format(XorStr("[{:%d-%m-%Y %X}] "), time);

	#if _DEBUG
	// open our dump file to write in (here is not exception handle because dump is not critical)
	fsDumpFile.open(C::GetWorkingPath().append(szDumpFileName), std::ios::out | std::ios::trunc);

	if (fsDumpFile.good())
		// write current date, time and info
		fsDumpFile << szTime << XorStr("qo0 | netvars dump\n\n");
	#endif

	for (auto pClass = I::Client->GetAllClasses(); pClass != nullptr; pClass = pClass->pNext)
	{
		if (pClass->pRecvTable == nullptr)
			continue;

		StoreProps(pClass->pRecvTable, 0U, 0);
	}

	#if _DEBUG
	// close dump file
	fsDumpFile.close();
	#endif

	return !mapProps.empty();
}

void CNetvarManager::GrabOffsets()
{
	// @note: arranged in order similar to the tables!
	flFallVelocity =		mapProps[FNV1A::HashConst("DT_Local->m_flFallVelocity")].uOffset;
	viewPunchAngle =		mapProps[FNV1A::HashConst("DT_Local->m_viewPunchAngle")].uOffset;
	aimPunchAngle =			mapProps[FNV1A::HashConst("DT_Local->m_aimPunchAngle")].uOffset;
	flStepSize =			mapProps[FNV1A::HashConst("DT_Local->m_flStepSize")].uOffset;
	vecViewOffset =			mapProps[FNV1A::HashConst("DT_LocalPlayerExclusive->m_vecViewOffset[0]")].uOffset;
	flFriction =			mapProps[FNV1A::HashConst("DT_LocalPlayerExclusive->m_flFriction")].uOffset;
	nTickBase =				mapProps[FNV1A::HashConst("DT_LocalPlayerExclusive->m_nTickBase")].uOffset;
	nNextThinkTick =		mapProps[FNV1A::HashConst("DT_LocalPlayerExclusive->m_nNextThinkTick")].uOffset;
	vecVelocity =			mapProps[FNV1A::HashConst("DT_LocalPlayerExclusive->m_vecVelocity[0]")].uOffset;
	hConstraintEntity =		mapProps[FNV1A::HashConst("DT_LocalPlayerExclusive->m_hConstraintEntity")].uOffset;
	hGroundEntity =			mapProps[FNV1A::HashConst("DT_BasePlayer->m_hGroundEntity")].uOffset;
	iHealth =				mapProps[FNV1A::HashConst("DT_BasePlayer->m_iHealth")].uOffset;
	lifeState =				mapProps[FNV1A::HashConst("DT_BasePlayer->m_lifeState")].uOffset;
	flMaxspeed =			mapProps[FNV1A::HashConst("DT_BasePlayer->m_flMaxspeed")].uOffset;
	fFlags =				mapProps[FNV1A::HashConst("DT_BasePlayer->m_fFlags")].uOffset;
	iObserverMode =			mapProps[FNV1A::HashConst("DT_BasePlayer->m_iObserverMode")].uOffset;
	hObserverTarget =		mapProps[FNV1A::HashConst("DT_BasePlayer->m_hObserverTarget")].uOffset;
	hViewModel =			mapProps[FNV1A::HashConst("DT_BasePlayer->m_hViewModel[0]")].uOffset;
	szLastPlaceName =		mapProps[FNV1A::HashConst("DT_BasePlayer->m_szLastPlaceName")].uOffset;

	deadflag =				mapProps[FNV1A::HashConst("DT_PlayerState->deadflag")].uOffset;

	iShotsFired =			mapProps[FNV1A::HashConst("DT_CSLocalPlayerExclusive->m_iShotsFired")].uOffset;
	iAccount =				mapProps[FNV1A::HashConst("DT_CSPlayer->m_iAccount")].uOffset;
	totalHitsOnServer =		mapProps[FNV1A::HashConst("DT_CSPlayer->m_totalHitsOnServer")].uOffset;
	ArmorValue =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_ArmorValue")].uOffset;
	angEyeAngles =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_angEyeAngles")].uOffset;
	bIsDefusing =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_bIsDefusing")].uOffset;
	bIsScoped =				mapProps[FNV1A::HashConst("DT_CSPlayer->m_bIsScoped")].uOffset;
	bIsGrabbingHostage =	mapProps[FNV1A::HashConst("DT_CSPlayer->m_bIsGrabbingHostage")].uOffset;
	bIsRescuing =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_bIsRescuing")].uOffset;
	bHasHelmet =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_bHasHelmet")].uOffset;
	bHasHeavyArmor =		mapProps[FNV1A::HashConst("DT_CSPlayer->m_bHasHeavyArmor")].uOffset;
	bHasDefuser =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_bHasDefuser")].uOffset;
	iCrosshairId =			bHasDefuser + 0x5C;
	bGunGameImmunity =		mapProps[FNV1A::HashConst("DT_CSPlayer->m_bGunGameImmunity")].uOffset;
	bInBuyZone =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_bInBuyZone")].uOffset;
	flFlashMaxAlpha =		mapProps[FNV1A::HashConst("DT_CSPlayer->m_flFlashMaxAlpha")].uOffset;
	flFlashDuration =		mapProps[FNV1A::HashConst("DT_CSPlayer->m_flFlashDuration")].uOffset;
	iGlowIndex =			flFlashDuration + 0x18;
	flLowerBodyYawTarget =	mapProps[FNV1A::HashConst("DT_CSPlayer->m_flLowerBodyYawTarget")].uOffset;
	nSurvivalTeam =			mapProps[FNV1A::HashConst("DT_CSPlayer->m_nSurvivalTeam")].uOffset;

	flAnimTime =			mapProps[FNV1A::HashConst("DT_AnimTimeMustBeFirst->m_flAnimTime")].uOffset;

	flSimulationTime =		mapProps[FNV1A::HashConst("DT_BaseEntity->m_flSimulationTime")].uOffset;
	vecOrigin =				mapProps[FNV1A::HashConst("DT_BaseEntity->m_vecOrigin")].uOffset;
	angRotation =			mapProps[FNV1A::HashConst("DT_BaseEntity->m_angRotation")].uOffset;
	iTeamNum =				mapProps[FNV1A::HashConst("DT_BaseEntity->m_iTeamNum")].uOffset;
	hOwnerEntity =			mapProps[FNV1A::HashConst("DT_BaseEntity->m_hOwnerEntity")].uOffset;
	Collision =				mapProps[FNV1A::HashConst("DT_BaseEntity->m_Collision")].uOffset;
	CollisionGroup =		mapProps[FNV1A::HashConst("DT_BaseEntity->m_CollisionGroup")].uOffset;
	bSpotted =				mapProps[FNV1A::HashConst("DT_BaseEntity->m_bSpotted")].uOffset;

	flNextAttack =			mapProps[FNV1A::HashConst("DT_BCCLocalPlayerExclusive->m_flNextAttack")].uOffset;

	hActiveWeapon =			mapProps[FNV1A::HashConst("DT_BaseCombatCharacter->m_hActiveWeapon")].uOffset;
	hMyWeapons =			mapProps[FNV1A::HashConst("DT_BaseCombatCharacter->m_hMyWeapons")].uOffset;
	hMyWearables =			mapProps[FNV1A::HashConst("DT_BaseCombatCharacter->m_hMyWearables")].uOffset;

	flNextPrimaryAttack =	mapProps[FNV1A::HashConst("DT_LocalActiveWeaponData->m_flNextPrimaryAttack")].uOffset;
	flNextSecondaryAttack =	mapProps[FNV1A::HashConst("DT_LocalActiveWeaponData->m_flNextSecondaryAttack")].uOffset;

	iClip1 =				mapProps[FNV1A::HashConst("DT_BaseCombatWeapon->m_iClip1")].uOffset;
	iPrimaryReserveAmmoCount = mapProps[FNV1A::HashConst("DT_BaseCombatWeapon->m_iPrimaryReserveAmmoCount")].uOffset;
	iViewModelIndex =		mapProps[FNV1A::HashConst("DT_BaseCombatWeapon->m_iViewModelIndex")].uOffset;
	iWorldModelIndex =		mapProps[FNV1A::HashConst("DT_BaseCombatWeapon->m_iWorldModelIndex")].uOffset;
	hWeaponWorldModel =		mapProps[FNV1A::HashConst("DT_BaseCombatWeapon->m_hWeaponWorldModel")].uOffset;

	zoomLevel =				mapProps[FNV1A::HashConst("DT_WeaponCSBaseGun->m_zoomLevel")].uOffset;
	iBurstShotsRemaining =	mapProps[FNV1A::HashConst("DT_WeaponCSBaseGun->m_iBurstShotsRemaining")].uOffset;

	fAccuracyPenalty =		mapProps[FNV1A::HashConst("DT_WeaponCSBase->m_fAccuracyPenalty")].uOffset;
	bBurstMode =			mapProps[FNV1A::HashConst("DT_WeaponCSBase->m_bBurstMode")].uOffset;
	flPostponeFireReadyTime = mapProps[FNV1A::HashConst("DT_WeaponCSBase->m_flPostponeFireReadyTime")].uOffset;
	bReloadVisuallyComplete = mapProps[FNV1A::HashConst("DT_WeaponCSBase->m_bReloadVisuallyComplete")].uOffset;

	bPinPulled =			mapProps[FNV1A::HashConst("DT_BaseCSGrenade->m_bPinPulled")].uOffset;
	fThrowTime =			mapProps[FNV1A::HashConst("DT_BaseCSGrenade->m_fThrowTime")].uOffset;
	flThrowStrength =		mapProps[FNV1A::HashConst("DT_BaseCSGrenade->m_flThrowStrength")].uOffset;

	nExplodeEffectTickBegin = mapProps[FNV1A::HashConst("DT_BaseCSGrenadeProjectile->m_nExplodeEffectTickBegin")].uOffset;

	nSmokeEffectTickBegin =	mapProps[FNV1A::HashConst("DT_SmokeGrenadeProjectile->m_nSmokeEffectTickBegin")].uOffset;

	nFireEffectTickBegin =	mapProps[FNV1A::HashConst("DT_Inferno->m_nFireEffectTickBegin")].uOffset;

	flC4Blow =				mapProps[FNV1A::HashConst("DT_PlantedC4->m_flC4Blow")].uOffset;
	flDefuseCountDown =		mapProps[FNV1A::HashConst("DT_PlantedC4->m_flDefuseCountDown")].uOffset;
	flTimerLength =			mapProps[FNV1A::HashConst("DT_PlantedC4->m_flTimerLength")].uOffset;
	flDefuseLength =		mapProps[FNV1A::HashConst("DT_PlantedC4->m_flDefuseLength")].uOffset;
	bBombTicking =			mapProps[FNV1A::HashConst("DT_PlantedC4->m_bBombTicking")].uOffset;
	hBombDefuser =			mapProps[FNV1A::HashConst("DT_PlantedC4->m_hBombDefuser")].uOffset;
	bBombDefused =			mapProps[FNV1A::HashConst("DT_PlantedC4->m_bBombDefused")].uOffset;

	bIsBroken =				mapProps[FNV1A::HashConst("DT_BreakableSurface->m_bIsBroken")].uOffset;

	bUseCustomAutoExposureMin = mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_bUseCustomAutoExposureMin")].uOffset;
	bUseCustomAutoExposureMax = mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_bUseCustomAutoExposureMax")].uOffset;
	bUseCustomBloomScale =	mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_bUseCustomBloomScale")].uOffset;
	flCustomAutoExposureMin = mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_flCustomAutoExposureMin")].uOffset;
	flCustomAutoExposureMax = mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_flCustomAutoExposureMax")].uOffset;
	flCustomBloomScale =	mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_flCustomBloomScale")].uOffset;
	flCustomBloomScaleMinimum = mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_flCustomBloomScaleMinimum")].uOffset;
	flBloomExponent =		mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_flBloomExponent")].uOffset;
	flBloomSaturation =		mapProps[FNV1A::HashConst("DT_EnvTonemapController->m_flBloomSaturation")].uOffset;

	iPing =					mapProps[FNV1A::HashConst("DT_PlayerResource->m_iPing")].uOffset;

	iCompetitiveRanking =	mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_iCompetitiveRanking")].uOffset;
	iCompetitiveWins =		mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_iCompetitiveWins")].uOffset;
	nActiveCoinRank =		mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_nActiveCoinRank")].uOffset;
	nMusicID =				mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_nMusicID")].uOffset;
	nPersonaDataPublicLevel = mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_nPersonaDataPublicLevel")].uOffset;
	nPersonaDataPublicCommendsLeader = mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_nPersonaDataPublicCommendsLeader")].uOffset;
	nPersonaDataPublicCommendsTeacher = mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_nPersonaDataPublicCommendsTeacher")].uOffset;
	nPersonaDataPublicCommendsFriendly = mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_nPersonaDataPublicCommendsFriendly")].uOffset;
	szClan =				mapProps[FNV1A::HashConst("DT_CSPlayerResource->m_szClan")].uOffset;

	nSequence =				mapProps[FNV1A::HashConst("DT_BaseAnimating->m_nSequence")].uOffset;
	nForceBone =			mapProps[FNV1A::HashConst("DT_BaseAnimating->m_nForceBone")].uOffset;
	dwBoneMatrix =			nForceBone + 0x1C;
	nHitboxSet =			mapProps[FNV1A::HashConst("DT_BaseAnimating->m_nHitboxSet")].uOffset;
	flPoseParameter =		mapProps[FNV1A::HashConst("DT_BaseAnimating->m_flPoseParameter")].uOffset;
	bClientSideAnimation =	mapProps[FNV1A::HashConst("DT_BaseAnimating->m_bClientSideAnimation")].uOffset;

	flCycle =				mapProps[FNV1A::HashConst("DT_ServerAnimationData->m_flCycle")].uOffset;

	nModelIndex =			mapProps[FNV1A::HashConst("DT_BaseViewModel->m_nModelIndex")].uOffset;
	hOwner =				mapProps[FNV1A::HashConst("DT_BaseViewModel->m_hOwner")].uOffset;
	hWeapon =				mapProps[FNV1A::HashConst("DT_BaseViewModel->m_hWeapon")].uOffset;

	iItemDefinitionIndex =	mapProps[FNV1A::HashConst("DT_ScriptCreatedItem->m_iItemDefinitionIndex")].uOffset;
	iItemIDHigh =			mapProps[FNV1A::HashConst("DT_ScriptCreatedItem->m_iItemIDHigh")].uOffset;
	iItemIDLow =			mapProps[FNV1A::HashConst("DT_ScriptCreatedItem->m_iItemIDLow")].uOffset;
	iAccountID =			mapProps[FNV1A::HashConst("DT_ScriptCreatedItem->m_iAccountID")].uOffset;
	iEntityQuality =		mapProps[FNV1A::HashConst("DT_ScriptCreatedItem->m_iEntityQuality")].uOffset;
	szCustomName =			mapProps[FNV1A::HashConst("DT_ScriptCreatedItem->m_szCustomName")].uOffset;

	OriginalOwnerXuidLow =	mapProps[FNV1A::HashConst("DT_BaseAttributableItem->m_OriginalOwnerXuidLow")].uOffset;
	OriginalOwnerXuidHigh =	mapProps[FNV1A::HashConst("DT_BaseAttributableItem->m_OriginalOwnerXuidHigh")].uOffset;
	nFallbackPaintKit =		mapProps[FNV1A::HashConst("DT_BaseAttributableItem->m_nFallbackPaintKit")].uOffset;
	nFallbackSeed =			mapProps[FNV1A::HashConst("DT_BaseAttributableItem->m_nFallbackSeed")].uOffset;
	flFallbackWear =		mapProps[FNV1A::HashConst("DT_BaseAttributableItem->m_flFallbackWear")].uOffset;
	nFallbackStatTrak =		mapProps[FNV1A::HashConst("DT_BaseAttributableItem->m_nFallbackStatTrak")].uOffset;
}

void CNetvarManager::StoreProps(RecvTable_t* pRecvTable, const std::uintptr_t uOffset, int nDumpTabs)
{
	#if _DEBUG
	std::string szTable;

	for (int i = 0; i < nDumpTabs; i++)
		szTable.append(XorStr("\t"));

	if (fsDumpFile.good())
		fsDumpFile << szTable << XorStr("[") << pRecvTable->szNetTableName << XorStr("]\n");
	#endif

	for (int i = 0; i < pRecvTable->nProps; ++i)
	{
		const auto pCurrentProp = &pRecvTable->pProps[i];

		// base tables filter
		if (pCurrentProp == nullptr || isdigit(pCurrentProp->szVarName[0]))
			continue;

		// skip baseclass
		if (FNV1A::Hash(pCurrentProp->szVarName) == FNV1A::HashConst("baseclass"))
			continue;

		// has child table
		if (auto pChildTable = pCurrentProp->pDataTable; pChildTable != nullptr &&
			// has props
			pChildTable->nProps > 0 &&
			// first char is 'D' ("DT" - "DataTable")
			pChildTable->szNetTableName[0] == 'D' &&
			// type is data table
			pCurrentProp->iRecvType == ESendPropType::DPT_DATATABLE)
			// recursively get props in all child tables
			StoreProps(pChildTable, std::uintptr_t(pCurrentProp->iOffset + uOffset), nDumpTabs + 1);

		// make own netvar pushing format
		const FNV1A_t uHash = FNV1A::Hash(fmt::format(XorStr("{}->{}"), pRecvTable->szNetTableName, pCurrentProp->szVarName).c_str());
		const std::uintptr_t uTotalOffset = std::uintptr_t(pCurrentProp->iOffset + uOffset);

		// check if we not already grabbed property pointer and offset
		if (!mapProps[uHash].uOffset)
		{
			#if _DEBUG
			if (fsDumpFile.good())
				fsDumpFile << szTable << XorStr("\t") << arrPropTypes.at(pCurrentProp->iRecvType) << " " << pCurrentProp->szVarName << XorStr(" = 0x") << std::uppercase << std::hex << uTotalOffset << ";\n";
			#endif

			// write values to map entry
			mapProps[uHash] = { pCurrentProp, uTotalOffset };
			// count total stored props
			iStoredProps++;
		}
	}

	// count total stored tables
	iStoredTables++;
}

std::uintptr_t CNetvarManager::FindInDataMap(DataMap_t* pMap, FNV1A_t uFieldHash)
{
	while (pMap != nullptr)
	{
		for (int i = 0; i < pMap->nDataFields; i++)
		{
			if (pMap->pDataDesc[i].szFieldName == nullptr)
				continue;

			if (FNV1A::Hash(pMap->pDataDesc[i].szFieldName) == uFieldHash)
				return pMap->pDataDesc[i].iFieldOffset[TD_OFFSET_NORMAL];

			if (pMap->pDataDesc[i].iFieldType == FIELD_EMBEDDED)
			{
				if (pMap->pDataDesc[i].pTypeDescription != nullptr)
				{
					if (const auto uOffset = FindInDataMap(pMap->pDataDesc[i].pTypeDescription, uFieldHash); uOffset != 0U)
						return uOffset;
				}
			}
		}

		pMap = pMap->pBaseMap;
	}

	return 0U;
}
