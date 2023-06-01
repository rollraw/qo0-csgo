#pragma once
// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#pragma region winapi_nt_types
using NTSTATUS = LONG;
using MEMORY_INFORMATION_CLASS = INT;
#pragma endregion

#pragma region winapi_nt_definitions
#define NtCurrentProcess() (reinterpret_cast<HANDLE>(-1))
#define NtCurrentThread() (reinterpret_cast<HANDLE>(-2))

/*
 * NT_SUCCESS =		[0x00000000 .. 0x3FFFFFFF]
 * NT_INFORMATION =	[0x40000000 .. 0x7FFFFFFF]
 * NT_WARNING =		[0x80000000 .. 0xBFFFFFFF]
 * NT_ERROR =		[0xC0000000 .. 0xFFFFFFFF]
 */
#define NT_SUCCESS(STATUS) (static_cast<NTSTATUS>(STATUS) >= 0)
#define NT_INFORMATION(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 1UL)
#define NT_WARNING(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 2UL)
#define NT_ERROR(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 3UL)
#pragma endregion

#pragma region winapi_nt
// @todo: support both architectures with templates
// actual structures WINNT v10.0, unions omitted
// @credits: https://www.vergiliusproject.com/kernels/x86/Windows%2010

#pragma pack(push, 8)
typedef struct _UNICODE_STRING
{
	std::uint16_t Length; // 0x0
	std::uint16_t MaximumLength; // 0x2
	wchar_t* Buffer; // 0x4
} UNICODE_STRING, *PUNICODE_STRING;
static_assert(sizeof(_UNICODE_STRING) == 0x8);

struct _RTL_BALANCED_NODE32
{
	_RTL_BALANCED_NODE32* Children[2]; // 0x0
	std::uint32_t ParentValue; // 0x8
};
static_assert(sizeof(_RTL_BALANCED_NODE32) == 0xC);

struct _LDR_DATA_TABLE_ENTRY32
{
	_LIST_ENTRY InLoadOrderLinks; // 0x00
	_LIST_ENTRY InMemoryOrderLinks; // 0x08
	_LIST_ENTRY InInitializationOrderLinks; // 0x10
	void* DllBase; // 0x18
	void* EntryPoint; // 0x1C
	std::uint32_t SizeOfImage; // 0x20
	_UNICODE_STRING FullDllName; // 0x24
	_UNICODE_STRING BaseDllName; // 0x2C
	std::uint32_t Flags; // 0x34
	std::uint16_t ObsoleteLoadCount; // 0x38
	std::uint16_t TlsIndex; // 0x3A - always
	_LIST_ENTRY HashLinks; // 0x3C
	std::uint32_t TimeDateStamp; // 0x44
	void* EntryPointActivationContext; // 0x48
	void* Lock; // 0x4C
	struct _LDR_DDAG_NODE* DdagNode; // 0x50
	_LIST_ENTRY NodeModuleLink; // 0x54
	struct _LDRP_LOAD_CONTEXT* LoadContext; // 0x5C
	void* ParentDllBase; // 0x60
	void* SwitchBackContext; // 0x64
	_RTL_BALANCED_NODE32 BaseAddressIndexNode; // 0x68
	_RTL_BALANCED_NODE32 MappingInfoIndexNode; // 0x74
	std::uint32_t OriginalBase; // 0x80
	std::int64_t LoadTime; // 0x84
	std::uint32_t BaseNameHashValue; // 0x90
	std::uint32_t LoadReason; // 0x94
	std::uint32_t ImplicitPathOptions; // 0x98
	std::uint32_t ReferenceCount; // 0x9C
};
static_assert(sizeof(_LDR_DATA_TABLE_ENTRY32) == 0xA0);

struct _PEB_LDR_DATA32
{
	std::uint32_t Length; // 0x00
	std::uint8_t Initialized; // 0x04
	void* SsHandle; // 0x08
	_LIST_ENTRY InLoadOrderModuleList; // 0x0C
	_LIST_ENTRY InMemoryOrderModuleList; // 0x14
	_LIST_ENTRY InInitializationOrderModuleList; // 0x1C
	void* EntryInProgress; // 0x24
	std::uint8_t ShutdownInProgress; // 0x28
	void* ShutdownThreadId; // 0x2C
};
static_assert(sizeof(_PEB_LDR_DATA32) == 0x30);

struct _CURDIR
{
	_UNICODE_STRING DosPath; // 0x0
	void* Handle; // 0x8
};
static_assert(sizeof(_CURDIR) == 0xC);

struct _STRING32
{
	std::uint16_t Length; // 0x0
	std::uint16_t MaximumLength; // 0x2
	char* Buffer; // 0x4
};
static_assert(sizeof(_STRING32) == 0x8);

struct _RTL_DRIVE_LETTER_CURDIR32
{
	std::uint16_t Flags; // 0x0
	std::uint16_t Length; // 0x2
	std::uint32_t Timestamp; // 0x4
	_STRING32 DosPath; // 0x8
};
static_assert(sizeof(_RTL_DRIVE_LETTER_CURDIR32) == 0x10);

struct _RTL_USER_PROCESS_PARAMETERS32
{
	std::uint32_t MaximumLength; // 0x00
	std::uint32_t Length; // 0x04
	std::uint32_t Flags; // 0x08
	std::uint32_t DebugFlags; // 0x0C
	void* ConsoleHandle; // 0x10
	std::uint32_t ConsoleFlags; // 0x14
	void* StandardInput; // 0x18
	void* StandardOutput; // 0x1C
	void* StandardError; // 0x20
	_CURDIR CurrentDirectory; // 0x24
	_UNICODE_STRING DllPath; // 0x30
	_UNICODE_STRING ImagePathName; // 0x38
	_UNICODE_STRING CommandLine; // 0x40
	void* Environment; // 0x48
	std::uint32_t StartingX; // 0x4C
	std::uint32_t StartingY; // 0x50
	std::uint32_t CountX; // 0x54
	std::uint32_t CountY; // 0x58
	std::uint32_t CountCharsX; // 0x5C
	std::uint32_t CountCharsY; // 0x60
	std::uint32_t FillAttribute; // 0x64
	std::uint32_t WindowFlags; // 0x68
	std::uint32_t ShowWindowFlags; // 0x6C
	_UNICODE_STRING WindowTitle; // 0x70
	_UNICODE_STRING DesktopInfo; // 0x78
	_UNICODE_STRING ShellInfo; // 0x80
	_UNICODE_STRING RuntimeData; // 0x88
	_RTL_DRIVE_LETTER_CURDIR32 CurrentDirectores[32]; // 0x90
	std::uint32_t EnvironmentSize; // 0x290
	std::uint32_t EnvironmentVersion; // 0x294
	void* PackageDependencyData; // 0x298
	std::uint32_t ProcessGroupId; // 0x29C
	std::uint32_t LoaderThreads; // 0x2A0
};
static_assert(sizeof(_RTL_USER_PROCESS_PARAMETERS32) == 0x2A4);

struct _PEB32
{
	std::uint8_t InheritedAddressSpace; // 0x00
	std::uint8_t ReadImageFileExecOptions; // 0x01
	std::uint8_t BeingDebugged; // 0x02
	std::uint8_t BitField; // 0x03
	void* Mutant; // 0x04
	void* ImageBaseAddress; // 0x08
	_PEB_LDR_DATA32* Ldr; // 0x0C
	_RTL_USER_PROCESS_PARAMETERS32* ProcessParameters; // 0x10
	void* SubSystemData; // 0x14
	void* ProcessHeap; // 0x18
	_RTL_CRITICAL_SECTION* FastPebLock; // 0x1c
	void* AtlThunkSListPtr; // 0x20
	void* IFEOKey; // 0x24
	std::uint32_t CrossProcessFlags; // 0x28
	void* KernelCallbackTable; // 0x2C
	std::uint32_t SystemReserved[1]; // 0x30
	std::uint32_t AtlThunkSListPtr32; // 0x34
	void* ApiSetMap; // 0x38
	std::uint32_t TlsExpansionCounter; // 0x3C
	void* TlsBitmap; // 0x40
	std::uint32_t TlsBitmapBits[2]; // 0x44
	void* ReadOnlySharedMemoryBase; // 0x4C
	void* SparePvoid0; // 0x50
	void** ReadOnlyStaticServerData; // 0x54
	void* AnsiCodePageData; // 0x58
	void* OemCodePageData; // 0x5C
	void* UnicodeCaseTableData; // 0x60
	std::uint32_t NumberOfProcessors; // 0x64
	std::uint32_t NtGlobalFlag; // 0x68
	std::int64_t CriticalSectionTimeout; // 0x70
	std::uint32_t HeapSegmentReserve; // 0x78
	std::uint32_t HeapSegmentCommit; // 0x7C
	std::uint32_t HeapDeCommitTotalFreeThreshold; // 0x80
	std::uint32_t HeapDeCommitFreeBlockThreshold; // 0x84
	std::uint32_t NumberOfHeaps; // 0x88
	std::uint32_t MaximumNumberOfHeaps; // 0x8C
	void** ProcessHeaps; // 0x90
	void* GdiSharedHandleTable; // 0x94
	void* ProcessStarterHelper; // 0x98
	std::uint32_t GdiDCAttributeList; // 0x9C
	_RTL_CRITICAL_SECTION* LoaderLock; // 0xA0
	std::uint32_t OSMajorVersion; // 0xA4
	std::uint32_t OSMinorVersion; // 0xA8
	std::uint16_t OSBuildNumber; // 0xAC
	std::uint16_t OSCSDVersion; // 0xAE
	std::uint32_t OSPlatformId; // 0xB0
	std::uint32_t ImageSubsystem; // 0xB4
	std::uint32_t ImageSubsystemMajorVersion; // 0xB8
	std::uint32_t ImageSubsystemMinorVersion; // 0xBC
	std::uint32_t ActiveProcessAffinityMask; // 0xC0
	std::uint32_t GdiHandleBuffer[34]; // 0xC4
	void (*PostProcessInitRoutine)(); // 0x14C
	void* TlsExpansionBitmap; // 0x150
	std::uint32_t TlsExpansionBitmapBits[32]; // 0x154
	std::uint32_t SessionId; // 0x1D4
	std::uint64_t AppCompatFlags; // 0x1D8
	std::uint64_t AppCompatFlagsUser; // 0x1E0
	void* pShimData; // 0x1E8
	void* AppCompatInfo; // 0x1EC
	_UNICODE_STRING CSDVersion; // 0x1F0
	struct _ACTIVATION_CONTEXT_DATA* ActivationContextData; // 0x1F8
	struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap; // 0x1FC
	_ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData; // 0x200
	_ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap; // 0x204
	std::uint32_t MinimumStackCommit; // 0x208
	struct _FLS_CALLBACK_INFO* FlsCallback; // 0x20C
	_LIST_ENTRY FlsListHead; // 0x210
	void* FlsBitmap; // 0x218
	std::uint32_t FlsBitmapBits[4]; // 0x21C
	std::uint32_t FlsHighIndex; // 0x22C
	void* WerRegistrationData; // 0x230
	void* WerShipAssertPtr; // 0x234
	void* pUnused; // 0x238
	void* pImageHeaderHash; // 0x23C
	std::uint32_t TracingFlags; // 0x240
	std::uint64_t CsrServerReadOnlySharedMemoryBase; // 0x248
};
static_assert(sizeof(_PEB32) == 0x250);

struct _CLIENT_ID32
{
	std::uint32_t UniqueProcess; // 0x0
	std::uint32_t UniqueThread; // 0x4
};
static_assert(sizeof(_CLIENT_ID32) == 0x8);

struct _GDI_TEB_BATCH32
{
	std::uint32_t Offset : 31; // 0x0
	std::uint32_t HasRenderingCommand : 1; // 0x0
	std::uint32_t HDC; // 0x4
	std::uint32_t Buffer[310]; // 0x8
};
static_assert(sizeof(_GDI_TEB_BATCH32) == 0x4E0);

struct _TEB32
{
	_NT_TIB NtTib; // 0x0
	void* EnvironmentPointer; // 0x1C
	_CLIENT_ID32 ClientId; // 0x20
	void* ActiveRpcHandle; // 0x28
	void* ThreadLocalStoragePointer; // 0x2C
	_PEB32* ProcessEnvironmentBlock; // 0x30
	std::uint32_t LastErrorValue; // 0x34
	std::uint32_t CountOfOwnedCriticalSections; // 0x38
	void* CsrClientThread; // 0x3C
	void* Win32ThreadInfo; // 0x40
	std::uint32_t User32Reserved[26]; // 0x44
	std::uint32_t UserReserved[5]; // 0xAC
	void* WOW32Reserved; // 0xC0
	std::uint32_t CurrentLocale; // 0xC4
	std::uint32_t FpSoftwareStatusRegister; // 0xC8
	void* ReservedForDebuggerInstrumentation[16]; // 0xCC
	void* SystemReserved1[38]; // 0x10C
	std::int32_t ExceptionCode; // 0x1A4
	struct _ACTIVATION_CONTEXT_STACK* ActivationContextStackPointer; // 0x1A8
	std::uint32_t InstrumentationCallbackSp; // 0x1AC
	std::uint32_t InstrumentationCallbackPreviousPc; // 0x1B0
	std::uint32_t InstrumentationCallbackPreviousSp; // 0x1B4
	std::uint8_t InstrumentationCallbackDisabled; // 0x1B8
	std::uint8_t SpareBytes[23]; // 0x1B9
	std::uint32_t TxFsContext; // 0x1D0
	_GDI_TEB_BATCH32 GdiTebBatch; // 0x1D4
	_CLIENT_ID32 RealClientId; // 0x6B4
	void* GdiCachedProcessHandle; // 0x6BC
	std::uint32_t GdiClientPID; // 0x6C0
	std::uint32_t GdiClientTID; // 0x6C4
	void* GdiThreadLocalInfo; // 0x6C8
	std::uint32_t Win32ClientInfo[62]; // 0x6CC
	void* glDispatchTable[233]; // 0x7C4
	std::uint32_t glReserved1[29]; // 0xB68
	void* glReserved2; // 0xBDC
	void* glSectionInfo; // 0xBE0
	void* glSection; // 0xBE4
	void* glTable; // 0xBE8
	void* glCurrentRC; // 0xBEC
	void* glContext; // 0xBF0
	std::uint32_t LastStatusValue; // 0xBF4
	_UNICODE_STRING StaticUnicodeString; // 0xBF8
	wchar_t StaticUnicodeBuffer[261]; // 0xC00
	void* DeallocationStack; // 0xE0C
	void* TlsSlots[64]; // 0xE10
	_LIST_ENTRY TlsLinks; // 0xF10
	void* Vdm; // 0xF18
	void* ReservedForNtRpc; // 0xF1C
	void* DbgSsReserved[2]; // 0xF20
	std::uint32_t HardErrorMode; // 0xF28
	void* Instrumentation[9]; // 0xF2C
	_GUID ActivityId; // 0xF50
	void* SubProcessTag; // 0xF60
	void* PerflibData; // 0xF64
	void* EtwTraceData; // 0xF68
	void* WinSockData; // 0xF6C
	std::uint32_t GdiBatchCount; // 0xF70
	_PROCESSOR_NUMBER CurrentIdealProcessor; // 0xF74
	std::uint32_t GuaranteedStackBytes; // 0xF78
	void* ReservedForPerf; // 0xF7C
	void* ReservedForOle; // 0xF80
	std::uint32_t WaitingOnLoaderLock; // 0xF84
	void* SavedPriorityState; // 0xF88
	std::uint32_t ReservedForCodeCoverage; // 0xF8C
	void* ThreadPoolData; // 0xF90
	void** TlsExpansionSlots; // 0xF94
	std::uint32_t MuiGeneration; // 0xF98
	std::uint32_t IsImpersonating; // 0xF9C
	void* NlsCache; // 0xFA0
	void* pShimData; // 0xFA4
	std::uint16_t HeapVirtualAffinity; // 0xFA8
	std::uint16_t LowFragHeapDataSlot; // 0xFAA
	void* CurrentTransactionHandle; // 0xFAC
	struct _TEB_ACTIVE_FRAME* ActiveFrame; // 0xFB0
	void* FlsData; // 0xFB4
	void* PreferredLanguages; // 0xFB8
	void* UserPrefLanguages; // 0xFBC
	void* MergedPrefLanguages; // 0xFC0
	std::uint32_t MuiImpersonation; // 0xFC4
	volatile std::uint16_t CrossTebFlags; // 0xFC8
	std::uint16_t SameTebFlags; // 0xFCA
	void* TxnScopeEnterCallback; // 0xFCC
	void* TxnScopeExitCallback; // 0xFD0
	void* TxnScopeContext; // 0xFD4
	std::uint32_t LockCount; // 0xFD8
	std::int32_t WowTebOffset; // 0xFDC
	void* ResourceRetValue; // 0xFE0
	void* ReservedForWdf; // 0xFE4
	std::uint64_t ReservedForCrt; // 0xFE8
	_GUID EffectiveContainerId; // 0xFF0
};
static_assert(sizeof(_TEB32) == 0x1000);
#pragma pack(pop)
#pragma endregion
