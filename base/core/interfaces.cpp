#include "interfaces.h"

// used: logging
#include "../utilities/logging.h"

bool I::Setup()
{
	Client =			Capture<IBaseClientDll>(CLIENT_DLL, XorStr("VClient"));
	ClientEntityList =	Capture<IClientEntityList>(CLIENT_DLL, XorStr("VClientEntityList"));
	Effects =			Capture<IEffects>(CLIENT_DLL, XorStr("IEffects"));
	GameMovement =		Capture<IGameMovement>(CLIENT_DLL, XorStr("GameMovement"));
	Prediction =		Capture<IPrediction>(CLIENT_DLL, XorStr("VClientPrediction"));
	GameConsole =		Capture<IGameConsole>(CLIENT_DLL, XorStr("GameConsole"));
	GameUI =			Capture<IGameUI>(CLIENT_DLL, XorStr("GameUI"));
	Engine =			Capture<IEngineClient>(ENGINE_DLL, XorStr("VEngineClient"));
	EngineVGui =		Capture<IEngineVGui>(ENGINE_DLL, XorStr("VEngineVGui"));
	EngineTrace =		Capture<IEngineTrace>(ENGINE_DLL, XorStr("EngineTraceClient"));
	EngineSound =		Capture<IEngineSound>(ENGINE_DLL, XorStr("IEngineSoundClient"));
	StringContainer =	Capture<INetworkContainer>(ENGINE_DLL, XorStr("VEngineClientStringTable"));
	GameEvent =			Capture<IGameEventManager2>(ENGINE_DLL, XorStr("GAMEEVENTSMANAGER002"));
	RenderView =		Capture<IVRenderView>(ENGINE_DLL, XorStr("VEngineRenderView"));
	DebugOverlay =		Capture<IVDebugOverlay>(ENGINE_DLL, XorStr("VDebugOverlay"));
	EngineEffects =		Capture<IVEngineEffects>(ENGINE_DLL, XorStr("VEngineEffects"));
	ModelInfo =			Capture<IVModelInfo>(ENGINE_DLL, XorStr("VModelInfoClient"));
	ModelRender =		Capture<IVModelRender>(ENGINE_DLL, XorStr("VEngineModel"));
	MDLCache =			Capture<IMDLCache>(DATACACHE_DLL, XorStr("MDLCache"));
	StudioRender =		Capture<IStudioRender>(STUDIORENDER_DLL, XorStr("VStudioRender"));
	ConVar =			Capture<IConVar>(VSTDLIB_DLL, XorStr("VEngineCvar"));
	PhysicsProps =		Capture<IPhysicsSurfaceProps>(PHYSICS_DLL, XorStr("VPhysicsSurfaceProps"));
	MaterialSystem =	Capture<IMaterialSystem>(MATERIALSYSTEM_DLL, XorStr("VMaterialSystem"));
	Surface =			Capture<ISurface>(VGUI_DLL, XorStr("VGUI_Surface"));
	Panel =				Capture<IVPanel>(VGUI2_DLL, XorStr("VGUI_Panel"));
	InputSystem =		Capture<IInputSystem>(INPUTSYSTEM_DLL, XorStr("InputSystemVersion"));
	Localize =			Capture<ILocalize>(LOCALIZE_DLL, XorStr("Localize_"));
	MatchFramework =	Capture<IMatchFramework>(MATCHMAKING_DLL, XorStr("MATCHFRAMEWORK_"));
	Server =			Capture<IServerGameDLL>(SERVER_DLL, XorStr("ServerGameDLL"));
	PlayerInfoManager =	Capture<IPlayerInfoManager>(SERVER_DLL, XorStr("PlayerInfoManager"));

	Globals = PlayerInfoManager->GetGlobalVars();
	if (Globals == nullptr)
		return false;

	SteamClient = Engine->GetSteamAPIContext()->pSteamClient;
	if (SteamClient == nullptr)
		return false;

	SteamUser =	Engine->GetSteamAPIContext()->pSteamUser;
	if (SteamUser == nullptr)
		return false;

	const HSteamUser hSteamUser = ((std::add_pointer_t<HSteamUser(void)>)GetProcAddress(GetModuleHandle(STEAM_API_DLL), XorStr("SteamAPI_GetHSteamUser")))();
	const HSteamPipe hSteamPipe = ((std::add_pointer_t<HSteamPipe(void)>)GetProcAddress(GetModuleHandle(STEAM_API_DLL), XorStr("SteamAPI_GetHSteamPipe")))();

	SteamGameCoordinator = (ISteamGameCoordinator*)I::SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, XorStr("SteamGameCoordinator001"));
	if (SteamGameCoordinator == nullptr)
		return false;

	ClientMode = **(IClientModeShared***)((*(PDWORD*)Client)[10] + 0x5);
	if (ClientMode == nullptr)
		return false;

	MemAlloc = *(IMemAlloc**)GetProcAddress(GetModuleHandle(TIER0_DLL), XorStr("g_pMemAlloc"));
	if (MemAlloc == nullptr)
		return false;

	DirectDevice = **(IDirect3DDevice9***)(MEM::FindPattern(SHADERPIDX9_DLL, XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1); // @xref: "HandleLateCreation"
	if (DirectDevice == nullptr)
		return false;

	ViewRenderBeams = *(IViewRenderBeams**)(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 0x1);
	if (ViewRenderBeams == nullptr)
		return false;

	Input =	*(IInput**)(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1); // @note: or address of some indexed input function in chlclient class (like IN_ActivateMouse, IN_DeactivateMouse, IN_Accumulate, IN_ClearStates) + 0x1 (jmp to m_pInput)
	if (Input == nullptr)
		return false;

	ClientState = **(IBaseClientState***)(MEM::FindPattern(ENGINE_DLL, XorStr("A1 ? ? ? ? 8B 80 ? ? ? ? C3")) + 0x1);
	if (ClientState == nullptr)
		return false;

	DemoPlayer = **(IDemoPlayer***)(MEM::FindPattern(ENGINE_DLL, XorStr("8B 0D ? ? ? ? 8B 01 8B 40 1C FF D0 84 C0 74 0A")) + 0x2);
	if (DemoPlayer == nullptr)
		return false;

	WeaponSystem = *(IWeaponSystem**)(MEM::FindPattern(CLIENT_DLL, XorStr("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 0x2);
	if (WeaponSystem == nullptr)
		return false;

	GlowManager = *(IGlowObjectManager**)(MEM::FindPattern(CLIENT_DLL, XorStr("0F 11 05 ? ? ? ? 83 C8 01")) + 0x3);
	if (GlowManager == nullptr)
		return false;

	return true;
}

template <typename T>
T* I::Capture(const char* szModule, std::string_view szInterface)
{
	static auto GetRegisterList = [szModule]() -> CInterfaceRegister*
	{
		FARPROC oCreateInterface = nullptr;

		if (const auto hModule = GetModuleHandle(szModule); hModule != nullptr)
			oCreateInterface = GetProcAddress(hModule, XorStr("CreateInterface"));
		
		if (oCreateInterface == nullptr)
			throw std::runtime_error(XorStr("failed get createinterface address"));

		const std::uintptr_t	uCreateInterfaceJmp = (std::uintptr_t)oCreateInterface + 0x4;
		const std::int32_t		iJmpDisp = *(std::int32_t*)(uCreateInterfaceJmp + 0x1);
		const std::uintptr_t	uCreateInterface = uCreateInterfaceJmp + 0x5 + iJmpDisp;
		return **(CInterfaceRegister***)(uCreateInterface + 0x6);
	};

	for (auto pRegister = GetRegisterList(); pRegister != nullptr; pRegister = pRegister->pNext)
	{
		// found needed interface
		if ((!std::string(pRegister->szName).compare(0U, szInterface.length(), szInterface) &&
			// and has digits after name
			atoi(pRegister->szName + szInterface.length()) > 0) ||
			// or given full interface with hardcoded digits
			!szInterface.compare(pRegister->szName))
		{
			// log interface address
			L::Print(fmt::format(XorStr("captured {} interface -> {:#08X}"), pRegister->szName, (std::uintptr_t)((void*)pRegister->pCreateFn())));
			// capture our interface
			return (T*)pRegister->pCreateFn();
		}
	}

	#if DEBUG_CONSOLE && _DEBUG
	L::PushConsoleColor(FOREGROUND_INTENSE_RED);
	L::Print(fmt::format(XorStr("[error] failed to find interface \"{}\" in \"{}\""), szInterface, szModule));
	L::PopConsoleColor();
	#endif

	return nullptr;
}
