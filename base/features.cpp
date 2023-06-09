#include "features.h"

// used: print
#include "utilities/log.h"
// used: cheat variables
#include "core/variables.h"
// used: sensitivity, m_pitch, m_yaw
#include "core/convar.h"
// used: gamerulesproxy
#include "core/entitylistener.h"

// used: interface handles
#include "core/interfaces.h"
// used: interface declarations
#include "sdk/interfaces/iinput.h"

// features
#include "features/animationcorrection.h"
#include "features/lagcompensation.h"
#include "features/legit.h"
#include "features/misc.h"
#include "features/prediction.h"
#include "features/rage.h"
#include "features/visual.h"

// movement correction angles
static QAngle_t angCorrectionView = { };
// final client angles of the local player view that will guaranteed match to actual angles on the server
static QAngle_t angServerView = { };
// final client angles of the local player view that may differ from the actual angles on the server
static QAngle_t angClientView = { };

static void ValidateUserCommand(CUserCmd* pCmd, const CUserCmd* pPreviousCmd)
{
	// check is view angles finite
	if (pCmd->angViewPoint.IsValid())
	{
		// check is player on the valve secured server
		if (ENTITY::pCSGameRules == nullptr || ENTITY::pCSGameRules->IsValveDS())
		{
			pCmd->angViewPoint.Clamp();
			pCmd->angViewPoint.z = 0.0f;
		}
		else if (C::Get<bool>(Vars.bMiscAntiSMAC)) // @todo: or m_bGSSecure
			pCmd->angViewPoint.Clamp();
	}
	else
	{
		pCmd->angViewPoint = { };
		L_PRINT(LOG_WARNING) << Q_XOR("view angles have a NaN component, the value is reset");
	}

	// @todo: we should call this only in places where it gets modified (antiaim, autostrafe etc)
	F::MovementCorrection(pCmd, angCorrectionView);

	// get max speed limits by convars
	const float flMaxForwardSpeed = CONVAR::cl_forwardspeed->GetFloat();
	const float flMaxSideSpeed = CONVAR::cl_sidespeed->GetFloat();
	const float flMaxUpSpeed = CONVAR::cl_upspeed->GetFloat();

	// clamp movement speed to limits
	pCmd->flForwardMove = CRT::Clamp(pCmd->flForwardMove, -flMaxForwardSpeed, flMaxForwardSpeed);
	pCmd->flSideMove = CRT::Clamp(pCmd->flSideMove, -flMaxSideSpeed, flMaxSideSpeed);
	pCmd->flUpMove = CRT::Clamp(pCmd->flUpMove, -flMaxUpSpeed, flMaxUpSpeed);

	// correct movement buttons while player move have different to buttons values
	// clear all of the move buttons states
	pCmd->nButtons &= (~IN_FORWARD | ~IN_BACK | ~IN_LEFT | ~IN_RIGHT);

	// re-store buttons by active forward/side moves
	if (pCmd->flForwardMove > 0.0f)
		pCmd->nButtons |= IN_FORWARD;
	else if (pCmd->flForwardMove < 0.0f)
		pCmd->nButtons |= IN_BACK;

	if (pCmd->flSideMove > 0.0f)
		pCmd->nButtons |= IN_RIGHT;
	else if (pCmd->flSideMove < 0.0f)
		pCmd->nButtons |= IN_LEFT;

	// @test: exclude IN_BULLRUSH on valve ds / anti-smac | better would be to block using it or activating it through gui

	// correct mouse delta while player's x/y mouse delta have different to view angles values
	// @source: master/game/client/in_main.cpp#L1389
	/*
	 * that's how game actually converts mouse movement to angles
	 *
	 * CInput::GetAccumulatedMouseDeltasAndResetAccumulators():
	 *   CInputSystem::GetRawMouseAccumulators(raw.x, raw.y);
	 *   mouse.xy = raw.xy;
	 *
	 * CInput::ScaleMouse():
	 *   mouse.xy *= sensitivity;
	 *
	 * CInput::ApplyMouse():
	 *   view.x += mouse.y * m_pitch;
	 *   view.y -= mouse.x * m_yaw;
	 */

	if (!pCmd->angViewPoint.IsZero() && !pPreviousCmd->angViewPoint.IsZero())
	{
		const float flDeltaX = std::remainderf(pCmd->angViewPoint.x - pPreviousCmd->angViewPoint.x, 360.f);
		const float flDeltaY = std::remainderf(pCmd->angViewPoint.y - pPreviousCmd->angViewPoint.y, 360.f);

		float flPitch = CONVAR::m_pitch->GetFloat();
		if (flPitch == 0.0f)
			flPitch = CRT::StringToFloat<float>(CONVAR::m_pitch->szDefaultValue);

		float flYaw = CONVAR::m_yaw->GetFloat();
		if (flYaw == 0.0f)
			flYaw = CRT::StringToFloat<float>(CONVAR::m_yaw->szDefaultValue);

		float flSensitivity = CONVAR::sensitivity->GetFloat();
		if (flSensitivity == 0.0f)
			flSensitivity = 1.0f;

		pCmd->shMouseDeltaX = static_cast<short>(flDeltaX / (flSensitivity * flPitch));
		pCmd->shMouseDeltaY = static_cast<short>(-flDeltaY / (flSensitivity * flYaw));
	}
}

/*
 * @todo: originally the idea was to have all methods nested in the 'F' (FEATURES) and child namespaces called here in specific hook callbacks, such as Pre/Post the original,
 * but some of them may require to not call original at all/call it more than once and this makes it hard to organize well | declare smth like hookstack idk?
 *
 * upd. currently it is even worse than expected, this are used commonly for "internal" callbacks, features init/destroy and some backend workaround for createmove, other features have their own managers for sub-features
 *
 * upd.2 added "stack" structures for some hooks to save/restore feature variables between pre/on/post callbacks, seems to be an solution, just design it better,
 *       like: merge those structures declarations to 'features.h', finally add main callbacks to those features and their sub-features here and cleanup hooks
 *
 * upd.3 feels like bloated shit lol but i guess in real use case it would feel better and work as expected
 */

bool F::Setup()
{
	if (!VISUAL::Setup())
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to setup visuals");
		return false;
	}

	return true;
}

void F::Destroy()
{
	VISUAL::Destroy();
}

#pragma region features_extra
void F::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, bool* pbSendPacket, int nSequenceNumber)
{
	// store global active pointers
	pActiveCmd = pCmd;

	// store original view angles after game finished processing this command
	angCorrectionView = pCmd->angViewPoint;

	// reset all of other angles due to new command
	angServerView = pCmd->angViewPoint;
	angClientView = pCmd->angViewPoint;

	LAGCOMP::OnPreMove(pLocal, pCmd, pbSendPacket);

	MISC::OnPreMove(pLocal, pCmd, pbSendPacket);

	PREDICTION::Update();
	PREDICTION::OnPreMove(pLocal, pCmd);

	MISC::OnMove(pLocal, pCmd, pbSendPacket);

	LEGIT::OnMove(pLocal, pCmd, pbSendPacket);

	RAGE::OnMove(pLocal, pCmd, pbSendPacket);

	PREDICTION::OnPostMove(pLocal, pCmd);

	MISC::OnPostMove(pLocal, pCmd, pbSendPacket);

	if (C::Get<bool>(Vars.bMiscFakeLatency))
		LAGCOMP::UpdateLatencySequences();
	else
		LAGCOMP::ClearLatencySequences();

	if (pLocal->IsAlive())
	{
		// process final corrections before command being sent
		const CUserCmd* pPreviousCommand = I::Input->GetUserCmd(nSequenceNumber - 1);
		ValidateUserCommand(pCmd, pPreviousCommand);
	}

	// store last data
	bLastSendPacket = *pbSendPacket;

	// reset global pointers, otherwise they may point to invalid address
	pActiveCmd = nullptr;
}

void F::OnFrame(const EClientFrameStage nStage)
{
	ANIMATION::OnFrame(nStage);
	VISUAL::OnFrame(nStage);
}

void F::OnPreOverrideView(CViewSetup* pSetup)
{
	VISUAL::OnOverrideView(pSetup);
}

void F::OnPostOverrideView(CViewSetup* pSetup)
{
	// store camera origin after all manipulations
	VISUAL::vecCameraOrigin = pSetup->vecOrigin;
}

void F::OnPreSendDatagram(INetChannel* pNetChannel, SendDatagramStack_t& stack)
{
	LAGCOMP::OnPreSendDatagram(pNetChannel, &stack.nOldInReliableState, &stack.nOldInSequenceNr);
}

void F::OnPostSendDatagram(INetChannel* pNetChannel, SendDatagramStack_t& stack)
{
	LAGCOMP::OnPostSendDatagram(pNetChannel, stack.nOldInReliableState, stack.nOldInSequenceNr);
}

void F::OnGetViewModelFOV(float* pflViewModelFOV)
{
	VISUAL::OnGetViewModelFOV(pflViewModelFOV);
}

void F::OnDoPostScreenSpaceEffects(CViewSetup* pSetup)
{
	VISUAL::OnDoPostScreenSpaceEffects();
}

void F::OnPreRenderView(const CViewSetup& viewSetup, const CViewSetup& viewSetupHUD, int nClearFlags, int* pnWhatToDraw)
{
	VISUAL::OnPreRenderView(viewSetup, pnWhatToDraw);
}

void F::OnPostRenderView(const CViewSetup& viewSetup, const CViewSetup& viewSetupHUD, int nClearFlags, int* pnWhatToDraw)
{
	VISUAL::OnPostRenderView(viewSetup);
}

void F::OnPreCalcView(CCSPlayer* pPlayer, CalcViewStack_t& stack)
{
	VISUAL::OnPreCalcView(pPlayer, &stack.angOldViewPunch, &stack.angOldAimPunch);
}

void F::OnPostCalcView(CCSPlayer* pPlayer, CalcViewStack_t& stack)
{
	VISUAL::OnPostCalcView(pPlayer, &stack.angOldViewPunch, &stack.angOldAimPunch);
}

void F::OnEvent(IGameEvent& gameEvent)
{
	const FNV1A_t uEventHash = FNV1A::Hash(gameEvent.GetName());

	VISUAL::OnEvent(uEventHash, gameEvent);
}

void F::OnPlayerCreated(CCSPlayer* pPlayer)
{
	ANIMATION::OnPlayerCreated(pPlayer);
}

void F::OnPlayerDeleted(CCSPlayer* pPlayer)
{
	ANIMATION::OnPlayerDeleted(pPlayer);
	LAGCOMP::OnPlayerDeleted(pPlayer);
}
#pragma endregion

#pragma region features_get
void F::MovementCorrection(CUserCmd* pCmd, const QAngle_t& angDesiredViewPoint)
{
	Vector_t vecForward = { }, vecRight = { }, vecUp = { };
	angDesiredViewPoint.ToDirections(&vecForward, &vecRight, &vecUp);

	// we don't attempt on forward/right roll, and on up pitch/yaw
	vecForward.z = vecRight.z = vecUp.x = vecUp.y = 0.0f;

	vecForward.NormalizeInPlace();
	vecRight.NormalizeInPlace();
	vecUp.NormalizeInPlace();

	Vector_t vecOldForward = { }, vecOldRight = { }, vecOldUp = { };
	pCmd->angViewPoint.ToDirections(&vecOldForward, &vecOldRight, &vecOldUp);

	// we don't attempt on forward/right roll, and on up pitch/yaw
	vecOldForward.z = vecOldRight.z = vecOldUp.x = vecOldUp.y = 0.0f;

	vecOldForward.NormalizeInPlace();
	vecOldRight.NormalizeInPlace();
	vecOldUp.NormalizeInPlace();

	const float flPitchForward = vecForward.x * pCmd->flForwardMove;
	const float flYawForward = vecForward.y * pCmd->flForwardMove;
	const float flPitchSide = vecRight.x * pCmd->flSideMove;
	const float flYawSide = vecRight.y * pCmd->flSideMove;
	const float flRollUp = vecUp.z * pCmd->flUpMove;

	// solve corrected movement speed
	pCmd->flForwardMove = vecOldForward.x * flPitchSide + vecOldForward.y * flYawSide + vecOldForward.x * flPitchForward + vecOldForward.y * flYawForward + vecOldForward.z * flRollUp;
	pCmd->flSideMove = vecOldRight.x * flPitchSide + vecOldRight.y * flYawSide + vecOldRight.x * flPitchForward + vecOldRight.y * flYawForward + vecOldRight.z * flRollUp;
	pCmd->flUpMove = vecOldUp.x * flYawSide + vecOldUp.y * flPitchSide + vecOldUp.x * flYawForward + vecOldUp.y * flPitchForward + vecOldUp.z * flRollUp;
}
#pragma endregion

#pragma region features_command_access
QAngle_t F::GetClientAngles()
{
	if (LAGCOMP::IsHoldAimCycle())
		return LAGCOMP::GetHoldAimCycleViewAngles();

	return angClientView;
}

QAngle_t F::GetServerAngles()
{
	if (LAGCOMP::IsHoldAimCycle())
		return LAGCOMP::GetHoldAimCycleViewAngles();

	return (pActiveCmd != nullptr ? pActiveCmd->angViewPoint : angServerView);
}

void F::SetClientAngles(const QAngle_t& angView)
{
	pActiveCmd->angViewPoint = angClientView = angView;
}

void F::SetServerAngles(const QAngle_t& angView)
{
	// even if we write that angles during hold aim cycle, they wont appear on server anyway @todo: is it correct behaviour? or we still should let user set them | atm we let needs further testing
	//if (!LAGCOMP::IsHoldAimCycle())
	pActiveCmd->angViewPoint = angServerView = angView;
}
#pragma endregion
