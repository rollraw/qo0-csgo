#pragma once
// used: [ext] minhook
// @credits: https://github.com/TsudaKageyu/minhook
#include "../../dependencies/minhook/minhook.h"

// used: l_print
#include "log.h"

// @todo: move directly to hooks.h/hooks.cpp
/*
 * detour hooking method implementation
 * @todo: desc
 */
template <typename Gadget_t = void>
class CHookObject
{
public:
	/// setup hook and replace function
	/// @returns: true if hook has been successfully created, false otherwise
	bool Create(void* pFunction, void* pDetour)
	{
		if (pFunction == nullptr || pDetour == nullptr)
			return false;

		pBaseFn = pFunction;
		pReplaceFn = pDetour;

		if (const MH_STATUS status = MH_CreateHook(pBaseFn, pReplaceFn, &pOriginalFn); status != MH_OK)
		{
		#ifdef _DEBUG
			L_PRINT(LOG_ERROR) << Q_XOR("failed to create hook, status: \"") << MH_StatusToString(status) << Q_XOR("\" with base address: ") << L::AddFlags(LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX) << reinterpret_cast<std::uintptr_t>(pBaseFn);
		#else
			L_PRINT(LOG_ERROR) << Q_XOR("failed to create hook");
		#endif

			Q_ASSERT(false);
			return false;
		}

		if (!Replace())
			return false;

		return true;
	}

	/// patch memory to jump to our function instead of original
	/// @returns: true if hook has been successfully applied, false otherwise
	bool Replace()
	{
		// check is hook has been created
		if (pBaseFn == nullptr)
			return false;

		// check that function isn't already hooked
		if (bIsHooked)
			return false;

		if (const MH_STATUS status = MH_EnableHook(pBaseFn); status != MH_OK)
		{
		#ifdef _DEBUG
			L_PRINT(LOG_ERROR) << Q_XOR("failed to enable hook, status: \"") << MH_StatusToString(status) << Q_XOR("\" with base address: ") << L::AddFlags(LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX) << reinterpret_cast<std::uintptr_t>(pBaseFn);
		#else
			L_PRINT(LOG_ERROR) << Q_XOR("failed to enable hook");
		#endif

			Q_ASSERT(false);
			return false;
		}

		// switch hook state
		bIsHooked = true;
		return true;
	}

	/// restore original function call and cleanup hook data
	/// @returns: true if hook has been successfully removed, false otherwise
	bool Remove()
	{
		// restore it at first
		if (!Restore())
			return false;

		if (const MH_STATUS status = MH_RemoveHook(pBaseFn); status != MH_OK)
		{
		#ifdef _DEBUG
			L_PRINT(LOG_ERROR) << Q_XOR("failed to remove hook, status: \"") << MH_StatusToString(status) << Q_XOR("\" with base address: ") << L::AddFlags(LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX) << reinterpret_cast<std::uintptr_t>(pBaseFn);
		#else
			L_PRINT(LOG_ERROR) << Q_XOR("failed to remove hook");
		#endif

			Q_ASSERT(false);
			return false;
		}

		return true;
	}

	/// restore patched memory to original function call
	/// @returns: true if hook has been successfully restored, false otherwise
	bool Restore()
	{
		// check that function is hooked
		if (!bIsHooked)
			return false;

		if (const MH_STATUS status = MH_DisableHook(pBaseFn); status != MH_OK)
		{
		#ifdef _DEBUG
			L_PRINT(LOG_ERROR) << Q_XOR("failed to restore hook, status: \"") << MH_StatusToString(status) << Q_XOR("\" with base address: ") << L::AddFlags(LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX) << reinterpret_cast<std::uintptr_t>(pBaseFn);
		#else
			L_PRINT(LOG_ERROR) << Q_XOR("failed to restore hook");
		#endif

			Q_ASSERT(false);
			return false;
		}

		// switch hook state
		bIsHooked = false;
		return true;
	}

	/// @returns: original, unwrapped function that would be called without the hook
	template <typename Fn>
	Q_INLINE Fn GetOriginal()
	{
		return reinterpret_cast<Fn>(pOriginalFn);
	}

	/// call the original function with spoofed return address, as if the hook had not been applied
	/// @note: reference and const reference arguments must be forwarded as pointers or wrapped with 'std::ref'/'std::cref' calls!
	/// @returns: call invoke return value
	template <typename T, typename... Args_t>
	Q_INLINE auto CallOriginal(void* thisptr, int edx, Args_t... argList)
	{
	#ifdef Q_PARANOID_DISABLE_RETURN_SPOOF
		return reinterpret_cast<T(Q_FASTCALL*)(void*, int, Args_t...)>(pOriginalFn)(thisptr, edx, argList...);
	#else
		ROP::SpoofContext_t context;
		return InvokeFastCall<T>(reinterpret_cast<std::uintptr_t>(thisptr), edx, reinterpret_cast<std::uintptr_t>(pOriginalFn), &context, Gadget_t::uReturnGadget, argList...);
	#endif
	}

	/// @returns: true if hook is applied at the time, false otherwise
	Q_INLINE bool IsHooked() const
	{
		return bIsHooked;
	}

private:
	// current hook state
	bool bIsHooked = false;
	// function base handle
	void* pBaseFn = nullptr;
	// function that being replace the original call
	void* pReplaceFn = nullptr;
	// original function
	void* pOriginalFn = nullptr;
};
