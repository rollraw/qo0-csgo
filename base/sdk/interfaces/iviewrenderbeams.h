#pragma once
#include "../datatypes/vector.h"
#include "../datatypes/basehandle.h"

// used: cdefaultclientrenderable
#include "../entity.h"

// @source: master/game/client/iviewrender_beams.h

#pragma region renderbeams_enumerations
enum EBeamType : int
{
	TE_BEAMPOINTS =			0, // beam effect between two points
	TE_SPRITE =				1, // additive sprite, plays 1 cycle
	TE_BEAMDISK =			2, // disk that expands to max radius over lifetime
	TE_BEAMCYLINDER =		3, // cylinder that expands to max radius over lifetime
	TE_BEAMFOLLOW =			4, // create a line of decaying beam segments until entity stops moving
	TE_BEAMRING =			5, // connect a beam ring to two entities
	TE_BEAMSPLINE =			6,
	TE_BEAMRINGPOINT =		7,
	TE_BEAMLASER =			8, // fades according to viewpoint
	TE_BEAMTESLA =			9,
	MAX_BEAM_ENTITIES = 	10,
	NOISE_DIVISIONS =		128
};

enum EBeamFlags : int
{
	FBEAM_NONE =			0,
	FBEAM_STARTENTITY =		(1 << 0),
	FBEAM_ENDENTITY =		(1 << 1),
	FBEAM_FADEIN =			(1 << 2),
	FBEAM_FADEOUT =			(1 << 3),
	FBEAM_SINENOISE =		(1 << 4),
	FBEAM_SOLID =			(1 << 5),
	FBEAM_SHADEIN =			(1 << 6),
	FBEAM_SHADEOUT =		(1 << 7),
	FBEAM_ONLYNOISEONCE =	(1 << 8),  // only calculate our noise once
	FBEAM_NOTILE =			(1 << 9),
	FBEAM_USE_HITBOXES =	(1 << 10), // attachment indices represent hitbox indices instead when this is set
	FBEAM_STARTVISIBLE =	(1 << 11), // has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE =		(1 << 12), // has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE =		(1 << 13),
	FBEAM_FOREVER =			(1 << 14),
	FBEAM_HALOBEAM =		(1 << 15), // when drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED =		(1 << 16)
};
#pragma endregion

// forward declarations
class CBaseEntity;
class CBeam; // not implemented
class ITraceFilter;
struct RenderableInstance_t;

#pragma pack(push, 4)
struct BeamTrail_t
{
	BeamTrail_t* pNext; // 0x00
	float flDie; // 0x04
	Vector_t vecOrigin; // 0x08
	Vector_t vecVelocity; // 0x14
};
static_assert(sizeof(BeamTrail_t) == 0x20);

// @source: master/game/client/beamdraw.h
struct Beam_t : CDefaultClientRenderable
{
	Vector_t vecMins = { }; // 0x000C
	Vector_t vecMaxs = { }; // 0x0018
	int* pQueryHandleHalo = nullptr; // 0x0024
	float flHaloProxySize = 0.0f; // 0x0028
	Beam_t* pNext = nullptr; // 0x002C
	int nType = TE_BEAMPOINTS; // 0x0030
	int nFlags = FBEAM_NONE; // 0x0034

	// control points for the beam
	int nAttachments = 0; // 0x0038
	Vector_t arrAttachments[MAX_BEAM_ENTITIES] = { }; // 0x003C
	Vector_t vecDelta = { }; // 0x00B4

	// 0 .. 1 over lifetime of beam
	float flTime = 0.0f; // 0x00C0
	float flFrequence = 0.0f; // 0x00C4

	// time when beam should die
	float flDie = -1.0f; // 0x00C8
	float flWidth = 1.0f; // 0x00CC
	float flEndWidth = 1.0f; // 0x00D0
	float flFadeLength = 0.0f; // 0x00D4
	float flAmplitude = 0.0f; // 0x00D8
	float flLife = -1.0f; // 0x00DC

	// color
	float r = 1.0f; // 0x00E0
	float g = 1.0f; // 0x00E4
	float b = 1.0f; // 0x00E8
	float flBrightness = 1.0f; // 0x00EC

	// speed
	float flSpeed = 1.0f; // 0x00F0

	// animation
	float flFrameRate = 0.0f; // 0x00F4
	float flFrame = 0.0f; // 0x00F8
	int nSegments = -1; // 0x00FC

	// attachment entities for the beam
	CBaseHandle arrAttachmentEntities[MAX_BEAM_ENTITIES] = { }; // 0x0100
	int arrAttachmentIndicies[MAX_BEAM_ENTITIES] = { }; // 0x0128

	// model info
	int nModelIndex; // 0x0150
	int nHaloIndex; // 0x0154
	float flHaloScale; // 0x0158
	int iFrameCount; // 0x015C
	float flRgNoise[NOISE_DIVISIONS + 1]; // 0x0160

	// trail for beam follows to use
	BeamTrail_t* pTrail; // 0x0364

	// for 'TE_BEAMRINGPOINT'
	float flStartRadius; // 0x0368
	float flEndRadius; // 0x036C

	// for 'FBEAM_ONLYNOISEONCE'
	bool bCalculatedNoise; // 0x0370
	float flHDRColorScale; // 0x0374
};
static_assert(sizeof(Beam_t) == 0x378); // size verify @ida: (CViewRenderBeams::BeamAlloc) client.dll -> [ABS["E8 ? ? ? ? 8B F8 85 FF 74 76 8B 0D" + 0x1] + 0x1E]

struct BeamInfo_t
{
	int nType = TE_BEAMPOINTS; // 0x00

	// entities
	CBaseEntity* pStartEntity = nullptr; // 0x04
	int iStartAttachment = -1; // 0x08
	CBaseEntity* pEndEntity = nullptr; // 0x0C
	int iEndAttachment = -1; // 0x10

	// points
	Vector_t vecStart = { }; // 0x14
	Vector_t vecEnd = { }; // 0x20

	int nModelIndex = -1; // 0x2C
	const char* szModelName = nullptr; // 0x30
	int nHaloIndex = -1; // 0x34
	const char* szHaloName = nullptr; // 0x38
	float flHaloScale = 1.0f; // 0x3C
	float flLife = -1.0f; // 0x40
	float flWidth = 1.0f; // 0x44
	float flEndWidth = 1.0; // 0x48
	float flFadeLength = 0.0f; // 0x4C
	float flAmplitude = 0.0f; // 0x50
	float flBrightness = 1.0f; // 0x54
	float flSpeed = 0.0f; // 0x58
	int iStartFrame = 0; // 0x5C
	float flFrameRate = 0.0f; // 0x60
	float flRed = 1.0f; // 0x64
	float flGreen = 1.0f; // 0x68
	float flBlue = 1.0f; // 0x6C
	bool bRenderable = true; // 0x70
	int nSegments = -1; // 0x74
	int nFlags = FBEAM_NONE; // 0x78

	// rings
	Vector_t vecCenter = { }; // 0x78
	float flStartRadius = 0.0f; // 0x88
	float flEndRadius = 0.0f; // 0x8C
};
static_assert(sizeof(BeamInfo_t) == 0x90);
#pragma pack(pop)

class IViewRenderBeams : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	void DrawBeam(Beam_t* pBeam)
	{
		CallVFunc<void, 4U>(this, pBeam);
	}

	void DrawBeam(CBeam* pBeam, const RenderableInstance_t& instance, ITraceFilter* pEntityBeamTraceFilter = nullptr)
	{
		CallVFunc<void, 5U>(this, pBeam, &instance, pEntityBeamTraceFilter);
	}

	[[nodiscard]] Beam_t* CreateBeamEnts(BeamInfo_t& beamInfo)
	{
		return CallVFunc<Beam_t*, 8U>(this, &beamInfo);
	}

	[[nodiscard]] Beam_t* CreateBeamEntPoint(BeamInfo_t& beamInfo)
	{
		return CallVFunc<Beam_t*, 10U>(this, &beamInfo);
	}

	[[nodiscard]] Beam_t* CreateBeamPoints(BeamInfo_t& beamInfo)
	{
		return CallVFunc<Beam_t*, 12U>(this, &beamInfo);
	}

	[[nodiscard]] Beam_t* CreateBeamRing(BeamInfo_t& beamInfo)
	{
		return CallVFunc<Beam_t*, 14U>(this, &beamInfo);
	}

	[[nodiscard]] Beam_t* CreateBeamRingPoint(BeamInfo_t& beamInfo)
	{
		return CallVFunc<Beam_t*, 16U>(this, &beamInfo);
	}

	[[nodiscard]] Beam_t* CreateBeamCirclePoints(BeamInfo_t& beamInfo)
	{
		return CallVFunc<Beam_t*, 18U>(this, &beamInfo);
	}

	[[nodiscard]] Beam_t* CreateBeamFollow(BeamInfo_t& beamInfo)
	{
		return CallVFunc<Beam_t*, 20U>(this, &beamInfo);
	}

	void FreeBeam(Beam_t* pBeam)
	{
		CallVFunc<void, 21U>(this, pBeam);
	}

	void UpdateBeamInfo(Beam_t* pBeam, BeamInfo_t& beamInfo)
	{
		CallVFunc<void, 22U>(this, pBeam, &beamInfo);
	}
};
