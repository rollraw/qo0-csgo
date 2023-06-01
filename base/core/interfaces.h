#pragma once
// used: [d3d] idirect3ddevice9
#include <d3d9.h>

// used: cinterfaceregister
#include "../sdk/interfaces/ibaseinterface.h"

// forward declarations
class IBaseClientDll;
class IClientEntityList;
class IClientLeafSystem;
class IEffects;
class IGameMovement;
class IMoveHelper;
class IPrediction;
class IEngineClient;
class IEngineVGui;
class IEngineTrace;
class IEngineSound;
class INetworkStringTableContainer;
class IGameEventManager2;
class IVRenderView;
class IVDebugOverlay;
class IVEngineEffects;
class IVModelInfo;
class IVModelRender;
class IServerGameDLL;
class IMDLCache;
class IStudioRender;
class IConVar;
class IPhysicsSurfaceProps;
class IMaterialSystem;
class ISurface;
class IPanel;
class IInputSystem;
class ILocalize;
class IBaseFileSystem;
class IFileSystem;
class IMatchFramework;
class ISteamClient;
class ISteamUser;
class ISteamGameCoordinator;
class IMemAlloc;
class IKeyValuesSystem;
class IClientModeShared;
class IGlobalVarsBase;
class IViewRender;
class IViewRenderBeams;
class IInput;
class IClientState;
class IWeaponSystem;
class IGlowObjectManager;
class IHud;
class IEconItemSystem;

/*
 * INTERFACES
 * - pointers to commonly-used valve source-sdk interfaces,
 *   interfaces are virtual classes (tables) that have various virtual functions declared by game engine, that we can either call or hook
 */
namespace I
{
	bool Setup();

	/* @section: get */
	// search for a specific interface with given name in given module
	template <class T>
	T* Capture(const CInterfaceRegister* pModuleRegister, const char* szInterfaceName);

	/* @section: handles */
	inline IBaseClientDll* Client;
	inline IClientEntityList* ClientEntityList;
	inline IClientLeafSystem* ClientLeafSystem;
	inline IEffects* Effects;
	inline IGameMovement* GameMovement;
	inline IMoveHelper* MoveHelper;
	inline IPrediction* Prediction;
	inline IEngineClient* Engine;
	inline IEngineVGui* EngineVGui;
	inline IEngineTrace* EngineTrace;
	inline IEngineSound* EngineSound;
	inline INetworkStringTableContainer* NetworkStringTable;
	inline IGameEventManager2* EventManager;
	inline IVRenderView* RenderView;
	inline IVDebugOverlay* DebugOverlay;
	inline IVEngineEffects* EngineEffects;
	inline IVModelInfo* ModelInfo;
	inline IVModelRender* ModelRender;
	inline IServerGameDLL* Server;
	inline IMDLCache* MDLCache;
	inline IStudioRender* StudioRender;
	inline IConVar* ConVar;
	inline IPhysicsSurfaceProps* PhysicsProps;
	inline IMaterialSystem* MaterialSystem;
	inline ISurface* Surface;
	inline IPanel* Panel;
	inline IInputSystem* InputSystem;
	inline ILocalize* Localize;
	inline IBaseFileSystem* BaseFileSystem;
	inline IFileSystem* FileSystem;
	inline IMatchFramework* MatchFramework;
	inline ISteamClient* SteamClient;
	inline ISteamUser* SteamUser;
	inline ISteamGameCoordinator* SteamGameCoordinator;
	inline IMemAlloc* MemAlloc;
	inline IKeyValuesSystem* KeyValuesSystem;
	inline IClientModeShared* ClientMode;
	inline IGlobalVarsBase* Globals;
	inline IDirect3DDevice9* DirectDevice;
	inline IViewRender* ViewRender;
	inline IViewRenderBeams* ViewRenderBeams;
	inline IInput* Input;
	inline IClientState* ClientState;
	inline IWeaponSystem* WeaponSystem;
	inline IGlowObjectManager* GlowManager;
	inline IHud* Hud;
	inline IEconItemSystem* ItemSystem;
}
