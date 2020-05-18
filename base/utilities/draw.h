#pragma once
// used: std::function
#include <functional>

// used: winapi, directx, imgui, fmt includes
#include "../common.h"
// used: color
#include "../sdk/datatypes/color.h"
// used: vector
#include "../sdk/datatypes/vector.h"

enum EFontRenderFlag
{
	FONT_DROPSHADOW = 0,
	FONT_OUTLINE = 1,
};

enum EFontRenderFlags
{
	FONT_LEFT = 0,
	FONT_RIGHT = 1,
	FONT_CENTER = 2
};

/*
 * FONTS
 */
namespace F
{
	/*
	 * fonts navigation:
	 * [N][purpose]	[max size]	[flags]
	 */

	// 0	main		13		autohinting
	inline ImFont* Whitney;
	// 1	extra		14		bold
	inline ImFont* Verdana;
	// 2	visuals		40		lighthinting
	inline ImFont* SmallestPixel;
	// 3	icons		40		lighthinting
	inline ImFont* Icons;
}

// expanded imgui functionality
namespace ImGui
{
	// Main
	void HelpMarker(const char* szDescription);
	bool ListBox(const char* szLabel, int* iCurrentItem, std::function<const char* (int)> pLambda, int nItemsCount, int iHeightInItems);
	bool HotKey(const char* szLabel, int* v);
	bool MultiCombo(const char* szLabel, const char** szDisplayName, std::vector<bool>& v, int nHeightInItems);
	bool ColorEdit3(const char* szLabel, Color* v, ImGuiColorEditFlags flags);
	bool ColorEdit4(const char* szLabel, Color* v, ImGuiColorEditFlags flags);

	// Extra
	void AddText(ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const char* szText, ImU32 colText, bool bOutline = false, ImU32 colOutline = 0x000000FF);
	void AddText(ImDrawList* pDrawList, const ImVec2& vecPosition, const char* szText, ImU32 colText, bool bOutline = false, ImU32 colOutline = 0x000000FF);
}

/*
 * DRAW
 * ready rendering framework
 */
namespace D
{
	// Get
	/* create fonts, set styles etc */
	void Setup(IDirect3DDevice9* pDevice, unsigned int uFontFlags = 0x0);
	/* shutdown imgui */
	void Destroy();

	// Extra
	/* converts 3d game world space to screen space */
	bool WorldToScreen(const Vector& vecOrigin, Vector2D& vecScreen);

	// Values
	/* directx init state */
	inline bool	bInitialized = false;
}
