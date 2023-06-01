#pragma once
// used: iappsystem
#include "iclientmode.h"

// @source: master/public/datacache/imdlcache.h

using MDLHandle_t = std::uint16_t;

#pragma region mdlcache_enumerations

enum
{
	MDLHANDLE_INVALID = static_cast<MDLHandle_t>(~0)
};

enum EMDLCacheDataType : int
{
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK
};

enum EMDLCacheFlushFlags : unsigned int
{
	MDLCACHE_FLUSH_STUDIOHDR = (1U << 0U),
	MDLCACHE_FLUSH_STUDIOHWDATA = (1U << 1U),
	MDLCACHE_FLUSH_VCOLLIDE = (1U << 2U),
	MDLCACHE_FLUSH_ANIMBLOCK = (1U << 3U),
	MDLCACHE_FLUSH_VIRTUALMODEL = (1U << 4U),
	MDLCACHE_FLUSH_AUTOPLAY = (1U << 5U),
	MDLCACHE_FLUSH_VERTEXES = (1U << 6U),
	MDLCACHE_FLUSH_COMBINED_DATA = (1U << 7U),
	MDLCACHE_FLUSH_IGNORELOCK = (1U << 31U),
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};

#pragma endregion

// forward declarations
struct vertexfileheader_t; // @todo: not implemented
struct vcollide_t;

class IMDLCacheNotify
{
public:
	virtual void OnDataLoaded(EMDLCacheDataType nType, MDLHandle_t hModel) = 0;
	virtual void OnCombinerPreCache(MDLHandle_t hOld, MDLHandle_t hNew) = 0;
	virtual void OnDataUnloaded(EMDLCacheDataType nType, MDLHandle_t hModel) = 0;
	virtual bool ShouldSupressLoadWarning(MDLHandle_t hModel) = 0;
};

class IMDLCache : public IAppSystem, ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void SetCacheNotify(IMDLCacheNotify* pNotify)
	{
		CallVFunc<void, 9U>(this, pNotify);
	}

	[[nodiscard]] MDLHandle_t FindMDL(const char* szMDLRelativePath)
	{
		return CallVFunc<MDLHandle_t, 10U>(this, szMDLRelativePath);
	}

	[[nodiscard]] int AddReference(MDLHandle_t hModel)
	{
		return CallVFunc<int, 11U>(this, hModel);
	}

	[[nodiscard]] int Release(MDLHandle_t hModel)
	{
		return CallVFunc<int, 12U>(this, hModel);
	}

	[[nodiscard]] int GetReference(MDLHandle_t hModel)
	{
		return CallVFunc<int, 13U>(this, hModel);
	}

	[[nodiscard]] studiohdr_t* GetStudioHdr(MDLHandle_t hModel)
	{
		return CallVFunc<studiohdr_t*, 14U>(this, hModel);
	}

	[[nodiscard]] studiohwdata_t* GetHardwareData(MDLHandle_t hModel)
	{
		return CallVFunc<studiohwdata_t*, 15U>(this, hModel);
	}

	[[nodiscard]] vcollide_t* GetVCollide(MDLHandle_t hModel)
	{
		return CallVFunc<vcollide_t*, 16U>(this, hModel);
	}

	[[nodiscard]] vcollide_t* GetVCollide(MDLHandle_t hModel, float flScale)
	{
		return CallVFunc<vcollide_t*, 17U>(this, hModel, flScale);
	}

	[[nodiscard]] unsigned char* GetAnimBlock(MDLHandle_t hModel, int nBlock, bool bPreloadIfMissing)
	{
		return CallVFunc<unsigned char*, 18U>(this, hModel, nBlock, bPreloadIfMissing);
	}

	[[nodiscard]] bool HasAnimBlockBeenPreloaded(MDLHandle_t hModel, int nBlock)
	{
		return CallVFunc<bool, 19U>(this, hModel, nBlock);
	}

	[[nodiscard]] virtualmodel_t* GetVirtualModel(MDLHandle_t hModel)
	{
		return CallVFunc<virtualmodel_t*, 20U>(this, hModel);
	}

	[[nodiscard]] int GetAutoplayList(MDLHandle_t hModel, unsigned short** pOut)
	{
		return CallVFunc<int, 21U>(this, hModel, pOut);
	}

	[[nodiscard]] vertexfileheader_t* GetVertexData(MDLHandle_t hModel)
	{
		return CallVFunc<vertexfileheader_t*, 22U>(this, hModel);
	}

	[[nodiscard]] bool IsErrorModel(MDLHandle_t hModel)
	{
		return CallVFunc<bool, 26U>(this, hModel);
	}

	void Flush(EMDLCacheFlushFlags nFlushFlags = MDLCACHE_FLUSH_ALL)
	{
		CallVFunc<void, 28U>(this, nFlushFlags);
	}

	void Flush(MDLHandle_t hModel, EMDLCacheFlushFlags nFlushFlags = MDLCACHE_FLUSH_ALL)
	{
		CallVFunc<void, 29U>(this, hModel, nFlushFlags);
	}

	[[nodiscard]] const char* GetModelName(MDLHandle_t hModel)
	{
		return CallVFunc<const char*, 30U>(this, hModel);
	}

	[[nodiscard]] void* GetCacheSection(EMDLCacheDataType nType)
	{
		return CallVFunc<void*, 31U>(this, nType);
	}

	[[nodiscard]] virtualmodel_t* GetVirtualModelFast(const studiohdr_t* pStudioHdr, MDLHandle_t hModel)
	{
		return CallVFunc<virtualmodel_t*, 32U>(this, pStudioHdr, hModel);
	}

	void BeginLock()
	{
		CallVFunc<void, 33U>(this);
	}

	void EndLock()
	{
		CallVFunc<void, 34U>(this);
	}

	[[nodiscard]] vcollide_t* GetVCollideEx(MDLHandle_t hModel, bool bSynchronousLoad = true)
	{
		return CallVFunc<vcollide_t*, 37U>(this, hModel, bSynchronousLoad);
	}

	[[nodiscard]] bool GetVCollideSize(MDLHandle_t hModel, int* pVCollideSize)
	{
		return CallVFunc<bool, 38U>(this, hModel, pVCollideSize);
	}

	[[nodiscard]] studiohdr_t* LockStudioHdr(MDLHandle_t hModel)
	{
		return CallVFunc<studiohdr_t*, 48U>(this, hModel);
	}

	void UnlockStudioHdr(MDLHandle_t hModel)
	{
		CallVFunc<void, 49U>(this, hModel);
	}

	[[nodiscard]] bool PreloadModel(MDLHandle_t hModel)
	{
		return CallVFunc<bool, 50U>(this, hModel);
	}

	void BeginCoarseLock()
	{
		CallVFunc<void, 53U>(this);
	}

	void EndCoarseLock()
	{
		CallVFunc<void, 54U>(this);
	}

	void ReloadVCollide(MDLHandle_t hModel)
	{
		CallVFunc<void, 55U>(this, hModel);
	}

	void DisableVCollideLoad()
	{
		CallVFunc<void, 58U>(this);
	}

	void EnableVCollideLoad()
	{
		CallVFunc<void, 59U>(this);
	}
};

class CMDLCacheCriticalSection
{
public:
	CMDLCacheCriticalSection(IMDLCache* pCache) :
		pCache(pCache)
	{
		this->pCache->BeginLock();
	}

	~CMDLCacheCriticalSection()
	{
		this->pCache->EndLock();
	}

	Q_CLASS_NO_ASSIGNMENT(CMDLCacheCriticalSection)

private:
	IMDLCache* pCache;
};
