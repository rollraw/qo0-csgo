#pragma once
#include "../datatypes/color.h"
#include "../datatypes/vector.h"
#include "../datatypes/qangle.h"

// used: callvfunc
#include "../../utilities/memory.h"

#define MAX_DLIGHTS 32

#pragma region engineeffects_enumerations
enum
{
	DLIGHT_NO_WORLD_ILLUMINATION = 0x1,
	DLIGHT_NO_MODEL_ILLUMINATION = 0x2,
	DLIGHT_ADD_DISPLACEMENT_ALPHA = 0x4,
	DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA = 0x8,
	DLIGHT_DISPLACEMENT_MASK = (DLIGHT_ADD_DISPLACEMENT_ALPHA | DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA)
};
#pragma endregion

// forward declarations
class IMaterial;
struct Model_t;

struct DLight_t
{
	int iFlags; // 0x00
	Vector_t vecOrigin; // 0x04
	float flRadius; // 0x10
	ColorRGBExp32 color; // 0x14
	float flDie; // 0x18
	float flDecay; // 0x1C
	float flMinLight; // 0x20
	int iKey; // 0x24
	int iStyle; // 0x28
	Vector_t vecDirection; // 0x2C
	float flInnerAngle; // 0x38
	float flOuterAngle; // 0x3C
};
static_assert(sizeof(DLight_t) == 0x40);

class IVEngineEffects : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	[[nodiscard]] DLight_t* CL_AllocDlight(int nKeyIndex)
	{
		return CallVFunc<DLight_t*, 4U>(this, nKeyIndex);
	}

	[[nodiscard]] DLight_t* CL_AllocElight(int nKeyIndex)
	{
		return CallVFunc<DLight_t*, 5U>(this, nKeyIndex);
	}

	[[nodiscard]] int CL_GetActiveDLights(DLight_t* pList[MAX_DLIGHTS])
	{
		return CallVFunc<int, 6U>(this, pList);
	}
};
