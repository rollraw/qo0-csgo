#pragma once
// used: [stl] vector
#include <vector>
// used: [stl] type_info
#include <typeinfo>

#include "../common.h"

#pragma region memory_definitions
#pragma warning(push)
#pragma warning(disable: 6255) // '_alloca' indicates failure by raising a stack overflow exception. consider using '_malloca' instead
#define MEM_STACKALLOC(SIZE) _alloca(SIZE)
#pragma warning(pop)
#define MEM_STACKFREE(MEMORY) static_cast<void>(0)
#pragma endregion

#pragma region memory_rtti
enum EBaseClassAttributes : std::uint32_t
{
	BCD_NOTVISIBLE = (1U << 0U),
	BCD_AMBIGUOUS = (1U << 1U),
	BCD_PRIVORPROTINCOMPOBJ = (1U << 2U),
	BCD_PRIVORPROTBASE = (1U << 3U),
	BCD_VBOFCONTOBJ = (1U << 4U),
	BCD_NONPOLYMORPHIC = (1U << 5U),
	BCD_HASPCHD = (1U << 6U)
};

enum EClassHierarchyAttributes : std::uint32_t
{
	CHD_MULTIPLE_INHERITANCE = (1U << 0U),
	CHD_VIRTUAL_INHERITANCE = (1U << 1U),
	CHD_AMBIGUOUS_INHERITANCE = (1U << 2U)
};

#pragma pack(push, 1)
struct RTTITypeDescriptor_t
{
	std::type_info* pTypeInfo; // 0x00
	std::uintptr_t uData; // 0x04 // null until loaded at runtime
	const char* szName; // 0x08 // mangled name
};

struct RTTIPointerToMember_t
{
	int iMDisp; // 0x00 // member displacement
	int iPDisp; // 0x04 // vtable displacement
	int iVDisp; // 0x08 // displacement inside vtable
};

// describes all base classes together with information to derived class access dynamically
struct RTTIBaseClassDescriptor_t
{
	RTTITypeDescriptor_t* pTypeDescriptor; // 0x00 // type descriptor of the class
	std::uint32_t nContainedBaseCount; // 0x04 // number of nested classes following in the base class Array
	RTTIPointerToMember_t pointerToMember; // 0x08 // pointer-to-member displacement info
	std::uint32_t uAttributes; // 0x14 // flags
	// 0x18 when uAttributes & BCD_HASPCHD
};

// describes the inheritance hierarchy of a class; shared by all COLs for the class
struct RTTIClassHierarchyDescriptor_t
{
	std::uint32_t uOffset; // 0x00
	std::uint32_t uAttributes; // 0x04
	std::uint32_t nBaseClassCount; // 0x08
	RTTIBaseClassDescriptor_t* pBaseClassDescriptor; // 0x0C
};

// location of the complete object from a specific vtable pointer
struct RTTICompleteObjectLocator_t
{
	std::uint32_t uSignature; // 0x00 // 32-bit = zero, 64-bit = one, until loaded
	std::uint32_t uOffset; // 0x04 // offset of this vtable in the complete class
	std::uint32_t uOffsetCD; // 0x08 // constructor displacement offset
	RTTITypeDescriptor_t* pTypeDescriptor; // 0x0C // type descriptor of the complete class
	RTTIClassHierarchyDescriptor_t* pClassHierarchyDescriptor; // 0x10 // describes inheritance hierarchy
#ifdef Q_ARCH_X64
	std::uint32_t uObjectBase; // 0x14 // object base offset
#endif
};
#pragma pack(pop)
#pragma endregion

/*
 * RETURN ORIENTED PROGRAMMING
 * - return address spoofing, context and gadgets
 */
namespace ROP
{
	struct SpoofContext_t
	{
		std::uintptr_t uRegisterBackup;
		std::uintptr_t uRestoreAddress;
		std::uintptr_t uReturnAddress;
	};

	// 'engine.dll' gadget holder
	struct EngineGadget_t
	{
		static std::uintptr_t uReturnGadget;
	};

	// 'client.dll' gadget holder
	struct ClientGadget_t
	{
		static std::uintptr_t uReturnGadget;
	};

	// @todo: generally i think it would be better to change callvfunc to macro, and inherit only gadgets, but this way have its pros and cons, e.g. it should result to better inlining but needs to declare spoof context locally on expansion
	/*
	 * add ability to call virtual methods of this interface from the game
	 * just specify needed gadget holder and inherit this last in order with 'private', or 'protected' if class is base to other interfaces, access specifiers
	 */
	template <typename Gadget_t>
	struct VirtualCallable_t
	{
		/// call virtual function of specified class at given index
		/// @note: reference and const reference arguments must be forwarded as pointers or wrapped with 'std::ref'/'std::cref' calls!
		/// @returns: result of virtual function call
		template <typename T, std::size_t nIndex, class CBaseClass, typename... Args_t>
		static Q_INLINE T CallVFunc(CBaseClass* thisptr, Args_t... argList)
		{
		#ifdef Q_PARANOID_DISABLE_RETURN_SPOOF
			using VirtualFn_t = T(__thiscall*)(const void*, decltype(argList)...);
			return (*reinterpret_cast<VirtualFn_t* const*>(reinterpret_cast<std::uintptr_t>(thisptr)))[nIndex](thisptr, argList...);
		#else
			SpoofContext_t context;
			return InvokeFastCall<T>(reinterpret_cast<std::uintptr_t>(thisptr), 0U, (*reinterpret_cast<std::uintptr_t* const*>(reinterpret_cast<std::uintptr_t>(thisptr)))[nIndex], &context, Gadget_t::uReturnGadget, argList...);
		#endif
		}
	};

	// wrapped method invoker to spoof it's return address
	// @note: you can use any non-volatile register, except 'esp', such as: 'edx' (jmp edx; "FF E2", jmp [edx]; "FF 22", jmp [edx+0]; "FF 62 00"), 'ebx' (jmp ebx; "FF E3", jmp [ebx]; "FF 23", jmp [ebx+0]; "FF 63 00"), 'esi' (jmp esi; "FF E6", jmp [esi]; "FF 26", jmp [esi+0]; "FF 66 00"), by default it is 'ebx'
	// @credits: danielkrupinski
	template <typename T, typename... Args_t>
	__declspec(naked) T Q_FASTCALL InvokeFastCall(std::uintptr_t ecx, std::uintptr_t edx, std::uintptr_t uFunctionAddress, SpoofContext_t* pContext, std::uintptr_t uGadgetAddress, Args_t... argList)
	{
		__asm
		{
			mov eax, [esp+8] // load a pointer to context
			mov [eax], ebx // save register into context
			lea ebx, RESTORE // load the address of the restore label
			mov [eax+4], ebx // save the restore address in context
			pop dword ptr [eax+8] // pop return address from stack into context

			lea ebx, [eax+4] // load the address of restore address to register
			ret 4 // pop function address from stack and jump to it, skip context on stack; stack pointer will point to the gadget address

		RESTORE:
			push [ebx+4] // restore original return address
			mov ebx, [ebx-4] // restore original register
			ret
		}
	}

	template <typename T, typename... Args_t>
	__declspec(naked) T Q_CDECL InvokeCdecl(std::uintptr_t uFunctionAddress, SpoofContext_t* pContext, std::uintptr_t uGadgetAddress, Args_t... argList)
	{
		__asm
		{
			mov eax, [esp+8] // load a pointer to context
			mov [eax], ebx // save register into context
			lea ebx, RESTORE // load the address of the restore label
			mov [eax+4], ebx // save the restore address in context
			pop dword ptr [eax+8] // pop return address from stack into context

			lea ebx, [eax+4] // load the address of restore address to register
			ret 4 // pop function address from stack and jump to it, skip context on stack; stack pointer will point to the gadget address

		RESTORE:
			sub esp, 0Ch // allocate stack for '__cdecl' calling convention
			push [ebx+4] // restore original return address
			mov ebx, [ebx-4] // restore original register
			ret
		}
	}
}

/*
 * MEMORY
 * - memory management, search and manipulation
 */
namespace MEM
{
	/// store ROP gadgets, exported functions from game modules
	/// @returns: true if ROP gadget addresses and all exports were successfully found, false otherwise
	[[nodiscard]] bool Setup();

	/* @section: allocation */
	// allocate a block of memory from a heap
	[[nodiscard]] void* HeapAlloc(const std::size_t nSize);
	// free a memory block allocated from a heap
	void HeapFree(void* pMemory);
	// reallocate a block of memory from a heap
	// @note: we're expect this to allocate instead when passed null, and free if size is null
	void* HeapRealloc(void* pMemory, const std::size_t nNewSize);

	/* @section: get */
	/// alternative of 'GetModuleHandle()'
	/// @param[in] wszModuleName module name to search base handle for, null means current process
	/// @returns: base handle of module with given name if it exist, null otherwise
	[[nodiscard]] void* GetModuleBaseHandle(const wchar_t* wszModuleName);
	/// retrieve size of module image
	/// @param[in] hModuleBase module base to get image size for, null means current process
	/// @returns: image size of given module if it's valid, null otherwise
	[[nodiscard]] std::size_t GetModuleBaseSize(const void* hModuleBase);
	/// alternative of 'GetModuleFileName()'
	/// @param[in] hModuleBase module base to search filename for, null means current process
	/// @returns: name of given module if it's valid, null otherwise
	[[nodiscard]] const wchar_t* GetModuleBaseFileName(const void* hModuleBase);
	/// alternative of 'GetProcAddress()'
	/// @remarks: doesn't support forwarded exports, this means you may need to manual call 'LoadLibrary'/'FreeLibrary' for export library
	/// @returns: pointer to exported procedure
	[[nodiscard]] void* GetExportAddress(const void* hModuleBase, const char* szProcedureName);
	/// search for procedure in module import table, can be used for IAT hooking
	/// @returns: pointer to imported procedure thunk
	[[nodiscard]] void* GetImportAddress(const void* hModuleBase, const char* szProcedureName);
	/// @param[in] szSectionName section to get info of (e.g. ".rdata", ".text", etc)
	/// @param[out] ppSectionStart output for section start address
	/// @param[out] pnSectionSize output for section size
	/// @returns: true if code section has been found, false otherwise
	[[nodiscard]] bool GetSectionInfo(const void* hModuleBase, const char* szSectionName, std::uint8_t** ppSectionStart, std::size_t* pnSectionSize);
	/// get pointer to function of virtual-function table
	/// @returns: pointer to virtual function
	template <typename T = void*>
	[[nodiscard]] Q_INLINE T GetVFunc(const void* thisptr, std::size_t nIndex)
	{
		return (*static_cast<T* const*>(thisptr))[nIndex];
	}
	/// get absolute address from relative address
	/// @param[in] pRelativeAddress pointer to relative address, e.g. destination address from JMP, JE, JNE and others instructions
	/// @returns: pointer to absolute address
	[[nodiscard]] Q_INLINE std::uint8_t* GetAbsoluteAddress(std::uint8_t* pRelativeAddress)
	{
		// follow offset, can be negative
		return pRelativeAddress + 0x4 + *reinterpret_cast<std::int32_t*>(pRelativeAddress);
	}

	/* @section: search */
	/// ida style pattern byte comparison in a specific module
	/// @param[in] wszModuleName module name where to search for pattern
	/// @param[in] szPattern ida style pattern, e.g. "55 8B 40 ? 30", wildcard can be either '?' or "??", bytes always presented by two numbers in a row [00 .. FF], whitespaces can be omitted (wildcards in this case should be two-character)
	/// @returns: pointer to address of the first found occurrence with equal byte sequence on success, null otherwise
	[[nodiscard]] std::uint8_t* FindPattern(const wchar_t* wszModuleName, const char* szPattern);
	/// naive style pattern byte comparison in a specific module
	/// @param[in] wszModuleName module name where to search for pattern
	/// @param[in] szBytePattern naive style pattern, e.g. "\x55\x8B\x40\x00\x30", wildcard bytes value ignored
	/// @param[in] szByteMask wildcard mask for byte array, e.g. "xxx?x", should always correspond to bytes count
	/// @returns: pointer to address of the first found occurrence with equal byte sequence on success, null otherwise
	[[nodiscard]] std::uint8_t* FindPattern(const wchar_t* wszModuleName, const char* szBytePattern, const char* szByteMask);
	/// pattern byte comparison in the specific region
	/// @param[in] arrByteBuffer byte sequence to search
	/// @param[in] nByteCount count of search bytes
	/// @param[in] szByteMask [optional] wildcard mask for byte array
	/// @returns: pointer to address of the first found occurrence with equal byte sequence on success, null otherwise
	[[nodiscard]] std::uint8_t* FindPatternEx(const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask = nullptr);
	/// pattern byte comparison in the specific region
	/// @param[in] arrByteBuffer byte sequence to search
	/// @param[in] nByteCount count of search bytes
	/// @param[in] szByteMask [optional] wildcard mask for byte array
	/// @returns: pointers to addresses of the all found occurrences with equal byte sequence on success, empty otherwise
	[[nodiscard]] std::vector<std::uint8_t*> FindPatternAllOccurrencesEx(const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask = nullptr);
	/// class RTTI type descriptor search in a specific module
	/// @returns: pointer to the found type descriptor of the class on success, null otherwise
	[[nodiscard]] RTTITypeDescriptor_t* FindClassTypeDescriptor(const void* hModuleHandle, const char* szClassName);
	/// class RTTI virtual table search in a specific module
	/// @returns: pointer to the found virtual table on success, null otherwise
	[[nodiscard]] std::uint8_t* FindVTable(const wchar_t* wszModuleName, const char* szVTableName);

	/* @section: extra */
	/// convert ida-style pattern to byte array
	/// @param[in] szPattern ida-style pattern, e.g. "55 8B 40 ? 30", wildcard can be either '?' or "??", bytes are always presented by two numbers in a row [00 .. FF], blank delimiters are ignored and not necessary (wildcard in this case should be two-character)
	/// @param[out] pOutByteBuffer output for converted, zero-terminated byte array
	/// @param[out] szOutMaskBuffer output for wildcard, zero-terminated byte mask
	/// @returns: count of the converted bytes from the pattern
	std::size_t PatternToBytes(const char* szPattern, std::uint8_t* pOutByteBuffer, char* szOutMaskBuffer);
	/// convert byte array to ida-style pattern
	/// @param[in] pByteBuffer buffer of bytes to convert
	/// @param[in] nByteCount count of bytes to convert
	/// @param[out] szOutBuffer output for converted pattern
	/// @returns: length of the converted ida-style pattern, not including the terminating null
	std::size_t BytesToPattern(const std::uint8_t* pByteBuffer, const std::size_t nByteCount, char* szOutBuffer);

	/* @section: game imports */
	inline void*(Q_CDECL* fnRTDynamicCast)(void* pPolymorphObject, int nVFDelta, void* pSourceType, void* pTargetType, bool bIsReference) = nullptr;

	/* @section: game exports */
	inline int(Q_CDECL* fnRandomSeed)(int iSeed) = nullptr;
	inline float(Q_CDECL* fnRandomFloat)(float flMinValue, float flMaxValue) = nullptr;
	inline float(Q_CDECL* fnRandomFloatExp)(float flMinValue, float flMaxValue, float flExponent) = nullptr;
	inline int(Q_CDECL* fnRandomInt)(int iMinValue, int iMaxValue) = nullptr;
	inline float(Q_CDECL* fnRandomGaussianFloat)(float flMean, float flStdDev) = nullptr;
	inline unsigned long(Q_STDCALL* fnUnDecorateSymbolName)(const char* szName, char* pszOutput, unsigned long nMaxStringLength, unsigned long dwFlags) = nullptr;
}
