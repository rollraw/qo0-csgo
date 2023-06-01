#pragma once
#include "../datatypes/color.h"

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "iglobalvars.h"

#define END_OF_FREE_LIST (-1)
#define ENTRY_IN_USE (-2)

enum EGlowRenderStyle : int
{
	GLOWRENDERSTYLE_DEFAULT = 0,
	GLOWRENDERSTYLE_RIMGLOW3D,
	GLOWRENDERSTYLE_EDGE_HIGHLIGHT,
	GLOWRENDERSTYLE_EDGE_HIGHLIGHT_PULSE,
	GLOWRENDERSTYLE_COUNT
};

#pragma pack(push, 4)
class IGlowObjectManager
{
public:
	struct GlowObject_t
	{
		[[nodiscard]] Q_INLINE bool IsEmpty() const
		{
			return nNextFreeSlot != ENTRY_IN_USE;
		}

		int nNextFreeSlot; // 0x00
		CBaseEntity* pEntity; // 0x04
		float arrColor[4]; // 0x08
		bool bAlphaCappedByRenderAlpha; // 0x18
		float flAlphaFunctionOfMaxVelocity; // 0x1C
		float flBloomAmount; // 0x20
		float flPulseOverdrive; // 0x24
		bool bRenderWhenOccluded; // 0x28
		bool bRenderWhenUnoccluded; // 0x29
		bool bFullBloomRender; // 0x2A
		int iFullBloomStencilTestValue; // 0x2C
		int nRenderStyle; // 0x30
		int nSplitScreenSlot; // 0x34
	};
	static_assert(sizeof(GlowObject_t) == 0x38);

	struct GlowBoxObject_t
	{
		Vector_t vecPosition; // 0x00
		QAngle_t angOrientation; // 0x0C
		Vector_t vecMins; // 0x18
		Vector_t vecMaxs; // 0x24
		float flBirthTimeIndex; // 0x30
		float flTerminationTimeIndex; // 0x34
		Color_t colBox; // 0x38
	};
	static_assert(sizeof(GlowBoxObject_t) == 0x3C);

	int RegisterGlowBox(const Vector_t& vecOrigin, const QAngle_t& angOrientation, const Vector_t& vecMins, const Vector_t& vecMaxs, const Color_t& colBox, const float flLifetime)
	{
		// @ida CGlowObjectManager::AddGlowBox(): client.dll -> "55 8B EC 53 56 8D 59"

		const int nIndex = vecGlowBoxDefinitions.AddToTail();
		GlowBoxObject_t& boxObject = vecGlowBoxDefinitions[nIndex];
		boxObject.vecPosition = vecOrigin;
		boxObject.angOrientation = angOrientation;
		boxObject.vecMins = vecMins;
		boxObject.vecMaxs = vecMaxs;
		boxObject.colBox = colBox;
		boxObject.flBirthTimeIndex = I::Globals->flCurrentTime;
		boxObject.flTerminationTimeIndex = I::Globals->flCurrentTime + flLifetime;
		return nIndex;
	}

	void UnregisterAllGlowBoxes()
	{
		vecGlowBoxDefinitions.RemoveAll();
	}

	CUtlVector<GlowObject_t> vecGlowObjectDefinitions; // 0x00
	int nFirstFreeSlot; // 0x14
	CUtlVector<GlowBoxObject_t> vecGlowBoxDefinitions; // 0x18
};
static_assert(sizeof(IGlowObjectManager) == 0x2C);
#pragma pack(pop)
