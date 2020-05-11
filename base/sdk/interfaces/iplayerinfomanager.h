#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/game/server/iplayerinfo.h

class IPlayerInfoManager
{
private:
	virtual void function0() = 0;
public:
	virtual IGlobalVarsBase* GetGlobalVars() = 0;
};
