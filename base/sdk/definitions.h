#pragma once
// @note: contains other simplified/modified/updated valve's definitions (i tried keep it look's not like shit)

#define MAX_COORD_FLOAT		16'384.f			// max world coordinates
#define MIN_COORD_FLOAT		-(MAX_COORD_FLOAT)	// min world coordinates
#define MAX_DISTANCE		8'192.f				// max bullet distance, etc
#define MAX_FOV				180.f				// max field of view
#define MAX_DAMAGE			500.f				// max weapons damage
#define MAX_WEAPONS			64					// max number of weapons available

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier0/basetypes.h
#pragma region valve_basetypes
inline float BitsToFloat(std::uint32_t i)
{
	union Convertor_t
	{
		float fl;
		unsigned long ul;
	} tmp;

	tmp.ul = i;
	return tmp.fl;
}

#define FLOAT32_NAN_BITS		0x7FC00000U
#define FLOAT32_NAN				BitsToFloat( FLOAT32_NAN_BITS )
#define VEC_T_NAN				FLOAT32_NAN
#pragma endregion

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/const.h
#pragma region valve_consts
#define MAX_AREA_STATE_BYTES		32
#define MAX_AREA_PORTAL_STATE_BYTES 24

/* baseentity lifestate */
enum ELifeState : int
{
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

/* baseentity flags */
enum EFlags : int
{
	FL_ONGROUND =				(1 << 0),
	FL_DUCKING =				(1 << 1),
	FL_ANIMDUCKING =			(1 << 2),
	FL_WATERJUMP =				(1 << 3),
	FL_ONTRAIN =				(1 << 4),
	FL_INRAIN =					(1 << 5),
	FL_FROZEN =					(1 << 6),
	FL_ATCONTROLS =				(1 << 7),
	FL_CLIENT =					(1 << 8),
	FL_FAKECLIENT =				(1 << 9),
	FL_INWATER =				(1 << 10),
	FL_FLY =					(1 << 11),
	FL_SWIM =					(1 << 12),
	FL_CONVEYOR =				(1 << 13),
	FL_NPC =					(1 << 14),
	FL_GODMODE =				(1 << 15),
	FL_NOTARGET =				(1 << 16),
	FL_AIMTARGET =				(1 << 17),
	FL_PARTIALGROUND =			(1 << 18),
	FL_STATICPROP =				(1 << 19),
	FL_GRAPHED =				(1 << 20),
	FL_GRENADE =				(1 << 21),
	FL_STEPMOVEMENT =			(1 << 22),
	FL_DONTTOUCH =				(1 << 23),
	FL_BASEVELOCITY =			(1 << 24),
	FL_WORLDBRUSH =				(1 << 25),
	FL_OBJECT =					(1 << 26),
	FL_KILLME =					(1 << 27),
	FL_ONFIRE =					(1 << 28),
	FL_DISSOLVING =				(1 << 29),
	FL_TRANSRAGDOLL =			(1 << 30),
	FL_UNBLOCKABLE_BY_PLAYER =	(1 << 31)
};

enum EMoveType : int
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
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

enum ESolidFlags : unsigned int
{
	FSOLID_CUSTOMRAYTEST =			0x0001,	// ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST =			0x0002,	// ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID =				0x0004,	// are we currently not solid?
	FSOLID_TRIGGER =				0x0008,	// this is something may be collideable but fires touch functions
	FSOLID_NOT_STANDABLE =			0x0010,	// you can't stand on this
	FSOLID_VOLUME_CONTENTS =		0x0020,	// contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED =	0x0040,	// forces the collision rep to be world-aligned even if it's solid_bsp or solid_vphysics
	FSOLID_USE_TRIGGER_BOUNDS =		0x0080,	// uses a special trigger bounds separate from the normal obb
	FSOLID_ROOT_PARENT_ALIGNED =	0x0100,	// collisions are defined in root parent's local coordinate space
	FSOLID_TRIGGER_TOUCH_DEBRIS =	0x0200,	// this trigger will touch debris objects
	FSOLID_MAX_BITS =				10
};

enum EEffectFlags : unsigned int
{
	EF_BONEMERGE =					0x001,	// performs bone merge on client side
	EF_BRIGHTLIGHT =				0x002,	// dlight centered at entity origin
	EF_DIMLIGHT =					0x004,	// player flashlight
	EF_NOINTERP =					0x008,	// don't interpolate the next frame
	EF_NOSHADOW =					0x010,	// don't cast no shadow
	EF_NODRAW =						0x020,	// don't draw entity
	EF_NORECEIVESHADOW =			0x040,	// don't receive no shadow
	EF_BONEMERGE_FASTCULL =			0x080,	// for use with EF_BONEMERGE. if this is set, then it places this ent's origin at its
											// parent and uses the parent's bbox + the max extents of the aiment.
											// otherwise, it sets up the parent's bones every frame to figure out where to place
											// the aiment, which is inefficient because it'll setup the parent's bones even if
											// the parent is not in the pvs.
	EF_ITEM_BLINK =					0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES =			0x200,	// always assume that the parent entity is animating
	EF_MARKED_FOR_FAST_REFLECTION = 0x400,	// marks an entity for reflection rendering when using $reflectonlymarkedentities material variable
	EF_NOSHADOWDEPTH =				0x800,	// indicates this entity does not render into any shadow depthmap
	EF_SHADOWDEPTH_NOCACHE =		0x1000,	// indicates this entity cannot be cached in shadow depthmap and should render every frame
	EF_NOFLASHLIGHT =				0x2000,
	EF_NOCSM =						0x4000,	// indicates this entity does not render into the cascade shadow depthmap
	EF_MAX_BITS =					15
};

enum ECollisionGroup : int
{
	COLLISION_GROUP_NONE = 0,
	COLLISION_GROUP_DEBRIS,						// collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER,				// same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS,			// collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE,				// collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,			// for HL2, same as Collision_Group_Player, for / TF2, this filters out other players and CBaseObjects
	COLLISION_GROUP_NPC,						// generic NPC group
	COLLISION_GROUP_IN_VEHICLE,					// for any entity inside a vehicle
	COLLISION_GROUP_WEAPON,						// for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP,				// vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE,					// projectiles!
	COLLISION_GROUP_DOOR_BLOCKER,				// blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR,				// doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING,					// things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY,					// nonsolid on client and server, pushaway in player code
	COLLISION_GROUP_NPC_ACTOR,					// used so NPCs in scripts ignore the player.
	COLLISION_GROUP_NPC_SCRIPTED,				// used for NPCs in scripts that should not collide with each other
	COLLISION_GROUP_PZ_CLIP,
	COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE,	// only collides with bullets
	LAST_SHARED_COLLISION_GROUP
};
#pragma endregion

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/shareddefs.h
#pragma region valve_shareddefs
#define TICK_INTERVAL			( I::Globals->flIntervalPerTick )
#define TIME_TO_TICKS( t )		( static_cast<int>( 0.5f + static_cast<float>( t ) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL * static_cast<float>( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		( -1 )

enum EMultiplayerPhysicsMode : int
{
	PHYSICS_MULTIPLAYER_AUTODETECT =	0,	// use multiplayer physics mode as defined in model prop data
	PHYSICS_MULTIPLAYER_SOLID =			1,	// solid, pushes player away
	PHYSICS_MULTIPLAYER_NON_SOLID =		2,	// nonsolid, but pushed by player
	PHYSICS_MULTIPLAYER_CLIENTSIDE =	3	// clientside only, nonsolid
};

/* hitgroup standarts */
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

/* settings for m_takedamage */
enum EDamageType : int
{
	DAMAGE_NO = 0,
	DAMAGE_EVENTS_ONLY,
	DAMAGE_YES,
	DAMAGE_AIM
};

/* spectator movement modes */
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
#pragma endregion

#pragma region valve_textures
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/texture_group_names.h
#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER			"Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"

#define TEXTURE_SKYBOX_BAGGAGE						"cs_baggage_skybox_"
#define TEXTURE_SKYBOX_TIBET						"cs_tibet"
#define TEXTURE_SKYBOX_EMBASSY						"embassy"
#define TEXTURE_SKYBOX_ITALY						"italy"
#define TEXTURE_SKYBOX_JUNGLE						"jungle"
#define TEXTURE_SKYBOX_NUKEBLANK					"nukeblank"
#define TEXTURE_SKYBOX_OFFICE						"office"
#define TEXTURE_SKYBOX_DAYLIGHT01					"sky_cs15_daylight01_hdr"
#define TEXTURE_SKYBOX_DAYLIGHT02					"sky_cs15_daylight02_hdr"
#define TEXTURE_SKYBOX_DAYLIGHT03					"sky_cs15_daylight03_hdr"
#define TEXTURE_SKYBOX_DAYLIGHT04					"sky_cs15_daylight04_hdr"
#define TEXTURE_SKYBOX_CLOUDY01						"sky_csgo_cloudy01"
#define TEXTURE_SKYBOX_NIGHT02						"sky_csgo_night02"
#define TEXTURE_SKYBOX_NIGHT02B						"sky_csgo_night02b"
#define TEXTURE_SKYBOX_DUST							"sky_dust"
#define TEXTURE_SKYBOX_VENICE						"sky_venice"
#define TEXTURE_SKYBOX_VERTIGO						"vertigo"
#define TEXTURE_SKYBOX_VERTIGO_HDR					"vertigoblue_hdr"
#define TEXTURE_SKYBOX_VIETNAM						"vietnam"
#pragma endregion

#pragma region valve_decals
#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E'
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_SNOW			'K'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_REFLECTIVE		'R'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z'
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

// @credits: from protobufs (cstrike15_usermessages.proto)
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

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/model_types.h
enum EDrawModelFlags : unsigned int
{
	STUDIO_NONE = 0x00000000,
	STUDIO_RENDER = 0x00000001,
	STUDIO_VIEWXFORMATTACHMENTS = 0x00000002,
	STUDIO_DRAWTRANSLUCENTSUBMODELS = 0x00000004,
	STUDIO_TWOPASS = 0x00000008,
	STUDIO_STATIC_LIGHTING = 0x00000010,
	STUDIO_WIREFRAME = 0x00000020,
	STUDIO_ITEM_BLINK = 0x00000040,
	STUDIO_NOSHADOWS = 0x00000080,
	STUDIO_WIREFRAME_VCOLLIDE = 0x00000100,
	STUDIO_NOLIGHTING_OR_CUBEMAP = 0x00000200,
	STUDIO_SKIP_FLEXES = 0x00000400,
	STUDIO_DONOTMODIFYSTENCILSTATE = 0x00000800,
	STUDIO_SKIP_DECALS = 0x10000000,
	STUDIO_SHADOWTEXTURE = 0x20000000,
	STUDIO_SHADOWDEPTHTEXTURE = 0x40000000,
	STUDIO_TRANSPARENCY = 0x80000000
};

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/shaderapi/ishadershadow.h
enum EShaderStencilFunc : int
{
	SHADER_STENCILFUNC_NEVER = 0,
	SHADER_STENCILFUNC_LESS,
	SHADER_STENCILFUNC_EQUAL,
	SHADER_STENCILFUNC_LEQUAL,
	SHADER_STENCILFUNC_GREATER,
	SHADER_STENCILFUNC_NOTEQUAL,
	SHADER_STENCILFUNC_GEQUAL,
	SHADER_STENCILFUNC_ALWAYS
};

enum EShaderStencilOp : int
{
	SHADER_STENCILOP_KEEP = 0,
	SHADER_STENCILOP_ZERO,
	SHADER_STENCILOP_SET_TO_REFERENCE,
	SHADER_STENCILOP_INCREMENT_CLAMP,
	SHADER_STENCILOP_DECREMENT_CLAMP,
	SHADER_STENCILOP_INVERT,
	SHADER_STENCILOP_INCREMENT_WRAP,
	SHADER_STENCILOP_DECREMENT_WRAP,
};

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/string_t.h
struct string_t
{
public:
	bool operator!() const { return (szValue == nullptr); }
	bool operator==(const string_t& rhs) const { return (szValue == rhs.szValue); }
	bool operator!=(const string_t& rhs) const { return (szValue != rhs.szValue); }
	bool operator<(const string_t& rhs) const { return (reinterpret_cast<void*>(const_cast<char*>(szValue)) < reinterpret_cast<void*>(const_cast<char*>(rhs.szValue))); }

	const char* c_str() const { return (szValue) ? szValue : ""; }
protected:
	const char* szValue;
};

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/props_shared.h
#pragma region valve_sharedprops
enum EPropertyDataInteractions : int
{
	PROPINTER_PHYSGUN_WORLD_STICK,		// "onworldimpact"		"stick"
	PROPINTER_PHYSGUN_FIRST_BREAK,		// "onfirstimpact"		"break"
	PROPINTER_PHYSGUN_FIRST_PAINT,		// "onfirstimpact"		"paintsplat"
	PROPINTER_PHYSGUN_FIRST_IMPALE,		// "onfirstimpact"		"impale"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_NONE,	// "onlaunch"			"spin_none"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_Z,	// "onlaunch"			"spin_zaxis"
	PROPINTER_PHYSGUN_BREAK_EXPLODE,	// "onbreak"			"explode_fire"
	PROPINTER_PHYSGUN_BREAK_EXPLODE_ICE,// "onbreak"			"explode_ice"
	PROPINTER_PHYSGUN_DAMAGE_NONE,		// "damage"				"none"
	PROPINTER_FIRE_FLAMMABLE,			// "flammable"			"yes"
	PROPINTER_FIRE_EXPLOSIVE_RESIST,	// "explosive_resist"	"yes"
	PROPINTER_FIRE_IGNITE_HALFHEALTH,	// "ignite"				"halfhealth"
	PROPINTER_PHYSGUN_CREATE_FLARE,		// "onpickup"			"create_flare"
	PROPINTER_PHYSGUN_ALLOW_OVERHEAD,	// "allow_overhead"		"yes"
	PROPINTER_WORLD_BLOODSPLAT,			// "onworldimpact",		"bloodsplat"
	PROPINTER_PHYSGUN_NOTIFY_CHILDREN,	// "onfirstimpact" - cause attached flechettes to explode
	PROPINTER_MELEE_IMMUNE,				// "melee_immune"		"yes"
	PROPINTER_NUM_INTERACTIONS			// if we get more than 32 of these, we'll need a different system
};

enum EMultiplayerBreak : int
{
	MULTIPLAYER_BREAK_DEFAULT,
	MULTIPLAYER_BREAK_SERVERSIDE,
	MULTIPLAYER_BREAK_CLIENTSIDE,
	MULTIPLAYER_BREAK_BOTH
};

class IMultiplayerPhysics
{
public:
	virtual int		GetMultiplayerPhysicsMode() = 0;
	virtual float	GetMass() = 0;
	virtual bool	IsAsleep() = 0;
};

class IBreakableWithPropData
{
public:
	// Damage modifiers
	virtual void		SetDmgModBullet(float flDmgMod) = 0;
	virtual void		SetDmgModClub(float flDmgMod) = 0;
	virtual void		SetDmgModExplosive(float flDmgMod) = 0;
	virtual float		GetDmgModBullet() = 0;
	virtual float		GetDmgModClub() = 0;
	virtual float		GetDmgModExplosive() = 0;
	virtual float		GetDmgModFire() = 0;

	// Explosive
	virtual void		SetExplosiveRadius(float flRadius) = 0;
	virtual void		SetExplosiveDamage(float flDamage) = 0;
	virtual float		GetExplosiveRadius() = 0;
	virtual float		GetExplosiveDamage() = 0;

	// Physics damage tables
	virtual void		SetPhysicsDamageTable(string_t iszTableName) = 0;
	virtual string_t	GetPhysicsDamageTable() = 0;

	// Breakable chunks
	virtual void		SetBreakableModel(string_t iszModel) = 0;
	virtual string_t 	GetBreakableModel() = 0;
	virtual void		SetBreakableSkin(int iSkin) = 0;
	virtual int			GetBreakableSkin() = 0;
	virtual void		SetBreakableCount(int iCount) = 0;
	virtual int			GetBreakableCount() = 0;
	virtual void		SetMaxBreakableSize(int iSize) = 0;
	virtual int			GetMaxBreakableSize() = 0;

	// LOS blocking
	virtual void		SetPropDataBlocksLOS(bool bBlocksLOS) = 0;
	virtual void		SetPropDataIsAIWalkable(bool bBlocksLOS) = 0;

	// Interactions
	virtual void		SetInteraction(EPropertyDataInteractions Interaction) = 0;
	virtual bool		HasInteraction(EPropertyDataInteractions Interaction) = 0;

	// Multiplayer physics mode
	virtual void		SetPhysicsMode(int iMode) = 0;
	virtual int			GetPhysicsMode() = 0;

	// Multiplayer breakable spawn behavior
	virtual void		SetMultiplayerBreakMode(EMultiplayerBreak mode) = 0;
	virtual EMultiplayerBreak GetMultiplayerBreakMode() const = 0;

	// Used for debugging
	virtual void		SetBasePropData(string_t iszBase) = 0;
	virtual string_t	GetBasePropData() = 0;
};
#pragma endregion
