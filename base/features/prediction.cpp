#include "prediction.h"

// used: md5 checksum
#include "../sdk/hash/md5.h"
// used: movehelper, prediction, globals interfaces
#include "../core/interfaces.h"

void CPrediction::Start(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (pLocal == nullptr || !pLocal->IsAlive() || I::MoveHelper == nullptr)
		return;

	memset(&cMoveData, 0, sizeof(CMoveData));

	// start command
	*pLocal->GetCurrentCommand() = pCmd;
	// setup prediction seed
	*iPredictionRandomSeed = MD5::PseudoRandom(pCmd->iCommandNumber) & std::numeric_limits<int>::max();;
	// set ourselves as a predictable entity
	pSetPredictionEntity = pLocal;

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
	I::Globals->flCurrentTime = TICKS_TO_TIME(GetTickbase(pCmd, pLocal));
	I::Globals->flFrameTime = I::Prediction->bEnginePaused ? 0 : TICK_INTERVAL;
	I::Globals->iTickCount = GetTickbase(pCmd, pLocal);

	I::Prediction->bIsFirstTimePredicted = false;
	I::Prediction->bInPrediction = true;

	/* skiped weapon select and vehicle predicts */

	if (pCmd->uImpulse)
		*(std::uint32_t*)((std::uintptr_t)pLocal + 0x31FC) = pCmd->uImpulse;

	// synchronize m_afButtonForced & m_afButtonDisabled
	pCmd->iButtons |= (*(std::uint8_t*)((std::uintptr_t)pLocal + 0x3334));
	pCmd->iButtons &= ~(*(std::uint8_t*)((std::uintptr_t)pLocal + 0x3330));

	// update button state
	const int iButtons = pCmd->iButtons;
	int* nPlayerButtons = (int*)((std::uintptr_t)pLocal + 0x31F8);
	const int nButtonsChanged = iButtons ^ *nPlayerButtons;

	// synchronize m_afButtonLast
	*(int*)((std::uintptr_t)pLocal + 0x31EC) = *nPlayerButtons;
	// synchronize m_nButtons
	*(int*)((std::uintptr_t)pLocal + 0x31F8) = iButtons;
	// synchronize m_afButtonPressed
	*(int*)((std::uintptr_t)pLocal + 0x31F0) = iButtons & nButtonsChanged;
	// synchronize m_afButtonReleased
	*(int*)((std::uintptr_t)pLocal + 0x31F4) = nButtonsChanged & ~iButtons;

	// @test: 28.03.20 doesnt need, we already do that
	//I::GameMovement->StartTrackPredictionErrors(pLocal);

	// check if the player is standing on a moving entity and adjusts velocity and basevelocity appropriately
	I::Prediction->CheckMovingGround(pLocal, I::Globals->flFrameTime);

	// copy angles from command to player
	I::Prediction->SetLocalViewAngles(pCmd->angViewPoint);

	// run prethink
	if (pLocal->PhysicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		pLocal->PreThink();

	// run think
	int* iNextThinkTick = pLocal->GetNextThinkTick();
	if (*iNextThinkTick > 0 && *iNextThinkTick <= GetTickbase(pCmd, pLocal))
	{
		*iNextThinkTick = TICK_NEVER_THINK;
		pLocal->Think();
	}

	// set host player
	I::MoveHelper->SetHost(pLocal);

	// setup move
	I::Prediction->SetupMove(pLocal, pCmd, I::MoveHelper, &cMoveData);
	I::GameMovement->ProcessMovement(pLocal, &cMoveData);

	// finish move
	I::Prediction->FinishMove(pLocal, pCmd, &cMoveData);
	I::MoveHelper->ProcessImpacts();

	// run post think
	pLocal->PostThink();

	// restore tickbase
	pLocal->GetTickBase() = iOldTickBase;

	// restore prediction states
	I::Prediction->bInPrediction = bOldInPrediction;
	I::Prediction->bIsFirstTimePredicted = bOldIsFirstPrediction;
}

void CPrediction::End(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (pLocal == nullptr || !pLocal->IsAlive() || I::MoveHelper == nullptr)
		return;

	// @test: 28.03.20 doesnt need, we already do that
	//I::GameMovement->FinishTrackPredictionErrors(pLocal);

	// reset host player
	I::MoveHelper->SetHost(nullptr);

	// restore globals
	I::Globals->flCurrentTime = flOldCurrentTime;
	I::Globals->flFrameTime = flOldFrameTime;
	I::Globals->iTickCount = iOldTickCount;

	// finish command
	*pLocal->GetCurrentCommand() = nullptr;

	// reset prediction seed
	*iPredictionRandomSeed = -1;

	// reset prediction entity
	pSetPredictionEntity = nullptr;

	// reset move
	I::GameMovement->Reset();
}

int CPrediction::GetTickbase(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static int iTick = 0;
	static CUserCmd* pLastCmd = nullptr;

	if (pCmd == nullptr)
		return iTick;

	if (pLastCmd == nullptr || pLastCmd->bHasBeenPredicted)
		iTick = pLocal->GetTickBase();
	else
		++iTick;

	pLastCmd = pCmd;
}
