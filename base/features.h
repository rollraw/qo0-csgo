#pragma once
#include "common.h"

#include "sdk/datatypes/vector.h"
#include "sdk/datatypes/qangle.h"
#include "sdk/datatypes/usercmd.h"

// used: ccsplayer
#include "sdk/entity.h"
// used: igameevent
#include "sdk/interfaces/igameeventmanager.h"

// forward declarations
enum EClientFrameStage : int;
class INetChannel;

struct SendDatagramStack_t
{
	int nOldInReliableState = 0;
	int nOldInSequenceNr = 0;
};

struct CalcViewStack_t
{
	QAngle_t angOldViewPunch = { };
	QAngle_t angOldAimPunch = { };
};

/*
 * FEATURES
 * - global manager of the all features callbacks
 */
namespace F
{
	[[nodiscard]] bool Setup();
	void Destroy();

	/* @section: callbacks */
	// called after 'CreateMove()' original
	void OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, bool* pbSendPacket, int nSequenceNumber);
	// called before 'FrameStageNotify()' original
	void OnFrame(const EClientFrameStage nStage);
	// called before 'OverrideView()' original
	void OnPreOverrideView(CViewSetup* pSetup);
	// called after 'OverrideView()' original
	void OnPostOverrideView(CViewSetup* pSetup);
	// called before 'SendDatagram()' original
	void OnPreSendDatagram(INetChannel* pNetChannel, SendDatagramStack_t& stack);
	// called after 'SendDatagram()' original
	void OnPostSendDatagram(INetChannel* pNetChannel, SendDatagramStack_t& stack);
	// called after 'GetViewModelFOV()' original
	void OnGetViewModelFOV(float* pflViewModelFOV);
	// called before 'DoPostScreenSpaceEffects()' original
	void OnDoPostScreenSpaceEffects(CViewSetup* pSetup);
	// called before 'RenderView()' original
	void OnPreRenderView(const CViewSetup& viewSetup, const CViewSetup& viewSetupHUD, int nClearFlags, int* pnWhatToDraw);
	// called after 'RenderView()' original
	void OnPostRenderView(const CViewSetup& viewSetup, const CViewSetup& viewSetupHUD, int nClearFlags, int* pnWhatToDraw);
	// called before 'CalcView()' original
	void OnPreCalcView(CCSPlayer* pPlayer, CalcViewStack_t& stack);
	// called after 'CalcView()' original
	void OnPostCalcView(CCSPlayer* pPlayer, CalcViewStack_t& stack);
	/// handle all features event callbacks
	/// @remarks: if your function doesn't getting called when it should, check did you registered needed event with "EVENT::AddToListener"
	void OnEvent(const FNV1A_t uEventHash, IGameEvent& gameEvent);
	/// handle all features callbacks on player create
	/// @remarks: note that this is called after the constructor
	void OnPlayerCreated(CCSPlayer* pPlayer);
	/// handle all features callbacks on player delete
	/// @remarks: note that this is called before the destructor
	void OnPlayerDeleted(CCSPlayer* pPlayer);

	/* @section: get */
	/// correct movement while player view have different to move angles
	/// @param[in] angDesiredViewPoint view angles to be used for correction
	void MovementCorrection(CUserCmd* pCmd, const QAngle_t& angDesiredViewPoint);

	/*
	 * @section: command access
	 * - safe wrappers to get/set command variables
	 *   you should always use those instead of direct access to them!
	 */
	/// original view angles that game stored after processed command and before our modifications
	/// @remarks: used for movement correction, see 'MISC::MovementCorrection()' for details
	[[nodiscard]] QAngle_t GetCorrectionAngles();
	/// client view angles that may differ from actual angles on the server
	/// @remarks: used to adjust client-side animations to visualize desynchronization between client and server, see 'ANIMATION::UpdateLocal()' for details
	[[nodiscard]] QAngle_t GetClientAngles();
	/// client view angles that guaranteed match to server
	/// @remarks: used to adjust client-side animations to match client data with server data, see 'ANIMATION::UpdateLocal()' for details
	[[nodiscard]] QAngle_t GetServerAngles();
	/// override original view angles
	/// @remarks: change this only when you need to go to a specific position with different to this position angles
	void SetCorrectionAngles(const QAngle_t& angView);
	/// write command view angles that may differ from actual angles on the server
	/// @remarks: change this only when angles you intend to set may not be sent to the server
	void SetClientAngles(const QAngle_t& angView);
	/// write command view angles that will guaranteed match to server
	/// @remarks: change this only when angles you intend to set will always be sent to the server
	void SetServerAngles(const QAngle_t& angView);

	/* @section: values */
	// @todo: do not expose vals in header and add get methods for them
	// active command pointer, this is only valid during 'CreateMove()' hook call and never else!
	inline CUserCmd* pActiveCmd = nullptr;
	// active send packet pointer, this is only valid during 'CreateMove()' hook call and never else!
	inline bool* pbActiveSendPacket = nullptr;
	// send packet state of the last processed command
	inline bool bLastSendPacket = true;
}
