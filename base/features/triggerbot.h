#pragma once
// used: winapi includes, singleton
#include "../common.h"
// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: baseentity, baseweapon
#include "../sdk/entity.h"
// used: timer
#include "../utilities.h"

class CTriggerBot : public CSingleton<CTriggerBot>
{
public:
	// Get
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal);
private:
	// Values
	/* timer for delay before shoot */
	CTimer timer = { };
};
