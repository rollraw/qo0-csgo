#pragma once
// used: std::vector
#include <vector>

// used: logging
#include "logging.h"

/*
 * virtual method table hooking implementation
 * @todo: replace that if really detected
 */
class CVMTHook
{
public:
	CVMTHook() = default;

	explicit CVMTHook(void* pBaseAddress)
		: pBaseTable((std::uintptr_t**)pBaseAddress), nTableLength(0U), vecReplaceFn{ }, pOriginalFn(nullptr) { }

	~CVMTHook()
	{
		Restore();
	}

	/* replace vitrual table */
	bool Setup(void* pBaseAddress)
	{
		if (pBaseAddress != nullptr)
			pBaseTable = (std::uintptr_t**)pBaseAddress;

		if (pBaseTable == nullptr)
			return false;

		// save original table
		pOriginalFn = *pBaseTable;
		// get saved table lenght
		nTableLength = EstimateVirtualTableLength(pOriginalFn);

		if (nTableLength == 0U)
			return false;

		// allocate table
		vecReplaceFn.assign(nTableLength * sizeof(std::uintptr_t) + sizeof(std::uintptr_t), 0U);

		/*
		 * @note: used two calls to show it detailed
		 * could be only 1 call:
		 */
		#if 0
		std::copy_n(&pOriginalFn[-1], (nTableLength + 1) * sizeof(std::uintptr_t) + sizeof(std::uintptr_t), vecReplaceFn.data());
		//			| from -1 for rtti				| full table size		+		rtti size		| replace table pointer
		#else
		// copy original table without rtti
		std::copy_n(pOriginalFn, nTableLength * sizeof(std::uintptr_t), &vecReplaceFn.at(1));
		// copy rtti from original table
		std::copy_n(&pOriginalFn[-1], sizeof(std::uintptr_t), &vecReplaceFn.at(0));
		#endif

		try
		{
			// set new flags for rtti memory page
			CWrappedProtect protection = CWrappedProtect{ pBaseTable, sizeof(std::uintptr_t), PAGE_READWRITE };
			// set new virtual table (except rtti)
			*pBaseTable = &vecReplaceFn.at(1);
			// switch hook state
			bTableHooked = true;
		}
		catch (...)
		{
			throw std::runtime_error(XorStr("failed to setup virtual method table"));
			return false;
		}

		return true;
	}

	/* replace the function in the table at choosen index to our modified */
	template <typename Fn>
	void Replace(Fn pFunction, std::uintptr_t nIndex)
	{
		if (nIndex < 0U || nIndex > nTableLength)
			throw std::out_of_range(fmt::format("trying replace function with {:d} while max index: {:d}", nIndex, nTableLength));

		// add rtti +1 index and replace
		vecReplaceFn.at(nIndex + 1) = (std::uintptr_t)pFunction;
		L::Print(fmt::format(XorStr("replaced function at {:#03d} index -> {:#08X}"), nIndex, (std::uintptr_t)((void*)pFunction)));
	}

	/* replace our modified function in the table at choosen index to original */
	void Restore(std::uintptr_t nIndex)
	{
		if (nIndex < 0U || nIndex > nTableLength)
			throw std::out_of_range(fmt::format("trying restore function with {:d} while max index: {:d}", nIndex, nTableLength));

		// add rtti +1 index and restore
		vecReplaceFn.at(nIndex + 1) = pOriginalFn[nIndex];
		L::Print(fmt::format(XorStr("restored function at {:#03d} index"), nIndex));
	}

	/* reset all modified functions to original */
	void Restore()
	{
		if (pOriginalFn == nullptr)
			return;

		// set new flags for rtti memory page
		CWrappedProtect protection = CWrappedProtect{ pBaseTable, sizeof(std::uintptr_t), PAGE_READWRITE };
		// set back saved original pointer
		pBaseTable = &pOriginalFn;
		// clear saved pointer
		pOriginalFn = nullptr;
		// switch hook state
		bTableHooked = false;
	}

	/* get original function pointer at current table with choosen index */
	template <typename Fn>
	Fn GetOriginal(std::uintptr_t nIndex)
	{
		if (nIndex < 0U || nIndex > nTableLength)
			throw std::out_of_range(fmt::format("trying get original function pointer with {:d} while max index: {:d}", nIndex, nTableLength));

		return (Fn)pOriginalFn[nIndex];
	}

	// Values
	bool bTableHooked = false;
private:
	// Utilities
	std::size_t EstimateVirtualTableLength(std::uintptr_t* pVirtualTable) const
	{
		MEMORY_BASIC_INFORMATION memInfo = { };
		std::size_t nSize = 0U;

		while (MEM::IsValidCodePtr(pVirtualTable[nSize]))
			++nSize;

		return nSize;
	}

	// Values
	/* used table */
	std::uintptr_t** pBaseTable = nullptr;
	/* amount of functions within the table */
	std::size_t		nTableLength = 0U;
	/* in future that is being modified and replaced the original */
	std::vector<std::uintptr_t> vecReplaceFn = { };
	/* save current table to get available restore hook later */
	std::uintptr_t* pOriginalFn = nullptr;
};
