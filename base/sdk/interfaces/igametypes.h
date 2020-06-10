#pragma once

#pragma region gametypes_enumerations
enum EGameType : int
{
	GAMETYPE_UNKNOWN = -1,
	GAMETYPE_CLASSIC,
	GAMETYPE_GUNGAME,
	GAMETYPE_TRAINING,
	GAMETYPE_CUSTOM,
	GAMETYPE_COOPERATIVE,
	GAMETYPE_SKIRMISH,
	GAMETYPE_FREEFORALL
};

// @credits: https://developer.valvesoftware.com/wiki/CSGO_Game_Mode_Commands
enum EGameMode : int
{
	GAMEMODE_UNKNOWN = 0,
	GAMEMODE_CASUAL,
	GAMEMODE_COMPETITIVE,
	GAMEMODE_WINGMAN,
	GAMEMODE_ARMSRACE,
	GAMEMODE_DEMOLITION,
	GAMEMODE_DEATHMATCH,
	GAMEMODE_GUARDIAN,
	GAMEMODE_COOPSTRIKE,
	GAMEMODE_DANGERZONE
};
#pragma endregion

// @credits: https://github.com/scen/ionlib/blob/master/src/sdk/hl2_csgo/public/matchmaking/igametypes.h
class IGameTypes
{
public:
	int GetCurrentGameType()
	{
		return MEM::CallVFunc<int>(this, 8);
	}

	int GetCurrentGameMode()
	{
		return MEM::CallVFunc<int>(this, 9);
	}

	const char* GetCurrentMapName()
	{
		return MEM::CallVFunc<const char*>(this, 10);
	}

	const char* GetCurrentGameTypeNameID()
	{
		return MEM::CallVFunc<const char*>(this, 11);
	}

	const char* GetCurrentGameModeNameID()
	{
		return MEM::CallVFunc<const char*>(this, 13);
	}
};
