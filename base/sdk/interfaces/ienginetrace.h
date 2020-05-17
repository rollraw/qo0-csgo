#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/IEngineTrace.h

// used: matrix3x4_t
#include "../datatypes/matrix.h"
// used: utlvector
#include "../datatypes/utlvector.h"
// used: mask, content, surf flags
#include "../bspflags.h"

#pragma region enginetrace_definitions

#pragma endregion

#pragma region enginetrace_enumerations
enum EDispSurfFlags
{
	DISPSURF_FLAG_SURFACE =		(1 << 0),
	DISPSURF_FLAG_WALKABLE =	(1 << 1),
	DISPSURF_FLAG_BUILDABLE =	(1 << 2),
	DISPSURF_FLAG_SURFPROP1 =	(1 << 3),
	DISPSURF_FLAG_SURFPROP2 =	(1 << 4)
};
enum ETraceType
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
	TRACE_FILTERSKY
};

enum EDebugTraceCounterBehavior
{
	TRACE_COUNTER_SET = 0,
	TRACE_COUNTER_INC,
};
#pragma endregion

struct BrushSideInfo_t
{
	VectorAligned	vecPlane;	// the plane of the brush side
	std::uint16_t	uBevel;		// bevel plane?
	std::uint16_t	uThin;		// thin?
};

struct cplane_t
{
	Vector		vecNormal;
	float		flDistance;
	std::byte	dType;
	std::byte	dSignBits;
	std::byte	pad[0x2];
};

struct csurface_t
{
	const char*		szName;
	short			nSurfaceProps;
	std::uint16_t	uFlags;
};

class CBaseTrace
{
public:
	CBaseTrace() { }

	Vector			vecStart;		// start position
	Vector			vecEnd;			// final position
	cplane_t		plane;			// surface normal at impact
	float			flFraction;		// time completed, 1.0 = didn't hit anything
	int				iContents;		// contents on other side of surface hit
	std::uint16_t	fDispFlags;		// displacement flags for marking surfaces with data
	bool			bAllSolid;		// if true, plane is not valid
	bool			bStartSolid;	// if true, the initial point was in a solid area
};

class CBaseEntity;
class CGameTrace : public CBaseTrace
{
public:
	CGameTrace() { }

	CGameTrace& operator=(const CGameTrace& other)
	{
		vecStart = other.vecStart;
		vecEnd = other.vecEnd;
		plane = other.plane;
		flFraction = other.flFraction;
		iContents = other.iContents;
		fDispFlags = other.fDispFlags;
		bAllSolid = other.bAllSolid;
		bStartSolid = other.bStartSolid;
		flFractionLeftSolid = other.flFractionLeftSolid;
		surface = other.surface;
		iHitGroup = other.iHitGroup;
		sPhysicsBone = other.sPhysicsBone;
		uWorldSurfaceIndex = other.uWorldSurfaceIndex;
		pHitEntity = other.pHitEntity;
		iHitbox = other.iHitbox;
		return *this;
	}

	float				flFractionLeftSolid;	// time we left a solid, only valid if we started in solid
	csurface_t			surface;				// surface hit (impact surface)
	int					iHitGroup;				// 0 == generic, non-zero is specific body part
	short				sPhysicsBone;			// physics bone hit by trace in studio
	std::uint16_t		uWorldSurfaceIndex;		// index of the msurface2_t, if applicable
	CBaseEntity*		pHitEntity;				// entity hit by trace
	int					iHitbox;				// box hit by trace in studio
	
	inline bool DidHit() const
	{
		return (flFraction < 1.0f || bAllSolid || bStartSolid);
	}

	inline bool IsVisible() const
	{
		return (flFraction > 0.97f);
	}

private:
	CGameTrace(const CGameTrace& vOther);
};

using Trace_t = CGameTrace;

struct Ray_t
{
	Ray_t() : matWorldAxisTransform(nullptr) { }

	VectorAligned		vecStart;
	VectorAligned		vecDelta;
	VectorAligned		vecStartOffset;
	VectorAligned		vecExtents;
	const matrix3x4_t*	matWorldAxisTransform;
	bool				bIsRay;
	bool				bIsSwept;

	void Init(const Vector& vecStart, const Vector& vecEnd)
	{
		this->vecDelta = vecEnd - vecStart;
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);
		this->vecExtents.Init();
		this->matWorldAxisTransform = nullptr;
		this->bIsRay = true;
		this->vecStartOffset.Init();
		this->vecStart = vecStart;
	}
};

class IHandleEntity;
class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntity, int fContentsMask) = 0;
	virtual ETraceType GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int fContentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	virtual ETraceType GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilterSkipEntity : public ITraceFilter
{
public:
	CTraceFilterSkipEntity(IHandleEntity* pEntityHandle) :
		pSkip(pEntityHandle) { }

	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int fContentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	virtual ETraceType GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
    CTraceFilterSkipTwoEntities(void* pFirstEntity, void* pSecondEntity) :
		pSkip1(pFirstEntity), pSkip2(pSecondEntity) { }

	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int fContentsMask)
    {
        return !(pEntityHandle == pSkip1 || pEntityHandle == pSkip2);
    }

    virtual ETraceType GetTraceType() const
    {
        return TRACE_EVERYTHING;
    }

	void* pSkip1;
	void* pSkip2;
};

class CTraceFilterEntitiesOnly : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int fContentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	virtual ETraceType GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}

	void* pSkip;
};

class CTraceFilterWorldOnly : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int fContentsMask)
	{
		return false;
	}

	virtual ETraceType GetTraceType() const
	{
		return TRACE_WORLD_ONLY;
	}
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int fContentsMask)
	{
		return false;
	}

	virtual ETraceType GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

class ITraceListData
{
public:
	virtual			~ITraceListData() { }
	virtual void	Reset() = 0;
	virtual bool	IsEmpty() = 0;
	virtual bool	CanTraceRay(const Ray_t& ray) = 0;
};

class IEntityEnumerator
{
public:
	// This gets called with each handle
	virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};

struct virtualmeshlist_t;
class ICollideable;
class CPhysCollide;
class IEngineTrace
{
public:
	virtual int GetPointContents(const Vector& vecAbsPosition, int fContentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr) = 0;
	virtual int GetPointContents_WorldOnly(const Vector& vecAbsPosition, int fContentsMask = MASK_ALL) = 0;
	virtual int GetPointContents_Collideable(ICollideable* pCollide, const Vector& vecAbsPosition) = 0;
	virtual void ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEntity, Trace_t* pTrace) = 0;
	virtual void ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, Trace_t* pTrace) = 0;
	virtual void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SetupLeafAndEntityListRay(const Ray_t& ray, ITraceListData* pTraceData) = 0;
	virtual void SetupLeafAndEntityListBox(const Vector& vecBoxMin, const Vector& vecBoxMax, ITraceListData* pTraceData) = 0;
	virtual void TraceRayAgainstLeafAndEntityList(const Ray_t& ray, ITraceListData* pTraceData, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SweepCollideable(ICollideable* pCollide, const Vector& vecAbsStart, const Vector& vecAbsEnd, const Vector& vecAngles, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void EnumerateEntities(const Ray_t& ray, bool bTriggers, IEntityEnumerator* pEnumerator) = 0;
	virtual void EnumerateEntities(const Vector& vecAbsMins, const Vector& vecAbsMaxs, IEntityEnumerator* pEnumerator) = 0;
	virtual ICollideable* GetCollideable(IHandleEntity* pEntity) = 0;
	virtual int GetStatByIndex(int nIndex, bool bClear) = 0;
	virtual void GetBrushesInAABB(const Vector& vecMins, const Vector& vecMaxs, CUtlVector<int>* pOutput, int fContentsMask = MASK_ALL) = 0;
	virtual CPhysCollide* GetCollidableFromDisplacementsInAABB(const Vector& vecMins, const Vector& vecMaxs) = 0;
	virtual int GetNumDisplacements() = 0;
	virtual void GetDisplacementMesh(int nIndex, virtualmeshlist_t* pMeshTriList) = 0;
	virtual bool GetBrushInfo(int iBrush, CUtlVector<BrushSideInfo_t>* pBrushSideInfoOut, int* pContentsOut) = 0;
	virtual bool PointOutsideWorld(const Vector& vecPoint) = 0;
	virtual int GetLeafContainingPoint(const Vector& vecPoint) = 0;
	virtual ITraceListData* AllocTraceListData() = 0;
	virtual void FreeTraceListData(ITraceListData* pListData) = 0;
	virtual int GetSetDebugTraceCounter(int iValue, EDebugTraceCounterBehavior behavior) = 0;
};
