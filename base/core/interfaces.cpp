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
	NetworkContainer =	Capture<INetworkContainer>(ENGINE_DLL, XorStr("VEngineClientStringTable"));
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
	GameTypes =			Capture<IGameTypes>(MATCHMAKING_DLL, XorStr("VENGINE_GAMETYPES_VERSION"));
	Server =			Capture<IServerGameDLL>(SERVER_DLL, XorStr("ServerGameDLL"));

	SteamClient = Engine->GetSteamAPIContext()->pSteamClient;
	if (SteamClient == nullptr)
		return false;

	SteamUser =	Engine->GetSteamAPIContext()->pSteamUser;
	if (SteamUser == nullptr)
		return false;

	const void* hSteamAPI = MEM::GetModuleBaseHandle(STEAM_API_DLL);
	const HSteamUser hSteamUser = reinterpret_cast<std::add_pointer_t<HSteamUser()>>(MEM::GetExportAddress(hSteamAPI, XorStr("SteamAPI_GetHSteamUser")))();
	const HSteamPipe hSteamPipe = reinterpret_cast<std::add_pointer_t<HSteamPipe()>>(MEM::GetExportAddress(hSteamAPI, XorStr("SteamAPI_GetHSteamPipe")))();

	SteamGameCoordinator = static_cast<ISteamGameCoordinator*>(SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, XorStr("SteamGameCoordinator001")));
	if (SteamGameCoordinator == nullptr)
		return false;

	ClientMode = **reinterpret_cast<IClientModeShared***>(MEM::GetVFunc<std::uintptr_t>(Client, 10) + 0x5); // get it from CHLClient::HudProcessInput
	if (ClientMode == nullptr)
		return false;

	Globals = **reinterpret_cast<IGlobalVarsBase***>(MEM::GetVFunc<std::uintptr_t>(Client, 11) + 0xA); // get it from CHLClient::HudUpdate @xref: "(time_int)", "(time_float)"
	if (Globals == nullptr)
		return false;

	MemAlloc = *static_cast<IMemAlloc**>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(TIER0_DLL), XorStr("g_pMemAlloc")));
	if (MemAlloc == nullptr)
		return false;

	KeyValuesSystem = reinterpret_cast<KeyValuesSystemFn>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem")))();
	if (KeyValuesSystem == nullptr)
		return false;

	DirectDevice = **reinterpret_cast<IDirect3DDevice9***>(MEM::FindPattern(SHADERPIDX9_DLL, XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1); // @xref: "HandleLateCreation"
	if (DirectDevice == nullptr)
		return false;

	ViewRender = **reinterpret_cast<IViewRender***>(MEM::FindPattern(CLIENT_DLL, XorStr("8B 0D ? ? ? ? FF 75 0C 8B 45 08")) + 0x2);
	if (ViewRender == nullptr)
		return false;

	ViewRenderBeams = *reinterpret_cast<IViewRenderBeams**>(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 0x1); // @xref: "r_drawbrushmodels"
	if (ViewRenderBeams == nullptr)
		return false;

	Input =	*reinterpret_cast<IInput**>(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1); // @note: or address of some indexed input function in chlclient class (like IN_ActivateMouse, IN_DeactivateMouse, IN_Accumulate, IN_ClearStates) + 0x1 (jmp to m_pInput)
	if (Input == nullptr)
		return false;

	ClientState = **reinterpret_cast<IClientState***>(MEM::FindPattern(ENGINE_DLL, XorStr("A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07")) + 0x1);
	if (ClientState == nullptr)
		return false;

	WeaponSystem = *reinterpret_cast<IWeaponSystem**>(MEM::FindPattern(CLIENT_DLL, XorStr("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 0x2);
	if (WeaponSystem == nullptr)
		return false;

	GlowManager = *reinterpret_cast<IGlowObjectManager**>(MEM::FindPattern(CLIENT_DLL, XorStr("0F 11 05 ? ? ? ? 83 C8 01")) + 0x3);
	if (GlowManager == nullptr)
		return false;

	return true;
}

template <typename T>
T* I::Capture(const std::string_view szModuleName, const std::string_view szInterface)
{
	const auto GetRegisterList = [&szModuleName]() -> CInterfaceRegister*
	{
		void* oCreateInterface = nullptr;

		if (const auto hModule = MEM::GetModuleBaseHandle(szModuleName); hModule != nullptr)
			oCreateInterface = MEM::GetExportAddress(hModule, XorStr("CreateInterface"));

		if (oCreateInterface == nullptr)
			throw std::runtime_error(XorStr("failed get createinterface address"));

		const std::uintptr_t uCreateInterfaceRelative = reinterpret_cast<std::uintptr_t>(oCreateInterface) + 0x5;
		const std::uintptr_t uCreateInterface = uCreateInterfaceRelative + 4U + *reinterpret_cast<std::int32_t*>(uCreateInterfaceRelative);
		return **reinterpret_cast<CInterfaceRegister***>(uCreateInterface + 0x6);
	};

	for (auto pRegister = GetRegisterList(); pRegister != nullptr; pRegister = pRegister->pNext)
	{
		// found needed interface
		if ((std::string_view(pRegister->szName).compare(0U, szInterface.length(), szInterface) == 0 &&
			// and it have digits after name
			std::atoi(pRegister->szName + szInterface.length()) > 0) ||
			// or given full name with hardcoded digits
			szInterface.compare(pRegister->szName) == 0)
		{
			// capture our interface
			auto pInterface = pRegister->pCreateFn();

			// log interface address
			L::Print(XorStr("captured {} interface -> {:#08X}"), pRegister->szName, reinterpret_cast<std::uintptr_t>(pInterface));

			return static_cast<T*>(pInterface);
		}
	}

	#ifdef DEBUG_CONSOLE
	L::PushConsoleColor(FOREGROUND_INTENSE_RED);
	L::Print(XorStr("[error] failed to find interface \"{}\" in \"{}\""), szInterface, szModuleName);
	L::PopConsoleColor();
	#endif

	return nullptr;
}
