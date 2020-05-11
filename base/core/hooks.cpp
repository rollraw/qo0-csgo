// used: std::array
#include <array>

#include "hooks.h"
// used: global variables
#include "../global.h"
// used: cheat variables
#include "variables.h"
// used: input system
#include "../utilities/inputsystem.h"
// used: logging
#include "../utilities/logging.h"
// used: initialize state
#include "../utilities/draw.h"
// used: get localplayer, find pattern, set localplayer ready
#include "../utilities.h"
// used: render windows
#include "menu.h"

/* features */
#include "../features/lagcompensation.h"
#include "../features/prediction.h"
#include "../features/ragebot.h"
#include "../features/antiaim.h"
#include "../features/legitbot.h"
#include "../features/triggerbot.h"
#include "../features/visuals.h"
#include "../features/misc.h"
#include "../features/skinchanger.h"

constexpr std::array<const char*, 3U> arrSmokeMaterials =
{
	//XorStr("particle/vistasmokev1/vistasmokev1_fire"),  // to look cool fresh fashionable yo :sunglasses: (if u wont be cool just uncomment this)
	XorStr("particle/vistasmokev1/vistasmokev1_smokegrenade"),
	XorStr("particle/vistasmokev1/vistasmokev1_emods"),
	XorStr("particle/vistasmokev1/vistasmokev1_emods_impactdust"),
};

#pragma region hooks_get
bool H::Setup()
{
	if (!VMT::Direct.Setup(I::DirectDevice))
		return false;

	VMT::Direct.Replace(H::hkReset, VTABLE::RESET);
	VMT::Direct.Replace(H::hkEndScene, VTABLE::ENDSCENE);

	if (!VMT::Client.Setup(I::Client))
		return false;

	VMT::Client.Replace(H::hkFrameStageNotify, VTABLE::FRAMESTAGENOTIFY);
	VMT::Client.Replace(H::hkDispatchUserMessage, VTABLE::DISPATCHUSERMESSAGE);

	if (!VMT::ClientMode.Setup(I::ClientMode))
		return false;

	VMT::ClientMode.Replace(H::hkOverrideView, VTABLE::OVERRIDEVIEW);

	// @note: be useful for mouse event aimbot
	#if 0
	VMT::ClientMode.Replace(H::hkOverrideMouseInput, VTABLE::OVERRIDEMOUSEINPUT); 
	#endif

	VMT::ClientMode.Replace(H::hkCreateMove, VTABLE::CREATEMOVE);
	VMT::ClientMode.Replace(H::hkGetViewModelFOV, VTABLE::GETVIEWMODELFOV);
	VMT::ClientMode.Replace(H::hkDoPostScreenEffects, VTABLE::DOPOSTSCREENEFFECTS);

	if (!VMT::Engine.Setup(I::Engine))
		return false;

	VMT::Engine.Replace(H::hkIsConnected, VTABLE::ISCONNECTED);

	if (!VMT::BspQuery.Setup(I::Engine->GetBSPTreeQuery()))
		return false;

	VMT::BspQuery.Replace(H::hkListLeavesInBox, VTABLE::LISTLEAVESINBOX);

	if (!VMT::Panel.Setup(I::Panel))
		return false;

	VMT::Panel.Replace(H::hkPaintTraverse, VTABLE::PAINTTRAVERSE);

	if (!VMT::ModelRender.Setup(I::ModelRender))
		return false;

	VMT::ModelRender.Replace(H::hkDrawModelExecute, VTABLE::DRAWMODELEXECUTE);

	if (!VMT::Prediction.Setup(I::Prediction))
		return false;

	VMT::Prediction.Replace(H::hkRunCommand, VTABLE::RUNCOMMAND);

	if (!VMT::SteamGameCoordinator.Setup(I::SteamGameCoordinator))
		return false;

	VMT::SteamGameCoordinator.Replace(H::hkSendMessage, VTABLE::SENDMESSAGE);
	VMT::SteamGameCoordinator.Replace(H::hkRetrieveMessage, VTABLE::RETRIEVEMESSAGE);

	if (!VMT::Sound.Setup(I::EngineSound))
		return false;

	VMT::Sound.Replace(H::hkEmitSound, VTABLE::EMITSOUND);

	if (!VMT::Surface.Setup(I::Surface))
		return false;

	VMT::Surface.Replace(H::hkLockCursor, VTABLE::LOCKCURSOR);
	VMT::Surface.Replace(H::hkPlaySound, VTABLE::PLAYSOUND);

	static CConVar* sv_cheats = I::ConVar->FindVar(XorStr("sv_cheats"));
	if (!VMT::SvCheats.Setup(sv_cheats))
		return false;

	VMT::SvCheats.Replace(H::hkSvCheatsGetBool, VTABLE::SVCHEATS_GETBOOL);

	return true;
}

void H::Restore()
{
	VMT::Direct.~CVMTHook();
	VMT::Panel.~CVMTHook();
	VMT::NetChannel.~CVMTHook();
	VMT::Client.~CVMTHook();
	VMT::ClientMode.~CVMTHook();
	VMT::Engine.~CVMTHook();
	VMT::BspQuery.~CVMTHook();
	VMT::Prediction.~CVMTHook();
	VMT::SteamGameCoordinator.~CVMTHook();
	VMT::Sound.~CVMTHook();
	VMT::ModelRender.~CVMTHook();
	VMT::Surface.~CVMTHook();
	VMT::GameEvent.~CVMTHook();
	VMT::SvCheats.~CVMTHook();
}
#pragma endregion

#pragma region hooks_handlers
long D3DAPI H::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	static auto oReset = VMT::Direct.GetOriginal<decltype(&hkReset)>(VTABLE::RESET);

	// check for first initialization
	if (!D::bInitialized)
		return oReset(pDevice, pPresentationParameters);

	// invalidate vertex & index buffer, release fonts texture
	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hReset = oReset(pDevice, pPresentationParameters);

	// get directx device and create fonts texture
	if (hReset == D3D_OK)
		ImGui_ImplDX9_CreateDeviceObjects();

	return hReset;
}

long D3DAPI H::hkEndScene(IDirect3DDevice9* pDevice)
{
	static auto oEndScene = VMT::Direct.GetOriginal<decltype(&hkEndScene)>(VTABLE::ENDSCENE);
	static void* pUsedAddress = _ReturnAddress();

	// search for gameoverlay address
	MEMORY_BASIC_INFORMATION memInfo;
	VirtualQuery(_ReturnAddress(), &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

	TCHAR wModulePath[MAX_PATH];
	GetModuleFileName((HMODULE)memInfo.AllocationBase, wModulePath, MAX_PATH);

	if (strstr(wModulePath, XorStr("gameoverlay")) != nullptr)
		pUsedAddress = _ReturnAddress();

	// check for called from gameoverlay and render here to bypass capturing programs
	if (_ReturnAddress() == pUsedAddress)
	{
		// init gui (fonts, sizes, styles, colors) once
		if (!D::bInitialized)
			D::Setup(pDevice);

		DWORD dwColorWriteOld = 0UL, dwSRGBWriteOld = 0UL;

		// save
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwColorWriteOld);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &dwSRGBWriteOld);

		// set
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// render cheat menu & visuals
		W::MainWindow(pDevice);

		ImGui::EndFrame();
		ImGui::Render();

		// render draw lists from draw data
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		// restore
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwColorWriteOld);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, dwSRGBWriteOld);
	}

	return oEndScene(pDevice);
}

bool FASTCALL H::hkCreateMove(IClientModeShared* thisptr, int edx, float flInputSampleTime, CUserCmd* pCmd)
{
	static auto oCreateMove = VMT::ClientMode.GetOriginal<decltype(&hkCreateMove)>(VTABLE::CREATEMOVE);

	/*
	 * get global localplayer pointer
	 * @note: dont forget check global localplayer for nullptr when using not in createmove
	 * also not recommended use so far
	 */
	CBaseEntity* pLocal = G::pLocal = U::GetLocalPlayer();
	
	if (pCmd->iCommandNumber == 0)
		return oCreateMove(thisptr, edx, flInputSampleTime, pCmd);

	// get global cmd pointer
	G::pCmd = pCmd;

	if (I::ClientState == nullptr || I::Engine->IsPlayingDemo())
		return oCreateMove(thisptr, edx, flInputSampleTime, pCmd);

	// get stack frame without asm inlines
	// safe and will not break if you omitting frame pointer
	const volatile auto vlBaseAddress = *(std::uintptr_t*)((std::uintptr_t)_AddressOfReturnAddress() - sizeof(std::uintptr_t));

	/*
	 * get sendpacket pointer from stack frame
	 * if use global sendpacket value then sendpacket applies only on next tick
	 * im not recommend use globals anywhere
	 */
	bool& bSendPacket = *(bool*)(vlBaseAddress - 0x1C);

	/*
	 * if you are not choking commands your real angle
	 * will be sent to the server every tick
	 * and the code to get 'real angles' will not be run 
	 * your angles will be stuck on your last choked command's angles until you start choking again
	 */
	static QAngle angLastChokedReal;

	// save previous view angles for movement correction
	QAngle angOldViewPoint = pCmd->angViewPoint;

	CMiscellaneous::Get().Run(pCmd, pLocal, bSendPacket);

	/*
	 * CL_RunPrediction
	 * correct prediction when framerate is lower than tickrate
	 * https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/cl_pred.cpp#L41
	 */
	if (I::ClientState->iDeltaTick > 0)
		I::Prediction->Update(I::ClientState->iDeltaTick, I::ClientState->iDeltaTick > 0, I::ClientState->nLastCommandAck, I::ClientState->nLastOutgoingCommand + I::ClientState->iChokedCommands);

	// @note: need do bunnyhop and other movements before prediction
	CPrediction::Get().Start(pCmd, pLocal);
	{
		if (C::Get<bool>(Vars.bAutoPistol))
			CMiscellaneous::Get().AutoPistol(pCmd, pLocal);

		if (C::Get<bool>(Vars.bRage))
			CRageBot::Get().Run(pCmd, pLocal, bSendPacket);

		if (C::Get<bool>(Vars.bFakeLag) || C::Get<bool>(Vars.bAntiAim))
			CMiscellaneous::Get().FakeLag(pCmd, pLocal, bSendPacket);

		if (C::Get<bool>(Vars.bAntiAim))
			CAntiAim::Get().Run(pCmd, pLocal, pCmd->angViewPoint, bSendPacket);

		if (C::Get<bool>(Vars.bLegit))
			CLegitBot::Get().Run(pCmd, pLocal, bSendPacket);

		if (C::Get<bool>(Vars.bTrigger))
			CTriggerBot::Get().Run(pCmd, pLocal);
	}
	CPrediction::Get().End(pCmd, pLocal);

	CMiscellaneous::Get().MovementCorrection(pCmd, angOldViewPoint);

	// clamp & normalize view angles
	if (C::Get<bool>(Vars.bAntiUntrusted))
	{
		pCmd->angViewPoint.Normalize();
		pCmd->angViewPoint.Clamp();
	}

	// we are choking a command, and we have not already choked a command
	if (!bSendPacket && I::ClientState->iChokedCommands == 0)
		// save our first choked angles for later use
		angLastChokedReal = pCmd->angViewPoint;
	// if we are sending a command, and we have previously choked commands
	else if (bSendPacket && I::ClientState->iChokedCommands > 0)
		// apply the first choked angles
		G::angRealView = angLastChokedReal;
	// if we are sending a command, and have not choked any commands
	else if (bSendPacket && I::ClientState->iChokedCommands == 0)
		// apply the sent angles
		G::angRealView = pCmd->angViewPoint;
	else if (!bSendPacket)
	{
		G::angFakeView = pCmd->angViewPoint;
		G::vecFakeOrigin = pLocal->GetOrigin();
	}

	// netchannel pointer
	INetChannel* pNetChannel = (INetChannel*)I::ClientState->pNetChannel;

	if (C::Get<bool>(Vars.bPingSpike))
		CLagCompensation::Get().UpdateIncomingSequences(pNetChannel);
	else
		CLagCompensation::Get().ClearIncomingSequences();

	if (!VMT::NetChannel.bTableHooked)
	{
		if (pNetChannel != nullptr)
		{
			if (VMT::NetChannel.Setup(pNetChannel))
			{
				VMT::NetChannel.Replace(H::hkSendNetMsg, VTABLE::SENDNETMSG);
				VMT::NetChannel.Replace(H::hkSendDatagram, VTABLE::SENDDATAGRAM);
			}
		}
		else
			VMT::NetChannel.~CVMTHook();
	}

	// get next global sendpacket state
	G::bSendPacket = bSendPacket;

	// @note: i seen many times this mistake and please do not set/clamp angles here cuz u get confused with psilent aimbot later!

	return false;
}

void FASTCALL H::hkPaintTraverse(ISurface* thisptr, int edx, unsigned int iPanel, bool bForceRepaint, bool bForce)
{
	static auto oPaintTraverse = VMT::Panel.GetOriginal<decltype(&hkPaintTraverse)>(VTABLE::PAINTTRAVERSE);
	FNV1A_t uPanelHash = FNV1A::Hash(I::Panel->GetName(iPanel));

	// remove zoom panel
	if (!I::Engine->IsTakingScreenshot() && C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_SCOPE) && uPanelHash == FNV1A::HashConst("HudZoom"))
		return;

	oPaintTraverse(thisptr, edx, iPanel, bForceRepaint, bForce);
}

void FASTCALL H::hkPlaySound(ISurface* thisptr, int edx, const char* szFileName)
{
	static auto oPlaySound = VMT::Surface.GetOriginal<decltype(&hkPlaySound)>(VTABLE::PLAYSOUND);
	oPlaySound(thisptr, edx, szFileName);
}

void FASTCALL H::hkLockCursor(ISurface* thisptr, int edx)
{
	static auto oLockCursor = VMT::Surface.GetOriginal<decltype(&hkLockCursor)>(VTABLE::LOCKCURSOR);

	if (W::bMainOpened)
	{
		I::Surface->UnLockCursor();
		return;
	}

	oLockCursor(thisptr, edx);
}

void FASTCALL H::hkFrameStageNotify(IBaseClientDll* thisptr, int edx, EClientFrameStage stage)
{
	static auto oFrameStageNotify = VMT::Client.GetOriginal<decltype(&hkFrameStageNotify)>(VTABLE::FRAMESTAGENOTIFY);

	if (!I::Engine->IsInGame() || I::Engine->IsTakingScreenshot())
		return oFrameStageNotify(thisptr, edx, stage);

	CBaseEntity* pLocal = U::GetLocalPlayer();

	if (pLocal == nullptr)
		return oFrameStageNotify(thisptr, edx, stage);

	static QAngle angAimPunchOld = { }, angViewPunchOld = { };

	switch (stage)
	{
	case FRAME_RENDER_START:
	{
		/*
		 * start rendering the scene
		 * e.g. remove visual punch, thirdperson, other render/update stuff
		 */

		 // set max flash alpha
		*pLocal->GetFlashMaxAlpha() = C::Get<bool>(Vars.bWorld) ? C::Get<int>(Vars.iWorldMaxFlash) * 2.55f : 255.f;

		// no draw smoke
		for (auto szSmokeMaterial : arrSmokeMaterials)
		{
			IMaterial* pMaterial = I::MaterialSystem->FindMaterial(szSmokeMaterial, TEXTURE_GROUP_OTHER);

			if (pMaterial != nullptr && !pMaterial->IsErrorMaterial())
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, (C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_SMOKE)) ? true : false);
		}

		// remove smoke overlay
		static std::uintptr_t uSmokeCount = (MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0")) + 0x8); // @xref: "effects/overlaysmoke"
		if (C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_SMOKE))
			*(int*)(*(std::uintptr_t*)uSmokeCount) = 0;

		 // remove visual punch
		if (pLocal->IsAlive() && C::Get<bool>(Vars.bWorld))
		{
			// save old values
			angViewPunchOld = pLocal->GetViewPunch();
			angAimPunchOld = pLocal->GetPunch();
			if (C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_PUNCH))
			{
				// change current values
				pLocal->GetViewPunch() = QAngle(0, 0, 0);
				pLocal->GetPunch() = QAngle(0, 0, 0);
			}
		}

		// thirdperson
		static bool bThirdPerson = false;

		if (C::Get<bool>(Vars.bWorld) && C::Get<int>(Vars.iWorldThirdPersonKey) > 0)
		{
			if (!I::Engine->IsConsoleVisible() && !W::bMainOpened && IPT::IsKeyReleased(C::Get<int>(Vars.iWorldThirdPersonKey)))
				bThirdPerson = !bThirdPerson;
		}

		// my solution is here cuz camera offset is dynamically by standard functions without any garbage in overrideview hook
		I::Input->bCameraInThirdPerson = bThirdPerson && pLocal->IsAlive() && !I::Engine->IsTakingScreenshot();
		I::Input->vecCameraOffset.z = bThirdPerson ? C::Get<float>(Vars.flWorldThirdPersonOffset) : 150.f;

		/*
		 * @note: set custom thirdperson angles (e.g. fake's)
		 * works, but ducarii shared more properly thirdperson angles setting here:
		 * https://www.unknowncheats.me/forum/counterstrike-global-offensive/340475-properly-setting-thirdperson-angles.html
		 * and it will be more efficient
		 */
		if (bThirdPerson && pLocal->IsAlive() && I::Input->bCameraInThirdPerson && C::Get<bool>(Vars.bAntiAim))
			*pLocal->GetThirdPersonAngles() = G::angFakeView;

		break;
	};
	case FRAME_NET_UPDATE_END:
	{
		/*
		 * received all packets, now do interpolation, prediction, etc
		 * e.g. bactrack stuff
		 */

		break;
	};
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
	{
		/*
		 * data has been received and we are going to start calling postdataupdate
		 * e.g. resolver or skinchanger and other visuals
		 */

		break;
	};
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
	{
		/*
		 * data has been received and called postdataupdate on all data recipients
		 * e.g. now we can modify interpolation, other lagcompensation stuff
		 */

		break;
	}
	default:
		break;
	}

	oFrameStageNotify(thisptr, edx, stage);

	if (stage == FRAME_RENDER_START)
	{
		// restore original visual punch values
		if (C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_PUNCH))
		{
			pLocal->GetViewPunch() = angViewPunchOld;
			pLocal->GetPunch() = angAimPunchOld;
		}
	}
}

bool FASTCALL H::hkDispatchUserMessage(IBaseClientDll* thisptr, int edx, int iMessageType, unsigned int a3, unsigned int uBytes, const void* bfMessageData)
{
	static auto oDispatchUserMessage = VMT::Client.GetOriginal<decltype(&hkDispatchUserMessage)>(VTABLE::DISPATCHUSERMESSAGE);

	return oDispatchUserMessage(thisptr, edx, iMessageType, a3, uBytes, bfMessageData);
}

void FASTCALL H::hkDrawModelExecute(IVModelRender* thisptr, int edx, IMatRenderContext* pContext, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	static auto oDrawModelExecute = VMT::ModelRender.GetOriginal<decltype(&hkDrawModelExecute)>(VTABLE::DRAWMODELEXECUTE);

	if (!I::Engine->IsInGame() || I::ModelRender->IsForcedMaterialOverride() || I::Engine->IsTakingScreenshot())
		return oDrawModelExecute(thisptr, edx, pContext, state, pInfo, pCustomBoneToWorld);

	CBaseEntity* pLocal = U::GetLocalPlayer();

	// master check
	if (pLocal != nullptr && C::Get<bool>(Vars.bEsp) && C::Get<bool>(Vars.bEspChams))
		CVisuals::Get().Chams(pLocal, pContext, state, pInfo, pCustomBoneToWorld);

	// draw original model
	oDrawModelExecute(thisptr, edx, pContext, state, pInfo, pCustomBoneToWorld);

	// clear overrides
	I::ModelRender->ForcedMaterialOverride(nullptr);
}

int FASTCALL H::hkListLeavesInBox(void* thisptr, int edx, Vector& vecMins, Vector& vecMaxs, unsigned short* puList, int iListMax)
{
	static auto oListLeavesInBox = VMT::BspQuery.GetOriginal<decltype(&hkListLeavesInBox)>(VTABLE::LISTLEAVESINBOX);

	// @credits: soufiw
	// occulusion getting updated on player movement/angle change,
	// in RecomputeRenderableLeaves ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L674 )
	static std::uintptr_t uInsertIntoTree = (MEM::FindPattern(CLIENT_DLL, XorStr("56 52 FF 50 18")) + 0x5);

		// check esp state
	if (!C::Get<bool>(Vars.bEsp) | !C::Get<bool>(Vars.bEspChams) ||
		// filters check
		!C::Get<bool>(Vars.bEspChamsEnemies) || !C::Get<bool>(Vars.bEspChamsAllies)
		// check for return in CClientLeafSystem::InsertIntoTree
		|| _ReturnAddress() != (void*)uInsertIntoTree)
		return oListLeavesInBox(thisptr, edx, vecMins, vecMaxs, puList, iListMax);

	// get current renderable info from stack ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470 )
	RenderableInfo_t* pInfo = *(RenderableInfo_t**)((std::uintptr_t)_AddressOfReturnAddress() + 0x14);

	if (pInfo == nullptr || pInfo->pRenderable == nullptr)
		return oListLeavesInBox(thisptr, edx, vecMins, vecMaxs, puList, iListMax);

	// check if disabling occulusion for players ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1491 )
	CBaseEntity* pEntity = pInfo->pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (pEntity == nullptr || !pEntity->IsPlayer())
		return oListLeavesInBox(thisptr, edx, vecMins, vecMaxs, puList, iListMax);

	// fix render order, force translucent group ( https://www.unknowncheats.me/forum/2429206-post15.html )
	// AddRenderablesToRenderLists: https://i.imgur.com/hcg0NB5.png ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473 )
	pInfo->uFlags &= ~RENDER_FLAGS_FORCE_OPAQUE_PASS;
	pInfo->uFlags2 |= 0xC0;

	// extend world space bounds to maximum ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707 )
	static Vector vecMapMin(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
	static Vector vecMapMax(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
	return oListLeavesInBox(thisptr, edx, vecMapMin, vecMapMax, puList, iListMax);
}

bool FASTCALL H::hkIsConnected(IEngineClient* thisptr, int edx)
{
	static auto oIsConnected = VMT::Engine.GetOriginal<decltype(&hkIsConnected)>(VTABLE::ISCONNECTED);

	// @xref: "IsLoadoutAllowed"
	// sub above the string
	// sub in that function
	// .text : 103A2120 84 C0		test    al, al; Logical Compare
	static std::uintptr_t uLoadoutAllowedReturn = (MEM::FindPattern(CLIENT_DLL, XorStr("75 04 B0 01 5F")) - 0x2);

	// @credits: gavreel
	if (_ReturnAddress() == (void*)uLoadoutAllowedReturn && C::Get<bool>(Vars.bUnlockInventory))
		return false;

	return oIsConnected(thisptr, edx);
}

bool FASTCALL H::hkSendNetMsg(INetChannel* thisptr, int edx, INetMessage* pMessage, bool bForceReliable, bool bVoice)
{
	static auto oSendNetMsg = VMT::NetChannel.GetOriginal<decltype(&hkSendNetMsg)>(VTABLE::SENDNETMSG);

	/*
	 * @note: disable files crc check (sv_pure)
	 * dont send message if it has FileCRCCheck type
	 */
	if (pMessage->GetType() == 14)
		return false;

	/*
	 * @note: fix lag with chocking packets when voice chat is active
	 * check for voicedata group and enable voice stream
	 * @credits: Flaww
	 */
	if (pMessage->GetGroup() == 9)
		bVoice = true;

	return oSendNetMsg(thisptr, edx, pMessage, bForceReliable, bVoice);
}

int FASTCALL H::hkSendDatagram(INetChannel* pNetChannel, int edx, bf_write* pDatagram)
{
	static auto oSendDatagram = VMT::NetChannel.GetOriginal<decltype(&hkSendDatagram)>(VTABLE::SENDDATAGRAM);

	if (!C::Get<bool>(Vars.bPingSpike) || pDatagram != nullptr)
		return oSendDatagram(pNetChannel, edx, pDatagram);

	int oInReliableState = pNetChannel->iInReliableState;
	int oInSequenceNr = pNetChannel->iInSequenceNr;

	CLagCompensation::Get().AddLatencyToNetChannel(pNetChannel, CLagCompensation::Get().flFakeLatencyAmount);

	int iReturn = oSendDatagram(pNetChannel, edx, pDatagram);

	pNetChannel->iInReliableState = oInReliableState;
	pNetChannel->iInSequenceNr = oInSequenceNr;

	return iReturn;
}

void FASTCALL H::hkOverrideView(IClientModeShared* thisptr, int edx, CViewSetup* pSetup)
{
	static auto oOverrideView = VMT::ClientMode.GetOriginal<decltype(&hkOverrideView)>(VTABLE::OVERRIDEVIEW);
	
	if (!I::Engine->IsInGame() || I::Engine->IsTakingScreenshot())
		return oOverrideView(thisptr, edx, pSetup);

	// get camera origin
	G::vecCamera = pSetup->vecOrigin;

	CBaseEntity* pLocal = U::GetLocalPlayer();

	if (pLocal == nullptr)
		return oOverrideView(thisptr, edx, pSetup);

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return oOverrideView(thisptr, edx, pSetup);

	if (CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(*pWeapon->GetItemDefinitionIndex());
		// check for not zoomed sniper
		(pWeaponData != nullptr && pWeaponData->nWeaponType == WEAPONTYPE_SNIPER) ? pWeapon->GetZoomLevel() == 0 : true &&
		C::Get<bool>(Vars.bScreen) && C::Get<float>(Vars.flScreenCameraFOV) != 0.f)
		// set camera fov
		pSetup->flFOV += C::Get<float>(Vars.flScreenCameraFOV);

	oOverrideView(thisptr, edx, pSetup);
}

void FASTCALL H::hkOverrideMouseInput(IClientModeShared* thisptr, int edx, float* x, float* y)
{
	static auto oOverrideMouseInput = VMT::ClientMode.GetOriginal<decltype(&hkOverrideMouseInput)>(VTABLE::OVERRIDEMOUSEINPUT);

	if (!I::Engine->IsInGame())
		return oOverrideMouseInput(thisptr, edx, x, y);

	oOverrideMouseInput(thisptr, edx, x, y);
}

float FASTCALL H::hkGetViewModelFOV(IClientModeShared* thisptr, int edx)
{
	static auto oGetViewModelFOV = VMT::ClientMode.GetOriginal<decltype(&hkGetViewModelFOV)>(VTABLE::GETVIEWMODELFOV);

	if (!I::Engine->IsInGame() || I::Engine->IsTakingScreenshot())
		return oGetViewModelFOV(thisptr, edx);

	if (auto pLocal = U::GetLocalPlayer();
		pLocal != nullptr && pLocal->IsAlive() &&
		C::Get<bool>(Vars.bScreen) && C::Get<float>(Vars.flScreenViewModelFOV) != 0.f)
		return oGetViewModelFOV(thisptr, edx) + C::Get<float>(Vars.flScreenViewModelFOV);

	return oGetViewModelFOV(thisptr, edx);
}

int FASTCALL H::hkDoPostScreenEffects(IClientModeShared* thisptr, int edx, CViewSetup* pSetup)
{
	static auto oDoPostScreenEffects = VMT::ClientMode.GetOriginal<decltype(&hkDoPostScreenEffects)>(VTABLE::DOPOSTSCREENEFFECTS);

	if (!I::Engine->IsInGame() || I::Engine->IsTakingScreenshot())
		return oDoPostScreenEffects(thisptr, edx, pSetup);

	CBaseEntity* pLocal = U::GetLocalPlayer();

	if (pLocal != nullptr && I::GlowManager != nullptr && C::Get<bool>(Vars.bEsp) && C::Get<bool>(Vars.bEspGlow))
		CVisuals::Get().Glow(pLocal);

	return oDoPostScreenEffects(thisptr, edx, pSetup);
}

void FASTCALL H::hkRunCommand(IPrediction* thisptr, int edx, CBaseEntity* pEntity, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
{
	static auto oRunCommand = VMT::Prediction.GetOriginal<decltype(&hkRunCommand)>(VTABLE::RUNCOMMAND);
	oRunCommand(thisptr, edx, pEntity, pCmd, pMoveHelper);

	// get movehelper interface pointer
	I::MoveHelper = pMoveHelper;
}

int FASTCALL H::hkSendMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t uMsgType, const void* pData, std::uint32_t uData)
{
	static auto oSendMessage = VMT::SteamGameCoordinator.GetOriginal<decltype(&hkSendMessage)>(VTABLE::SENDMESSAGE);

	std::uint32_t uMessageType = uMsgType & 0x7FFFFFFF;
	void* pDataMutable = const_cast<void*>(pData);

	int gcStatus = oSendMessage(thisptr, edx, uMsgType, pDataMutable, uData);

	if (gcStatus != EGCResultOK)
		return gcStatus;

	#if _DEBUG
	L::PushConsoleColor(FOREGROUND_INTENSE_GREEN | FOREGROUND_RED);
	L::Print(fmt::format(XorStr("[<-] Message sent to GC {:d}!"), uMessageType));
	L::PopConsoleColor();
	#endif

	return gcStatus;
}

int FASTCALL H::hkRetrieveMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize)
{
	static auto oRetrieveMessage = VMT::SteamGameCoordinator.GetOriginal<decltype(&hkRetrieveMessage)>(VTABLE::RETRIEVEMESSAGE);
	int gcStatus = oRetrieveMessage(thisptr, edx, puMsgType, pDest, uDest, puMsgSize);

	if (gcStatus != EGCResultOK)
		return gcStatus;

	std::uint32_t uMessageType = *puMsgType & 0x7FFFFFFF;

	#if _DEBUG
	L::PushConsoleColor(FOREGROUND_INTENSE_GREEN | FOREGROUND_RED);
	L::Print(fmt::format(XorStr("[->] Message received from GC {:d}!"), uMessageType));
	L::PopConsoleColor();
	#endif

	if (C::Get<bool>(Vars.bAutoAccept) && uMessageType == 9177 /*k_EMsgGCCStrike15_v2_GCToClientSteamdatagramTicket*/)
		U::SetLocalPlayerReady();

	return gcStatus;
}

void FASTCALL H::hkEmitSound(IEngineSound* thisptr, int edx, IRecipientFilter& filter, int nEntityIndex, int iChannel, const char* szSoundEntry, unsigned int uSoundEntryHash, const char* szSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector* vecOrigin, const Vector* vecDirection, CUtlVector<Vector>* pUtlVecOrigins, bool bUpdatePositions, int flSoundTime, int nSpeakerEntity, StartSoundParams_t& parameters)
{
	static auto oEmitSound = VMT::Sound.GetOriginal<decltype(&hkEmitSound)>(VTABLE::EMITSOUND);

	// @note: for sound esp use: "player/footsteps", "player/land", "clipout" sounds check

	if (C::Get<bool>(Vars.bAutoAccept) && strcmp(szSoundEntry, XorStr("UIPanorama.popup_accept_match_beep")) == 0)
	{
		U::SetLocalPlayerReady();

		FLASHWINFO fwInfo;
		fwInfo.cbSize = sizeof(FLASHWINFO);
		fwInfo.hwnd = IPT::hWindow;
		fwInfo.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		fwInfo.uCount = 0;
		fwInfo.dwTimeout = 0;
		Beep(100, 5);
		FlashWindowEx(&fwInfo);
	}

	oEmitSound(thisptr, edx, filter, nEntityIndex, iChannel, szSoundEntry, uSoundEntryHash, szSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, vecOrigin, vecDirection, pUtlVecOrigins, bUpdatePositions, flSoundTime, nSpeakerEntity, parameters);
}

bool FASTCALL H::hkSvCheatsGetBool(CConVar* thisptr, int edx)
{
	static auto oSvCheatsGetBool = VMT::SvCheats.GetOriginal<decltype(&hkSvCheatsGetBool)>(VTABLE::SVCHEATS_GETBOOL);
	static std::uintptr_t uCAM_ThinkReturn = (MEM::FindPattern(CLIENT_DLL, XorStr("85 C0 75 30 38 86"))); // @xref: "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %16s"

	if (_ReturnAddress() == (void*)uCAM_ThinkReturn)
		return true;

	return oSvCheatsGetBool(thisptr, edx);
}

long CALLBACK H::hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// process keys
	IPT::Process(uMsg, wParam, lParam);

	// switch window state on key click
	if (C::Get<int>(Vars.iMenuKey) > 0 && IPT::IsKeyReleased(C::Get<int>(Vars.iMenuKey)))
		W::bMainOpened = !W::bMainOpened;

	// disable game input when menu is opened
	I::InputSystem->EnableInput(!W::bMainOpened);

	/*
	 * @note: we can use imgui input handler to our binds if remove menu state check
	 * with ImGui::IsKeyDown, ImGui::IsKeyPressed, etc functions
	 * but imgui api's keys down durations doesnt have forward compatibility
	 * and i dont want spend a lot of time on recode it
	 */
	if (D::bInitialized && W::bMainOpened && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return 1L;

	// return input controls to the game
	return CallWindowProcW(IPT::pOldWndProc, hWnd, uMsg, wParam, lParam);
}
#pragma endregion

#pragma region proxies_get
bool P::Setup()
{
	// @note: as example
	#if 0
	RecvProp_t* pSmokeEffectTickBegin = CNetvarManager::Get().mapProps[FNV1A::HashConst("DT_SmokeGrenadeProjectile->m_nSmokeEffectTickBegin")].pRecvProp;
	if (pSmokeEffectTickBegin == nullptr)
		return false;

	RVP::SmokeEffectTickBegin = std::make_shared<CRecvPropHook>(pSmokeEffectTickBegin, P::SmokeEffectTickBegin);
	#endif

	return true;
}

void P::Restore()
{
	#if 0
	// restore smoke effect
	RVP::SmokeEffectTickBegin->~CRecvPropHook();
	#endif
}
#pragma endregion

#pragma region proxies_handlers
void P::SmokeEffectTickBegin(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	static auto oSmokeEffectTickBegin = RVP::SmokeEffectTickBegin->GetOriginal();

	if (C::Get<bool>(Vars.bWorld) && C::Get<std::vector<bool>>(Vars.vecWorldRemovals).at(REMOVAL_SMOKE))
	{
		if (auto pEntity = (CBaseEntity*)pStruct; pEntity != nullptr)
			pEntity->GetOrigin() = Vector(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
	}

	oSmokeEffectTickBegin(pData, pStruct, pOut);
}
#pragma endregion
