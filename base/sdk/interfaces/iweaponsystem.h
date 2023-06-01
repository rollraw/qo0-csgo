#pragma once
#include "../datatypes/vector.h"

// used: virtualcallable_t
#include "../../utilities/memory.h"

#pragma region weaponsystem_enumerations
// @ida CCSWeaponData::WeaponIdFromString(): client.dll -> ABS["E8 ? ? ? ? 8B 75 F8 3B 35 ? ? ? ?" + 0x1]
using ItemDefinitionIndex_t = std::uint16_t;
enum EWeaponIndex : ItemDefinitionIndex_t
{
	WEAPON_NONE = 0U,

	//WEAPON_FIRST = 1U,
	WEAPON_DEAGLE = 1U, // = WEAPON_FIRST
	WEAPON_ELITE = 2U,
	WEAPON_FIVESEVEN = 3U,
	WEAPON_GLOCK = 4U,
	WEAPON_AK47 = 7U,
	WEAPON_AUG = 8U,
	WEAPON_AWP = 9U,
	WEAPON_FAMAS = 10U,
	WEAPON_G3SG1 = 11U,
	WEAPON_GALILAR = 13U,
	WEAPON_M249 = 14U,
	WEAPON_M4A1 = 16U,
	WEAPON_MAC10 = 17U,
	WEAPON_P90 = 19U,
	WEAPON_ZONE_REPULSOR = 20U,
	WEAPON_MP5SD = 23U,
	WEAPON_UMP45 = 24U,
	WEAPON_XM1014 = 25U,
	WEAPON_BIZON = 26U,
	WEAPON_MAG7 = 27U,
	WEAPON_NEGEV = 28U,
	WEAPON_SAWEDOFF = 29U,
	WEAPON_TEC9 = 30U,
	WEAPON_TASER = 31U,
	WEAPON_HKP2000 = 32U,
	WEAPON_MP7 = 33U,
	WEAPON_MP9 = 34U,
	WEAPON_NOVA = 35U,
	WEAPON_P250 = 36U,
	WEAPON_SHIELD = 37U,
	WEAPON_SCAR20 = 38U,
	WEAPON_SG556 = 39U,
	WEAPON_SSG08 = 40U,
	//WEAPON_LAST = WEAPON_SSG08,

	//WEAPON_ITEM_FIRST = 41U,
	WEAPON_KNIFE_GG = 41U, // = WEAPON_ITEM_FIRST
	WEAPON_KNIFE = 42U,
	WEAPON_FLASHBANG = 43U,
	WEAPON_HEGRENADE = 44U,
	WEAPON_SMOKEGRENADE = 45U,
	WEAPON_MOLOTOV = 46U,
	WEAPON_DECOY = 47U,
	WEAPON_INCGRENADE = 48U,
	WEAPON_C4 = 49U,
	//WEAPON_ITEM_LAST = WEAPON_C4,

	//WEAPON_EQUIPMENT_FIRST = 50U,
	WEAPON_KEVLAR = 50U, // = WEAPON_EQUIPMENT_FIRST
	WEAPON_ASSAULTSUIT = 51U,
	WEAPON_HEAVYASSAULTSUIT = 52U,
	WEAPON_NVG = 53U,
	WEAPON_DEFUSER = 54U,
	WEAPON_CUTTERS = 55U,
	//WEAPON_EQUIPMENT_LAST = 56U, // @note: valve retards forgot to assign previous value, so it's one more index...

	WEAPON_HEALTHSHOT = 57U,
	WEAPON_KNIFE_T = 59U,
	WEAPON_M4A1_SILENCER = 60U,
	WEAPON_USP_SILENCER = 61U,
	WEAPON_CZ75A = 63U,
	WEAPON_REVOLVER = 64U,
	WEAPON_TAGRENADE = 68U,
	WEAPON_FISTS = 69U,
	WEAPON_BREACHCHARGE = 70U,
	WEAPON_TABLET = 72U,
	WEAPON_MELEE = 74U,
	WEAPON_AXE = 75U,
	WEAPON_HAMMER = 76U,
	WEAPON_WRENCH = 78U,
	WEAPON_KNIFE_GHOST = 80U,
	WEAPON_FIREBOMB = 81U,
	WEAPON_DIVERSION = 82U,
	WEAPON_FRAG_GRENADE = 83U,
	WEAPON_SNOWBALL = 84U,
	WEAPON_BUMPMINE = 85U
};

enum EWeaponType : int // @ida CCSWeaponData::WeaponClassFromString(): ABS["E8 ? ? ? ? 8B C8 89 86 ? ? ? ? E8 ? ? ? ? 6A 00" + 0x1]
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL = 1,
	WEAPONTYPE_SUBMACHINEGUN = 2,
	WEAPONTYPE_RIFLE = 3,
	WEAPONTYPE_SHOTGUN = 4,
	WEAPONTYPE_SNIPER = 5,
	WEAPONTYPE_MACHINEGUN = 6,
	WEAPONTYPE_C4 = 7,
	WEAPONTYPE_PLACEHOLDER = 8,
	WEAPONTYPE_GRENADE = 9,
	WEAPONTYPE_STACKABLEITEM = 11,
	WEAPONTYPE_FISTS = 12,
	WEAPONTYPE_BREACHCHARGE = 13,
	WEAPONTYPE_BUMPMINE = 14,
	WEAPONTYPE_TABLET = 15,
	WEAPONTYPE_MELEE = 16
};
#pragma endregion

// forward declarations
class IEconItemDefinition;

#pragma pack(push, 4)
// functions used to verify offsets:
// @ida CWeaponData::CWeaponData(): client.dll -> "55 8B EC 83 E4 F8 83 EC 1C 53 56 8B F1 57 89"
class CWeaponData
{
public:
	virtual ~CWeaponData() { }

	const char* szDefinitionName; // 0x0004
	ItemDefinitionIndex_t nDefinitionIndex; // 0x0008
	bool bParsedScript; // 0x000A
	std::byte pad0[0x4 + 0x1]; // 0x00C // @todo: int
	IEconItemDefinition* pItemDefinition; // 0x0010
	int iMaxClip1; // 0x0014 // "primary clip size"
	int iMaxClip2; // 0x0018 // "secondary clip size"
	int iDefaultClip1; // 0x001C // "primary default clip size"
	int iDefaultClip2; // 0x0020 // "secondary default clip size"
	int iMaxReserveAmmo1; // 0x0024 // "primary reserve ammo max"
	int iMaxReserveAmmo2; // 0x0028 // "secondary reserve ammo max"
	const char* szWorldModel; // 0x002C
	const char* szViewModel; // 0x0030
	const char* szDroppedModel; // 0x0034
	std::byte pad1[0x48]; // 0x0038 // 0x38 - model related string, 0x3C-0x7C sound names strings
	const char* szAmmo1; // 0x0080 // @xref: "primary_ammo"
	const char* szAmmo2; // 0x0084 // @xref: "secondary_ammo"
	const char* szHudName; // 0x0088
	const char* szWeaponName; // 0x008C
	bool bAllowHandFlipping; // 0x0090 // "allow hand flipping"
	bool bModelRightHanded; // 0x0091 // "model right handed"
	bool bIsMeleeWeapon; // 0x0092 // "is melee weapon"
	bool bAutoSwitchTo; // 0x0093 // hardcoded true
	bool bAutoSwitchFrom; // 0x0094 // hardcoded true
	int nFlags; // 0x0098
	int iWeight; // 0x009C // "weapon weight"
	std::byte pad2[0x1 + 0x3]; // 0x00A0 // @todo: bool ammo related ig
	int iSlot; // 0x00A4 // "bucket slot"
	int iPosition; // 0x00A8
	std::byte pad3[0x14]; // 0x00AC
	int iRumbleEffect; // 0x00C0 // "rumble effect"
};
static_assert(sizeof(CWeaponData) == 0xC4);

// functions used to verify offsets:
// @ida CCSWeaponData::CCSWeaponData(): client.dll -> "55 8B EC 83 E4 F8 83 EC 14 53 56 8B F1 57 8B 7E 10"
// @ida: client.dll -> "55 8B EC 83 E4 F8 83 EC 34 53 56 8B C1 8B"
class CCSWeaponData : public CWeaponData
{
public:
	int nWeaponID; // 0x00C4
	int nWeaponType; // 0x00C8
	std::byte pad4[0x4]; // 0x00CC // @todo: int ammo related? in range 1 .. 5 @ida: "83 F9 10 77 2C"
	int iWeaponPrice; // 0x00D0 // "in game price"
	int iKillAward; // 0x00D4 // "kill award"
	const char* szAnimationExtension; // 0x00D8 // @xref: "player_animation_extension"
	float flCycleTime; // 0x00DC // "cycletime"
	float flCycleTimeAlt; // 0x00E0 // "cycletime alt"
	float flTimeToIdleAfterFire; // 0x00E4 // "time to idle"
	float flIdleInterval; // 0x00E8 // "idle interval"
	bool bFullAuto; // 0x00EC // "is full auto"
	int iDamage; // 0x00F0 // "damage"
	float flHeadShotMultiplier; // 0x00F4 // "headshot multiplier"
	float flArmorRatio; // 0x00F8 // "armor ratio"
	int iBullets; // 0x00FC // "bullets"
	float flPenetration; // 0x0100 // "penetration"
	float flFlinchVelocityModifierLarge; // 0x0104 // "flinch velocity modifier large"
	float flFlinchVelocityModifierSmall; // 0x0108 // "flinch velocity modifier small"
	float flRange; // 0x010C // "range"
	float flRangeModifier; // 0x0110 // "range modifier"
	float flThrowVelocity; // 0x0114 // "throw velocity"
	Vector_t vecSmokeColor; // 0x0118 // @xref: "grenade_smoke_color"
	bool bHasSilencer; // 0x0124 // "has silencer"
	const char* szSilencerModel; // 0x0128 // "silencer model"
	int iCrosshairMinDistance; // 0x012C // "crosshair min distance"
	int iCrosshairDeltaDistance; // 0x0130 // "crosshair delta distance"
	float flMaxSpeed[2]; // 0x0134 // "max player speed", "max player speed alt"
	float flAttackMoveSpeedFactor; // 0x013C "attack movespeed factor"
	float flSpread[2]; // 0x0140 // "spread", "spread alt"
	float flInaccuracyCrouch[2]; // 0x0148 // "inaccuracy crouch", "inaccuracy crouch alt"
	float flInaccuracyStand[2]; // 0x0150 // "inaccuracy stand", "inaccuracy stand alt"
	float flInaccuracyJumpInitial; // 0x0158 // "inaccuracy jump initial"
	float flInaccuracyJumpApex; // 0x015C // "inaccuracy jump apex"
	float flInaccuracyJump[2]; // 0x0160 // "inaccuracy jump", "inaccuracy jump alt"
	float flInaccuracyLand[2]; // 0x0168 // "inaccuracy land", "inaccuracy land alt"
	float flInaccuracyLadder[2]; // 0x0170 // "inaccuracy ladder", "inaccuracy ladder alt"
	float flInaccuracyFire[2]; // 0x0178 // "inaccuracy fire", "inaccuracy fire alt"
	float flInaccuracyMove[2]; // 0x0180 // "inaccuracy move", "inaccuracy move alt"
	float flInaccuracyReload; // 0x0188 // "inaccuracy reload"
	int nRecoilSeed; // 0x018C // "recoil seed"
	float flRecoilAngle[2]; // 0x0190 // "recoil angle", "recoil angle alt"
	float flRecoilAngleVariance[2]; // 0x0198 // "recoil angle variance", "recoil angle variance alt"
	float flRecoilMagnitude[2]; // 0x01A0 // "recoil magnitude", "recoil magnitude alt"
	float flRecoilMagnitudeVariance[2]; // 0x01A8 // "recoil magnitude variance", "recoil magnitude variance alt"
	int nSpreadSeed; // 0x01B0 // "spread seed"
	float flRecoveryTimeCrouch; // 0x01B4 // "recovery time crouch"
	float flRecoveryTimeStand; // 0x01B8 // "recovery time stand"
	float flRecoveryTimeCrouchFinal; // 0x01BC // "recovery time crouch final"
	float flRecoveryTimeStandFinal; // 0x01C0 // "recovery time stand final"
	int iRecoveryTransitionStartBullet; // 0x01C4 // "recovery transition start bullet"
	int iRecoveryTransitionEndBullet; // 0x01C8 // "recovery transition end bullet"
	bool bUnzoomAfterShot;// 0x01CC // "unzoom after shot"
	bool bHideViewModelZoomed; // 0x01CD // "hide view model zoomed"
	int iZoomLevels; // 0x01D0 // "zoom levels"
	int iZoomFOV[2]; // 0x01D4 // "zoom fov 1", "zoom time 2"
	float flZoomTime[3]; // 0x01DC // "zoom time 0", "zoom time 1", "zoom time 2"
	const char* szAddonLocation; // 0x01E8 // @xref: "addon location"
	float flAddonScale; // 0x01EC // "addon scale"
	std::byte pad5[0x8]; // 0x01F0 // @todo: shell casing rel
	const char* szTracerEffectName; // 0x01F8 // @xref: "tracer_effect"
	int iTracerFrequency; // 0x01FC // "tracer frequency"
	int iTracerFrequencyAlt; // 0x0200 // "tracer frequency alt"
	const char* szMuzzleFlashEffectName1stPerson; // 0x0204 // @xref: "muzzle_flash_effect_1st_person"
	const char* szMuzzleFlashEffectName1stPersonAlt; // 0x0208 // @xref: "muzzle_flash_effect_1st_person_alt"
	const char* szMuzzleFlashEffectName3rdPerson; // 0x020C // @xref: "muzzle_flash_effect_3rd_person"
	const char* szMuzzleFlashEffectName3rdPersonAlt; // 0x0210 // @xref: "muzzle_flash_effect_3rd_person_alt"
	const char* szHeatEffectName; // 0x0214 // @xref: "heat_effect"
	float flHeatPerShot; // 0x0218 // "heat per shot"
	const char* szZoomInSound; // 0x021C
	const char* szZoomOutSound; // 0x0220
	float flInaccuracyPitchShift; // 0x0224 // "inaccuracy pitch shift"
	float flInaccuracyAltSoundThreshold; // 0x0228 // "inaccuracy alt sound threshold"
	float flBotAudibleRange; // 0x022C // "bot audible range"
	std::byte pad6[0x8]; // 0x0230
	const char* szWrongTeamMsg; // 0x0238 // "wrong team msg"
	bool bHasBurstMode; // 0x023C // "has burst mode"
	bool bIsRevolver; // 0x023D // "is revolver"
	bool bCannotShootUnderwater; // 0x023E // "cannot shoot underwater"

	[[nodiscard]] bool IsGun() const
	{
		switch (nWeaponType)
		{
		case WEAPONTYPE_PISTOL:
		case WEAPONTYPE_SUBMACHINEGUN:
		case WEAPONTYPE_RIFLE:
		case WEAPONTYPE_SHOTGUN:
		case WEAPONTYPE_SNIPER:
		case WEAPONTYPE_MACHINEGUN:
			return true;
		default:
			break;
		}

		return false;
	}

	[[nodiscard]] bool IsItem() const
	{
		switch (nWeaponType)
		{
		case WEAPONTYPE_C4:
		case WEAPONTYPE_GRENADE:
		case WEAPONTYPE_STACKABLEITEM:
		case WEAPONTYPE_FISTS:
		case WEAPONTYPE_BREACHCHARGE:
		case WEAPONTYPE_BUMPMINE:
		case WEAPONTYPE_TABLET:
		case WEAPONTYPE_MELEE:
			return true;
		default:
			break;
		}

		return false;
	}
};
static_assert(sizeof(CCSWeaponData) == 0x240); // size verify @ida: client.dll -> ["68 ? ? ? ? 8B 08 8B 01 FF 50 04 8B F0 85 F6 74 25" + 0x1]
#pragma pack(pop)

class IWeaponSystem : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	CCSWeaponData* GetWeaponData(ItemDefinitionIndex_t nItemDefinitionIndex)
	{
		return CallVFunc<CCSWeaponData*, 2U>(this, nItemDefinitionIndex);
	}
};
