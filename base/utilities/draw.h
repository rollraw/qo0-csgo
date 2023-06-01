#pragma once
// used: [d3d]
#include <d3d9.h>

#include "../common.h"

#include "../sdk/datatypes/color.h"
#include "../sdk/datatypes/vector.h"

// used: itemdefinitionindex_t
#include "../sdk/interfaces/iweaponsystem.h"

// used: [ext] imgui
#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/imgui_internal.h"

// forward declarations
struct KeyBind_t;

#pragma region draw_objects_enumerations
enum ERectRenderFlags : unsigned int
{
	DRAW_RECT_NONE = 0,
	DRAW_RECT_OUTLINE = (1 << 0),
	DRAW_RECT_BORDER = (1 << 1),
	DRAW_RECT_FILLED = (1 << 2)
};

enum ECircleRenderFlags : unsigned int
{
	DRAW_CIRCLE_NONE = 0,
	DRAW_CIRCLE_OUTLINE = (1 << 0),
	DRAW_CIRCLE_FILLED = (1 << 1)
};

enum ETriangleRenderFlags : unsigned int
{
	DRAW_TRIANGLE_NONE = 0,
	DRAW_TRIANGLE_OUTLINE = (1 << 0),
	DRAW_TRIANGLE_FILLED = (1 << 1)
};

enum EQuadRenderFlags : unsigned int
{
	DRAW_QUAD_NONE = 0,
	DRAW_QUAD_OUTLINE = (1 << 0),
	DRAW_QUAD_FILLED = (1 << 1)
};

enum EPolygonRenderFlags : unsigned int
{
	DRAW_POLYGON_NONE = 0,
	DRAW_POLYGON_OUTLINE = (1 << 0),
	DRAW_POLYGON_FILLED = (1 << 1)
};

enum ETextRenderFlags : unsigned int
{
	DRAW_TEXT_NONE = 0,
	DRAW_TEXT_DROPSHADOW = (1 << 0),
	DRAW_TEXT_OUTLINE = (1 << 1)
};
#pragma endregion

/*
 * FONTS
 */
namespace FONT
{
	// 0. verdana, size: 12px; lighthinting
	inline ImFont* pMenu;
	// 1. verdana, size: 14px; bold
	inline ImFont* pExtra;
	// 2. smallest pixel, size: 40px; lighthinting
	inline ImFont* pVisual;
	// 3. icons, size: 40px; lighthinting
	inline ImFont* pIcons;
}

// extended imgui functionality
namespace ImGui
{
	/* @section: main */
	void HelpMarker(const char* szDescription);
	bool HotKey(const char* szLabel, unsigned int* pValue);
	bool HotKey(const char* szLabel, KeyBind_t* pKeyBind, const bool bAllowSwitch = true);
	bool MultiCombo(const char* szLabel, unsigned int* pFlags, const char* const* arrItems, int nItemsCount);

	/* @section: wrappers */
	bool ColorEdit3(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags);
	bool ColorEdit4(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags);
}

/*
 * DRAW
 * - rendering framework
 */
namespace D
{
	// initialize rendering engine, create fonts, set styles etc
	bool Setup(IDirect3DDevice9* pDevice, unsigned int uFontFlags = 0x0);
	// shutdown rendering engine
	void Destroy();

	/* @section: callbacks */
	void OnPreReset(IDirect3DDevice9* pDevice);
	void OnPostReset(IDirect3DDevice9* pDevice);
	bool OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* @section: main */
	// render primitives by stored safe data
	void RenderDrawData(ImDrawData* pDrawData);
	// reset active draw data container
	void ResetDrawData();
	// swap active draw data container to safe one
	void SwapDrawData();

	/* @section: get */
	/// @returns: custom font icon codepoint for weapon of given index
	const char8_t* GetWeaponIcon(const ItemDefinitionIndex_t nItemDefinitionIndex);
	/// convert world space to screen space coordinates by game's conversion matrix
	/// @param[out] pvecScreen output for converted screen position
	/// @returns: true if converted coordinates fit into display size, false otherwise
	bool WorldToScreen(const Vector_t& vecOrigin, ImVec2* pvecScreen);

	/* @section: bindings */
	void AddDrawListRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colRect, const unsigned int uFlags = DRAW_RECT_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flRounding = 0.f, const ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_All, float flThickness = 1.0f, const float flOutlineThickness = 1.0f);
	void AddDrawListRectMultiColor(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colUpperLeft, const Color_t& colUpperRight, const Color_t& colBottomRight, const Color_t& colBottomLeft);
	void AddDrawListCircle(ImDrawList* pDrawList, const ImVec2& vecCenter, const float flRadius, const Color_t& colCircle, const int nSegments, const unsigned int uFlags = DRAW_CIRCLE_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 1.0f, const float flOutlineThickness = 1.0f);
	void AddDrawListArc(ImDrawList* pDrawList, const ImVec2& vecPosition, const float flRadius, const float flMinimumAngle, const float flMaximumAngle, const Color_t& colArc = Color_t(255, 255, 255, 255), const float flThickness = 1.0f);
	void AddDrawListLine(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const Color_t& colLine, const float flThickness = 1.0f);
	void AddDrawListTriangle(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color_t& colTriangle, const unsigned int uFlags = DRAW_TRIANGLE_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 0.f);
	void AddDrawListQuad(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const ImVec2& vecFourth, const Color_t& colQuad, const unsigned int uFlags = DRAW_QUAD_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 0.f);
	void AddDrawListPolygon(ImDrawList* pDrawList, const ImVec2* vecPoints, const int nPointsCount, const Color_t& colPolygon, unsigned int uFlags = DRAW_POLYGON_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const bool bClosed = true, const float flThickness = 1.0f);
	void AddDrawListText(ImDrawList* pDrawList, const ImFont* pFont, const float flFontSize, const ImVec2& vecPosition, const char* szText, const Color_t& colText, const unsigned int uFlags = DRAW_TEXT_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 1.0f);

	/* @section: values */
	// rendering engine initialization state
	inline bool	bInitialized = false;
	// active draw data container used to store
	inline ImDrawList* pDrawListActive = nullptr;
	// safe draw data container
	inline ImDrawList* pDrawListSafe = nullptr;
	// actual draw data container used to render
	inline ImDrawList* pDrawListRender = nullptr;
}
