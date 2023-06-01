#include "interfaces.h"

// used: l_print
#include "../utilities/log.h"
// used: stringcomparen, stringtointeger
#include "../utilities/crt.h"
// used: getexportaddress
#include "../utilities/memory.h"
// used: steamapicontext_t
#include "../sdk/interfaces/isteamclient.h"
// used: getsteamapicontext
#include "../sdk/interfaces/iengineclient.h"
// used: keyvaluessystemfn
#include "../sdk/interfaces/ikeyvaluessystem.h"

static CInterfaceRegister* GetRegisterList(const wchar_t* wszModuleName)
{
	std::uint8_t* pCreateInterface = nullptr;

	if (const void* hModule = MEM::GetModuleBaseHandle(wszModuleName); hModule != nullptr)
		pCreateInterface = static_cast<std::uint8_t*>(MEM::GetExportAddress(hModule, Q_XOR("CreateInterface")));

	if (pCreateInterface == nullptr)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to get \"CreateInterface\" address");
		return nullptr;
	}

	/*
	 * every game module have public 'CreateInterface' export that looks like:
	 *                 CreateInterface proc near
	 * 55                              push    ebp
	 * 8B EC                           mov     ebp, esp
	 * 5D                              pop     ebp
	 * E9 ?? ?? ?? ??                  jmp     CreateInterfaceInternal ; module specific interface creation method
	 *                 CreateInterface endp
	 *
	 * the method it jumps to is 'CreateInterfaceInternal' whose prologue looks like:
	 * 55                              push    ebp
	 * 8B EC                           mov     ebp, esp
	 * 56                              push    esi
	 * 8B 35 ?? ?? ?? ??               mov     esi, g_pInterfaceRegister ; module specific interface register
	 */
	std::uint8_t* pCreateInterfaceInternal = MEM::GetAbsoluteAddress(pCreateInterface + 0x5);
	return **reinterpret_cast<CInterfaceRegister***>(pCreateInterfaceInternal + 0x6);
}

bool I::Setup()
{
	const CInterfaceRegister* pClientRegister = GetRegisterList(CLIENT_DLL);
	Client = Capture<IBaseClientDll>(pClientRegister, Q_XOR("VClient"));
	ClientEntityList = Capture<IClientEntityList>(pClientRegister, Q_XOR("VClientEntityList"));
	ClientLeafSystem = Capture<IClientLeafSystem>(pClientRegister, Q_XOR("ClientLeafSystem"));
	Effects = Capture<IEffects>(pClientRegister, Q_XOR("IEffects"));
	GameMovement = Capture<IGameMovement>(pClientRegister, Q_XOR("GameMovement"));
	Prediction = Capture<IPrediction>(pClientRegister, Q_XOR("VClientPrediction"));

	const CInterfaceRegister* pEngineRegister = GetRegisterList(ENGINE_DLL);
	Engine = Capture<IEngineClient>(pEngineRegister, Q_XOR("VEngineClient"));
	EngineVGui = Capture<IEngineVGui>(pEngineRegister, Q_XOR("VEngineVGui"));
	EngineTrace = Capture<IEngineTrace>(pEngineRegister, Q_XOR("EngineTraceClient"));
	EngineSound = Capture<IEngineSound>(pEngineRegister, Q_XOR("IEngineSoundClient"));
	NetworkStringTable = Capture<INetworkStringTableContainer>(pEngineRegister, Q_XOR("VEngineClientStringTable"));
	EventManager = Capture<IGameEventManager2>(pEngineRegister, Q_XOR("GAMEEVENTSMANAGER002"));
	RenderView = Capture<IVRenderView>(pEngineRegister, Q_XOR("VEngineRenderView"));
	DebugOverlay = Capture<IVDebugOverlay>(pEngineRegister, Q_XOR("VDebugOverlay"));
	EngineEffects = Capture<IVEngineEffects>(pEngineRegister, Q_XOR("VEngineEffects"));
	ModelInfo = Capture<IVModelInfo>(pEngineRegister, Q_XOR("VModelInfoClient"));
	ModelRender = Capture<IVModelRender>(pEngineRegister, Q_XOR("VEngineModel"));

	const CInterfaceRegister* pServerRegister = GetRegisterList(SERVER_DLL);
	Server = Capture<IServerGameDLL>(pServerRegister, Q_XOR("ServerGameDLL"));

	const CInterfaceRegister* pDataCacheRegister = GetRegisterList(DATACACHE_DLL);
	MDLCache = Capture<IMDLCache>(pDataCacheRegister, Q_XOR("MDLCache"));

	const CInterfaceRegister* pStudioRenderRegister = GetRegisterList(STUDIORENDER_DLL);
	StudioRender = Capture<IStudioRender>(pStudioRenderRegister, Q_XOR("VStudioRender"));

	const CInterfaceRegister* pVStdLibRegister = GetRegisterList(VSTDLIB_DLL);
	ConVar = Capture<IConVar>(pVStdLibRegister, Q_XOR("VEngineCvar"));

	const CInterfaceRegister* pVPhysicsRegister = GetRegisterList(VPHYSICS_DLL);
	PhysicsProps = Capture<IPhysicsSurfaceProps>(pVPhysicsRegister, Q_XOR("VPhysicsSurfaceProps"));

	const CInterfaceRegister* pMaterialSystemRegister = GetRegisterList(MATERIALSYSTEM_DLL);
	MaterialSystem = Capture<IMaterialSystem>(pMaterialSystemRegister, Q_XOR("VMaterialSystem"));

	const CInterfaceRegister* pVGUIMatSurfaceRegister = GetRegisterList(VGUIMATSURFACE_DLL);
	Surface = Capture<ISurface>(pVGUIMatSurfaceRegister, Q_XOR("VGUI_Surface"));

	const CInterfaceRegister* pVGUI2Register = GetRegisterList(VGUI2_DLL);
	Panel = Capture<IPanel>(pVGUI2Register, Q_XOR("VGUI_Panel"));

	const CInterfaceRegister* pInputSystemRegister = GetRegisterList(INPUTSYSTEM_DLL);
	InputSystem = Capture<IInputSystem>(pInputSystemRegister, Q_XOR("InputSystemVersion"));

	const CInterfaceRegister* pLocalizeRegister = GetRegisterList(LOCALIZE_DLL);
	Localize = Capture<ILocalize>(pLocalizeRegister, Q_XOR("Localize_"));

	const CInterfaceRegister* pFileSystemRegister = GetRegisterList(FILESYSTEM_STDIO_DLL);
	BaseFileSystem = Capture<IBaseFileSystem>(pFileSystemRegister, Q_XOR("VBaseFileSystem"));
	FileSystem = Capture<IFileSystem>(pFileSystemRegister, Q_XOR("VFileSystem"));

	const CInterfaceRegister* pMatchmakingRegister = GetRegisterList(MATCHMAKING_DLL);
	MatchFramework = Capture<IMatchFramework>(pMatchmakingRegister, Q_XOR("MATCHFRAMEWORK_"));

	const SteamAPIContext_t* pSteamApiContext = Engine->GetSteamAPIContext();
	SteamClient = pSteamApiContext->pSteamClient;
	SteamUser = pSteamApiContext->pSteamUser;

	const void* hSteamAPI = MEM::GetModuleBaseHandle(STEAM_API_DLL);
	const HSteamUser hSteamUser = reinterpret_cast<HSteamUser(Q_CDECL*)()>(MEM::GetExportAddress(hSteamAPI, Q_XOR("SteamAPI_GetHSteamUser")))();
	const HSteamPipe hSteamPipe = reinterpret_cast<HSteamPipe(Q_CDECL*)()>(MEM::GetExportAddress(hSteamAPI, Q_XOR("SteamAPI_GetHSteamPipe")))();

	SteamGameCoordinator = static_cast<ISteamGameCoordinator*>(SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, Q_XOR("SteamGameCoordinator001")));
	if (SteamGameCoordinator == nullptr)
		return false;

	MemAlloc = *static_cast<IMemAlloc**>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(TIER0_DLL), Q_XOR("g_pMemAlloc")));
	if (MemAlloc == nullptr)
		return false;

	// @test: [27.10.22] instead of getting it by call, get straight global ptr? if so, just add 0x1 to adr and deref. idk why they didn't this same as g_pMemAlloc...
	KeyValuesSystem = reinterpret_cast<KeyValuesSystemFn_t>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(VSTDLIB_DLL), Q_XOR("KeyValuesSystem")))();
	if (KeyValuesSystem == nullptr)
		return false;

	ClientMode = **reinterpret_cast<IClientModeShared***>(MEM::GetVFunc<std::uint8_t*>(Client, 10U) + 0x5); // get it from CHLClient::HudProcessInput
	if (ClientMode == nullptr)
		return false;

	Globals = **reinterpret_cast<IGlobalVarsBase***>(MEM::GetVFunc<std::uint8_t*>(Client, 11U) + 0xA); // get it from CHLClient::HudUpdate @xref: "(time_int)", "(time_float)"
	if (Globals == nullptr)
		return false;

	DirectDevice = **reinterpret_cast<IDirect3DDevice9***>(MEM::FindPattern(SHADERPIDX9_DLL, Q_XOR("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1); // @xref: "HandleLateCreation"
	if (DirectDevice == nullptr)
		return false;

	ViewRender = **reinterpret_cast<IViewRender***>(MEM::FindPattern(CLIENT_DLL, Q_XOR("8B 0D ? ? ? ? FF 75 0C 8B 45 08")) + 0x2);
	if (ViewRender == nullptr)
		return false;

	ViewRenderBeams = *reinterpret_cast<IViewRenderBeams**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 0x1); // @xref: "r_drawbrushmodels"
	if (ViewRenderBeams == nullptr)
		return false;

	Input = *reinterpret_cast<IInput**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1); // @note: or address of some indexed input function in chlclient class (like IN_ActivateMouse, IN_DeactivateMouse, IN_Accumulate, IN_ClearStates) + 0x1 (jmp to m_pInput)
	if (Input == nullptr)
		return false;

	ClientState = **reinterpret_cast<IClientState***>(MEM::FindPattern(ENGINE_DLL, Q_XOR("A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07")) + 0x1);
	if (ClientState == nullptr)
		return false;

	WeaponSystem = *reinterpret_cast<IWeaponSystem**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 0x2);
	if (WeaponSystem == nullptr)
		return false;

	GlowManager = *reinterpret_cast<IGlowObjectManager**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("0F 11 05 ? ? ? ? 83 C8 01")) + 0x3);
	if (GlowManager == nullptr)
		return false;

	Hud = *reinterpret_cast<IHud**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("B9 ? ? ? ? E8 ? ? ? ? 85 C0 74 19")) + 0x1); // @xref: "CCSGO_HudRosettaSelector"
	if (Hud == nullptr)
		return false;

	ItemSystem = *reinterpret_cast<IEconItemSystem**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("A1 ? ? ? ? 85 C0 75 53")) + 0x1);
	if (ItemSystem == nullptr) // @test: [27.10.22] or call fn void*(__cdecl*)() with same pattern
		return false;

	return true;
}

#pragma region interfaces_get
template <class T>
T* I::Capture(const CInterfaceRegister* pModuleRegister, const char* szInterfaceName)
{
	for (const CInterfaceRegister* pRegister = pModuleRegister; pRegister != nullptr; pRegister = pRegister->pNext)
	{
		if (const std::size_t nInterfaceNameLength = CRT::StringLength(szInterfaceName);
			// found needed interface
			CRT::StringCompareN(szInterfaceName, pRegister->szName, nInterfaceNameLength) == 0 &&
			// and we've given full name with hardcoded digits
			(CRT::StringLength(pRegister->szName) == nInterfaceNameLength ||
			// or it contains digits after name
			CRT::StringToInteger<int>(pRegister->szName + nInterfaceNameLength, nullptr, 10) > 0))
		{
			// capture our interface
			void* pInterface = pRegister->fnCreate();

		#ifdef _DEBUG
			// log interface address
			L_PRINT(LOG_NONE) << Q_XOR("captured \"") << pRegister->szName << Q_XOR("\" interface at address: ") << L::AddFlags(LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX) << reinterpret_cast<std::uintptr_t>(pInterface);
		#else
			L_PRINT(LOG_NONE) << Q_XOR("captured \"") << pRegister->szName << Q_XOR("\" interface");
		#endif

			return static_cast<T*>(pInterface);
		}
	}

	L_PRINT(LOG_ERROR) << Q_XOR("failed to find interface \"") << szInterfaceName << Q_XOR("\"");
	return nullptr;
}
#pragma endregion
