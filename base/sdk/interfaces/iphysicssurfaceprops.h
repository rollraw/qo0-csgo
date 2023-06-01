#pragma once
// used: MaterialHandle_t
#include "imaterialsystem.h"

#pragma region physicssurfaceprops_definitions
// @source: master/game/shared/decals.h
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
#define CHAR_TEX_FAKE			'X'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z'
#define CHAR_TEX_SANDBARREL		'\xC'
#pragma endregion

// @source: master/public/vphysics_interface.h

struct surfacephysicsparams_t
{
	float flFriction; // 0x00
	float flElasticity; // 0x04
	float flDensity; // 0x08
	float flThickness; // 0x0C
	float flDampening; // 0x10
};
static_assert(sizeof(surfacephysicsparams_t) == 0x14);

struct surfaceaudioparams_t
{
	float flReflectivity; // 0x00 // like elasticity, but how much sound should be reflected by this surface
	float flHardnessFactor; // 0x04 // like elasticity, but only affects impact sound choices
	float flRoughnessFactor; // 0x08 // like friction, but only affects scrape sound choices
	float flRoughThreshold; // 0x0C // surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float flHardThreshold; // 0x10 // surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float flHardVelocityThreshold; // 0x14 // collision velocity > this causes "hard" impacts, < this causes "soft" impacts
	float flHighPitchOcclusion; // 0x18 // a value betweeen 0 and 100 where 0 is not occluded at all and 100 is silent (except for any additional reflected sound)
	float flMidPitchOcclusion; // 0x1C
	float flLowPitchOcclusion; // 0x20
};
static_assert(sizeof(surfaceaudioparams_t) == 0x24);

struct surfacesoundnames_t
{
	std::uint16_t uWalkStepLeft; // 0x00
	std::uint16_t uWalkStepRight; // 0x02
	std::uint16_t uRunStepLeft; // 0x04
	std::uint16_t uRunStepRight; // 0x06
	std::uint16_t uImpactSoft; // 0x08
	std::uint16_t uImpactHard; // 0x0A
	std::uint16_t uScrapeSmooth; // 0x0C
	std::uint16_t uScrapeRough; // 0x0E
	std::uint16_t uBulletImpact; // 0x10
	std::uint16_t uRolling; // 0x12
	std::uint16_t uBreakSound; // 0x14
	std::uint16_t uStrainSound; // 0x16
};
static_assert(sizeof(surfacesoundnames_t) == 0x18);

struct surfacesoundhandles_t
{
	std::uint16_t uWalkStepLeft; // 0x00
	std::uint16_t uWalkStepRight; // 0x02
	std::uint16_t uRunStepLeft; // 0x04
	std::uint16_t uRunStepRight; // 0x06
	std::uint16_t uImpactSoft; // 0x08
	std::uint16_t uImpactHard; // 0x0A
	std::uint16_t uScrapeSmooth; // 0x0C
	std::uint16_t uScrapeRough; // 0x0E
	std::uint16_t uBulletImpact; // 0x10
	std::uint16_t uRolling; // 0x12
	std::uint16_t uBreakSound; // 0x14
	std::uint16_t uStrainSound; // 0x16
};
static_assert(sizeof(surfacesoundhandles_t) == 0x18);

struct surfacegameprops_t
{
	float flMaxSpeedFactor; // 0x00
	float flJumpFactor; // 0x04
	float flPenetrationModifier; // 0x08
	float flDamageModifier; // 0x0C
	MaterialHandle_t hMaterial; // 0x10
	unsigned char dClimbable; // 0x12
	std::byte pad0[0x5]; // 0x13
};
static_assert(sizeof(surfacegameprops_t) == 0x18);

struct surfacedata_t
{
	surfacephysicsparams_t physics; // 0x00
	surfaceaudioparams_t audio; // 0x14
	surfacesoundnames_t sounds; // 0x38
	surfacegameprops_t game; // 0x50
	surfacesoundhandles_t soundhandles; // 0x68
};
static_assert(sizeof(surfacedata_t) == 0x80);

class ISaveRestoreOps;
class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps() { }
	virtual int ParseSurfaceData(const char* szFileName, const char* szTextFile) = 0;
	virtual int SurfacePropCount() const = 0;
	virtual int GetSurfaceIndex(const char* szSurfacePropName) const = 0;
	virtual void GetPhysicsProperties(int iSurfaceDataIndex, float* flDensity, float* flThickness, float* flFriction, float* flElasticity) const = 0;
	virtual surfacedata_t* GetSurfaceData(int iSurfaceDataIndex) = 0;
	virtual const char* GetString(unsigned short shStringTableIndex) const = 0;
	virtual const char* GetPropName(int iSurfaceDataIndex) const = 0;
	virtual void SetWorldMaterialIndexTable(int* iMapArray, int iMapSize) = 0;
	virtual void GetPhysicsParameters(int iSurfaceDataIndex, surfacephysicsparams_t* pParamsOut) const = 0;
	virtual ISaveRestoreOps* GetMaterialIndexDataOps() const = 0;
};
