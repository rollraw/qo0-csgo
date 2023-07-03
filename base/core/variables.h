#pragma once
// used: [win] virtual key codes @todo: temp until new gui
#include <windows.h>

// used: c_add_variable
#include "config.h"

#pragma region variables_multi_entries
using VisualOverlayPlayerFlags_t = unsigned int;
enum EVisualOverlayPlayerFlags : VisualOverlayPlayerFlags_t
{
	VISUAL_OVERLAY_PLAYER_FLAG_NONE = 0U,
	VISUAL_OVERLAY_PLAYER_FLAG_HELMET = (1 << 0),
	VISUAL_OVERLAY_PLAYER_FLAG_KEVLAR = (1 << 1),
	VISUAL_OVERLAY_PLAYER_FLAG_KIT = (1 << 2),
	VISUAL_OVERLAY_PLAYER_FLAG_ZOOM = (1 << 3)
};

using VisualWorldRemovalFlags_t = unsigned int;
enum EVisualWorldRemovalFlags : VisualWorldRemovalFlags_t
{
	VISUAL_WORLD_REMOVAL_FLAG_NONE = 0U,
	VISUAL_WORLD_REMOVAL_FLAG_POSTPROCESSING = (1 << 0),
	VISUAL_WORLD_REMOVAL_FLAG_PUNCH = (1 << 1),
	VISUAL_WORLD_REMOVAL_FLAG_FLASH = (1 << 2),
	VISUAL_WORLD_REMOVAL_FLAG_SMOKE = (1 << 3),
	VISUAL_WORLD_REMOVAL_FLAG_SCOPE = (1 << 4)
};
#pragma endregion

#pragma region variables_combo_entries
using VisualOverlayBox_t = int;
enum EVisualOverlayBox : VisualOverlayBox_t
{
	VISUAL_OVERLAY_BOX_NONE = 0,
	VISUAL_OVERLAY_BOX_FULL,
	VISUAL_OVERLAY_BOX_CORNERS,
	VISUAL_OVERLAY_BOX_MAX
};

using VisualGlow_t = int;
enum EVisualGlow : VisualGlow_t
{
	VISUAL_GLOW_OUTER = 0,
	VISUAL_GLOW_COVER,
	VISUAL_GLOW_INNER,
	VISUAL_GLOW_MAX
};

using VisualChams_t = int;
enum EVisualChams : VisualChams_t
{
	VISUAL_CHAMS_NONE = 0,
	VISUAL_CHAMS_COVERED,
	VISUAL_CHAMS_FLAT,
	VISUAL_CHAMS_GLOW,
	VISUAL_CHAMS_REFLECTIVE,
	VISUAL_CHAMS_SCROLL,
	VISUAL_CHAMS_MAX
};
#pragma endregion

struct Variables_t
{
	#pragma region variables_rage
	/* @section: aimbot */
	C_ADD_VARIABLE(bool, bRage, false);

	/* @section: antiaim */
	C_ADD_VARIABLE(bool, bAntiAim, false);
	C_ADD_VARIABLE(bool, bAntiAimPitch, false);
	C_ADD_VARIABLE(int, iAntiAimPitch, 89);
	C_ADD_VARIABLE(bool, bAntiAimYawReal, false);
	C_ADD_VARIABLE(int, iAntiAimYawRealOffset, 180);
	C_ADD_VARIABLE(bool, bAntiAimYawFake, false);
	C_ADD_VARIABLE(KeyBind_t, keyAntiAimYawFakeInverter, KeyBind_t("anti-aim inverter", VK_XBUTTON1));
	#pragma endregion

	#pragma region variables_legit
	/* @section: aimbot */
	C_ADD_VARIABLE(bool, bLegit, false);

	/* @section: triggerbot */
	C_ADD_VARIABLE(bool, bTrigger, false);
	C_ADD_VARIABLE(KeyBind_t, keyTrigger, KeyBind_t("triggerbot"));
	C_ADD_VARIABLE(int, iTriggerDelay, 0);
	C_ADD_VARIABLE(bool, bTriggerAutoWall, false);
	C_ADD_VARIABLE(int, iTriggerMinimalDamage, 70);

	// filters
	C_ADD_VARIABLE(bool, bTriggerHead, true);
	C_ADD_VARIABLE(bool, bTriggerChest, true);
	C_ADD_VARIABLE(bool, bTriggerStomach, true);
	C_ADD_VARIABLE(bool, bTriggerArms, false);
	C_ADD_VARIABLE(bool, bTriggerLegs, false);
	#pragma endregion

	#pragma region variables_visuals
	C_ADD_VARIABLE(bool, bVisual, false);

	/* @section: render */
	C_ADD_VARIABLE(bool, bVisualOverlay, false);
	C_ADD_VARIABLE(bool, bVisualOverlayEnemies, false);
	C_ADD_VARIABLE(bool, bVisualOverlayAllies, false);
	C_ADD_VARIABLE(bool, bVisualOverlayLocal, false);
	C_ADD_VARIABLE(bool, bVisualOverlayWeapons, false);
	C_ADD_VARIABLE(bool, bVisualOverlayGrenades, false);
	C_ADD_VARIABLE(bool, bVisualOverlayBomb, false);

	/* @section: overlay players */
	C_ADD_VARIABLE(int, iVisualOverlayPlayerBox, VISUAL_OVERLAY_BOX_NONE);
	C_ADD_VARIABLE(Color_t, colVisualOverlayBoxEnemies, Color_t(160, 60, 60, 255));
	C_ADD_VARIABLE(Color_t, colVisualOverlayBoxEnemiesHidden, Color_t(200, 185, 110, 255));
	C_ADD_VARIABLE(Color_t, colVisualOverlayBoxAllies, Color_t(0, 200, 170, 255));
	C_ADD_VARIABLE(Color_t, colVisualOverlayBoxAlliesHidden, Color_t(170, 110, 200, 255));
	C_ADD_VARIABLE(Color_t, colVisualOverlayBoxLocal, Color_t(130, 150, 80, 255));
	C_ADD_VARIABLE(Color_t, colVisualOverlayBoxLocalHidden, Color_t(40, 150, 100, 255));

	C_ADD_VARIABLE(bool, bVisualOverlayPlayerInfo, false);

	// left
	C_ADD_VARIABLE(bool, bVisualOverlayPlayerHealth, true);
	C_ADD_VARIABLE(bool, bVisualOverlayPlayerMoney, false);

	// top
	C_ADD_VARIABLE(bool, bVisualOverlayPlayerName, true);
	C_ADD_VARIABLE(bool, bVisualOverlayPlayerFlash, false);

	// right
	C_ADD_VARIABLE(unsigned int, nVisualOverlayPlayerFlags, VISUAL_OVERLAY_PLAYER_FLAG_NONE);

	// bottom
	C_ADD_VARIABLE(bool, bVisualOverlayPlayerWeapons, true);
	C_ADD_VARIABLE(bool, bVisualOverlayPlayerAmmo, true);
	C_ADD_VARIABLE(bool, bVisualOverlayPlayerDistance, false);

	/* @section: render weapons */
	C_ADD_VARIABLE(int, iVisualOverlayWeaponBox, VISUAL_OVERLAY_BOX_NONE);
	C_ADD_VARIABLE(Color_t, colVisualOverlayBoxWeapons, Color_t(255, 255, 255, 220));

	C_ADD_VARIABLE(bool, bVisualOverlayWeaponInfo, false);
	C_ADD_VARIABLE(bool, bVisualOverlayWeaponIcon, true);
	C_ADD_VARIABLE(bool, bVisualOverlayWeaponAmmo, true);
	C_ADD_VARIABLE(bool, bVisualOverlayWeaponDistance, false);

	/* @section: glow */
	C_ADD_VARIABLE(bool, bVisualGlow, false);
	C_ADD_VARIABLE(bool, bVisualGlowEnemies, false);
	C_ADD_VARIABLE(bool, bVisualGlowAllies, false);
	C_ADD_VARIABLE(bool, bVisualGlowLocal, false);
	C_ADD_VARIABLE(bool, bVisualGlowWeapons, false);
	C_ADD_VARIABLE(bool, bVisualGlowGrenades, false);
	C_ADD_VARIABLE(bool, bVisualGlowBomb, false);

	C_ADD_VARIABLE(bool, bVisualGlowBloom, false);

	C_ADD_VARIABLE(Color_t, colVisualGlowEnemies, Color_t(230, 20, 60, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowEnemiesHidden, Color_t(255, 235, 240, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowAllies, Color_t(85, 140, 255, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowAlliesHidden, Color_t(240, 235, 255, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowLocal, Color_t(100, 120, 190, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowLocalHidden, Color_t(200, 170, 190, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowWeapons, Color_t(80, 0, 225, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowGrenades, Color_t(180, 130, 30, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowBomb, Color_t(140, 220, 80, 128));
	C_ADD_VARIABLE(Color_t, colVisualGlowBombPlanted, Color_t(200, 210, 80, 128));

	/* @section: chams */
	C_ADD_VARIABLE(bool, bVisualChams, false);

	// filters
	C_ADD_VARIABLE(bool, bVisualChamsEnemies, false);
	C_ADD_VARIABLE(bool, bVisualChamsAllies, false);
	C_ADD_VARIABLE(bool, bVisualChamsLocal, false);
	C_ADD_VARIABLE(bool, bVisualChamsViewModel, false);

	// @todo: all of those could be changed to array for each filter, this should simplify a lot of branches/calls later | same for glow and overlay
	C_ADD_VARIABLE(int, iVisualChamsEnemies, VISUAL_CHAMS_COVERED);
	C_ADD_VARIABLE(bool, bVisualChamsEnemiesXQZ, false);
	C_ADD_VARIABLE(bool, bVisualChamsEnemiesWireframe, false);
	C_ADD_VARIABLE(int, iVisualChamsAllies, VISUAL_CHAMS_COVERED);
	C_ADD_VARIABLE(bool, bVisualChamsAlliesXQZ, false);
	C_ADD_VARIABLE(bool, bVisualChamsAlliesWireframe, false);
	C_ADD_VARIABLE(int, iVisualChamsLocal, VISUAL_CHAMS_GLOW);
	C_ADD_VARIABLE(bool, bVisualChamsLocalXQZ, false);
	C_ADD_VARIABLE(bool, bVisualChamsLocalWireframe, false);
	C_ADD_VARIABLE(int, iVisualChamsLocalDesync, VISUAL_CHAMS_FLAT);
	C_ADD_VARIABLE(bool, bVisualChamsLocalDesyncXQZ, false);
	C_ADD_VARIABLE(bool, bVisualChamsLocalDesyncWireframe, false);
	C_ADD_VARIABLE(int, iVisualChamsViewModel, VISUAL_CHAMS_SCROLL);
	C_ADD_VARIABLE(bool, bVisualChamsViewModelXQZ, false);
	C_ADD_VARIABLE(bool, bVisualChamsViewModelWireframe, false);
	C_ADD_VARIABLE(Color_t, colVisualChamsEnemies, Color_t(180, 65, 65, 255));
	C_ADD_VARIABLE(Color_t, colVisualChamsEnemiesHidden, Color_t(180, 180, 40, 255));
	C_ADD_VARIABLE(Color_t, colVisualChamsAllies, Color_t(70, 40, 190, 255));
	C_ADD_VARIABLE(Color_t, colVisualChamsAlliesHidden, Color_t(50, 150, 150, 255));
	C_ADD_VARIABLE(Color_t, colVisualChamsLocal, Color_t(120, 230, 160, 255));
	C_ADD_VARIABLE(Color_t, colVisualChamsLocalHidden, Color_t(120, 120, 140, 255));
	C_ADD_VARIABLE(Color_t, colVisualChamsLocalDesync, Color_t(210, 230, 120, 100));
	C_ADD_VARIABLE(Color_t, colVisualChamsLocalDesyncHidden, Color_t(190, 210, 240, 100));
	C_ADD_VARIABLE(Color_t, colVisualChamsViewModel, Color_t(80, 255, 45, 255));
	C_ADD_VARIABLE(Color_t, colVisualChamsViewModelHidden, Color_t(0, 0, 255, 255));

	/* @section: world */
	C_ADD_VARIABLE(bool, bVisualWorld, false);
	C_ADD_VARIABLE(bool, bVisualWorldNightMode, false);
	C_ADD_VARIABLE(unsigned int, nVisualWorldRemovals, VISUAL_WORLD_REMOVAL_FLAG_NONE);
	C_ADD_VARIABLE(KeyBind_t, keyVisualWorldThirdPerson, KeyBind_t("thirdperson", 0U, EKeyBindMode::TOGGLE));
	C_ADD_VARIABLE(float, flVisualWorldThirdPersonOffset, 150.f);

	/* @section: screen */
	C_ADD_VARIABLE(bool, bVisualScreen, false);
	C_ADD_VARIABLE(float, flVisualScreenCameraFOV, 0.f);
	C_ADD_VARIABLE(float, flVisualScreenViewModelFOV, 0.f);
	C_ADD_VARIABLE(bool, bVisualScreenHitMarker, false);
	C_ADD_VARIABLE(bool, bVisualScreenHitMarkerDamage, false);
	C_ADD_VARIABLE(bool, bVisualScreenHitMarkerSound, false);
	C_ADD_VARIABLE(float, flVisualScreenHitMarkerTime, 1.0f);
	C_ADD_VARIABLE(int, iVisualScreenHitMarkerGap, 5);
	C_ADD_VARIABLE(int, iVisualScreenHitMarkerLength, 10);
	C_ADD_VARIABLE(Color_t, colVisualScreenHitMarker, Color_t(255, 255, 255, 255));
	C_ADD_VARIABLE(Color_t, colVisualScreenHitMarkerDamage, Color_t(200, 55, 20, 255));
	#pragma endregion

	#pragma region variables_misc
	/* @section: movement */
	C_ADD_VARIABLE(bool, bMiscBunnyHop, false);
	C_ADD_VARIABLE(int, iMiscBunnyHopChance, 100);
	C_ADD_VARIABLE(bool, bMiscAutoStrafe, false);
	C_ADD_VARIABLE(KeyBind_t, keyMiscEdgeJump, KeyBind_t("edge jump"));

	C_ADD_VARIABLE(bool, bMiscFakeLag, false);
	C_ADD_VARIABLE(int, iMiscFakeLagTicks, 14);
	C_ADD_VARIABLE(bool, bMiscAutoAccept, false);
	C_ADD_VARIABLE(bool, bMiscAutoPistol, false);
	C_ADD_VARIABLE(bool, bMiscNoCrouchCooldown, false);

	/* @section: extra */
	C_ADD_VARIABLE(bool, bMiscFakeLatency, false);
	C_ADD_VARIABLE(int, iMiscFakeLatencyAmount, 200);
	C_ADD_VARIABLE(bool, bMiscRevealRanks, false);
	C_ADD_VARIABLE(bool, bMiscUnlockInventory, false);
	C_ADD_VARIABLE(bool, bMiscAntiSMAC, true);
	#pragma endregion

	#pragma region variables_menu
	C_ADD_VARIABLE(int, iMenuKey, VK_HOME);
	C_ADD_VARIABLE(int, iPanicKey, VK_END);
	#pragma endregion
};

inline Variables_t Vars = { };
