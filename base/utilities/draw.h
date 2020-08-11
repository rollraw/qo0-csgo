#pragma once
// used: std::function
#include <functional>
// used: std::deque
#include <deque>
// used: std::unique_lock
#include <mutex>
// used: std::shared_mutex
#include <shared_mutex>

// used: winapi, directx, imgui, fmt includes
#include "../common.h"
// used: color
#include "../sdk/datatypes/color.h"
// used: vector
#include "../sdk/datatypes/vector.h"

#pragma region draw_enumerations
enum class EDrawType : int
{
	NONE = 0,
	LINE,
	RECT,
	RECT_MULTICOLOR,
	CIRCLE,
	TEXT
};

// rectangle rendering flags
enum ERectRenderFlags : int
{
	IMGUI_RECT_NONE =		0,
	IMGUI_RECT_OUTLINE =	(1 << 0),
	IMGUI_RECT_BORDER =		(1 << 1),
	IMGUI_RECT_FILLED =		(1 << 2)
};

// circle rendering flags
enum ECircleRenderFlags : int
{
	IMGUI_CIRCLE_NONE = 0,
	IMGUI_CIRCLE_OUTLINE = (1 << 0),
	IMGUI_CIRCLE_FILLED = (1 << 1)
};

// text rendering flags
enum ETextRenderFlags : int
{
	IMGUI_TEXT_NONE =		0,
	IMGUI_TEXT_DROPSHADOW = (1 << 0),
	IMGUI_TEXT_OUTLINE =	(1 << 1)
};
#pragma endregion

struct DrawObject_t
{
	EDrawType nType = EDrawType::NONE;

	// Values
	/* font */
	float flFontSize = 0.0f;
	const ImFont* pFont = nullptr;

	/* position */
	ImVec2 vecMin = { };
	ImVec2 vecMax = { };

	/* colors */
	ImU32 colFirst = 0;
	ImU32 colSecond = 0;
	ImU32 colThird = 0;
	ImU32 colFourth = 0;

	/* string */
	std::string szText = { };
	int iFlags = 0;

	/* primitive factors */
	float flRadius = 0.0f;
	int nSegments = 0;
	float flRounding = 0.0f;
	ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_None;
	float flThickness = 0.0f;
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

// extended imgui functionality
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
	/* render primitives by stored & safe data */
	void RenderDrawData(ImDrawList* pDrawList);
	/* clear native data to draw */
	void ClearDrawData();
	/* swap native draw data to safe */
	void SwapDrawData();

	// Render
	void AddLine(const ImVec2& vecStart, const ImVec2& vecEnd, Color colLine, float flThickness = 1.0f);
	void AddRect(const ImVec2& vecMin, const ImVec2& vecMax, Color colRect, int iFlags = IMGUI_RECT_NONE, Color colOutline = Color(0, 0, 0, 255), float flRounding = 0.f, ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_All, float flThickness = 1.0f);
	void AddRectMultiColor(const ImVec2& vecMin, const ImVec2& vecMax, Color colUpperLeft, Color colUpperRight, Color colBottomLeft, Color colBottomRight);
	void AddCircle(const ImVec2& vecCenter, float flRadius, Color colCircle, int nSegments = 12, int iFlags = IMGUI_CIRCLE_NONE, float flThinkness = 1.0f, Color colOutline = Color(0, 0, 0, 255));
	void AddText(const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, Color colText, int iFlags = IMGUI_TEXT_NONE, Color colOutline = Color(0, 0, 0, 255));
	void AddText(const ImVec2& vecPosition, const std::string& szText, Color colText, int iFlags = IMGUI_TEXT_NONE, Color colOutline = Color(0, 0, 0, 255));


	// Extra
	/* converts 3d game world space to screen space */
	bool WorldToScreen(const Vector& vecOrigin, Vector2D& vecScreen);

	// Values
	/* directx init state */
	inline bool	bInitialized = false;
	/* saved data of draw functions */
	inline std::deque<DrawObject_t> vecDrawData = { };
	/* thread-safe data of draw functions */
	inline std::deque<DrawObject_t> vecSafeDrawData = { };
	/* thread-safe render mutex */
	inline std::shared_mutex drawMutex = { };
}
