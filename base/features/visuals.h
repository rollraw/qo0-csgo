#pragma once
// using: std::deque for visuals objects
#include <deque>

// used: winapi, directx, imgui, fmt includes
#include "../common.h"
// used: vector
#include "../sdk/datatypes/vector.h"
// used: color
#include "../sdk/datatypes/color.h"
// used: baseentity, baseweapon, envtonemapcontroller classes
#include "../sdk/entity.h"
// used: listener event function
#include "../sdk/interfaces/igameeventmanager.h"
// used: convar, engine, cliententitylist, globals, input, materialsystem, model/view render, gameevent interfaces
#include "../core/interfaces.h"

struct Box_t
{
	float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f, width = 0.f, height = 0.f;
};

struct HitMarkerObject_t
{
	Vector	vecPosition;
	int		iDamage;
	float	flTime;
};

class CEnvTonemapController;
class CVisuals : public CSingleton<CVisuals>
{
public:
	// Get
	/* sort entities and render */
	void Run(ImDrawList* pDrawList, const ImVec2 vecScreenSize);
	/* get info for hitmarker or e.g. bullettracer */
	void Event(IGameEvent* pEvent);
	// Other
	/* color player models, returns true when need clear overrides */
	bool Chams(CBaseEntity* pLocal, DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector& vecModelOrigin, int nFlags);
	/* glow effect on entities like while spectating */
	void Glow(CBaseEntity* pLocal);

private:
	enum EPaddingDirection : unsigned int
	{
		DIR_LEFT = 0,
		DIR_TOP,
		DIR_RIGHT,
		DIR_BOTTOM,
		DIR_MAX
	};

	struct Context_t
	{
		// bounding box
		Box_t box = { };
		// dormant alpha
		float iAlpha = 0;
		// summary of all directions paddings
		std::array<int, DIR_MAX> arrPadding = { 0, 0, 0, 0 };
	};

	// Extra
	/* get collideable box points */
	Vector* GetPoints(CBaseEntity* pEntity);
	/* get bounding box points of given entity */
	bool GetBoundingBox(CBaseEntity* pEntity, Box_t& box);
	/* create .vmt materials with customized parameters for chams */
	IMaterial* CreateMaterial(std::string_view szName, std::string_view szShader, std::string_view szBaseTexture = XorStr("vgui/white"), std::string_view szEnvMap = "", bool bIgnorez = false, bool bWireframe = false, std::string_view szProxies = "");

	// On-Screen
	void HitMarker(ImDrawList* pDrawList, float flServerTime, const ImVec2 vecScreenSize, Color colLines, Color colDamage);

	// World
	/* changes the exposure to make the world looks like at night or fullbright */
	void NightMode(CEnvTonemapController* pController); // @credits: sapphyrus
	/* draw frame with title at bomb position */
	void Bomb(ImDrawList* pDrawList, Vector2D vecScreen, Context_t& ctx, Color colFrame);
	/* draw frame with title, timer and defuse bars at planted bomb position */
	void PlantedBomb(ImDrawList* pDrawList, CPlantedC4* pBomb, float flServerTime, Vector2D vecScreen, Context_t& ctx, Color colFrame, Color colDefuse, Color colFailDefuse, Color colBackground, Color colOutline);
	/* draw frame with name and timer bar at grenade position */
	void Grenade(ImDrawList* pDrawList, EClassIndex nIndex, CBaseEntity* pGrenade, float flServerTime, Vector2D vecScreen, Context_t& ctx, Color colFrame, Color colBackground, Color colOutline);
	/* draw weapon icon, ammo bar, distance at dropped weapons positions */
	void DroppedWeapons(ImDrawList* pDrawList, CBaseCombatWeapon* pWeapon, short nItemDefinitionIndex, Context_t& ctx, Color colPrimary, Color colAmmo, Color colBackground, Color colOutline);

	// Player
	/* draw box, bars, text info's, etc for player */
	void Player(ImDrawList* pDrawList, CBaseEntity* pLocal, CBaseEntity* pEntity, Context_t& ctx, Color colInfo, Color colFrame, Color colOutline);

	// Entities
	/* draw entity bounding box */
	void Box(ImDrawList* pDrawList, const Box_t& box, Color colPrimary, Color colOutline);
	/* draw vertical line with health-based height */
	void HealthBar(ImDrawList* pDrawList, float flFactor, Context_t& ctx, Color colPrimary, Color colBackground, Color colOutline);
	/* draw horizontal line with ammo-based width */
	void AmmoBar(ImDrawList* pDrawList, CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, Context_t& ctx, Color colPrimary, Color colBackground, Color colOutline);
	/* draw horizontal line with flashed duration-based width */
	void FlashBar(ImDrawList* pDrawList, CBaseEntity* pEntity, Context_t& ctx, Color colPrimary, Color colBackground, Color colOutline);

	// Values
	std::deque<HitMarkerObject_t> vecHitMarks;
};
