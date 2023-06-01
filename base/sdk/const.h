#pragma once

// @source: master/public/worldsize.h
// world coordinate bounds
#define MAX_COORD_FLOAT 16'384.f
#define MIN_COORD_FLOAT (-MAX_COORD_FLOAT)

// @source: master/public/vphysics_interface.h
// coordinates are in HL units. 1 unit == 1 inch
#define METERS_PER_INCH 0.0254f

// max bullet distance, etc
#define MAX_DISTANCE 8'192.f

// max weapons damage
#define MAX_DAMAGE 500.f

// max field of view
#define MAX_FOV 180.f

// @source: master/public/const.h
#pragma region valve_const
// a client can have up to 4 customization files (logo, sounds, models, txt)
#define MAX_CUSTOM_FILES 4
// max customization file size (half a megabyte)
#define MAX_CUSTOM_FILE_SIZE (1 << 19)

#define MAX_PLAYER_NAME_LENGTH 128
#define MAX_PLAYERS_PER_CLIENT 1

// max number of chars for a map name
#define MAX_MAP_NAME 64

// max number of chars for a network (i.e steam) ID
#define	MAX_NETWORKID_LENGTH 64

enum ELifeState : int
{
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

enum EFlags : int
{
	FL_ONGROUND =				(1 << 0),  // entity is at rest / on the ground
	FL_DUCKING =				(1 << 1),  // player is fully crouched/uncrouched
	FL_ANIMDUCKING =			(1 << 2),  // player is in the process of crouching or uncrouching but could be in transition
	FL_WATERJUMP =				(1 << 3),  // player is jumping out of water
	FL_ONTRAIN =				(1 << 4),  // player is controlling a train, so movement commands should be ignored on client during prediction
	FL_INRAIN =					(1 << 5),  // entity is standing in rain
	FL_FROZEN =					(1 << 6),  // player is frozen for 3rd-person camera
	FL_ATCONTROLS =				(1 << 7),  // player can't move, but keeps key inputs for controlling another entity
	FL_CLIENT =					(1 << 8),  // entity is a client (player)
	FL_FAKECLIENT =				(1 << 9),  // entity is a fake client, simulated server side; don't send network messages to them
	FL_INWATER =				(1 << 10), // entity is in water
	FL_FLY =					(1 << 11),
	FL_SWIM =					(1 << 12),
	FL_CONVEYOR =				(1 << 13),
	FL_NPC =					(1 << 14),
	FL_GODMODE =				(1 << 15),
	FL_NOTARGET =				(1 << 16),
	FL_AIMTARGET =				(1 << 17),
	FL_PARTIALGROUND =			(1 << 18), // entity is standing on a place where not all corners are valid
	FL_STATICPROP =				(1 << 19), // entity is a static property
	FL_GRAPHED =				(1 << 20),
	FL_GRENADE =				(1 << 21),
	FL_STEPMOVEMENT =			(1 << 22),
	FL_DONTTOUCH =				(1 << 23),
	FL_BASEVELOCITY =			(1 << 24), // entity have applied base velocity this frame
	FL_WORLDBRUSH =				(1 << 25), // entity is not moveable/removeable brush (part of the world, but represented as an entity for transparency or something)
	FL_OBJECT =					(1 << 26),
	FL_KILLME =					(1 << 27), // entity is marked for death and will be freed by the game
	FL_ONFIRE =					(1 << 28),
	FL_DISSOLVING =				(1 << 29),
	FL_TRANSRAGDOLL =			(1 << 30), // entity is turning into client-side ragdoll
	FL_UNBLOCKABLE_BY_PLAYER =	(1 << 31)
};

enum EEFlags : int
{
	EFL_KILLME =				(1 << 0),
	EFL_DORMANT =				(1 << 1),
	EFL_NOCLIP_ACTIVE =			(1 << 2),
	EFL_SETTING_UP_BONES =		(1 << 3),
	EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4),
	EFL_DIRTY_SHADOWUPDATE =	(1 << 5),
	EFL_NOTIFY =				(1 << 6),
	EFL_FORCE_CHECK_TRANSMIT =	(1 << 7),
	EFL_BOT_FROZEN =			(1 << 8),
	EFL_SERVER_ONLY =			(1 << 9),
	EFL_NO_AUTO_EDICT_ATTACH =	(1 << 10),
	EFL_DIRTY_ABSTRANSFORM =	(1 << 11),
	EFL_DIRTY_ABSVELOCITY =		(1 << 12),
	EFL_DIRTY_ABSANGVELOCITY =	(1 << 13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
	EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
	EFL_HAS_PLAYER_CHILD =		(1 << 16),
	EFL_IN_SKYBOX =				(1 << 17),
	EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),
	EFL_TOUCHING_FLUID =		(1 << 19),
	EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
	EFL_NO_ROTORWASH_PUSH =		(1 << 21),
	EFL_NO_THINK_FUNCTION =		(1 << 22),
	EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),
	EFL_CHECK_UNTOUCH =			(1 << 24),
	EFL_DONTBLOCKLOS =			(1 << 25),
	EFL_DONTWALKON =			(1 << 26),
	EFL_NO_DISSOLVE =			(1 << 27),
	EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),
	EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),
	EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),
	EFL_NO_DAMAGE_FORCES =		(1 << 31)
};

enum EMoveType : int
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,			// no gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY,	// flies through the air and is affected by gravity
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,			// no clip to world, push and crush
	MOVETYPE_NOCLIP,		// no gravity, no collisions, still do velocity/absvelocity
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,		// observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

enum ESolidType : int
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP,		// a bsp tree
	SOLID_BBOX,		// an aabb
	SOLID_OBB,		// an obb (not implemented yet)
	SOLID_OBB_YAW,	// an obb, constrained so that it can only yaw
	SOLID_CUSTOM ,	// always call into the entity for tests
	SOLID_VPHYSICS,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST
};

enum ESolidFlags : int
{
	FSOLID_CUSTOMRAYTEST =			(1 << 0),  // ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST =			(1 << 1),  // ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID =				(1 << 2),  // assume this currently not solid
	FSOLID_TRIGGER =				(1 << 3),  // this is something may be collideable but fires touch functions
	FSOLID_NOT_STANDABLE =			(1 << 4),  // you can't stand on this
	FSOLID_VOLUME_CONTENTS =		(1 << 5),  // contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED =	(1 << 6),  // forces the collision rep to be world-aligned even if it's solid_bsp or solid_vphysics
	FSOLID_USE_TRIGGER_BOUNDS =		(1 << 7),  // uses a special trigger bounds separate from the normal obb
	FSOLID_ROOT_PARENT_ALIGNED =	(1 << 8),  // collisions are defined in root parent's local coordinate space
	FSOLID_TRIGGER_TOUCH_DEBRIS =	(1 << 9),  // this trigger will touch debris objects
	FSOLID_TRIGGER_TOUCH_PLAYER =	(1 << 10), // this trigger will touch only players
	FSOLID_NOT_MOVEABLE =			(1 << 11), // assume this will not move
	FSOLID_MAX_BITS =				12
};

enum EEffectFlags : int
{
	EF_BONEMERGE =					(1 << 0),  // performs bone merge on client side
	EF_BRIGHTLIGHT =				(1 << 1),  // dlight centered at entity origin
	EF_DIMLIGHT =					(1 << 2),  // player flashlight
	EF_NOINTERP =					(1 << 3),  // don't interpolate the next frame
	EF_NOSHADOW =					(1 << 4),  // don't cast no shadow
	EF_NODRAW =						(1 << 5),  // don't draw entity
	EF_NORECEIVESHADOW =			(1 << 6),  // don't receive no shadow
	EF_BONEMERGE_FASTCULL =			(1 << 7),  // for use with 'EF_BONEMERGE'. if this is set, then it places this ent's origin at its parent and uses the parent's bbox + the max extents of the aiment. otherwise, it sets up the parent's bones every frame to figure out where to place the aiment, which is inefficient because it'll setup the parent's bones even if the parent is not in the pvs
	EF_ITEM_BLINK =					(1 << 8),  // blink an item so that the user notices it
	EF_PARENT_ANIMATES =			(1 << 9),  // always assume that the parent entity is animating
	EF_MARKED_FOR_FAST_REFLECTION = (1 << 10), // marks an entity for reflection rendering when using "$reflectonlymarkedentities" material variable
	EF_NOSHADOWDEPTH =				(1 << 11), // indicates this entity does not render into any shadow depthmap
	EF_SHADOWDEPTH_NOCACHE =		(1 << 12), // indicates this entity cannot be cached in shadow depthmap and should render every frame
	EF_NOFLASHLIGHT =				(1 << 13),
	EF_NOCSM =						(1 << 14), // indicates this entity does not render into the cascade shadow depthmap
	EF_MAX_BITS =					15
};
#pragma endregion

// @source: master/game/shared/cstrike15/gametypes.h
#pragma region valve_gametypes
enum EGameType : int
{
	GAMETYPE_UNKNOWN = -1,
	GAMETYPE_CLASSIC,
	GAMETYPE_GUNGAME,
	GAMETYPE_TRAINING,
	GAMETYPE_CUSTOM,
	GAMETYPE_COOPERATIVE,
	GAMETYPE_SKIRMISH,
	GAMETYPE_FREEFORALL
};

enum EGameMode : int
{
	GAMEMODE_UNKNOWN = -1,

	// GAMETYPE_CLASSIC
	GAMEMODE_CLASSIC_CASUAL = 0,
	GAMEMODE_CLASSIC_COMPETITIVE,
	GAMEMODE_CLASSIC_SCRIM_COMPETITIVE2V2,
	GAMEMODE_CLASSIC_SCRIM_COMPETITIVE5V5,

	// GAMETYPE_GUNGAME
	GAMEMODE_GUNGAME_PROGRESSIVE = 0,
	GAMEMODE_GUNGAME_BOMB,
	GAMEMODE_GUNGAME_DEATHMATCH,

	// GAMETYPE_TRAINING
	GAMEMODE_TRAINING_DEFAULT = 0,

	// GAMETYPE_CUSTOM
	GAMEMODE_CUSTOM_DEFAULT = 0,

	// GAMETYPE_COOPERATIVE
	GAMEMODE_COOPERATIVE_DEFAULT = 0,
	GAMEMODE_COOPERATIVE_MISSION,

	// GAMETYPE_SKIRMISH
	GAMEMODE_SKIRMISH_DEFAULT = 0,

	// GAMETYPE_FREEFORALL
	GAMEMODE_FREEFORALL_SURVIVAL = 0
};
#pragma endregion

// @source: master/game/shared/cstrike15/cs_shareddefs.h
#pragma region valve_cs_shareddefs
// @note: max for new tags is actually 12, this allows some backward compat
#define MAX_CLAN_TAG_LENGTH 16

#define MAX_PLACE_NAME_LENGTH 18

#define MAX_HOSTAGES 12
#define MAX_HOSTAGE_RESCUES 4

#define MAX_MATCH_STATS_ROUNDS 30
#define MATCH_STATS_TEAM_SWAP_ROUND 15

#define GRENADE_DEFAULT_SIZE 2.0f

// CCSPlayer::m_iAddonBits
enum EAddonFlags : int
{
	ADDON_FLASHBANG_1 = (1 << 0),
	ADDON_FLASHBANG_2 = (1 << 1),
	ADDON_HE_GRENADE = (1 << 2),
	ADDON_SMOKE_GRENADE = (1 << 3),
	ADDON_C4 = (1 << 4),
	ADDON_DEFUSEKIT = (1 << 5),
	ADDON_PRIMARY = (1 << 6),
	ADDON_PISTOL = (1 << 7),
	ADDON_PISTOL2 = (1 << 8),
	ADDON_DECOY = (1 << 9),
	ADDON_KNIFE = (1 << 10),
	ADDON_MASK = (1 << 11),
	ADDON_TAGRENADE = (1 << 12),
	ADDON_BITS_COUNT = 13,

	ADDON_CLIENTSIDE_HOLIDAY_HAT = (1 << ADDON_BITS_COUNT),
	ADDON_CLIENTSIDE_GHOST = (2 << ADDON_BITS_COUNT),
	ADDON_CLIENTSIDE_ASSASSINATION_TARGET = (4 << ADDON_BITS_COUNT),
	ADDON_CLIENTSIDE_BITS_COUNT = 3
};

// CBaseEntity::m_iTeamNum
enum ETeamID : int
{
	TEAM_UNASSIGNED = 0,
	TEAM_SPECTATOR,
	TEAM_TT,
	TEAM_CT
};

enum EGrenadeType : int
{
	GRENADE_TYPE_EXPLOSIVE,
	GRENADE_TYPE_FLASH,
	GRENADE_TYPE_FIRE,
	GRENADE_TYPE_DECOY,
	GRENADE_TYPE_SMOKE,
	GRENADE_TYPE_SENSOR,
	GRENADE_TYPE_TOTAL
};

#pragma endregion

// @source: master/game/shared/shareddefs.h
#pragma region valve_shareddefs
#define TICK_INTERVAL (I::Globals->flIntervalPerTick)
#define TIME_TO_TICKS(TIME) (static_cast<int>(0.5f + static_cast<float>(TIME) / TICK_INTERVAL))
#define TICKS_TO_TIME(TICKS) (TICK_INTERVAL * static_cast<float>(TICKS))
#define ROUND_TO_TICKS(TIME) (TICK_INTERVAL * TIME_TO_TICKS(TIME))
#define TICK_NEVER_THINK (-1)

// absolute max players supported
#define MAX_PLAYERS 64

// max number of weapons available
#define MAX_WEAPONS 64 // @ida: client.dll -> U8["BA ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? 6A 00 6A 04 68 ? ? ? ? BA ? ? ? ? 8D 4D C4" - 0x8] @xref: "m_hMyWeapons"
// max number of wearables available
#define MAX_WEARABLES 1 // @xref: "m_hMyWearables"
// hud item selection slots
#define MAX_WEAPON_SLOTS 6
// max number of items within a slot
#define MAX_WEAPON_POSITIONS 6
// hud item selection slots
#define MAX_ITEM_TYPES 6

// max number of teams in a game
#define MAX_TEAMS 32
// max length of a team's name
#define MAX_TEAM_NAME_LENGTH 32

#define MAX_VIEWMODELS 2

enum EHitGroupIndex : int
{
	HITGROUP_GENERIC = 0,
	HITGROUP_HEAD,
	HITGROUP_CHEST,
	HITGROUP_STOMACH,
	HITGROUP_LEFTARM,
	HITGROUP_RIGHTARM,
	HITGROUP_LEFTLEG,
	HITGROUP_RIGHTLEG,
	HITGROUP_NECK,
	HITGROUP_GEAR = 10
};

enum EDamageType : int
{
	DAMAGE_NO = 0,
	DAMAGE_EVENTS_ONLY,
	DAMAGE_YES,
	DAMAGE_AIM
};

enum EObserverMode : int
{
	OBS_MODE_NONE = 0,
	OBS_MODE_DEATHCAM,
	OBS_MODE_FREEZECAM,
	OBS_MODE_FIXED,
	OBS_MODE_IN_EYE,
	OBS_MODE_CHASE,
	OBS_MODE_ROAMING
};

enum EModelScaleType : int
{
	HIERARCHICAL_MODEL_SCALE = 0,
	NONHIERARCHICAL_MODEL_SCALE
};
#pragma endregion

// @note: see "sv_dump_class_info" command
enum class EClassIndex : int
{
	CAI_BaseNPC = 0,
	CAK47,
	CBaseAnimating,
	CBaseAnimatingOverlay,
	CBaseAttributableItem,
	CBaseButton,
	CBaseCombatCharacter,
	CBaseCombatWeapon,
	CBaseCSGrenade,
	CBaseCSGrenadeProjectile,
	CBaseDoor,
	CBaseEntity,
	CBaseFlex,
	CBaseGrenade,
	CBaseParticleEntity,
	CBasePlayer,
	CBasePropDoor,
	CBaseTeamObjectiveResource,
	CBaseTempEntity,
	CBaseToggle,
	CBaseTrigger,
	CBaseViewModel,
	CBaseVPhysicsTrigger,
	CBaseWeaponWorldModel,
	CBeam,
	CBeamSpotlight,
	CBoneFollower,
	CBRC4Target,
	CBreachCharge,
	CBreachChargeProjectile,
	CBreakableProp,
	CBreakableSurface,
	CBumpMine,
	CBumpMineProjectile,
	CC4,
	CCascadeLight,
	CChicken,
	CColorCorrection,
	CColorCorrectionVolume,
	CCSGameRulesProxy,
	CCSPlayer,
	CCSPlayerResource,
	CCSRagdoll,
	CCSTeam,
	CDangerZone,
	CDangerZoneController,
	CDEagle,
	CDecoyGrenade,
	CDecoyProjectile,
	CDrone,
	CDronegun,
	CDynamicLight,
	CDynamicProp,
	CEconEntity,
	CEconWearable,
	CEmbers,
	CEntityDissolve,
	CEntityFlame,
	CEntityFreezing,
	CEntityParticleTrail,
	CEnvAmbientLight,
	CEnvDetailController,
	CEnvDOFController,
	CEnvGasCanister,
	CEnvParticleScript,
	CEnvProjectedTexture,
	CEnvQuadraticBeam,
	CEnvScreenEffect,
	CEnvScreenOverlay,
	CEnvTonemapController,
	CEnvWind,
	CFEPlayerDecal,
	CFireCrackerBlast,
	CFireSmoke,
	CFireTrail,
	CFish,
	CFists,
	CFlashbang,
	CFogController,
	CFootstepControl,
	CFunc_Dust,
	CFunc_LOD,
	CFuncAreaPortalWindow,
	CFuncBrush,
	CFuncConveyor,
	CFuncLadder,
	CFuncMonitor,
	CFuncMoveLinear,
	CFuncOccluder,
	CFuncReflectiveGlass,
	CFuncRotating,
	CFuncSmokeVolume,
	CFuncTrackTrain,
	CGameRulesProxy,
	CGrassBurn,
	CHandleTest,
	CHEGrenade,
	CHostage,
	CHostageCarriableProp,
	CIncendiaryGrenade,
	CInferno,
	CInfoLadderDismount,
	CInfoMapRegion,
	CInfoOverlayAccessor,
	CItem_Healthshot,
	CItemCash,
	CItemDogtags,
	CKnife,
	CKnifeGG,
	CLightGlow,
	CMapVetoPickController,
	CMaterialModifyControl,
	CMelee,
	CMolotovGrenade,
	CMolotovProjectile,
	CMovieDisplay,
	CParadropChopper,
	CParticleFire,
	CParticlePerformanceMonitor,
	CParticleSystem,
	CPhysBox,
	CPhysBoxMultiplayer,
	CPhysicsProp,
	CPhysicsPropMultiplayer,
	CPhysMagnet,
	CPhysPropAmmoBox,
	CPhysPropLootCrate,
	CPhysPropRadarJammer,
	CPhysPropWeaponUpgrade,
	CPlantedC4,
	CPlasma,
	CPlayerPing,
	CPlayerResource,
	CPointCamera,
	CPointCommentaryNode,
	CPointWorldText,
	CPoseController,
	CPostProcessController,
	CPrecipitation,
	CPrecipitationBlocker,
	CPredictedViewModel,
	CProp_Hallucination,
	CPropCounter,
	CPropDoorRotating,
	CPropJeep,
	CPropVehicleDriveable,
	CRagdollManager,
	CRagdollProp,
	CRagdollPropAttached,
	CRopeKeyframe,
	CSCAR17,
	CSceneEntity,
	CSensorGrenade,
	CSensorGrenadeProjectile,
	CShadowControl,
	CSlideshowDisplay,
	CSmokeGrenade,
	CSmokeGrenadeProjectile,
	CSmokeStack,
	CSnowball,
	CSnowballPile,
	CSnowballProjectile,
	CSpatialEntity,
	CSpotlightEnd,
	CSprite,
	CSpriteOriented,
	CSpriteTrail,
	CStatueProp,
	CSteamJet,
	CSun,
	CSunlightShadowControl,
	CSurvivalSpawnChopper,
	CTablet,
	CTeam,
	CTeamplayRoundBasedRulesProxy,
	CTEArmorRicochet,
	CTEBaseBeam,
	CTEBeamEntPoint,
	CTEBeamEnts,
	CTEBeamFollow,
	CTEBeamLaser,
	CTEBeamPoints,
	CTEBeamRing,
	CTEBeamRingPoint,
	CTEBeamSpline,
	CTEBloodSprite,
	CTEBloodStream,
	CTEBreakModel,
	CTEBSPDecal,
	CTEBubbles,
	CTEBubbleTrail,
	CTEClientProjectile,
	CTEDecal,
	CTEDust,
	CTEDynamicLight,
	CTEEffectDispatch,
	CTEEnergySplash,
	CTEExplosion,
	CTEFireBullets,
	CTEFizz,
	CTEFootprintDecal,
	CTEFoundryHelpers,
	CTEGaussExplosion,
	CTEGlowSprite,
	CTEImpact,
	CTEKillPlayerAttachments,
	CTELargeFunnel,
	CTEMetalSparks,
	CTEMuzzleFlash,
	CTEParticleSystem,
	CTEPhysicsProp,
	CTEPlantBomb,
	CTEPlayerAnimEvent,
	CTEPlayerDecal,
	CTEProjectedDecal,
	CTERadioIcon,
	CTEShatterSurface,
	CTEShowLine,
	CTesla,
	CTESmoke,
	CTESparks,
	CTESprite,
	CTESpriteSpray,
	CTest_ProxyToggle_Networkable,
	CTestTraceline,
	CTEWorldDecal,
	CTriggerPlayerMovement,
	CTriggerSoundOperator,
	CVGuiScreen,
	CVoteController,
	CWaterBullet,
	CWaterLODControl,
	CWeaponAug,
	CWeaponAWP,
	CWeaponBaseItem,
	CWeaponBizon,
	CWeaponCSBase,
	CWeaponCSBaseGun,
	CWeaponCycler,
	CWeaponElite,
	CWeaponFamas,
	CWeaponFiveSeven,
	CWeaponG3SG1,
	CWeaponGalil,
	CWeaponGalilAR,
	CWeaponGlock,
	CWeaponHKP2000,
	CWeaponM249,
	CWeaponM3,
	CWeaponM4A1,
	CWeaponMAC10,
	CWeaponMag7,
	CWeaponMP5Navy,
	CWeaponMP7,
	CWeaponMP9,
	CWeaponNegev,
	CWeaponNOVA,
	CWeaponP228,
	CWeaponP250,
	CWeaponP90,
	CWeaponSawedoff,
	CWeaponSCAR20,
	CWeaponScout,
	CWeaponSG550,
	CWeaponSG552,
	CWeaponSG556,
	CWeaponShield,
	CWeaponSSG08,
	CWeaponTaser,
	CWeaponTec9,
	CWeaponTMP,
	CWeaponUMP45,
	CWeaponUSP,
	CWeaponXM1014,
	CWeaponZoneRepulsor,
	CWorld,
	CWorldVguiText,
	DustTrail,
	MovieExplosion,
	ParticleSmokeGrenade,
	RocketTrail,
	SmokeTrail,
	SporeExplosion,
	SporeTrail,
};

// @source: cstrike15_usermessages.proto
enum EUserMessages : int
{
	CS_UM_VGUIMenu = 1,
	CS_UM_Geiger = 2,
	CS_UM_Train = 3,
	CS_UM_HudText = 4,
	CS_UM_SayText = 5,
	CS_UM_SayText2 = 6,
	CS_UM_TextMsg = 7,
	CS_UM_HudMsg = 8,
	CS_UM_ResetHud = 9,
	CS_UM_GameTitle = 10,
	CS_UM_Shake = 12,
	CS_UM_Fade = 13,
	CS_UM_Rumble = 14,
	CS_UM_CloseCaption = 15,
	CS_UM_CloseCaptionDirect = 16,
	CS_UM_SendAudio = 17,
	CS_UM_RawAudio = 18,
	CS_UM_VoiceMask = 19,
	CS_UM_RequestState = 20,
	CS_UM_Damage = 21,
	CS_UM_RadioText = 22,
	CS_UM_HintText = 23,
	CS_UM_KeyHintText = 24,
	CS_UM_ProcessSpottedEntityUpdate = 25,
	CS_UM_ReloadEffect = 26,
	CS_UM_AdjustMoney = 27,
	CS_UM_UpdateTeamMoney = 28,
	CS_UM_StopSpectatorMode = 29,
	CS_UM_KillCam = 30,
	CS_UM_DesiredTimescale = 31,
	CS_UM_CurrentTimescale = 32,
	CS_UM_AchievementEvent = 33,
	CS_UM_MatchEndConditions = 34,
	CS_UM_DisconnectToLobby = 35,
	CS_UM_PlayerStatsUpdate = 36,
	CS_UM_DisplayInventory = 37,
	CS_UM_WarmupHasEnded = 38,
	CS_UM_ClientInfo = 39,
	CS_UM_XRankGet = 40,
	CS_UM_XRankUpd = 41,
	CS_UM_CallVoteFailed = 45,
	CS_UM_VoteStart = 46,
	CS_UM_VotePass = 47,
	CS_UM_VoteFailed = 48,
	CS_UM_VoteSetup = 49,
	CS_UM_ServerRankRevealAll = 50,
	CS_UM_SendLastKillerDamageToClient = 51,
	CS_UM_ServerRankUpdate = 52,
	CS_UM_ItemPickup = 53,
	CS_UM_ShowMenu = 54,
	CS_UM_BarTime = 55,
	CS_UM_AmmoDenied = 56,
	CS_UM_MarkAchievement = 57,
	CS_UM_MatchStatsUpdate = 58,
	CS_UM_ItemDrop = 59,
	CS_UM_GlowPropTurnOff = 60,
	CS_UM_SendPlayerItemDrops = 61,
	CS_UM_RoundBackupFilenames = 62,
	CS_UM_SendPlayerItemFound = 63,
	CS_UM_ReportHit = 64,
	CS_UM_XpUpdate = 65,
	CS_UM_QuestProgress = 66,
	CS_UM_ScoreLeaderboardData = 67,
	CS_UM_PlayerDecalDigitalSignature = 68
};
