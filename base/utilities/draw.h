#pragma once
// used: std::function
#include <functional>

// used: winapi, directx, imgui, fmt includes
#include "../common.h"
// used: color
#include "../sdk/datatypes/color.h"
// used: vector
#include "../sdk/datatypes/vector.h"

// custom imgui text rendering flags (up to 32)
enum ETextRenderFlag : int
{
	IMGUI_TEXT_NONE =		0,
	IMGUI_TEXT_DROPSHADOW = (1 << 0),
	IMGUI_TEXT_OUTLINE =	(1 << 1)
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

	// Wrappers
	bool Combo(const char* szLabel, std::vector<int>& v, int nIndex, const char* szItemsSeparatedByZeros, int nHeightInItems = -1);
	bool Checkbox(const char* szLabel, std::vector<bool>& v, int nIndex);
	bool SliderFloat(const char* szLabel, std::vector<float>& v, int nIndex, float flMin, float flMax, const char* szFormat = "%.3f", float flPower = 1.0f);
	bool SliderInt(const char* szLabel, std::vector<int>& v, int nIndex, int iMin, int iMax, const char* szFormat = "%d");
	bool ColorEdit3(const char* szLabel, Color* v, ImGuiColorEditFlags flags);
	bool ColorEdit4(const char* szLabel, Color* v, ImGuiColorEditFlags flags);

	// Extra
	void AddText(ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const char* szText, ImU32 colText, int iFlags = IMGUI_TEXT_NONE, ImU32 colOutline = 0x000000FF);
	void AddText(ImDrawList* pDrawList, const ImVec2& vecPosition, const char* szText, ImU32 colText, int iFlags = IMGUI_TEXT_NONE, ImU32 colOutline = 0x000000FF);
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
