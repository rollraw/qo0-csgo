#pragma once
// used: cheat variables
#include "../../core/variables.h"

// used: ccsplayer
#include "../../sdk/entity.h"

/*
 * OVERLAY
 * - draw information about entities
 */
namespace F::VISUAL::OVERLAY
{
	enum EAlignSide : std::uint8_t
	{
		SIDE_LEFT = 0U,
		SIDE_TOP,
		SIDE_RIGHT,
		SIDE_BOTTOM,
		SIDE_MAX
	};

	enum EAlignDirection : std::uint8_t
	{
		DIR_LEFT = 0U,
		DIR_TOP,
		DIR_RIGHT,
		DIR_BOTTOM,
		DIR_MAX = 4U // @todo: rework stuff based on this cuz one component can have only 3 possible directions at same time. vertical side: left & right + top | bottom, horizontal side: top & bottom + left | right
	};

	class CBaseComponent
	{
	public:
		[[nodiscard]] virtual ImVec2 GetBasePosition(const ImVec4& box) const;

		[[nodiscard]] virtual bool IsDirectional() const
		{
			return false;
		}

		virtual void Render(const ImVec2& vecPosition) = 0;

		EAlignSide nSide = SIDE_TOP;
		ImVec2 vecOffset = { };
		ImVec2 vecSize = { };
	};

	class CBaseDirectionalComponent : public CBaseComponent
	{
	public:
		[[nodiscard]] ImVec2 GetBasePosition(const ImVec4& box) const override;

		[[nodiscard]] bool IsDirectional() const override
		{
			return true;
		}

		EAlignDirection nDirection = DIR_TOP;
	};

	class CBarComponent : public CBaseComponent
	{
	public:
		CBarComponent(const EAlignSide nAlignSide, const ImVec4& vecBox, const float flProgressFactor, const float flThickness, const Color_t& colPrimary, const Color_t& colBackground, const float flOutlineThickness, const Color_t& colOutline);

		void Render(const ImVec2& vecPosition) override;

	private:
		float flProgressFactor = 0.0f;
		float flThickness = 0.0f;
		Color_t colPrimary = { };
		Color_t colBackground = { };
		float flOutlineThickness = 0.0f;
		Color_t colOutline = { };
	};

	class CTextComponent : public CBaseDirectionalComponent
	{
	public:
		CTextComponent(const EAlignSide nAlignSide, const EAlignDirection nAlignDirection, const ImFont* pFont, const float flFontSize, const char* szText, const Color_t& colPrimary, const float flOutlineThickness = 0.0f, const Color_t& colOutline = Color_t(0, 0, 0, 100));
		~CTextComponent();

		void Render(const ImVec2& vecPosition) override;

	private:
		const ImFont* pFont = nullptr;
		float flFontSize = 0.0f;
		char* szText = nullptr;
		Color_t colPrimary = { };
		float flOutlineThickness = 0.0f;
		Color_t colOutline = { };
	};

	/*
	 * overlay component auto-positioning system
	 * @note: was designed to support the reordering of components that can be implemented with minimal effort
	 *
	 * currently supported next sides and sub-directions:
	 *
	 *                DIR_TOP
	 *                   ^
	 *                   |
	 *        DIR_LEFT <-o-> DIR_RIGHT
	 * DIR_LEFT <-o *---------* o-> DIR_RIGHT
	 *            | |         | |
	 *            v |         | v
	 *   DIR_BOTTOM |         | DIR_BOTTOM
	 *              |         |
	 *      DIR_TOP |         | DIR_TOP
	 *            ^ |         | ^
	 *            | |         | |
	 *            o *---------* o
	 *        DIR_LEFT <-o-> DIR_RIGHT
	 *                   |
	 *                   v
	 *              DIR_BOTTOM
	 */
	struct Context_t
	{
		/* @section: special case components */
		/// add the box component to overlay
		/// @remarks: current implementation expects this to be first component, it's an immediate rendering component
		void AddBoxComponent(const ImVec4& vecBox, const VisualOverlayBox_t nType, float flThickness, const Color_t& colPrimary, float flOutlineThickness, const Color_t& colOutline);
		/// add the frame component to overlay
		/// @remarks: current implementation expects this to be added after components that should be inside it, it's an immediate rendering component
		/// @returns: size constraints of the added frame
		ImVec4 AddFrameComponent(const ImVec2& vecScreen, const EAlignSide nSide, const Color_t& colBackground, const float flRounding, const ImDrawCornerFlags nRoundingCorners);

		/* @section: common components */
		/// add new component to overlay
		/// @param[in] pComponent pointer to the one of supported component types
		void AddComponent(CBaseComponent* pComponent);

		/* @section: get */
		/// @returns: size of the all directional components currently assigned to @a'nSide'
		[[nodiscard]] ImVec2 GetTotalDirectionalSize(const EAlignSide nSide) const;

		// calculate final position of components and render them
		void Render(const ImVec4& vecBox) const;

	private:
		// storage of all components
		std::vector<CBaseComponent*> vecComponents = { };
		// additional spacing between components
		float flComponentSpacing = 1.0f;
		// summary padding of all align sides
		float arrSidePaddings[SIDE_MAX] = { };
		// summary padding for all align directions of all align sides
		float arrSideDirectionPaddings[SIDE_MAX][DIR_MAX] = { };
	};

	/* @section: callbacks */
	void OnDraw(CCSPlayer* pLocal);

	/* @section: get */
	/// get bounding box of entity
	/// @returns: true if entity has collision and all points of box are visible on screen, false otherwise
	bool GetEntityBoundingBox(CBaseEntity* pEntity, ImVec4* pvecBox);

	/* @section: main */
	// draw box, bars, text infos, etc at player position
	void Player(CCSPlayer* pLocal, CCSPlayer* pPlayer, const float flDistance);
	// draw frame with title at dropped bomb position
	void Bomb(CC4* pBomb);
	// draw frame with title, timer and defuse bars at planted bomb position
	void PlantedBomb(CPlantedC4* pBomb);
	// draw frame with name and timer bar at grenade position
	void Grenade(CBaseEntity* pGrenade, const EClassIndex nIndex);
	// draw weapon icon, ammo bar, distance at dropped weapon position
	void DroppedWeapon(CBaseCombatWeapon* pWeapon, const float flDistance);
}
