#pragma once
#include "../datatypes/matrix.h"
#include "../datatypes/utlvector.h"

// used: mask, content, surf flags
#include "../bspflags.h"

// @credits: master/public/engine/IEngineTrace.h

#pragma region enginetrace_enumerations
enum EDispSurfFlags : int
{
	DISPSURF_FLAG_SURFACE =		(1 << 0),
	DISPSURF_FLAG_WALKABLE =	(1 << 1),
	DISPSURF_FLAG_BUILDABLE =	(1 << 2),
	DISPSURF_FLAG_SURFPROP1 =	(1 << 3),
	DISPSURF_FLAG_SURFPROP2 =	(1 << 4)
};

enum ETraceType : int
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
	TRACE_FILTERSKY
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
	COLLISION_GROUP_LAST_SHARED
};
#pragma endregion

// forward declarations
class ICollideable;
class IHandleEntity;
class CBaseEntity;

#pragma pack(push, 4)
struct BrushSideInfo_t
{
	VectorAligned_t vecPlane; // 0x00
	std::uint16_t uBevel; // 0x1C
	std::uint16_t uThin; // 0x1E
};
static_assert(sizeof(BrushSideInfo_t) == 0x20);

struct CPlane_t
{
	CPlane_t() = default;

	Vector_t vecNormal = { }; // 0x00
	float flDistance = 0.0f; // 0x0C
	std::uint8_t nType = 0U; // 0x10
	std::uint8_t uSignBits = 0U; // 0x11
};
static_assert(sizeof(CPlane_t) == 0x14);

struct CSurface_t
{
	CSurface_t() = default;

	const char* szName = nullptr; // 0x00
	short nSurfaceProps = 0; // 0x04
	std::uint16_t uFlags = 0U; // 0x06
};
static_assert(sizeof(CSurface_t) == 0x8);

class CBaseTrace
{
public:
	CBaseTrace() = default;

	Vector_t vecStart = { }; // 0x00 // start position
	Vector_t vecEnd = { }; // 0x0C // final position
	CPlane_t plane = { }; // 0x18 // surface normal at impact
	float flFraction = 0.0f; // 0x2C // time completed, 1.0 = didn't hit anything
	int iContents = 0; // 0x30 // contents on other side of surface hit
	std::uint16_t uDispFlags = 0U; // 0x34 // displacement flags for marking surfaces with data
	bool bAllSolid = false; // 0x36 // if true, plane is not valid
	bool bStartSolid = false; // 0x37 // if true, the initial point was in a solid area
};
static_assert(sizeof(CBaseTrace) == 0x38);

class CGameTrace : public CBaseTrace
{
public:
	CGameTrace() = default;

	float flTimeFractionLeftSolid = 0.0f; // 0x00
	CSurface_t surface = { }; // 0x04
	int iHitGroup = 0; // 0x0C
	short shPhysicsBoneHit = 0; // 0x10
	std::uint16_t nWorldSurfaceIndex = 0U; // 0x12
	CBaseEntity* pHitEntity = nullptr; // 0x14
	int iHitBox = 0; // 0x18

	Q_INLINE bool DidHit() const
	{
		return (flFraction < 1.0f || bAllSolid || bStartSolid);
	}

	Q_INLINE bool IsVisible() const
	{
		return (flFraction > 0.97f);
	}

private:
	CGameTrace(const CGameTrace& other)
	{
		this->vecStart = other.vecStart;
		this->vecEnd = other.vecEnd;
		this->plane = other.plane;
		this->flFraction = other.flFraction;
		this->iContents = other.iContents;
		this->uDispFlags = other.uDispFlags;
		this->bAllSolid = other.bAllSolid;
		this->bStartSolid = other.bStartSolid;

		this->flTimeFractionLeftSolid = other.flTimeFractionLeftSolid;
		this->surface = other.surface;
		this->iHitGroup = other.iHitGroup;
		this->shPhysicsBoneHit = other.shPhysicsBoneHit;
		this->nWorldSurfaceIndex = other.nWorldSurfaceIndex;
		this->pHitEntity = other.pHitEntity;
		this->iHitBox = other.iHitBox;
	}
};
static_assert(sizeof(CGameTrace) == sizeof(CBaseTrace) + 0x1C);
using Trace_t = CGameTrace;

// functions used to verify offsets:
// @ida IEngineTrace::TraceRay(): engine.dll -> "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 56 8B F1" @xref: "TraceRay"
struct Ray_t
{
	Ray_t(const Vector_t& vecStart, const Vector_t& vecEnd) :
		vecStart(vecStart), vecDelta(vecEnd - vecStart), vecStartOffset(), vecExtents(), matWorldAxisTransform(nullptr), bIsRay(true), bIsSwept(vecDelta.LengthSqr() != 0.f) { }

	Ray_t(const Vector_t& vecStart, const Vector_t& vecEnd, const Vector_t& vecMins, const Vector_t& vecMaxs)
	{
		this->vecDelta = vecEnd - vecStart;

		this->matWorldAxisTransform = nullptr;
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);

		this->vecExtents = vecMaxs - vecMins;
		this->vecExtents *= 0.5f;
		this->bIsRay = (this->vecExtents.LengthSqr() < 1e-6f);

		this->vecStartOffset = vecMins + vecMaxs;
		this->vecStartOffset *= 0.5f;
		this->vecStart = vecStart + this->vecStartOffset;
		this->vecStartOffset *= -1.0f;
	}

	VectorAligned_t vecStart; // 0x00
	VectorAligned_t vecDelta; // 0x10
	VectorAligned_t vecStartOffset; // 0x20
	VectorAligned_t vecExtents; // 0x30
	const Matrix3x4_t* matWorldAxisTransform; // 0x40
	bool bIsRay; // 0x44 // @ida: engine.dll -> U8["F3 0F 11 47 ? 80 78 ? 00 75 32" + 0x8]
	bool bIsSwept; // 0x45
};
static_assert(sizeof(Ray_t) == 0x50);
#pragma pack(pop)

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntity, int iContentsMask) = 0;
	[[nodiscard]] virtual ETraceType GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	[[nodiscard]] ETraceType GetTraceType() const override
	{
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterEntitiesOnly : public ITraceFilter
{
public:
	[[nodiscard]] ETraceType GetTraceType() const override
	{
		return TRACE_ENTITIES_ONLY;
	}
};

class CTraceFilterWorldOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(IHandleEntity* pEntity, int iContentsMask) override
	{
		return false;
	}

	[[nodiscard]] ETraceType GetTraceType() const override
	{
		return TRACE_WORLD_ONLY;
	}
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(IHandleEntity* pEntity, int iContentsMask) override
	{
		return false;
	}

	[[nodiscard]] ETraceType GetTraceType() const override
	{
		return TRACE_EVERYTHING;
	}
};

// @source: master/game/shared/util_shared.h
using ShouldHitCallbackFn_t = bool(Q_CDECL*)(IHandleEntity* pHandleEntity, int iContentsMask);
class CTraceFilterSimple : public CTraceFilter
{
public:
	CTraceFilterSimple(const IHandleEntity* pSkipEntity, int nCollisionGroup = COLLISION_GROUP_NONE, ShouldHitCallbackFn_t pShouldHitCallback = nullptr) :
		pSkipEntity(pSkipEntity), nCollisionGroup(nCollisionGroup), pShouldHitCallback(pShouldHitCallback)
	{
		// @ida CTraceFilterSimple::_VTable: client.dll -> ["C7 45 ? ? ? ? ? F3 0F 59 D8 F3" + 0x3] @xref: 'sv_clip_penetration_traces_to_players'
	}

	bool ShouldHitEntity(IHandleEntity* pHandleEntity, int iContentsMask) override
	{
		static auto fnShouldHitEntity = reinterpret_cast<bool(Q_THISCALL*)(CTraceFilterSimple*, const IHandleEntity*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 8B 55 0C 56 8B 75 08 57")));
		return fnShouldHitEntity(this, pHandleEntity, iContentsMask);
	}

	[[nodiscard]] Q_INLINE const IHandleEntity* GetSkipEntity() const
	{
		return pSkipEntity;
	}

	[[nodiscard]] Q_INLINE int GetCollisionGroup() const
	{
		return nCollisionGroup;
	}

private:
	const IHandleEntity* pSkipEntity; // 0x00
	int nCollisionGroup; // 0x04
	ShouldHitCallbackFn_t pShouldHitCallback; // 0x08
};

class CTraceFilterSkipTwoEntities : public CTraceFilterSimple
{
public:
	CTraceFilterSkipTwoEntities(const IHandleEntity* pSkipEntity = nullptr, const IHandleEntity* pSkipEntity2 = nullptr, int nCollisionGroup = COLLISION_GROUP_NONE) :
		CTraceFilterSimple(pSkipEntity, nCollisionGroup), pSkipEntity2(pSkipEntity2)
	{
		// @ida CTraceFilterSkipTwoEntities::_VTable: client.dll -> ["C7 45 ? ? ? ? ? F3 0F 59 D8 F3" + 0x3] @xref: 'sv_clip_penetration_traces_to_players'
	}

	bool ShouldHitEntity(IHandleEntity* pHandleEntity, int iContentsMask) override
	{
		static auto fnShouldHitEntity = reinterpret_cast<bool(Q_THISCALL*)(CTraceFilterSkipTwoEntities*, const IHandleEntity*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 53 8B D9 56 57 8B 7D 08 8B 73 10")));
		return fnShouldHitEntity(this, pHandleEntity, iContentsMask);
	}

private:
	const IHandleEntity* pSkipEntity2;
};

class IEngineTrace : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	int GetPointContents(const Vector_t& vecAbsPosition, int uContentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr)
	{
		return CallVFunc<int, 0U>(this, &vecAbsPosition, uContentsMask, ppEntity);
	}

	int GetPointContents_WorldOnly(const Vector_t& vecAbsPosition, int uContentsMask = MASK_ALL)
	{
		return CallVFunc<int, 1U>(this, &vecAbsPosition, uContentsMask);
	}

	int GetPointContents_Collideable(ICollideable* pCollide, const Vector_t& vecAbsPosition)
	{
		return CallVFunc<int, 2U>(this, pCollide, &vecAbsPosition);
	}

	void ClipRayToEntity(const Ray_t& ray, unsigned int uMask, IHandleEntity* pEntity, Trace_t* pTrace)
	{
		CallVFunc<void, 3U>(this, &ray, uMask, pEntity, pTrace);
	}

	void ClipRayToCollideable(const Ray_t& ray, unsigned int uMask, ICollideable* pCollide, Trace_t* pTrace)
	{
		CallVFunc<void, 4U>(this, &ray, uMask, pCollide, pTrace);
	}

	void TraceRay(const Ray_t& ray, unsigned int uMask, ITraceFilter* pTraceFilter, Trace_t* pTrace)
	{
		CallVFunc<void, 5U>(this, &ray, uMask, pTraceFilter, pTrace);
	}

	bool GetBrushInfo(int iBrush, CUtlVector<BrushSideInfo_t>* pBrushSideInfoOut, int* pContentsOut)
	{
		return CallVFunc<bool, 18U>(this, iBrush, pBrushSideInfoOut, pContentsOut);
	}
};
