#include "hooks.h"

// used: cheat variables
#include "variables.h"
// used: render windows
#include "menu.h"
// used: input system
#include "../utilities/inputsystem.h"
// used: l_print
#include "../utilities/log.h"
// used: initialize state
#include "../utilities/draw.h"
// used: setlocalplayerready
#include "../sdk.h"
// used: cbonebitlist, cikcontext, studio_buildmatrices
#include "../sdk/bonesetup.h"
// used: cbonemergecache
#include "../sdk/bonemergecache.h"

// used: interface handles
#include "interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/iprediction.h"
#include "../sdk/interfaces/iinput.h"
#include "../sdk/interfaces/icliententitylist.h"
#include "../sdk/interfaces/iclientleafsystem.h"
#include "../sdk/interfaces/iviewrender.h"
#include "../sdk/interfaces/isteamgamecoordinator.h"
#include "../sdk/interfaces/imdlcache.h"

// used: features handler
#include "../features.h"
// used: features declarations
#include "../features/animationcorrection.h"
#include "../features/lagcompensation.h"
#include "../features/visual.h"

bool H::Setup()
{
	if (MH_Initialize() != MH_OK)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to initialize minhook");
		return false;
	}

	if (!hkReset.Create(MEM::GetVFunc(I::DirectDevice, VTABLE::DXDEVICE::RESET), reinterpret_cast<void*>(&Reset)))
		return false;

	if (!hkEndScene.Create(MEM::GetVFunc(I::DirectDevice, VTABLE::DXDEVICE::ENDSCENE), reinterpret_cast<void*>(&EndScene)))
		return false;

	if (!hkCreateMove.Create(MEM::GetVFunc(I::Client, VTABLE::CLIENT::CREATEMOVE), reinterpret_cast<void*>(&CreateMoveProxy)))
		return false;

	if (!hkFrameStageNotify.Create(MEM::GetVFunc(I::Client, VTABLE::CLIENT::FRAMESTAGENOTIFY), reinterpret_cast<void*>(&FrameStageNotify)))
		return false;

	if (!hkOverrideView.Create(MEM::GetVFunc(I::ClientMode, VTABLE::CLIENTMODE::OVERRIDEVIEW), reinterpret_cast<void*>(&OverrideView)))
		return false;

	if (!hkGetViewModelFOV.Create(MEM::GetVFunc(I::ClientMode, VTABLE::CLIENTMODE::GETVIEWMODELFOV), reinterpret_cast<void*>(&GetViewModelFOV)))
		return false;

	if (!hkDoPostScreenSpaceEffects.Create(MEM::GetVFunc(I::ClientMode, VTABLE::CLIENTMODE::DOPOSTSCREENSPACEEFFECTS), reinterpret_cast<void*>(&DoPostScreenSpaceEffects)))
		return false;

	if (!hkIsConnected.Create(MEM::GetVFunc(I::Engine, VTABLE::ENGINE::ISCONNECTED), reinterpret_cast<void*>(&IsConnected)))
		return false;

	if (!hkIsHLTV.Create(MEM::GetVFunc(I::Engine, VTABLE::ENGINE::ISHLTV), reinterpret_cast<void*>(&IsHLTV)))
		return false;

	if (!hkListLeavesInBox.Create(MEM::GetVFunc(I::Engine->GetBSPTreeQuery(), VTABLE::SPATIALQUERY::LISTLEAVESINBOX), reinterpret_cast<void*>(&ListLeavesInBox)))
		return false;

	const void* pCNetChannelVTable = MEM::FindVTable(ENGINE_DLL, Q_XOR("CNetChan"));
	if (!hkSendNetMsg.Create(MEM::GetVFunc(pCNetChannelVTable, VTABLE::NETCHANNEL::SENDNETMSG), reinterpret_cast<void*>(&SendNetMsg)))
		return false;

	if (!hkSendDatagram.Create(MEM::GetVFunc(pCNetChannelVTable, VTABLE::NETCHANNEL::SENDDATAGRAM), reinterpret_cast<void*>(&SendDatagram)))
		return false;

	if (!hkRunCommand.Create(MEM::GetVFunc(I::Prediction, VTABLE::PREDICTION::RUNCOMMAND), reinterpret_cast<void*>(&RunCommand)))
		return false;

	if (!hkLockCursor.Create(MEM::GetVFunc(I::Surface, VTABLE::SURFACE::LOCKCURSOR), reinterpret_cast<void*>(&LockCursor)))
		return false;

	if (!hkCAM_ToFirstPerson.Create(MEM::GetVFunc(I::Input, VTABLE::INPUT::CAM_TOFIRSTPERSON), reinterpret_cast<void*>(&CAM_ToFirstPerson)))
		return false;

	if (!hkRenderView.Create(MEM::GetVFunc(I::ViewRender, VTABLE::VIEWRENDER::RENDERVIEW), reinterpret_cast<void*>(&RenderView)))
		return false;

	if (!hkRenderSmokeOverlay.Create(MEM::GetVFunc(I::ViewRender, VTABLE::VIEWRENDER::RENDERSMOKEOVERLAY), reinterpret_cast<void*>(&RenderSmokeOverlay)))
		return false;

	if (!hkDrawModel.Create(MEM::GetVFunc(I::StudioRender, VTABLE::STUDIORENDER::DRAWMODEL), reinterpret_cast<void*>(&DrawModel)))
		return false;

	if (!hkSendMessage.Create(MEM::GetVFunc(I::SteamGameCoordinator, VTABLE::STEAMGAMECOORDINATOR::SENDMESSAGE), reinterpret_cast<void*>(&SendMessage)))
		return false;

	if (!hkRetrieveMessage.Create(MEM::GetVFunc(I::SteamGameCoordinator, VTABLE::STEAMGAMECOORDINATOR::RETRIEVEMESSAGE), reinterpret_cast<void*>(&RetrieveMessage)))
		return false;

	if (!hkModifyEyePosition.Create(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14")), reinterpret_cast<void*>(&ModifyEyePosition)))
		return false;

	const void* pCCSPlayerVTable = MEM::FindVTable(CLIENT_DLL, Q_XOR("C_CSPlayer"));
	// this comes from inherited 'C_CSPlayer : IClientRenderable' vtable
	if (!hkSetupBones.Create(/*MEM::GetVFunc(pCSPlayerRenderableVTable, VTABLE::CSPLAYER::SETUPBONES)*/MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 56 8B F1 51 8D")), reinterpret_cast<void*>(&SetupBones)))
		return false;

	if (!hkFireBullet.Create(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F0 81 EC ? ? ? ? F3 0F 7E")), reinterpret_cast<void*>(&FireBullet)))
		return false;

	if (!hkUpdateClientSideAnimation.Create(MEM::GetVFunc(pCCSPlayerVTable, VTABLE::CSPLAYER::UPDATECLIENTSIDEANIMATION), reinterpret_cast<void*>(&UpdateClientSideAnimation)))
		return false;

	if (!hkCalcView.Create(MEM::GetVFunc(pCCSPlayerVTable, VTABLE::CSPLAYER::CALCVIEW), reinterpret_cast<void*>(&CalcView)))
		return false;

	return true;
}

void H::Destroy()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	MH_Uninitialize();
}

#pragma region hooks_handlers
long D3DAPI H::Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	const auto oReset = hkReset.GetOriginal<decltype(&Reset)>();

	D::OnPreReset(pDevice);

	const HRESULT hReset = oReset(pDevice, pPresentationParameters);

	// get directx device and create fonts texture
	if (hReset == D3D_OK)
		D::OnPostReset(pDevice);

	return hReset;
}

long D3DAPI H::EndScene(IDirect3DDevice9* pDevice)
{
	const auto oEndScene = hkEndScene.GetOriginal<decltype(&EndScene)>();

	const void* pReturnAddress = Q_RETURN_ADDRESS();
	static const void* pGameOverlayReturnAddress = nullptr;

	if (pGameOverlayReturnAddress == nullptr)
	{
		MEMORY_BASIC_INFORMATION memInfo = { };
		::VirtualQuery(pReturnAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		// search for gameoverlay return address
		if (const wchar_t* wszCallModuleName = MEM::GetModuleBaseFileName(memInfo.AllocationBase); CRT::StringCompare(wszCallModuleName, GAMEOVERLAYRENDERER_DLL) == 0)
			pGameOverlayReturnAddress = pReturnAddress;
	}

	// check is called from gameoverlay and render within it to bypass most of screen capturing programs
	if (pReturnAddress == pGameOverlayReturnAddress)
	{
		// init gui (fonts, sizes, styles, colors) once
		if (!D::Setup(pDevice))
		{
			L_PRINT(LOG_ERROR) << Q_XOR("failed to initialize draw system");
			return oEndScene(pDevice);
		}
		
		MENU::OnEndScene(pDevice);
	}

	return oEndScene(pDevice);
}

static void Q_STDCALL CreateMove(int nSequenceNumber, float flInputSampleFrametime, bool bIsActive, bool* pbSendPacket)
{
	// process original CHLClient::CreateMove -> IInput::CreateMove
	H::hkCreateMove.CallOriginal<void>(I::Client, 0, nSequenceNumber, flInputSampleFrametime, bIsActive);

	CUserCmd* pCmd = I::Input->GetUserCmd(nSequenceNumber);
	CVerifiedUserCmd* pVerifiedCmd = I::Input->GetVerifiedCmd(nSequenceNumber);

	// check do we have valid commands, finished signing on to server and not playing back demos (where our commands are ignored)
	if (pCmd == nullptr || pVerifiedCmd == nullptr || !bIsActive)
		return;

	// local player is always valid during 'CreateMove()' call
	CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer();

	F::OnMove(pLocal, pCmd, pbSendPacket, nSequenceNumber);

	// @note: also update the backup values so the game doesn't know we've changed them and trigger a possible detection vector, added since ~11.06.22 (version 1.38.3.7, build 1490)
	// @ida CInput::CreateMove(): client.dll -> "8B 46 30 89 46 58"
	pCmd->angViewPointBackup = pCmd->angViewPoint;
	pCmd->nButtonsBackup = pCmd->nButtons;

	pVerifiedCmd->userCmd = *pCmd;
	pVerifiedCmd->uHashCRC = pCmd->GetChecksum();
}

// call hierarchy: [engine.dll] ... -> CL_Move() -> [client.dll] CHLClient::CreateMove() -> CInput::CreateMove() -> ClientModeShared::CreateMove() -> ...
// @note: to outperform that, you can manipulate send packet state directly through 'CL_Move'
Q_NAKED void Q_FASTCALL H::CreateMoveProxy(IBaseClientDll* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive)
{
	// @ida CHLClient::CreateMove(): client.dll -> "55 8B EC 8B 4D 04 83 EC 08 E8"
	__asm
	{
		push ebp // save register
		mov ebp, esp; // store stack to register
		push ebx; // save register
		push esp; // bSendPacket from caller stack
		push [ebp+10h]; // bIsActive
		push [ebp+0Ch]; // flInputSampleFrametime
		push [ebp+8]; // nSequenceNumber
		call CreateMove
		pop ebx // restore register
		pop ebp // restore register 
		retn 0Ch
	}
}

// call hierarchy: [engine.dll] ... -> ClientDLL_FrameStageNotify() -> [client.dll] CHLClient::FrameStageNotify() -> ...
void Q_FASTCALL H::FrameStageNotify(IBaseClientDll* thisptr, int edx, EClientFrameStage nStage)
{
	F::OnFrame(nStage);

	hkFrameStageNotify.CallOriginal<void>(thisptr, edx, nStage);
}

// call hierarchy: [client.dll] ... -> CViewRender::SetUpView() -> ClientModeShared::OverrideView() -> ...
void Q_FASTCALL H::OverrideView(IClientModeShared* thisptr, int edx, CViewSetup* pSetup)
{
	F::OnPreOverrideView(pSetup);

	hkOverrideView.CallOriginal<void>(thisptr, edx, pSetup);

	F::OnPostOverrideView(pSetup);
}

// call hierarchy: [client.dll] ... -> CViewRender::SetUpView() -> ClientModeShared::GetViewModelFOV()
float Q_FASTCALL H::GetViewModelFOV(IClientModeShared* thisptr, int edx)
{
	float flReturn = hkGetViewModelFOV.CallOriginal<float>(thisptr, edx);

	F::OnGetViewModelFOV(&flReturn);

	return flReturn;
}

// call hierarchy: [client.dll] ... -> CViewRender::RenderView() -> ClientModeCSNormal::DoPostScreenSpaceEffects()
void Q_FASTCALL H::DoPostScreenSpaceEffects(IClientModeShared* thisptr, int edx, CViewSetup* pSetup)
{
	F::OnDoPostScreenSpaceEffects(pSetup);

	hkDoPostScreenSpaceEffects.CallOriginal<void>(thisptr, edx, pSetup);
}

// call hierarchy: [engine.dll, client.dll, ...] ... -> [engine.dll] CEngineClient::IsConnected()
bool Q_FASTCALL H::IsConnected(IEngineClient* thisptr, int edx)
{
	// @xref: "IsLoadoutAllowed"
	static const auto pLoadoutAllowedReturn = MEM::FindPattern(CLIENT_DLL, Q_XOR("84 C0 75 05 B0 01 5F"));

	// check is called from 'CCSGameRules::IsLoadoutAllowed()' @credits: gavreel
	if (const volatile auto pReturnAddress = static_cast<std::uint8_t*>(Q_RETURN_ADDRESS()); pReturnAddress == pLoadoutAllowedReturn && C::Get<bool>(Vars.bMiscUnlockInventory))
		return false;

	return hkIsConnected.CallOriginal<bool>(thisptr, edx);
}

// call hierarchy: [engine.dll, client.dll, ...] ... -> [engine.dll] CEngineClient::IsHLTV()
bool Q_FASTCALL H::IsHLTV(IEngineClient* thisptr, int edx)
{
	const volatile auto pReturnAddress = static_cast<std::uint8_t*>(Q_RETURN_ADDRESS());

	if (static const auto pSetupVelocityReturn = MEM::FindPattern(CLIENT_DLL, Q_XOR("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0"));
		// force 'CCSGOPlayerAnimState::SetupVelocity()' to use 'C_CSPlayer::GetAbsVelocity()' instead of 'C_BaseEntity::EstimateAbsVelocity()'
		pReturnAddress == pSetupVelocityReturn)
		return true;

	return hkIsHLTV.CallOriginal<bool>(thisptr, edx);
}

// call hierarchy: [engine.dll] ... -> [client.dll] CHLClient::FrameStageNotify() -> OnRenderStart() -> CClientLeafSystem::RecomputeRenderableLeaves() -> CClientLeafSystem::InsertIntoTree() -> CEngineBSPTree::ListLeavesInBox()
int Q_FASTCALL H::ListLeavesInBox(void* thisptr, int edx, const Vector_t& vecMins, const Vector_t& vecMaxs, unsigned short* puList, int nListMax)
{
	// @credits: soufiw
	// occlusion getting updated on player movement/angle change, in 'CClientLeafSystem::RecomputeRenderableLeaves()'
	static const auto pInsertIntoTreeReturn = MEM::FindPattern(CLIENT_DLL, Q_XOR("56 52 FF 50 18")) + 0x5; // @xref: "<unknown renderable>"

	if (const volatile auto pReturnAddress = static_cast<std::uint8_t*>(Q_RETURN_ADDRESS());
		// check is called from 'CClientLeafSystem::InsertIntoTree()'
		pReturnAddress != pInsertIntoTreeReturn)
		return hkListLeavesInBox.CallOriginal<int>(thisptr, edx, &vecMins, &vecMaxs, puList, nListMax);

	if (C::Get<bool>(Vars.bVisual) && C::Get<bool>(Vars.bVisualChams) && C::Get<bool>(Vars.bVisualChamsEnemies))
	{
		const volatile auto pStackFrame = static_cast<std::uint8_t*>(Q_FRAME_ADDRESS());

		// get current renderable info from the stack
		if (const auto pInfo = *reinterpret_cast<RenderableInfo_t**>(pStackFrame + 0x14); pInfo != nullptr)
		{
			if (const auto pRenderable = pInfo->pRenderable; pRenderable != nullptr)
			{
				CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer();

				// check is disabling occlusion for enemy players
				if (const auto pCSPlayer = static_cast<CCSPlayer*>(pRenderable->GetIClientUnknown()->GetBaseEntity()); pCSPlayer != nullptr && pCSPlayer->IsPlayer() && pLocal != nullptr && pCSPlayer->IsOtherEnemy(pLocal))
				{
					// @test: seems like valve fixed render order themselves or somewhat else and now chams draw perfect through decals w/o any fixes

					constexpr Vector_t vecUnlimitedMins = { -MAX_COORD_FLOAT, -MAX_COORD_FLOAT, -MAX_COORD_FLOAT };
					constexpr Vector_t vecUnlimitedMaxs = { MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT };
					return hkListLeavesInBox.CallOriginal<int>(thisptr, edx, &vecUnlimitedMins, &vecUnlimitedMaxs, puList, nListMax);
				}
			}
		}
	}

	return hkListLeavesInBox.CallOriginal<int>(thisptr, edx, &vecMins, &vecMaxs, puList, nListMax);
}

// call hierarchy: [engine.dll] ... -> CNetChan::SendNetMsg()
bool Q_FASTCALL H::SendNetMsg(INetChannel* thisptr, int edx, INetMessage& message, bool bForceReliable, bool bVoice)
{
	// @todo: just exclude our listened events from mask instead of preventing sending it at all, otherwise other things like killfeed etc (whatever events client uses) will not work
	// @note: exclude listened events that are not used by the client from the message to the server, to bypass SMAC detection
	//if (message.GetType() == 12 /* CCLCMsg_ListenEvents */)
	//	return false;

	return hkSendNetMsg.CallOriginal<bool>(thisptr, edx, &message, bForceReliable, bVoice);
}

// call hierarchy: [engine.dll] ... -> CNetChan::SendDatagram()
int Q_FASTCALL H::SendDatagram(INetChannel* thisptr, int edx, CBitWrite* pDatagram)
{
	if (!I::Engine->IsInGame() || pDatagram != nullptr)
		return hkSendDatagram.CallOriginal<int>(thisptr, edx, pDatagram);

	SendDatagramStack_t stack;
	F::OnPreSendDatagram(thisptr, stack);

	const int iReturn = hkSendDatagram.CallOriginal<int>(thisptr, edx, pDatagram);

	F::OnPostSendDatagram(thisptr, stack);

	return iReturn;
}

// call hierarchy: [engine.dll] ... -> CL_RunPrediction() -> CL_Predict() -> [client.dll] ... -> CPrediction::RunSimulation() -> C_BasePlayer::PhysicsSimulate() -> CPrediction::RunCommand()
void Q_FASTCALL H::RunCommand(IPrediction* thisptr, int edx, CBasePlayer* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
{
	// get movehelper interface pointer
	I::MoveHelper = pMoveHelper;

	/* there is tickbase/compressed netvars corrections */

	hkRunCommand.CallOriginal<void>(thisptr, edx, pPlayer, pCmd, pMoveHelper);
}

// call hierarchy: [engine.dll, client.dll, vgui2.dll, vguimatsurface.dll] ... -> CMatSystemSurface::CalculateMouseVisible() -> CMatSystemSurface::LockCursor()
void Q_FASTCALL H::LockCursor(ISurface* thisptr, int edx)
{
	if (MENU::bMainOpened)
	{
		I::Surface->UnLockCursor();
		return;
	}

	hkLockCursor.CallOriginal<void>(thisptr, edx);
}

// call hierarchy: [client.dll] ... -> CInput::CAM_Think() -> CAM_ToFirstPerson() -> CInput::CAM_ToFirstPerson()
void Q_FASTCALL H::CAM_ToFirstPerson(IInput* thisptr, int edx)
{
	// prevent thirdperson flicking because of not passed "sv_cheats" check inside 'CInput::CAM_Think()'
	if (CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer(); C::Get<bool>(Vars.bVisualWorld) && IPT::GetBindState(C::Get<KeyBind_t>(Vars.keyVisualWorldThirdPerson)) && pLocal != nullptr && pLocal->IsAlive())
		return;

	hkCAM_ToFirstPerson.CallOriginal<void>(thisptr, edx);
}

// call hierarchy: [engine.dll] ... -> [client.dll] CHLClient::RenderView() -> CViewRender::RenderView()
void Q_FASTCALL H::RenderView(IViewRender* thisptr, int edx, const CViewSetup& viewSetup, const CViewSetup& viewSetupHUD, int nClearFlags, int nWhatToDraw)
{
	F::OnPreRenderView(viewSetup, viewSetupHUD, nClearFlags, &nWhatToDraw);

	hkRenderView.CallOriginal<void>(thisptr, edx, &viewSetup, &viewSetupHUD, nClearFlags, nWhatToDraw);

	F::OnPostRenderView(viewSetup, viewSetupHUD, nClearFlags, &nWhatToDraw);
}

// call hierarchy: [client.dll] ... -> CViewRender::RenderView() -> CCSViewRender::RenderSmokeOverlay()
void Q_FASTCALL H::RenderSmokeOverlay(IViewRender* thisptr, int edx, bool bPreViewModel)
{
	// prevent from drawing smoke overlay
	// and this is the only possible place to do that due to awesome optimization with disabling rendering of all models when we're fully obscured by smoke
	if (C::Get<bool>(Vars.bVisualWorld) && (C::Get<unsigned int>(Vars.nVisualWorldRemovals) & VISUAL_WORLD_REMOVAL_FLAG_SMOKE))
	{
		thisptr->flSmokeOverlayAmount = 0.0f;
		return;
	}

	hkRenderSmokeOverlay.CallOriginal<void>(thisptr, edx, bPreViewModel);
}

// call hierarchy: [tier3.dll] ... -> CMDL::Draw() | [client.dll] ... -> CModelRender::DrawModelExecute() / CModelRender::DrawModelShadow() -> CStudioRender::DrawModel() -> ...
void Q_FASTCALL H::DrawModel(IStudioRender* thisptr, int edx, DrawModelResults_t* pResults, const DrawModelInfo_t& info, Matrix3x4_t* pBoneToWorld, float* pflFlexWeights, float* pflFlexDelayedWeights, const Vector_t& vecModelOrigin, int nFlags)
{
	if (!F::VISUAL::OnDrawModel(pResults, info, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, vecModelOrigin, nFlags))
		hkDrawModel.CallOriginal<void>(thisptr, edx, pResults, &info, pBoneToWorld, pflFlexWeights, pflFlexDelayedWeights, &vecModelOrigin, nFlags);
}

// call hierarchy: @todo: [client.dll] ... -> GCSDK::CProtoBufMsgBase::BAsyncSendProto() -> ISteamGameCoordinator::SendMessage()
int Q_FASTCALL H::SendMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t uMessageHeader, const void* pData, std::uint32_t nDataSize)
{
	const std::uint32_t nMessageType = (uMessageHeader & 0x7FFFFFFF);

	/*
	 * @note: prevent from sending 'k_EMsgGCCStrike15_v2_ClientVarValueNotificationInfo' (9144) message
	 * used for possible detection of client-side variables modification
	 *
	 * @source: master/game/client/cstrike15/c_cs_player.cpp#L4704
	 * @ida C_CSPlayer::ClientThink(): client.dll -> "51 8D 4C 24 ? F2 0F 11 1D ? ? ? ? E8 ? ? ? ? 8B"
	 */
	if (nMessageType == 9144)
	{
		L_PRINT(LOG_INFO) << Q_XOR("[<-] prevented attempt to send GC message: ") << nMessageType;
		return EGCResultOK;
	}

	L_PRINT(LOG_NONE) << L::SetColor(LOG_COLOR_FORE_YELLOW | LOG_COLOR_FORE_INTENSITY) << Q_XOR("[<-] message sent to GC: ") << nMessageType;
	return hkSendMessage.CallOriginal<int>(thisptr, edx, uMessageHeader, pData, nDataSize);
}

// call hierarchy: @todo: [client.dll] ... -> ISteamGameCoordinator::RetrieveMessage()
int Q_FASTCALL H::RetrieveMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t* pMessageHeader, void* pDestination, std::uint32_t nDestinationSize, std::uint32_t* pnMessageSize)
{
	const int iResult = hkRetrieveMessage.CallOriginal<int>(thisptr, edx, pMessageHeader, pDestination, nDestinationSize, pnMessageSize);

	if (iResult != EGCResultOK)
		return iResult;

	const std::uint32_t nMessageType = (*pMessageHeader & 0x7FFFFFFF);
	L_PRINT(LOG_NONE) << L::SetColor(LOG_COLOR_FORE_YELLOW | LOG_COLOR_FORE_INTENSITY) << Q_XOR("[->] message received from GC: ") << nMessageType;

	// check for 'k_EMsgGCCStrike15_v2_GCToClientSteamdatagramTicket' message when we can accept the game
	if (C::Get<bool>(Vars.bMiscAutoAccept) && nMessageType == 9177)
		SDK::SetLocalPlayerReady();

	return iResult;
}

// call hierarchy: [client.dll] ... -> C_CSPlayer::Weapon_ShootPosition() / C_CSPlayer::CalcView() -> CCSGOPlayerAnimState::ModifyEyePosition()
void Q_FASTCALL H::ModifyEyePosition(CCSGOPlayerAnimState* thisptr, int edx, Vector_t& vecInputEyePosition)
{
	// force to use our server-side rebuild for all game calls
	thisptr->ModifyEyePosition(vecInputEyePosition);
}

// call hierarchy: [client.dll] ... -> C_CSPlayer::SetupBones()
bool Q_FASTCALL H::SetupBones(IClientRenderable* thisptr, int edx, Matrix3x4a_t* arrBonesToWorld, int iMaxBones, int nBoneMask, float flCurrentTime)
{
	/*
	 * server-side setup bones rebuild
	 * reworked to be client-server hybrid, all changes can be found by comments with "[side change]" marker
	 *
	 * @ida C_BaseAnimating::SetupBones(): client.dll -> ABS["E8 ? ? ? ? 5E 5D C2 10 00 32 C0" + 0x1] @xref: "C_BaseAnimating::SetupBones"
	 * @ida CBaseAnimating::SetupBones(): server.dll -> "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 8B C1" @xref: "CBaseAnimating::SetupBones"
	 *
	 * @ida C_CSPlayer::SetupBones(): client.dll -> "55 8B EC 56 8B F1 51 8D"
	 * @ida CCSPlayer::SetupBones(): server.dll -> "55 8B EC 83 E4 F8 51 53 FF"
	 */

#ifdef Q_PARANOID
	// manual cast to derived class
	CCSPlayer* pPlayer = static_cast<CCSPlayer*>(static_cast<void*>(&thisptr[-1]));
#else
	CCSPlayer* pPlayer = static_cast<CCSPlayer*>(thisptr);
#endif
	const int nPlayerIndex = pPlayer->GetIndex();

	/*
	 * [side change] instead of calling client's 'C_CSPlayer::ReevauluateAnimLOD' that also handles PVS, force to always build most detailed bones for players
	 * note that 'C_CSPlayer::ClientThink()' also calling this, but before this, so we don't care
	 *
	 * @ida C_CSPlayer::ReevauluateAnimLOD(): client.dll -> ABS["E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B 8F" + 0x1]
	 */
	pPlayer->GetCustomBlendingRuleMask() = -1;
	pPlayer->GetAnimationLODFlags() = 0U;
	pPlayer->GetOldAnimationLODFlags() = 0U;
	pPlayer->GetComputedAnimationLODFrame() = I::Globals->nFrameCount;

	// [side change] check if we aren't allowing the game to setup bones for this player and should use the cache instead
	if (!F::ANIMATION::IsGameAllowedToSetupBones(nPlayerIndex))
	{
		const Vector_t& vecAbsOrigin = pPlayer->GetAbsOrigin();

		auto& arrServerBones = F::ANIMATION::GetPlayerBoneMatrices(nPlayerIndex);
		F::ANIMATION::ConvertBonesPositionToWorldSpace(arrServerBones, nPlayerIndex, vecAbsOrigin);

		// force overwrite cache with our bones
		CUtlVectorAligned<Matrix3x4a_t>& vecCachedBonesData = pPlayer->GetCachedBonesData();
		CRT::MemoryCopy(vecCachedBonesData.Base(), arrServerBones, sizeof(Matrix3x4a_t) * vecCachedBonesData.Count());

		// force game to think that bone cache are still valid
		pPlayer->GetMostRecentModelBoneCounter() = CBaseAnimating::GetModelBoneCounter();

		if (arrBonesToWorld != nullptr)
			// perform a copy of bone matrices since caller may want to modify them
			CRT::MemoryCopy(arrBonesToWorld, arrServerBones, sizeof(Matrix3x4a_t) * iMaxBones);

		F::ANIMATION::ConvertBonesPositionToLocalSpace(arrServerBones, nPlayerIndex, vecAbsOrigin);

		// check is requested bones for attachments
		if (nBoneMask & BONE_USED_BY_ATTACHMENT)
		{
			CStudioHdr* pStudioHdr = pPlayer->GetModelPtr();

			if (pStudioHdr == nullptr)
				return false;

			pPlayer->SetupBones_AttachmentHelper(pStudioHdr);
		}

		return true;
	}

	// initialize RAII cache critical section
	CMDLCacheCriticalSection mdlCacheCriticalSection(I::MDLCache);

	CStudioHdr* pStudioHdr = pPlayer->GetModelPtr();
	if (pStudioHdr == nullptr)
	{
		Q_ASSERT(false); // tried to 'CBaseAnimating::GetSkeleton()' without a model
		return false;
	}

	// [side change] merged special cases of bone mask handling from client
	if (nBoneMask == -1)
	{
		Q_ASSERT(false); // @test: this never triggers so we can remove handling of this
		nBoneMask = pPlayer->GetPreviousBoneMask();
	}
	else
	{
		// if we're setting up LOD N, we have set up all lower LODs also, because lower LODs always use subsets of the bones of higher LODs
		int nLOD = 0;
		int nLODMask = BONE_USED_BY_VERTEX_LOD0;
		for (; nLOD < MAX_NUM_LODS; ++nLOD, nLODMask <<= 1)
		{
			if (nBoneMask & nLODMask)
				break;
		}
		for (; nLOD < MAX_NUM_LODS; ++nLOD, nLODMask <<= 1)
			nBoneMask |= nLODMask;
	}

	// get bone cache accessor and data it points to
	CBoneAccessor& boneAccessor = pPlayer->GetBoneAccessor();
	int& nEFlags = pPlayer->GetEFlags();

	// [side change] adapted for client's cache system, just adjust values that can be used by the game later
	int& nAccumulatedBoneMask = pPlayer->GetAccumulatedBoneMask();
	pPlayer->GetMostRecentModelBoneCounter() = CCSPlayer::GetModelBoneCounter();
	pPlayer->GetPreviousBoneMask() = nAccumulatedBoneMask;
	nAccumulatedBoneMask = nBoneMask;
	pPlayer->GetLastBoneSetupTime() = I::Globals->flCurrentTime;

	// allow access to the bones we're setting up
	boneAccessor.nReadableBones = nBoneMask;
	boneAccessor.nWritableBones = nBoneMask;

	// setting this flag forces move children to keep their abs transform invalidated
	Q_ASSERT((nEFlags & EFL_SETTING_UP_BONES) == 0);
	nEFlags |= EFL_SETTING_UP_BONES;

	// [side change] server also taking into account 'm_flEstIkOffset' for NPC's, we can safely ignore this
	const Vector_t& vecAbsOrigin = pPlayer->GetAbsOrigin();
	const QAngle_t& angAbsView = pPlayer->GetAbsAngles();

	// [side change] used client-side cache of bones position and rotation instead of new stack allocation
	// @todo: some game methods have alignment check that doesn't pass this way and cause debugger to break, so atm used stack allocated instead
#if 0
	BoneVector_t (&arrBonesPosition)[MAXSTUDIOBONES] = pPlayer->GetCachedBonesPosition();
	BoneQuaternionAligned_t (&arrBonesRotation)[MAXSTUDIOBONES] = pPlayer->GetCachedBonesRotation();
#else
	BoneVector_t arrBonesPosition[MAXSTUDIOBONES];
	BoneQuaternionAligned_t arrBonesRotation[MAXSTUDIOBONES];
#endif

	CBoneBitList arrBonesComputed = { };

	// [side change] removed 'CBaseAnimating::CanSkipAnimation()' branch (PVS optimization for NPC's)
	if (CIKContext* pIKContext = pPlayer->GetIKContext(); pIKContext != nullptr)
	{
		pIKContext->Init(pStudioHdr, angAbsView, vecAbsOrigin, flCurrentTime, I::Globals->nFrameCount, boneAccessor.nWritableBones);

		pPlayer->GetSkeleton(pStudioHdr, arrBonesPosition, arrBonesRotation, boneAccessor.nWritableBones);

		pIKContext->UpdateTargets(arrBonesPosition, arrBonesRotation, boneAccessor.matBones, arrBonesComputed);
		pPlayer->CalculateIKLocks(flCurrentTime);
		pIKContext->SolveDependencies(arrBonesPosition, arrBonesRotation, boneAccessor.matBones, arrBonesComputed);
	}
	else
		pPlayer->GetSkeleton(pStudioHdr, arrBonesPosition, arrBonesRotation, boneAccessor.nWritableBones);

	// [side change] rebased conditions to perform less checks
	if (pPlayer->GetEffects() & EF_BONEMERGE)
	{
		if (const CBaseHandle& hMoveParent = pPlayer->GetMoveParentHandle(); hMoveParent.IsValid())
		{
			if (CBaseAnimating* pParent = I::ClientEntityList->Get<CBaseEntity>(hMoveParent)->GetBaseAnimating(); pParent != nullptr)
				Q_ASSERT(false); // currently isn't used in cs:go
		}
	}
	// @test: i had never got a break, so probably the whole code block can be removed
	else if (CBoneMergeCache* pBoneMergeCache = pPlayer->GetBoneMergeCache(); pBoneMergeCache != nullptr)
	{
		delete pBoneMergeCache;
		pBoneMergeCache = nullptr;

		pPlayer->GetBoneMergeCache() = pBoneMergeCache;
	}

	// [side change] fixed old valve's fixme, passed mask of computed bones to skip transforms
	Studio_BuildMatrices(pStudioHdr, angAbsView, vecAbsOrigin, arrBonesPosition, arrBonesRotation, pPlayer->GetModelHierarchyScale(), boneAccessor.matBones, boneAccessor.nWritableBones, arrBonesComputed);
	nEFlags &= ~EFL_SETTING_UP_BONES;

	// @todo: see todo above local variables declarations
#if 1
	CRT::MemoryCopy(pPlayer->GetCachedBonesPosition(), arrBonesPosition, sizeof(BoneVector_t) * pStudioHdr->GetBoneCount());
	CRT::MemoryCopy(pPlayer->GetCachedBonesRotation(), arrBonesRotation, sizeof(BoneQuaternionAligned_t) * pStudioHdr->GetBoneCount());
#endif

	// [side change] added attachments handling because server doesn't handle them
	// check is requested bones for attachments and didn't previously
	if (nBoneMask & BONE_USED_BY_ATTACHMENT)
		pPlayer->SetupBones_AttachmentHelper(pStudioHdr);

	pPlayer->AdjustBonesToBBox(boneAccessor.matBones, boneAccessor.nWritableBones);

	// [side change] added handling for cache only requests
	if (arrBonesToWorld != nullptr)
	{
		Q_ASSERT((nEFlags & EFL_DIRTY_ABSTRANSFORM) == 0); // cached bone data has old abs origin/angles

		if (CUtlVectorAligned<Matrix3x4a_t>& vecCachedBonesData = pPlayer->GetCachedBonesData(); iMaxBones >= vecCachedBonesData.Count())
			CRT::MemoryCopy(arrBonesToWorld, vecCachedBonesData.Base(), sizeof(Matrix3x4a_t) * vecCachedBonesData.Count());
		else
		{
			Q_ASSERT(false); // requested more bones than cached
			return false;
		}
	}

	return true;
}

// call hierarchy: [client.dll] ... -> FX_FireBullets() -> C_CSPlayer::FireBullet() -> ...
void Q_FASTCALL H::FireBullet(CCSPlayer* thisptr, int edx, Vector_t vecSource, const QAngle_t& angShootView, float flDistance, float flPenetration, int nPenetrationCount, int nBulletType, int iDamage, float flRangeModifier, CBaseEntity* pAttacker, bool bDoEffects, float flSpreadX, float flSpreadY)
{
	hkFireBullet.CallOriginal<void>(thisptr, edx, vecSource, &angShootView, flDistance, flPenetration, nPenetrationCount, nBulletType, iDamage, flRangeModifier, pAttacker, bDoEffects, flSpreadX, flSpreadY);

	/*
	 * server gonna lock view angles to shoot angles for 'sv_maxusrcmdprocessticks_holdaim' ticks count on next sent tick
	 * this is getting stored during 'FX_FireBullets()' call inside 'CCSPlayer::NoteWeaponFired()' and 'CCSPlayer::FireBullet()' calls on server
	 *
	 * @ida C_CSPlayer::FireBullet(): client.dll -> "55 8B EC 83 E4 F0 81 EC ? ? ? ? F3 0F 7E" @xref: "\ntime\tbullet\trange\trecovery\tinaccuracy\n"
	 * @ida CCSPlayer::FireBullet(): server.dll -> "55 8B EC 83 E4 F0 81 EC ? ? ? ? 66 0F 6E 45 ? 8D 94" @xref: "add_bullet_hit_marker"
	 */
	if (thisptr->GetIndex() == I::Engine->GetLocalPlayer()) // @todo: this isn't quite correct, since 'NoteWeaponFired()' stores it on shot and after that 'FireBullet()' updates it only when bullet hits the enemy player
		F::LAGCOMP::UpdateHoldAimCycle();
}

// call hierarchy: [engine.dll] ... -> [client.dll] CHLClient::FrameStageNotify() -> OnRenderStart() -> C_BaseAnimating::UpdateClientSideAnimations() -> C_BaseAnimating::UpdateClientSideAnimation() -> CCSGOPlayerAnimState::Update()
void Q_FASTCALL H::UpdateClientSideAnimation(CCSPlayer* thisptr, int edx)
{
	// tell game to update animations for player only when we need this, and never else
	if (F::ANIMATION::IsGameAllowedToUpdateAnimations(thisptr->GetIndex()))
		hkUpdateClientSideAnimation.CallOriginal<void>(thisptr, edx);
}

// call hierarchy: [client.dll] ... -> CViewRender::SetUpView() -> C_CSPlayer::CalcView() -> ... 
void Q_FASTCALL H::CalcView(CCSPlayer* thisptr, int edx, Vector_t& vecEyeOrigin, QAngle_t& angEyeView, float& flNearZ, float& flFarZ, float& flFOV)
{
	// @ida C_CSPlayer::CalcView(): client.dll -> "55 8B EC 83 EC 14 53 56 57 FF 75 18"

	if (CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer(); pLocal == nullptr || (!pLocal->IsAlive() && pLocal->GetObserverMode() != OBS_MODE_IN_EYE))
		return hkCalcView.CallOriginal<void>(thisptr, edx, &vecEyeOrigin, &angEyeView, &flNearZ, &flFarZ, &flFOV);

	CalcViewStack_t stack;

	F::OnPreCalcView(thisptr, stack);

	// prevent 'CCSGOPlayerAnimState::ModifyEyePosition()' from being called, to fix visual bug when landing with low pitch
	const bool bOldIsUsingNewAnimState = thisptr->IsUsingNewAnimState();
	thisptr->IsUsingNewAnimState() = false;

	hkCalcView.CallOriginal<void>(thisptr, edx, &vecEyeOrigin, &angEyeView, &flNearZ, &flFarZ, &flFOV);

	// restore forced values back to not affect other functions
	thisptr->IsUsingNewAnimState() = bOldIsUsingNewAnimState;

	F::OnPostCalcView(thisptr, stack);
}

long CALLBACK H::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IPT::OnWndProc(hWnd, uMsg, wParam, lParam);

	if (D::OnWndProc(hWnd, uMsg, wParam, lParam))
		return 1L;

	// return input controls to the game
	return ::CallWindowProcW(IPT::pOldWndProc, hWnd, uMsg, wParam, lParam);
}
#pragma endregion
