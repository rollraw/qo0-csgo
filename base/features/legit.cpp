#include "legit.h"

#include "legit/triggerbot.h"

using namespace F;

#pragma region legit_callbacks
void LEGIT::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, bool* pbSendPacket)
{
	TRIGGER::OnMove(pLocal, pCmd);
}
#pragma endregion
