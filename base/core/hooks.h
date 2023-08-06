#pragma once
#include "../common.h"

#include "../sdk/datatypes/qangle.h"
#include "../sdk/datatypes/matrix.h"

// used: cbitwrite
#include "../sdk/bitbuf.h"
// used: cdetourhook
#include "../utilities/detourhook.h"
// used: interface baseclasses
#include "interfaces.h"

// forward declarations
enum EClientFrameStage : int;
class CViewSetup;
class INetChannel;
class INetMessage;
struct DrawModelInfo_t;
struct DrawModelResults_t;
class CCSGOPlayerAnimState;
class CUserCmd;
class IClientRenderable;
class CBaseEntity;
class CBasePlayer;
class CCSPlayer;

/*
 * VTABLE INDEXES
 * - indexes of methods in their virtual tables
 */
namespace VTABLE
{
	// @todo: this would be more convenient to move it to other place (like memory.h/inside classes etc) and keep all virtual indexes in such enums to make it easier to update them and visualize as little explanation, but since ppl are so lazy, it will lead to dumbest crap in real use of this base, which i would never want to see...

	namespace DXDEVICE
	{
		enum : std::size_t
		{
			RESET = 16U,
			ENDSCENE = 42U,
			RESETEX = 132U
		};
	}

	namespace CLIENT
	{
		enum : std::size_t
		{
			CREATEMOVE = 22U,
			FRAMESTAGENOTIFY = 37U
		};
	}

	namespace CLIENTMODE
	{
		enum : std::size_t
		{
			OVERRIDEVIEW = 18U,
			OVERRIDEMOUSEINPUT = 23U,
			CREATEMOVE2 = 24U,
			GETVIEWMODELFOV = 35U,
			DOPOSTSCREENSPACEEFFECTS = 44U
		};
	}

	namespace ENGINE
	{
		enum : std::size_t
		{
			ISCONNECTED = 27U,
			ISHLTV = 93U
		};
	}

	namespace SPATIALQUERY
	{
		enum : std::size_t
		{
			LISTLEAVESINBOX = 6U
		};
	}

	namespace NETCHANNEL
	{
		enum : std::size_t
		{
			SENDNETMSG = 40U,
			SENDDATAGRAM = 46U
		};
	}

	namespace PREDICTION
	{
		enum : std::size_t
		{
			RUNCOMMAND = 19U
		};
	}

	namespace SURFACE
	{
		enum : std::size_t
		{
			LOCKCURSOR = 67U
		};
	}

	namespace INPUT
	{
		enum : std::size_t
		{
			CAM_TOFIRSTPERSON = 36U
		};
	}

	namespace VIEWRENDER
	{
		enum : std::size_t
		{
			RENDERVIEW = 6U,
			RENDERSMOKEOVERLAY = 41U
		};
	}

	namespace STUDIORENDER
	{
		enum : std::size_t
		{
			DRAWMODEL = 29U
		};
	}

	namespace STEAMGAMECOORDINATOR
	{
		enum : std::size_t
		{
			SENDMESSAGE = 0U,
			RETRIEVEMESSAGE = 2U
		};
	}

	namespace CLIENTRENDERABLE
	{
		enum : std::size_t
		{
			SETUPBONES = 13U
		};
	}

	namespace BASEPLAYER
	{
		using namespace CLIENTRENDERABLE;

		enum : std::size_t
		{
			EYEANGLES = 170U
		};
	}

	namespace CSPLAYER
	{
		using namespace BASEPLAYER;

		enum : std::size_t
		{
			UPDATECLIENTSIDEANIMATION = 224U,
			CALCVIEW = 277U
		};
	}
}

/*
 * HOOKS
 * - functions call replacement
 */
namespace H
{
	bool Setup();
	void Destroy();

	/* @section: handlers */
	long D3DAPI Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	long D3DAPI EndScene(IDirect3DDevice9* pDevice);
	void Q_FASTCALL CreateMoveProxy(IBaseClientDll* thisptr, void* edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive);
	void Q_FASTCALL FrameStageNotify(IBaseClientDll* thisptr, void* edx, EClientFrameStage nStage);
	void Q_FASTCALL OverrideView(IClientModeShared* thisptr, void* edx, CViewSetup* pSetup);
	float Q_FASTCALL GetViewModelFOV(IClientModeShared* thisptr, void* edx);
	void Q_FASTCALL DoPostScreenSpaceEffects(IClientModeShared* thisptr, void* edx, CViewSetup* pSetup);
	bool Q_FASTCALL IsConnected(IEngineClient* thisptr, void* edx);
	bool Q_FASTCALL IsHLTV(IEngineClient* thisptr, void* edx);
	int Q_FASTCALL ListLeavesInBox(void* thisptr, void* edx, const Vector_t* pvecMins, const Vector_t* pvecMaxs, unsigned short* puList, int nListMax);
	bool Q_FASTCALL SendNetMsg(INetChannel* thisptr, void* edx, INetMessage* pMessage, bool bForceReliable, bool bVoice);
	int Q_FASTCALL SendDatagram(INetChannel* thisptr, void* edx, CBitWrite* pDatagram);
	void Q_FASTCALL RunCommand(IPrediction* thisptr, void* edx, CBasePlayer* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper);
	void Q_FASTCALL LockCursor(ISurface* thisptr, void* edx);
	void Q_FASTCALL CAM_ToFirstPerson(IInput* thisptr, void* edx);
	void Q_FASTCALL RenderView(IViewRender* thisptr, void* edx, const CViewSetup* pViewSetup, const CViewSetup* pViewSetupHUD, int nClearFlags, int nWhatToDraw);
	void Q_FASTCALL RenderSmokeOverlay(IViewRender* thisptr, void* edx, bool bPreViewModel);
	void Q_FASTCALL DrawModel(IStudioRender* thisptr, void* edx, DrawModelResults_t* pResults, const DrawModelInfo_t* pInfo, Matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector_t* pvecModelOrigin, int nFlags);
	int Q_FASTCALL SendMessage(ISteamGameCoordinator* thisptr, void* edx, std::uint32_t uMessageHeader, const void* pData, std::uint32_t nDataSize);
	int Q_FASTCALL RetrieveMessage(ISteamGameCoordinator* thisptr, void* edx, std::uint32_t* pMessageHeader, void* pDestination, std::uint32_t nDestinationSize, std::uint32_t* pnMessageSize);
	void Q_FASTCALL ModifyEyePosition(CCSGOPlayerAnimState* thisptr, void* edx, Vector_t* pvecInputEyePosition);
	bool Q_FASTCALL SetupBones(IClientRenderable* thisptr, void* edx, Matrix3x4a_t* arrBonesToWorld, int iMaxBones, int nBoneMask, float flCurrentTime);
	void Q_FASTCALL FireBullet(CCSPlayer* thisptr, void* edx, Vector_t vecSource, const QAngle_t* pangShootView, float flDistance, float flPenetration, int nPenetrationCount, int nBulletType, int iDamage, float flRangeModifier, CBaseEntity* pAttacker, bool bDoEffects, float flSpreadX, float flSpreadY);
	void Q_FASTCALL UpdateClientSideAnimation(CCSPlayer* thisptr, void* edx);
	void Q_FASTCALL CalcView(CCSPlayer* thisptr, void* edx, Vector_t* pvecEyeOrigin, QAngle_t* pangEyeView, float* pflNearZ, float* pflFarZ, float* pflFOV);
	long CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* @section: managers */
	inline CBaseHookObject<decltype(&Reset)> hkReset = { };
	inline CBaseHookObject<decltype(&EndScene)> hkEndScene = { };
	inline CHookObject<decltype(&CreateMoveProxy)> hkCreateMove = { };
	inline CHookObject<decltype(&FrameStageNotify)> hkFrameStageNotify = { };
	inline CHookObject<decltype(&OverrideView)> hkOverrideView = { };
	inline CHookObject<decltype(&GetViewModelFOV)> hkGetViewModelFOV = { };
	inline CHookObject<decltype(&DoPostScreenSpaceEffects)> hkDoPostScreenSpaceEffects = { };
	inline CHookObject<decltype(&IsConnected)> hkIsConnected = { };
	inline CHookObject<decltype(&IsHLTV)> hkIsHLTV = { };
	inline CHookObject<decltype(&ListLeavesInBox)> hkListLeavesInBox = { };
	inline CHookObject<decltype(&RunCommand)> hkRunCommand = { };
	inline CHookObject<decltype(&LockCursor)> hkLockCursor = { };
	inline CHookObject<decltype(&CAM_ToFirstPerson)> hkCAM_ToFirstPerson = { };
	inline CHookObject<decltype(&RenderView)> hkRenderView = { };
	inline CHookObject<decltype(&RenderSmokeOverlay)> hkRenderSmokeOverlay = { };
	inline CHookObject<decltype(&DrawModel)> hkDrawModel = { };
	inline CHookObject<decltype(&SendMessage)> hkSendMessage = { };
	inline CHookObject<decltype(&RetrieveMessage)> hkRetrieveMessage = { };
	inline CHookObject<decltype(&SendNetMsg)> hkSendNetMsg = { };
	inline CHookObject<decltype(&SendDatagram)> hkSendDatagram = { };
	inline CHookObject<decltype(&ModifyEyePosition)> hkModifyEyePosition = { };
	inline CHookObject<decltype(&SetupBones)> hkSetupBones = { };
	inline CHookObject<decltype(&FireBullet)> hkFireBullet = { };
	inline CHookObject<decltype(&UpdateClientSideAnimation)> hkUpdateClientSideAnimation = { };
	inline CHookObject<decltype(&CalcView)> hkCalcView = { };
}
