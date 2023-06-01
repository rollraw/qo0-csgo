#include "rage.h"

#include "rage/antiaim.h"

using namespace F;

#pragma region rage_callbacks
void RAGE::OnMove(CCSPlayer* pLocal, CUserCmd* pCmd, bool* pbSendPacket)
{
	ANTIAIM::OnMove(pLocal, pCmd, pbSendPacket);
}
#pragma endregion
