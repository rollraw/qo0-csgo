#pragma once
// used: std::map
#include <map>
// used: std::ifstream
#include <fstream>

// used: winapi includes
#include "../common.h"
// used: fnv1a hashing
#include "../sdk/hash/fnv1a.h"
// used: data table, prop, data type
#include "../sdk/datatypes/datatable.h"
// used: datamap struct
#include "../sdk/datatypes/datamap.h"

class CRecvPropHook
{
public:
	CRecvPropHook() = default;

	explicit CRecvPropHook(RecvProp_t* pRecvProp, const RecvVarProxyFn pNewProxyFn)
		: pRecvProp(pRecvProp), pOriginalFn(pRecvProp->oProxyFn)
	{
		SetProxy(pNewProxyFn);
	}

	/* restore original function */
	~CRecvPropHook()
	{
		this->pRecvProp->oProxyFn = this->pOriginalFn;
	}

	// Get
	/* replace with our function */
	void Replace(RecvProp_t* pRecvProp)
	{
		this->pRecvProp = pRecvProp;
		this->pOriginalFn = pRecvProp->oProxyFn;
	}

	const void SetProxy(const RecvVarProxyFn pNewProxyFn)
	{
		this->pRecvProp->oProxyFn = pNewProxyFn;
	}

	const RecvVarProxyFn GetOriginal()
	{
		return this->pOriginalFn;
	}

private:
	// Values
	RecvProp_t* pRecvProp; // in future that is being modified and replace the original prop
	RecvVarProxyFn pOriginalFn; // save current proxy function to get available restore it later
};

class CNetvarManager : public CSingleton<CNetvarManager>
{
public:
	struct NetvarObject_t
	{
		RecvProp_t* pRecvProp;
		std::uintptr_t uOffset;
	};

	// Get
	/* fill map with netvars and also dump it to given file */
	bool Setup(std::string_view szDumpFileName);
	/* get offsets of certain networked vars in certain tables */
	void GrabOffsets();
	/*
	 * stores the variables of objects in the hierarchy
	 * used to iterate through an object's data descriptions from data map
	 */
	std::uintptr_t FindInDataMap(DataMap_t* pMap, FNV1A_t uFieldHash);

	// Values
	/* logging counters */
	int iStoredProps = 0;
	int iStoredTables = 0;
	/* networkable properties map */
	std::map<FNV1A_t, NetvarObject_t> mapProps;

	/* netvars */
	// DT_BasePlayer
	std::uintptr_t
		flFallVelocity,
		viewPunchAngle,
		aimPunchAngle,
		vecViewOffset,
		nTickBase,
		nNextThinkTick,
		vecVelocity,
		hConstraintEntity,
		deadflag,
		hGroundEntity,
		iHealth,
		lifeState,
		flMaxspeed,
		fFlags,
		iObserverMode,
		hObserverTarget,
		hViewModel,
		szLastPlaceName;

	// DT_CSPlayer
	std::uintptr_t
		iShotsFired,
		iAccount,
		totalHitsOnServer,
		ArmorValue,
		angEyeAngles,
		bIsDefusing,
		bIsScoped,
		bIsGrabbingHostage,
		bIsRescuing,
		bHasHelmet,
		bHasHeavyArmor,
		bHasDefuser,
		iCrosshairId,
		bGunGameImmunity,
		bInBuyZone,
		flFriction,
		flStepSize,
		flFlashMaxAlpha,
		flFlashDuration,
		iGlowIndex,
		flLowerBodyYawTarget,
		nSurvivalTeam;

	// DT_AnimTimeMustBeFirst
	std::uintptr_t
		flAnimTime;

	// DT_BaseEntity
	std::uintptr_t
		flSimulationTime,
		vecOrigin,
		angRotation,
		iTeamNum,
		hOwnerEntity,
		Collision,
		CollisionGroup,
		bSpotted;

	// DT_BCCLocalPlayerExclusive
	std::uintptr_t
		flNextAttack;

	// DT_BaseCombatCharacter
	std::uintptr_t
		hActiveWeapon,
		hMyWeapons,
		hMyWearables;

	// DT_LocalActiveWeaponData
	std::uintptr_t
		flNextPrimaryAttack,
		flNextSecondaryAttack;

	// DT_BaseCombatWeapon
	std::uintptr_t
		iClip1,
		iPrimaryReserveAmmoCount,
		iViewModelIndex,
		iWorldModelIndex,
		hWeaponWorldModel;

	// DT_WeaponCSBaseGun
	std::uintptr_t
		zoomLevel,
		iBurstShotsRemaining;

	// DT_WeaponCSBase
	std::uintptr_t
		fAccuracyPenalty,
		bBurstMode,
		flPostponeFireReadyTime,
		bReloadVisuallyComplete;

	// DT_BaseCSGrenade
	std::uintptr_t
		bPinPulled,
		fThrowTime,
		flThrowStrength;

	// DT_BaseCSGrenadeProjectile
	std::uintptr_t
		nExplodeEffectTickBegin;

	// DT_SmokeGrenadeProjectile
	std::uintptr_t
		nSmokeEffectTickBegin;

	// DT_Inferno
	std::uintptr_t
		nFireEffectTickBegin;

	// DT_PlantedC4
	std::uintptr_t
		flC4Blow,
		flDefuseCountDown,
		flTimerLength,
		flDefuseLength,
		bBombTicking,
		hBombDefuser,
		bBombDefused;

	// DT_BreakableSurface
	std::uintptr_t
		bIsBroken;

	// DT_EnvTonemapController
	std::uintptr_t
		bUseCustomAutoExposureMin,
		bUseCustomAutoExposureMax,
		bUseCustomBloomScale,
		flCustomAutoExposureMin,
		flCustomAutoExposureMax,
		flCustomBloomScale,
		flCustomBloomScaleMinimum,
		flBloomExponent,
		flBloomSaturation;

	// DT_PlayerResource
	std::uintptr_t
		iPing;

	// DT_CSPlayerResource
	std::uintptr_t
		iCompetitiveRanking,
		iCompetitiveWins,
		nActiveCoinRank,
		nMusicID,
		nPersonaDataPublicLevel,
		nPersonaDataPublicCommendsLeader,
		nPersonaDataPublicCommendsTeacher,
		nPersonaDataPublicCommendsFriendly,
		szClan;

	// DT_CSGameRules
	std::uintptr_t
		bFreezePeriod,
		iRoundTime,
		bIsValveDS,
		bBombDropped,
		bBombPlanted;

	// DT_BaseAnimating
	std::uintptr_t
		nSequence,
		nForceBone,
		dwBoneMatrix,
		nHitboxSet,
		flPoseParameter,
		bClientSideAnimation;

	// DT_ServerAnimationData
	std::uintptr_t
		flCycle;

	// DT_BaseViewModel
	std::uintptr_t
		nModelIndex,
		hOwner,
		hWeapon;

	// DT_ScriptCreatedItem
	std::uintptr_t
		iItemDefinitionIndex,
		iItemIDHigh,
		iItemIDLow,
		iAccountID,
		iEntityQuality,
		szCustomName;

	// DT_BaseAttributableItem
	std::uintptr_t
		OriginalOwnerXuidLow,
		OriginalOwnerXuidHigh,
		nFallbackPaintKit,
		nFallbackSeed,
		flFallbackWear,
		nFallbackStatTrak;
private:
	/*
	 * recursively stores networked properties info from data tables in our map
	 * and also format our dump and write values to file
	 */
	void StoreProps(RecvTable_t* pRecvTable, const std::uintptr_t uOffset, int nDumpTabs);

	// Values
	/* output file */
	std::ofstream fsDumpFile;
};
