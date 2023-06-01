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
	void Q_FASTCALL CreateMoveProxy(IBaseClientDll* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive);
	bool Q_FASTCALL CreateMoveProxy2(IClientModeShared* thisptr, int edx, float flInputSampleFrametime, CUserCmd* pCmd);
	void Q_FASTCALL FrameStageNotify(IBaseClientDll* thisptr, int edx, EClientFrameStage nStage);
	void Q_FASTCALL OverrideView(IClientModeShared* thisptr, int edx, CViewSetup* pSetup);
	float Q_FASTCALL GetViewModelFOV(IClientModeShared* thisptr, int edx);
	void Q_FASTCALL DoPostScreenSpaceEffects(IClientModeShared* thisptr, int edx, CViewSetup* pSetup);
	bool Q_FASTCALL IsConnected(IEngineClient* thisptr, int edx);
	bool Q_FASTCALL IsHLTV(IEngineClient* thisptr, int edx);
	int Q_FASTCALL ListLeavesInBox(void* thisptr, int edx, const Vector_t& vecMins, const Vector_t& vecMaxs, unsigned short* puList, int nListMax);
	bool Q_FASTCALL SendNetMsg(INetChannel* thisptr, int edx, INetMessage& message, bool bForceReliable, bool bVoice);
	int Q_FASTCALL SendDatagram(INetChannel* thisptr, int edx, CBitWrite* pDatagram);
	void Q_FASTCALL RunCommand(IPrediction* thisptr, int edx, CBasePlayer* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper);
	void Q_FASTCALL LockCursor(ISurface* thisptr, int edx);
	void Q_FASTCALL CAM_ToFirstPerson(IInput* thisptr, int edx);
	void Q_FASTCALL RenderView(IViewRender* thisptr, int edx, const CViewSetup& viewSetup, const CViewSetup& viewSetupHUD, int nClearFlags, int nWhatToDraw);
	void Q_FASTCALL RenderSmokeOverlay(IViewRender* thisptr, int edx, bool bPreViewModel);
	void Q_FASTCALL DrawModel(IStudioRender* thisptr, int edx, DrawModelResults_t* pResults, const DrawModelInfo_t& info, Matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector_t& vecModelOrigin, int nFlags);
	int Q_FASTCALL SendMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t uMessageHeader, const void* pData, std::uint32_t nDataSize);
	int Q_FASTCALL RetrieveMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t* pMessageHeader, void* pDestination, std::uint32_t nDestinationSize, std::uint32_t* pnMessageSize);
	void Q_FASTCALL ModifyEyePosition(CCSGOPlayerAnimState* thisptr, int edx, Vector_t& vecInputEyePosition);
	bool Q_FASTCALL SetupBones(IClientRenderable* thisptr, int edx, Matrix3x4a_t* arrBonesToWorld, int iMaxBones, int nBoneMask, float flCurrentTime);
	void Q_FASTCALL FireBullet(CCSPlayer* thisptr, int edx, Vector_t vecSource, const QAngle_t& angShootView, float flDistance, float flPenetration, int nPenetrationCount, int nBulletType, int iDamage, float flRangeModifier, CBaseEntity* pAttacker, bool bDoEffects, float flSpreadX, float flSpreadY);
	void Q_FASTCALL UpdateClientSideAnimation(CCSPlayer* thisptr, int edx);
	void Q_FASTCALL CalcView(CCSPlayer* thisptr, int edx, Vector_t& vecEyeOrigin, QAngle_t& angEyeView, float& flNearZ, float& flFarZ, float& flFOV);
	long CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* @section: managers */
	inline CHookObject hkReset = { };
	inline CHookObject hkEndScene = { };
	inline CHookObject<ROP::EngineGadget_t> hkCreateMove = { };
	inline CHookObject<ROP::EngineGadget_t> hkFrameStageNotify = { };
	inline CHookObject<ROP::ClientGadget_t> hkOverrideView = { };
	inline CHookObject<ROP::ClientGadget_t> hkGetViewModelFOV = { };
	inline CHookObject<ROP::ClientGadget_t> hkDoPostScreenSpaceEffects = { };
	inline CHookObject<ROP::EngineGadget_t> hkIsConnected = { };
	inline CHookObject<ROP::EngineGadget_t> hkIsHLTV = { };
	inline CHookObject<ROP::EngineGadget_t> hkListLeavesInBox = { };
	inline CHookObject<ROP::ClientGadget_t> hkRunCommand = { };
	inline CHookObject<ROP::ClientGadget_t> hkLockCursor = { };
	inline CHookObject<ROP::ClientGadget_t> hkCAM_ToFirstPerson = { };
	inline CHookObject<ROP::ClientGadget_t> hkRenderView = { };
	inline CHookObject<ROP::ClientGadget_t> hkRenderSmokeOverlay = { };
	inline CHookObject<ROP::ClientGadget_t> hkDrawModel = { };
	inline CHookObject<ROP::ClientGadget_t> hkSendMessage = { };
	inline CHookObject<ROP::ClientGadget_t> hkRetrieveMessage = { };
	inline CHookObject<ROP::EngineGadget_t> hkSendNetMsg = { };
	inline CHookObject<ROP::EngineGadget_t> hkSendDatagram = { };
	inline CHookObject<ROP::ClientGadget_t> hkModifyEyePosition = { };
	inline CHookObject<ROP::ClientGadget_t> hkSetupBones = { };
	inline CHookObject<ROP::ClientGadget_t> hkFireBullet = { };
	inline CHookObject<ROP::ClientGadget_t> hkUpdateClientSideAnimation = { };
	inline CHookObject<ROP::ClientGadget_t> hkCalcView = { };
}
