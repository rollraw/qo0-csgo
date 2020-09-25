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
	/* sort entities and save data to draw */
	void Store();
	/* get info for hitmarker or e.g. bullettracer */
	void Event(IGameEvent* pEvent, const FNV1A_t uNameHash);
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
		std::array<float, DIR_MAX> arrPadding = { 0, 0, 0, 0 };
	};

	// Extra
	/* get bounding box points of given entity */
	bool GetBoundingBox(CBaseEntity* pEntity, Box_t* pBox);
	/* create .vmt materials with customized parameters for chams */
	IMaterial* CreateMaterial(std::string_view szName, std::string_view szShader, std::string_view szBaseTexture = XorStr("vgui/white"), std::string_view szEnvMap = "", bool bIgnorez = false, bool bWireframe = false, std::string_view szProxies = "");

	// On-Screen
	void HitMarker(const ImVec2& vecScreenSize, float flServerTime, Color colLines, Color colDamage);

	// World
	/* changes the exposure to make the world looks like at night or fullbright */
	void NightMode(CEnvTonemapController* pController); // @credits: sapphyrus
	/* draw frame with title at bomb position */
	void Bomb(const Vector2D& vecScreen, Context_t& ctx, const Color& colFrame);
	/* draw frame with title, timer and defuse bars at planted bomb position */
	void PlantedBomb(CPlantedC4* pBomb, float flServerTime, const Vector2D& vecScreen, Context_t& ctx, const Color& colFrame, const Color& colDefuse, const Color& colFailDefuse, const Color& colBackground, const Color& colOutline);
	/* draw frame with name and timer bar at grenade position */
	void Grenade(CBaseEntity* pGrenade, EClassIndex nIndex, float flServerTime, const Vector2D& vecScreen, Context_t& ctx, const Color& colFrame, const Color& colBackground, const Color& colOutline);
	/* draw weapon icon, ammo bar, distance at dropped weapons positions */
	void DroppedWeapons(CBaseCombatWeapon* pWeapon, short nItemDefinitionIndex, Context_t& ctx, const Color& colPrimary, const Color& colAmmo, const Color& colBackground, const Color& colOutline);

	// Player
	/* draw box, bars, text info's, etc for player */
	void Player(CBaseEntity* pLocal, CBaseEntity* pEntity, Context_t& ctx, const Color& colInfo, const Color& colFrame, const Color& colOutline);

	// Entities
	/* draw entity bounding box */
	void Box(const Box_t& box, const int nBoxType, const Color& colPrimary, const Color& colOutline);
	/* draw vertical line with health-based height */
	void HealthBar(Context_t& ctx, const float flFactor, const Color& colPrimary, const Color& colBackground, const Color& colOutline);
	/* draw horizontal line with ammo-based width */
	void AmmoBar(CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, Context_t& ctx, const Color& colPrimary, const Color& colBackground, const Color& colOutline);
	/* draw horizontal line with flashed duration-based width */
	void FlashBar(CBaseEntity* pEntity, Context_t& ctx, const Color& colPrimary, const Color& colBackground, const Color& colOutline);

	// Values
	std::deque<HitMarkerObject_t> vecHitMarks;
};
