#include "prediction.h"

// used: cbaseplayer
#include "../sdk/entity.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/iclientstate.h"
#include "../sdk/interfaces/iprediction.h"
#include "../sdk/interfaces/imovehelper.h"

using namespace F;

// current prediction random seed
static int* piPredictionRandomSeed = nullptr;
// current predictable player
static CBasePlayer** ppPredictionPlayer = nullptr;
// encapsulated input parameters of player movement
static CMoveData moveData = { };
// saved client time measurement values
static float flOldCurrentTime = 0.0f, flOldFrameTime = 0.0f;

#pragma region prediction_callbacks
void PREDICTION::Update()
{
	// @ida: CL_RunPrediction(): engine.dll -> "57 8B 3D ? ? ? ? 83 BF"

	if (I::ClientState->nSignonState == SIGNONSTATE_FULL && I::ClientState->nDeltaTick >= 0)
		I::Prediction->Update(I::ClientState->nDeltaTick, I::ClientState->nDeltaTick > 0, I::ClientState->iLastCommandAck, I::ClientState->iLastOutgoingCommand + I::ClientState->nChokedCommands);
}

void PREDICTION::OnPreMove(CBasePlayer* pLocal, CUserCmd* pCmd)
{
	// @ida CPrediction::RunCommand(): client.dll -> "55 8B EC 83 E4 C0 83 EC 34 53 56 8B 75" @xref: "CPrediction::ProcessMovement"
	// @ida CPlayerMove::RunCommand(): server.dll -> "55 8B EC 83 E4 F8 83 EC 2C 53 56 8B 75 08 8B" @xref: "sv_maxusrcmdprocessticks_warning at server tick %u: Ignored client %s usrcmd (%.6f < %.6f)!\n"

	if (!pLocal->IsAlive() || I::MoveHelper == nullptr)
		return;

	// force game to think we're in prediction
	I::Prediction->bInPrediction = true;
	I::Prediction->bIsFirstTimePredicted = false;

	// start command
	pLocal->GetCurrentCommand() = pCmd;
	pLocal->GetLastCommand() = *pCmd;

	if (piPredictionRandomSeed == nullptr)
		piPredictionRandomSeed = *reinterpret_cast<int**>(MEM::FindPattern(CLIENT_DLL, Q_XOR("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")) + 0x2); // @xref: "SelectWeightedSequence"

	if (ppPredictionPlayer == nullptr)
		ppPredictionPlayer = *reinterpret_cast<CBasePlayer***>(MEM::FindPattern(CLIENT_DLL, Q_XOR("89 35 ? ? ? ? F3 0F 10 48 20")) + 0x2);

	// set a random seed that was already generated in 'CInput::CreateMove'
	*piPredictionRandomSeed = pCmd->iRandomSeed;
	// set ourselves as player that being predicted
	*ppPredictionPlayer = pLocal;

	// backup original time measurement values
	flOldCurrentTime = I::Globals->flCurrentTime;
	flOldFrameTime = I::Globals->flFrameTime;

	const int iTickBase = pLocal->GetTickBase();

	// set corrected values
	I::Globals->flCurrentTime = static_cast<float>(iTickBase) * I::Globals->flIntervalPerTick;
	I::Globals->flFrameTime = I::Prediction->bEnginePaused ? 0.0f : I::Globals->flIntervalPerTick;

	// [side change] skipped parts with weapon selection and vehicle predicts

	// synchronize command buttons
	pCmd->nButtons |= pLocal->GetButtonForced();
	pCmd->nButtons &= ~(pLocal->GetButtonDisabled());

	I::GameMovement->StartTrackPredictionErrors(pLocal);

	// update buttons state
	const int nButtons = pCmd->nButtons;
	const int nLocalButtons = pLocal->GetButtons();
	const int nButtonsChanged = nButtons ^ nLocalButtons;

	// synchronize player buttons
	pLocal->GetButtonLast() = nLocalButtons;
	pLocal->GetButtons() = nButtons;
	pLocal->GetButtonPressed() = nButtonsChanged & nButtons;
	pLocal->GetButtonReleased() = nButtonsChanged & (~nButtons);

	I::Prediction->CheckMovingGround(pLocal, I::Globals->flFrameTime);

	// copy angles from command to player
	I::Prediction->SetLocalViewAngles(pCmd->angViewPoint);

	// run prethink
	if (pLocal->PhysicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		pLocal->PreThink();

	// [side change] @todo: skipped part with handling of no think function
	/*
		if (!(pPlayer->GetEFlags() & EFL_NO_THINK_FUNCTION))
		{
			bool result = [&]()
			{
				if (iNextThinkTick > 0)
					return 1;
				v3 = *(_DWORD *)(pPlayer + 0x2BC);
				v4 = 0;
				if (v3 > 0)
				{
				v5 = (_DWORD *)(*(_DWORD *)(pPlayer + 0x2B0) + 0x14);
				while (*v5 <= 0)
				{
					++v4;
					v5 += 8;
					if (v4 >= v3)
						return 0;
				}
				return 1;
			}();
			if (!result)
				pPlayer->GetEFlags() |= EFL_NO_THINK_FUNCTION;
		}
		*/

	// run think
	if (int& iNextThinkTick = pLocal->GetNextThinkTick(); iNextThinkTick > 0 && iNextThinkTick <= iTickBase)
	{
		iNextThinkTick = TICK_NEVER_THINK;
		pLocal->Think();
	}

	// set host player
	I::MoveHelper->SetHost(pLocal);

	// setup move
	I::Prediction->SetupMove(pLocal, pCmd, I::MoveHelper, &moveData);
	I::GameMovement->ProcessMovement(pLocal, &moveData);

	// finish move
	I::Prediction->FinishMove(pLocal, pCmd, &moveData);

	// @note: server handle hold aim lock here @source: master/game/server/player_command.cpp#L456

	I::MoveHelper->ProcessImpacts();

	// run post think
	pLocal->PostThink();
}

void PREDICTION::OnPostMove(CBasePlayer* pLocal, CUserCmd* pCmd)
{
	if (!pLocal->IsAlive() || I::MoveHelper == nullptr)
		return;

	I::GameMovement->FinishTrackPredictionErrors(pLocal);

	// reset host player
	I::MoveHelper->SetHost(nullptr);

	// restore globals
	I::Globals->flCurrentTime = flOldCurrentTime;
	I::Globals->flFrameTime = flOldFrameTime;

	// finish command
	pLocal->GetCurrentCommand() = nullptr;
	*piPredictionRandomSeed = -1;
	*ppPredictionPlayer = nullptr;

	// reset move
	I::GameMovement->Reset();

	// restore prediction state
	I::Prediction->bInPrediction = false;
}
#pragma endregion
