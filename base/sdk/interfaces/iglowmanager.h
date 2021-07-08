#pragma once
// used: color
#include "../datatypes/color.h"

#define END_OF_FREE_LIST -1
#define ENTRY_IN_USE -2

enum EGlowRenderStyle : int
{
	GLOWRENDERSTYLE_DEFAULT = 0,
	GLOWRENDERSTYLE_RIMGLOW3D,
	GLOWRENDERSTYLE_EDGE_HIGHLIGHT,
	GLOWRENDERSTYLE_EDGE_HIGHLIGHT_PULSE,
	GLOWRENDERSTYLE_COUNT
};

class IGlowObjectManager
{
public:
	struct GlowObject_t
	{
		void Set(const Color& colGlow, const int nRenderStyle = GLOWRENDERSTYLE_DEFAULT) // @note: styles not used cuz other styles doesnt have ignorez flag and needed to rebuild glow
		{
			this->arrColor = colGlow.BaseAlpha();
			this->flBloomAmount = 1.0f;
			this->bRenderWhenOccluded = true;
			this->bRenderWhenUnoccluded = false;
			this->nRenderStyle = nRenderStyle;
		}

		inline bool IsEmpty() const
		{
			return nNextFreeSlot != ENTRY_IN_USE;
		}

		int						nNextFreeSlot;					// 0x00
		CBaseEntity*			pEntity;						// 0x04
		std::array<float, 4U>	arrColor;						// 0x08
		bool					bAlphaCappedByRenderAlpha;		// 0x18
		std::byte				pad0[0x3];						// 0x19 - pack 1 bool as 4 bytes
		float					flAlphaFunctionOfMaxVelocity;	// 0x1C
		float					flBloomAmount;					// 0x20
		float					flPulseOverdrive;				// 0x24
		bool					bRenderWhenOccluded;			// 0x28
		bool					bRenderWhenUnoccluded;			// 0x29
		bool					bFullBloomRender;				// 0x2A
		std::byte				pad1[0x1];						// 0x2B  - pack 3 bool as 4 bytes
		int						iFullBloomStencilTestValue;		// 0x2C
		int						nRenderStyle;					// 0x30
		int						nSplitScreenSlot;				// 0x34
	}; // Size: 0x38

	struct GlowBoxObject_t
	{
		Vector	vecPosition;
		QAngle	angOrientation;
		Vector	vecMins;
		Vector	vecMaxs;
		float	flBirthTimeIndex;
		float	flTerminationTimeIndex;
		Color	colColor;
	};

	CUtlVector<GlowObject_t> vecGlowObjectDefinitions;
	int nFirstFreeSlot;
	CUtlVector<GlowBoxObject_t> vecGlowBoxDefinitions;
};
