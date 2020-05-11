#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h

#include "ivmodelinfo.h"

#pragma region mdlcache_definitions
#define MDCACHE_FINE_GRAINED 1

#if defined(MDCACHE_FINE_GRAINED)
#define MDLCACHE_CRITICAL_SECTION_( pCache ) CMDLCacheCriticalSection cacheCriticalSection(pCache)
#define MDLCACHE_COARSE_LOCK_( pCache ) ((void)(0))
#elif defined(MDLCACHE_LEVEL_LOCKED)
#define MDLCACHE_CRITICAL_SECTION_( pCache ) ((void)(0))
#define MDLCACHE_COARSE_LOCK_( pCache ) ((void)(0))
#else
#define MDLCACHE_CRITICAL_SECTION_( pCache ) ((void)(0))
#define MDLCACHE_COARSE_LOCK_( pCache ) CMDLCacheCriticalSection cacheCriticalSection(pCache)
#endif
#define MDLCACHE_CRITICAL_SECTION() MDLCACHE_CRITICAL_SECTION_(mdlcache)
#define MDLCACHE_COARSE_LOCK() MDLCACHE_COARSE_LOCK_(mdlcache)
#pragma endregion

#pragma region mdlcache_enumerations
enum
{
	MDLHANDLE_INVALID = (MDLHandle_t)~0
};

enum MDLCacheDataType_t
{
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK,
};

enum MDLCacheFlush_t
{
	MDLCACHE_FLUSH_STUDIOHDR = 0x01,
	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
	MDLCACHE_FLUSH_VCOLLIDE = 0x04,
	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
	MDLCACHE_FLUSH_AUTOPLAY = 0x20,
	MDLCACHE_FLUSH_VERTEXES = 0x40,
	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};
#pragma endregion

class IMDLCacheNotify
{
public:
	virtual void OnDataLoaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;
	virtual void OnDataUnloaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;
};

class IMDLCache : public IAppSystem
{
public:
	virtual void SetCacheNotify(IMDLCacheNotify* pNotify) = 0;
	virtual MDLHandle_t FindMDL(const char* szMDLRelativePath) = 0; // @idx:10

	// Reference counting 
	virtual int AddRef(MDLHandle_t handle) = 0;
	virtual int Release(MDLHandle_t handle) = 0;
	virtual int GetRef(MDLHandle_t handle) = 0;
private:
	virtual void function0() = 0;
	virtual void function1() = 0;
public:
	virtual studiohdr_t* GetStudioHdr(MDLHandle_t handle) = 0;
	virtual studiohwdata_t* GetHardwareData(MDLHandle_t handle) = 0;
	virtual vcollide_t* GetVCollide(MDLHandle_t handle) = 0;
	virtual vcollide_t* GetVCollide(MDLHandle_t handle, float flScale) = 0;
	virtual unsigned char* GetAnimBlock(MDLHandle_t handle, int nBlock) = 0;
	virtual virtualmodel_t* GetVirtualModel(MDLHandle_t handle) = 0;
	virtual int GetAutoplayList(MDLHandle_t handle, unsigned short** pOut) = 0;
	virtual vertexfileheader_t* GetVertexData(MDLHandle_t handle) = 0;
	virtual void TouchAllData(MDLHandle_t handle) = 0;
	virtual void SetUserData(MDLHandle_t handle, void* pData) = 0;
	virtual void* GetUserData(MDLHandle_t handle) = 0;
	virtual bool IsErrorModel(MDLHandle_t handle) = 0;
	virtual void Flush(MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;
	virtual void Flush(MDLHandle_t handle, int nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;
	virtual const char* GetModelName(MDLHandle_t handle) = 0;
	virtual virtualmodel_t* GetVirtualModelFast(const studiohdr_t* pStudioHdr, MDLHandle_t handle) = 0;

	// all cache entries that subsequently allocated or successfully checked 
	// are considered "locked" and will not be freed when additional memory is needed 
	virtual void BeginLock() = 0;

	// reset all protected blocks to normal 
	virtual void EndLock() = 0;

	virtual int* GetFrameUnlockCounterPtrOLD() = 0;
	virtual void FinishPendingLoads() = 0;
	virtual vcollide_t* GetVCollideEx(MDLHandle_t handle, bool bSynchronousLoad = true) = 0;
	virtual bool GetVCollideSize(MDLHandle_t handle, int* pVCollideSize) = 0;
	virtual bool GetAsyncLoad(MDLCacheDataType_t type) = 0;
	virtual bool SetAsyncLoad(MDLCacheDataType_t type, bool bAsync) = 0;
	virtual void BeginMapLoad() = 0;
	virtual void EndMapLoad() = 0;
	virtual void MarkAsLoaded(MDLHandle_t handle) = 0;
	virtual void InitPreloadData(bool bRebuild) = 0;
	virtual void ShutdownPreloadData() = 0;
	virtual bool IsDataLoaded(MDLHandle_t handle, MDLCacheDataType_t type) = 0;
	virtual int* GetFrameUnlockCounterPtr(MDLCacheDataType_t type) = 0;
	virtual studiohdr_t* LockStudioHdr(MDLHandle_t handle) = 0;
	virtual void UnlockStudioHdr(MDLHandle_t handle) = 0;
	virtual bool PreloadModel(MDLHandle_t handle) = 0;
	virtual void ResetErrorModelStatus(MDLHandle_t handle) = 0;
	virtual void MarkFrame() = 0;
};

class CMDLCacheCriticalSection
{
public:
	CMDLCacheCriticalSection(IMDLCache* pCache)
		: pCache(pCache)
	{
		pCache->BeginLock();
	}

	~CMDLCacheCriticalSection()
	{
		pCache->EndLock();
	}
private:
	IMDLCache* pCache;
};
