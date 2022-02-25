#include "prediction.h"

#include "../utilities/logging.h"
// used: md5 checksum
#include "../sdk/hash/md5.h"
// used: movehelper, prediction, globals interfaces
#include "../core/interfaces.h"

void CPrediction::Start(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	// @xref: "CPrediction::ProcessMovement"

	if (!pLocal->IsAlive() || I::MoveHelper == nullptr)
		return;

	// start command
	*pLocal->GetCurrentCommand() = pCmd;
	pLocal->GetLastCommand() = *pCmd;

	// random seed was already generated in CInput::CreateMove
	*uPredictionRandomSeed = pCmd->iRandomSeed;
	// set ourselves as a predictable entity
	*pPredictionPlayer = pLocal;

	// backup globals
	flOldCurrentTime = I::Globals->flCurrentTime;
	flOldFrameTime = I::Globals->flFrameTime;
	iOldTickCount = I::Globals->iTickCount;

	// backup tickbase
	const int iOldTickBase = pLocal->GetTickBase();

	// backup prediction states
	const bool bOldIsFirstPrediction = I::Prediction->bIsFirstTimePredicted;
	const bool bOldInPrediction = I::Prediction->bInPrediction;

	// set corrected values
	I::Globals->flCurrentTime = TICKS_TO_TIME(GetTickBase(pCmd, pLocal));
	I::Globals->flFrameTime = I::Prediction->bEnginePaused ? 0.f : TICK_INTERVAL;
	I::Globals->iTickCount = GetTickBase(pCmd, pLocal);

	I::Prediction->bIsFirstTimePredicted = false;
	I::Prediction->bInPrediction = true;

	/* skipped weapon select and vehicle predicts */

	// synchronize m_afButtonForced & m_afButtonDisabled
	pCmd->iButtons |= pLocal->GetButtonForced();
	pCmd->iButtons &= ~(pLocal->GetButtonDisabled());

	I::GameMovement->StartTrackPredictionErrors(pLocal);

	// update button state
	const int iButtons = pCmd->iButtons;
	const int nLocalButtons = *pLocal->GetButtons();
	const int nButtonsChanged = iButtons ^ nLocalButtons;

	// synchronize m_afButtonLast
	pLocal->GetButtonLast() = nLocalButtons;

	// synchronize m_nButtons
	*pLocal->GetButtons() = iButtons;

	// synchronize m_afButtonPressed
	pLocal->GetButtonPressed() = nButtonsChanged & iButtons;

	// synchronize m_afButtonReleased
	pLocal->GetButtonReleased() = nButtonsChanged & (~iButtons);

	// check if the player is standing on a moving entity and adjusts velocity and basevelocity appropriately
	I::Prediction->CheckMovingGround(pLocal, I::Globals->flFrameTime);

	// copy angles from command to player
	I::Prediction->SetLocalViewAngles(pCmd->angViewPoint);

	// run prethink
	if (pLocal->PhysicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		pLocal->PreThink();

	// run think
	if (int* iNextThinkTick = pLocal->GetNextThinkTick(); *iNextThinkTick > 0 && *iNextThinkTick <= GetTickBase(pCmd, pLocal))
	{
		*iNextThinkTick = TICK_NEVER_THINK;

		/*
		 * handle no think function
		 * pseudo i guess didnt seen before but not sure, most likely unnecessary

		nEFlags = pPlayer->GetEFlags();
		result = pPlayer->GetEFlags() & EFL_NO_THINK_FUNCTION;
		if (!result)
		{
			result = [&]()
			{
				if (pPlayer->GetNextThinkTick() > 0)
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
				pPlayer->GetEFlags() = nEFlags | EFL_NO_THINK_FUNCTION;
		}
		
		 */

		pLocal->Think();
	}

	// set host player
	I::MoveHelper->SetHost(pLocal);

	// setup move
	I::Prediction->SetupMove(pLocal, pCmd, I::MoveHelper, &moveData);
	I::GameMovement->ProcessMovement(pLocal, &moveData);

	// finish move
	I::Prediction->FinishMove(pLocal, pCmd, &moveData);
	I::MoveHelper->ProcessImpacts();

	// run post think
	pLocal->PostThink();

	// restore tickbase
	pLocal->GetTickBase() = iOldTickBase;

	// restore prediction states
	I::Prediction->bInPrediction = bOldInPrediction;
	I::Prediction->bIsFirstTimePredicted = bOldIsFirstPrediction;
}

void CPrediction::End(CUserCmd* pCmd, CBaseEntity* pLocal) const
{
	if (!pLocal->IsAlive() || I::MoveHelper == nullptr)
		return;

	I::GameMovement->FinishTrackPredictionErrors(pLocal);

	// reset host player
	I::MoveHelper->SetHost(nullptr);

	// restore globals
	I::Globals->flCurrentTime = flOldCurrentTime;
	I::Globals->flFrameTime = flOldFrameTime;
	I::Globals->iTickCount = iOldTickCount;

	// finish command
	*pLocal->GetCurrentCommand() = nullptr;

	// reset prediction seed
	*uPredictionRandomSeed = -1;

	// reset prediction entity
	*pPredictionPlayer = nullptr;

	// reset move
	I::GameMovement->Reset();
}

int CPrediction::GetTickBase(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static int iTick = 0;

	if (pCmd != nullptr)
	{
		static CUserCmd* pLastCmd = nullptr;

		// if command was not predicted - increment tickbase
		if (pLastCmd == nullptr || pLastCmd->bHasBeenPredicted)
			iTick = pLocal->GetTickBase();
		else
			iTick++;

		pLastCmd = pCmd;
	}

	return iTick;
}
