#pragma once
// @source: master/game/shared/props_shared.h

#pragma region breakablewithpropdata_enumerations
enum EPropertyDataInteractions : int
{
	PROPINTER_PHYSGUN_WORLD_STICK = 0,	// "onworldimpact"		"stick"
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
	PROPINTER_WORLD_BLOODSPLAT,			// "onworldimpact"		"bloodsplat"
	PROPINTER_PHYSGUN_NOTIFY_CHILDREN,	// "onfirstimpact" - cause attached flechettes to explode
	PROPINTER_MELEE_IMMUNE,				// "melee_immune"		"yes"
	PROPINTER_NUM_INTERACTIONS			// if we get more than 32 of these, we'll need a different system
};

enum EMultiplayerPhysicsMode : int
{
	PHYSICS_MULTIPLAYER_AUTODETECT = 0,	// use multiplayer physics mode as defined in model prop data
	PHYSICS_MULTIPLAYER_SOLID,			// solid, pushes player away
	PHYSICS_MULTIPLAYER_NON_SOLID,		// nonsolid, but pushed by player
	PHYSICS_MULTIPLAYER_CLIENTSIDE		// clientside only, nonsolid
};

enum EMultiplayerBreak : int
{
	MULTIPLAYER_BREAK_DEFAULT = 0,
	MULTIPLAYER_BREAK_SERVERSIDE,
	MULTIPLAYER_BREAK_CLIENTSIDE,
	MULTIPLAYER_BREAK_BOTH
};
#pragma endregion

class IMultiplayerPhysics
{
public:
	virtual int GetMultiplayerPhysicsMode() = 0;
	virtual float GetMass() = 0;
	virtual bool IsAsleep() = 0;
};

class IBreakableWithPropData
{
public:
	// damage modifiers
	virtual void SetDmgModBullet(float flDmgMod) = 0;
	virtual void SetDmgModClub(float flDmgMod) = 0;
	virtual void SetDmgModExplosive(float flDmgMod) = 0;
	virtual float GetDmgModBullet() = 0;
	virtual float GetDmgModClub() = 0;
	virtual float GetDmgModExplosive() = 0;
	virtual float GetDmgModFire() = 0;

	// explosive
	virtual void SetExplosiveRadius(float flRadius) = 0;
	virtual void SetExplosiveDamage(float flDamage) = 0;
	virtual float GetExplosiveRadius() = 0;
	virtual float GetExplosiveDamage() = 0;

	// physics damage tables
	virtual void SetPhysicsDamageTable(const char* szTableName) = 0;
	virtual const char* GetPhysicsDamageTable() = 0;

	// breakable chunks
	virtual void SetBreakableModel(const char* szModel) = 0;
	virtual const char* GetBreakableModel() = 0;
	virtual void SetBreakableSkin(int iSkin) = 0;
	virtual int GetBreakableSkin() = 0;
	virtual void SetBreakableCount(int iCount) = 0;
	virtual int GetBreakableCount() = 0;
	virtual void SetMaxBreakableSize(int iSize) = 0;
	virtual int GetMaxBreakableSize() = 0;

	// LOS blocking
	virtual void SetPropDataBlocksLOS(bool bBlocksLOS) = 0;
	virtual void SetPropDataIsAIWalkable(bool bBlocksLOS) = 0;

	// interactions
	virtual void SetInteraction(EPropertyDataInteractions Interaction) = 0;
	virtual bool HasInteraction(EPropertyDataInteractions Interaction) = 0;

	// multiplayer physics mode
	virtual void SetPhysicsMode(int iMode) = 0;
	virtual int GetPhysicsMode() = 0;

	// multiplayer breakable spawn behavior
	virtual void SetMultiplayerBreakMode(EMultiplayerBreak mode) = 0;
	virtual EMultiplayerBreak GetMultiplayerBreakMode() const = 0;

	// used for debugging
	virtual void SetBasePropData(const char* szBase) = 0;
	virtual const char* GetBasePropData() = 0;
};
