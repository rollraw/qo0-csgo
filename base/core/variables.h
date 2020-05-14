#pragma once
// used: define to add values to variables vector
#include "config.h"

#pragma region variables_array_entries
enum EVisualsInfoFlags : int
{
	INFO_FLAG_HELMET = 0,
	INFO_FLAG_KEVLAR,
	INFO_FLAG_KIT,
	INFO_FLAG_ZOOM,
	INFO_FLAG_MAX
};

enum EVisualsRemovals : int
{
	REMOVAL_POSTPROCESSING = 0,
	REMOVAL_PUNCH,
	REMOVAL_SMOKE,
	REMOVAL_SCOPE,
	REMOVAL_MAX
};
#pragma endregion

#pragma region variables_combo_entries
enum class EAntiAimPitchType : int
{
	NONE = 0,
	UP,
	DOWN,
	ZERO
};

enum class EAntiAimYawType : int
{
	NONE = 0,
	SIDEWAYS
};

enum class EVisualsBoxType : int
{
	NONE = 0,
	FULL,
	CORNERS
};

enum class EVisualsGlowStyle : int
{
	OUTER = 0,
	COVER,
	INNER
};

enum class EVisualsPlayersChams : int
{
	COVERED = 0,
	FLAT,
	WIREFRAME,
	REFLECTIVE
};

enum class EVisualsViewModelChams : int
{
	NO_DRAW = 0,
	COVERED,
	FLAT,
	WIREFRAME,
	GLOW,
	SCROLL
};
#pragma endregion

struct Variables_t
{
	#pragma region variables_legit
	C_ADDVARIABLE(bool, bLegit, false);

	C_ADDVARIABLE(bool, bTrigger, false);
	C_ADDVARIABLE(int, iTriggerKey, 0);
	C_ADDVARIABLE(int, iTriggerDelay, 0);
	C_ADDVARIABLE(bool, bTriggerAutoWall, false);
	C_ADDVARIABLE(int, iTriggerMinimalDamage, 70);

	C_ADDVARIABLE(bool, bTriggerHead, true);
	C_ADDVARIABLE(bool, bTriggerChest, true);
	C_ADDVARIABLE(bool, bTriggerStomach, true);
	C_ADDVARIABLE(bool, bTriggerArms, false);
	C_ADDVARIABLE(bool, bTriggerLegs, false);
	#pragma endregion

	#pragma region variables_rage
	C_ADDVARIABLE(bool, bRage, false);

	C_ADDVARIABLE(bool, bAntiAim, false);
	C_ADDVARIABLE(int, iAntiAimPitch, 0);
	C_ADDVARIABLE(int, iAntiAimYaw, 0);
	#pragma endregion

	#pragma region variables_visuals
	C_ADDVARIABLE(bool, bEsp, false);

	C_ADDVARIABLE(bool, bEspMain, false);
	C_ADDVARIABLE(bool, bEspMainEnemies, false);
	C_ADDVARIABLE(bool, bEspMainAllies, false);
	C_ADDVARIABLE(bool, bEspMainWeapons, false);
	C_ADDVARIABLE(bool, bEspMainGrenades, false);
	C_ADDVARIABLE(bool, bEspMainBomb, false);

	C_ADDVARIABLE(int, iEspMainBox, 1);
	C_ADDVARIABLE(Color, colEspMainBoxEnemies, Color(160, 60, 60, 255));
	C_ADDVARIABLE(Color, colEspMainBoxEnemiesWall, Color(200, 185, 110, 255));
	C_ADDVARIABLE(Color, colEspMainBoxAllies, Color(0, 200, 170, 255));
	C_ADDVARIABLE(Color, colEspMainBoxAlliesWall, Color(170, 110, 200, 255));

	C_ADDVARIABLE(bool, bEspMainInfo, false);

	// info
	/* left */
	C_ADDVARIABLE(bool, bEspMainInfoHealth, true);
	C_ADDVARIABLE(bool, bEspMainInfoMoney, false);

	/* top */
	C_ADDVARIABLE(bool, bEspMainInfoRank, false);
	C_ADDVARIABLE(bool, bEspMainInfoName, true);
	C_ADDVARIABLE(bool, bEspMainInfoFlash, false);

	/* right */
	C_ADDVARIABLE_VECTOR(bool, INFO_FLAG_MAX, vecEspMainInfoFlags, true);

	/* bottom */
	C_ADDVARIABLE(bool, bEspMainInfoWeapons, true);
	C_ADDVARIABLE(bool, bEspMainInfoAmmo, true);
	C_ADDVARIABLE(bool, bEspMainInfoDistance, false);

	C_ADDVARIABLE(bool, bEspGlow, false);
	C_ADDVARIABLE(bool, bEspGlowEnemies, false);
	C_ADDVARIABLE(bool, bEspGlowAllies, false);
	C_ADDVARIABLE(bool, bEspGlowWeapons, false);
	C_ADDVARIABLE(bool, bEspGlowGrenades, false);
	C_ADDVARIABLE(bool, bEspGlowBomb, false);

	C_ADDVARIABLE(bool, bEspGlowBloom, false);

	C_ADDVARIABLE(Color, colEspGlowEnemies, Color(230, 20, 60, 128));
	C_ADDVARIABLE(Color, colEspGlowEnemiesWall, Color(255, 235, 240, 128));
	C_ADDVARIABLE(Color, colEspGlowAllies, Color(85, 140, 255, 128));
	C_ADDVARIABLE(Color, colEspGlowAlliesWall, Color(240, 235, 255, 128));
	C_ADDVARIABLE(Color, colEspGlowWeapons, Color(80, 0, 225, 128));
	C_ADDVARIABLE(Color, colEspGlowGrenades, Color(180, 130, 30, 128));
	C_ADDVARIABLE(Color, colEspGlowBomb, Color(140, 220, 80, 128));
	C_ADDVARIABLE(Color, colEspGlowBombPlanted, Color(200, 210, 80, 128));

	C_ADDVARIABLE(bool, bEspChams, false);
	C_ADDVARIABLE(bool, bEspChamsEnemies, false);
	C_ADDVARIABLE(bool, bEspChamsAllies, false);
	C_ADDVARIABLE(bool, bEspChamsViewModel, false);

	C_ADDVARIABLE(bool, bEspChamsXQZ, false);
	C_ADDVARIABLE(int, iEspChamsPlayers, 0);
	C_ADDVARIABLE(int, iEspChamsViewModel, 0);
	C_ADDVARIABLE(Color, colEspChamsEnemies, Color(180, 65, 65, 255));
	C_ADDVARIABLE(Color, colEspChamsEnemiesWall, Color(180, 180, 40, 255));
	C_ADDVARIABLE(Color, colEspChamsAllies, Color(70, 40, 190, 255));
	C_ADDVARIABLE(Color, colEspChamsAlliesWall, Color(50, 150, 150, 255));
	C_ADDVARIABLE(Color, colEspChamsViewModel, Color(80, 255, 45, 255));
	C_ADDVARIABLE(Color, colEspChamsViewModelAdditional, Color(0, 0, 255, 255));

	C_ADDVARIABLE(bool, bWorld, false);
	C_ADDVARIABLE(bool, bWorldNightMode, false);
	C_ADDVARIABLE(int, iWorldMaxFlash, 100);
	C_ADDVARIABLE(int, iWorldThirdPersonKey, 0);
	C_ADDVARIABLE(float, flWorldThirdPersonOffset, 150.f);
	C_ADDVARIABLE_VECTOR(bool, REMOVAL_MAX, vecWorldRemovals, false);

	C_ADDVARIABLE(bool, bScreen, false);
	C_ADDVARIABLE(bool, bEspMainFarRadar, false);
	C_ADDVARIABLE(float, flScreenCameraFOV, 0.f);
	C_ADDVARIABLE(float, flScreenViewModelFOV, 0.f);
	C_ADDVARIABLE(bool, bScreenHitMarker, false);
	C_ADDVARIABLE(bool, bScreenHitMarkerDamage, false);
	C_ADDVARIABLE(bool, bScreenHitMarkerSound, false);
	C_ADDVARIABLE(float, flScreenHitMarkerTime, 1.0f);
	C_ADDVARIABLE(int, iScreenHitMarkerGap, 5);
	C_ADDVARIABLE(int, iScreenHitMarkerLenght, 10);
	C_ADDVARIABLE(Color, colScreenHitMarker, Color(255, 255, 255, 255));
	C_ADDVARIABLE(Color, colScreenHitMarkerDamage, Color(200, 55, 20, 255));
	#pragma endregion

	#pragma region variables_misc
	C_ADDVARIABLE(bool, bBunnyHop, false);
	C_ADDVARIABLE(int, iBunnyHopChance, 100);
	C_ADDVARIABLE(bool, bAutoStrafe, false);

	C_ADDVARIABLE(bool, bFakeLag, false);
	C_ADDVARIABLE(bool, bPingSpike, false);
	C_ADDVARIABLE(bool, bAutoPistol, false);

	C_ADDVARIABLE(bool, bNoCrouchCooldown, false);
	C_ADDVARIABLE(bool, bAutoAccept, false);
	C_ADDVARIABLE(bool, bRankReveal, false);
	C_ADDVARIABLE(bool, bUnlockInventory, false);
	C_ADDVARIABLE(bool, bAntiUntrusted, true);
	#pragma endregion

	#pragma region variables_menu
	C_ADDVARIABLE(int, iMenuKey, VK_HOME);
	C_ADDVARIABLE(int, iPanicKey, VK_END);
	#pragma endregion
};

inline Variables_t Vars;
