#include "draw.h"

// used: cheat variables
#include "../core/variables.h"
// used: open state
#include "../core/menu.h"
// used: eitemdefinitionindex
#include "../sdk/entity.h"
// used: current window
#include "inputsystem.h"
// used: m_deg2rad
#include "math.h"
// used: memoryset
#include "crt.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/iinputsystem.h"

#ifdef Q_FREETYPE_FROM_GAME
// @note: to make it link properly at runtime, the library should be compiled for the dll with same name as used in the game
#pragma comment(lib, "freetype_dynamic.lib")
#elif defined(_DEBUG)
#pragma comment(lib, "freetype_static_debug.lib")
#else
#pragma comment(lib, "freetype_static.lib")
#endif

// used: [ext] imgui
#include "../../dependencies/imgui/imgui_freetype.h"
#include "../../dependencies/imgui/win32/imgui_impl_win32.h"
#include "../../dependencies/imgui/dx9/imgui_impl_dx9.h"

// used: [res] fonts
#include "../../resources/smallest_pixel.h"
#include "../../resources/qo0icons.h"

#pragma region imgui_extended
/*
 * - what is changed in imgui framework files (currently used imgui version is v1.75):
 *   changed things can be found with "modified by qo0" commentary
 *
 * 1. changed type of imgui dx stateblock creation to fix in-game artefacts and create additional shader to compensate it
 * 2. removed unused GetStyleColorName function
 * 3. changed sliderscale grab length to range from frame start pos to current value pos
 * 4. modified combo, slider, inputtext bounding boxes and text position
 *    also pushed frame padding to decrease frame size on menu
 *    ocornut should add global scaling later and this will be obsolete (i've seen that in somewhat issue)
 * 5. added triangle direction switch when popup is open for combo and change arrow size scale
 * 6. added and used ImGuiCol_ControlBg, ImGuiCol_ControlBgHovered, ImGuiCol_ControlBgActive color entries for next control's widgets: Checkbox, BeginCombo, SliderScalar, InputTextEx
 *	  added and used ImGuiCol_Triangle color entry for combo etc triangle and make changes in RenderArrow function
 * 7. changed cursor color to (140, 40, 225, 100)
 * 8. now used freetype font rasterizier instead stb truetype to make small fonts clear and readable (https://github.com/ocornut/imgui/tree/master/misc/freetype)
 * 9. removed default imgui font (proggyclean) and 'GetDefaultCompressedFontDataTTFBase85()' function because we dont need it, using windows "tahoma.ttf" instead
 */

static constexpr const char* arrKeyNames[ ] =
{
	"",
	"mouse 1", "mouse 2", "cancel", "mouse 3", "mouse 4", "mouse 5", "",
	"backspace", "tab", "", "", "clear", "enter", "", "",
	"shift", "control", "alt", "pause", "caps", "", "", "", "", "", "",
	"escape", "", "", "", "", "space", "page up", "page down",
	"end", "home", "left", "up", "right", "down", "", "", "",
	"print", "insert", "delete", "",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"", "", "", "", "", "", "",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
	"l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
	"v", "w", "x", "y", "z", "lwin", "rwin", "", "", "",
	"num0", "num1", "num2", "num3", "num4", "num5",
	"num6", "num7", "num8", "num9",
	"*", "+", "", "-", ".", "/",
	"f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
	"f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16",
	"f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24",
	"", "", "", "", "", "", "", "",
	"num lock", "scroll lock",
	"", "", "", "", "", "", "",
	"", "", "", "", "", "", "",
	"lshift", "rshift", "lctrl",
	"rctrl", "lmenu", "rmenu"
};

void ImGui::HelpMarker(const char* szDescription)
{
	TextDisabled(Q_XOR("(?)"));
	if (IsItemHovered())
	{
		BeginTooltip();
		PushTextWrapPos(450.f);
		TextUnformatted(szDescription);
		PopTextWrapPos();
		EndTooltip();
	}
}

bool ImGui::HotKey(const char* szLabel, unsigned int* pValue)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* pWindow = g.CurrentWindow;

	if (pWindow->SkipItems)
		return false;

	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;
	const ImGuiID nIndex = pWindow->GetID(szLabel);

	const float flWidth = CalcItemWidth();
	const ImVec2 vecLabelSize = CalcTextSize(szLabel, nullptr, true);

	const ImRect rectFrame(pWindow->DC.CursorPos + ImVec2(vecLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + GetFrameHeight() : 0.0f, 0.0f), pWindow->DC.CursorPos + ImVec2(flWidth, vecLabelSize.x > 0.0f ? vecLabelSize.y + style.FramePadding.y : 0.f));
	const ImRect rectTotal(rectFrame.Min, rectFrame.Max);

	ItemSize(rectTotal, style.FramePadding.y);
	if (!ItemAdd(rectTotal, nIndex, &rectFrame))
		return false;

	const bool bHovered = ItemHoverable(rectFrame, nIndex);
	if (bHovered)
	{
		SetHoveredID(nIndex);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool bFocusRequested = FocusableItemRegister(pWindow, nIndex);
	const bool bClicked = bHovered && io.MouseClicked[0];
	const bool bDoubleClicked = bHovered && io.MouseDoubleClicked[0];
	if (bFocusRequested || bClicked || bDoubleClicked)
	{
		if (g.ActiveId != nIndex)
		{
			CRT::MemorySet(io.MouseDown, 0, sizeof(io.MouseDown));
			CRT::MemorySet(io.KeysDown, 0, sizeof(io.KeysDown));
			*pValue = 0U;
		}

		SetActiveID(nIndex, pWindow);
		FocusWindow(pWindow);
	}

	bool bValueChanged = false;
	if (unsigned int nKey = *pValue; g.ActiveId == nIndex)
	{
		for (int n = 0; n < IM_ARRAYSIZE(io.MouseDown); n++)
		{
			if (IsMouseDown(n))
			{
				switch (n)
				{
				case 0:
					nKey = VK_LBUTTON;
					break;
				case 1:
					nKey = VK_RBUTTON;
					break;
				case 2:
					nKey = VK_MBUTTON;
					break;
				case 3:
					nKey = VK_XBUTTON1;
					break;
				case 4:
					nKey = VK_XBUTTON2;
					break;
				}

				bValueChanged = true;
				ClearActiveID();
			}
		}

		if (!bValueChanged)
		{
			for (int n = VK_BACK; n <= VK_RMENU; n++)
			{
				if (IsKeyDown(n))
				{
					nKey = n;
					bValueChanged = true;
					ClearActiveID();
				}
			}
		}

		if (IsKeyPressed(io.KeyMap[ImGuiKey_Escape]))
		{
			*pValue = 0U;
			ClearActiveID();
		}
		else
			*pValue = nKey;
	}

	char szBuffer[64] = { };
	char* szBufferEnd = CRT::StringCopy(szBuffer, "[ ");
	if (*pValue != 0 && g.ActiveId != nIndex)
		szBufferEnd = CRT::StringCat(szBufferEnd, arrKeyNames[*pValue]);
	else if (g.ActiveId == nIndex)
		szBufferEnd = CRT::StringCat(szBufferEnd, Q_XOR("press"));
	else
		szBufferEnd = CRT::StringCat(szBufferEnd, Q_XOR("none"));
	CRT::StringCat(szBufferEnd, " ]");

	// modified by qo0
	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
	pWindow->DrawList->AddText(ImVec2(rectFrame.Max.x - CalcTextSize(szBuffer).x, rectTotal.Min.y + style.FramePadding.y), GetColorU32(g.ActiveId == nIndex ? ImGuiCol_Text : ImGuiCol_TextDisabled), szBuffer);

	if (vecLabelSize.x > 0.f)
		RenderText(ImVec2(rectTotal.Min.x, rectTotal.Min.y + style.FramePadding.y), szLabel);

	PopStyleVar();
	return bValueChanged;
}

bool ImGui::HotKey(const char* szLabel, KeyBind_t* pKeyBind, const bool bAllowSwitch)
{
	const bool bValueChanged = HotKey(szLabel, &pKeyBind->uKey);

	if (bAllowSwitch)
	{
		char* szUniqueID = static_cast<char*>(MEM_STACKALLOC(CRT::StringLength(szLabel) + 6));
		CRT::StringCat(CRT::StringCopy(szUniqueID, Q_XOR("key##")), szLabel);

		if (IsItemClicked(ImGuiMouseButton_Right))
			OpenPopup(szUniqueID);

		if (BeginPopup(szUniqueID))
		{
			SetNextItemWidth(90.f);
			if (Combo(Q_XOR("##keybind.mode"), reinterpret_cast<int*>(&pKeyBind->nMode), Q_XOR("Hold\0Toggle\0\0")))
				CloseCurrentPopup();

			EndPopup();
		}

		MEM_STACKFREE(szUniqueID);
	}

	return bValueChanged;
}

bool ImGui::MultiCombo(const char* szLabel, unsigned int* pFlags, const char* const* arrItems, int nItemsCount)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* pWindow = g.CurrentWindow;

	if (pWindow->SkipItems)
		return false;

	IM_ASSERT(nItemsCount < 32); // bitflags shift overflow, decrease items count or change variable type

	const ImGuiStyle& style = g.Style;
	const ImVec2 vecLabelSize = CalcTextSize(szLabel, nullptr, true);
	const float flActiveWidth = CalcItemWidth() - (vecLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + GetFrameHeight() : 0.0f);

	std::vector<const char*> vecActiveItems = { };

	// collect active items
	for (int i = 0; i < nItemsCount; i++)
	{
		if (*pFlags & (1 << i))
			vecActiveItems.push_back(arrItems[i]);
	}

	// fuck it, stl still haven't boost::join, fmt::join replacement
	std::string strBuffer = { };
	for (std::size_t i = 0U; i < vecActiveItems.size(); i++)
	{
		strBuffer.append(vecActiveItems[i]);

		if (i < vecActiveItems.size() - 1U)
			strBuffer.append(", ");
	}

	if (strBuffer.empty())
		strBuffer.assign("none");
	else
	{
		const char* szWrapPosition = g.Font->CalcWordWrapPositionA(GetCurrentWindow()->FontWindowScale, strBuffer.data(), strBuffer.data() + strBuffer.length(), flActiveWidth - style.FramePadding.x * 2.0f);
		const std::size_t nWrapLength = szWrapPosition - strBuffer.data();

		if (nWrapLength > 0U && nWrapLength < strBuffer.length())
		{
			strBuffer.resize(nWrapLength);
			strBuffer.append("...");
		}
	}

	bool bValueChanged = false;
	if (BeginCombo(szLabel, strBuffer.c_str()))
	{
		for (int i = 0; i < nItemsCount; i++)
		{
			const int nCurrentFlag = (1 << i);
			if (Selectable(arrItems[i], (*pFlags & nCurrentFlag), ImGuiSelectableFlags_DontClosePopups))
			{
				// flip bitflag
				*pFlags ^= nCurrentFlag;
				bValueChanged = true;
			}
		}

		EndCombo();
	}

	return bValueChanged;
}

bool ImGui::ColorEdit3(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags)
{
	return ColorEdit4(szLabel, pColor, flags);
}

bool ImGui::ColorEdit4(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags)
{
	float arrColor[4];
	pColor->BaseAlpha(arrColor);

	if (ColorEdit4(szLabel, &arrColor[0], flags))
	{
		*pColor = Color_t::FromBase4(arrColor);
		return true;
	}

	return false;
}
#pragma endregion

// thread-safe draw data mutex
static SRWLOCK drawLock = { };

static void* __cdecl ImGuiAllocWrapper(const std::size_t nSize, [[maybe_unused]] void* pUserData = nullptr)
{
	return MEM::HeapAlloc(nSize);
}

static void __cdecl ImGuiFreeWrapper(void* pMemory, [[maybe_unused]] void* pUserData = nullptr) noexcept
{
	MEM::HeapFree(pMemory);
}

bool D::Setup(IDirect3DDevice9* pDevice, unsigned int uFontFlags)
{
	// check is it were already initialized
	if (bInitialized)
		return true;

	ImGui::SetAllocatorFunctions(ImGuiAllocWrapper, ImGuiFreeWrapper);

	ImGui::CreateContext();

	// setup platform and renderer bindings
	if (!ImGui_ImplWin32_Init(IPT::hWindow))
		return false;

	if (!ImGui_ImplDX9_Init(pDevice))
		return false;

	// create draw data containers
	pDrawListActive = IM_NEW(ImDrawList)(ImGui::GetDrawListSharedData());
	pDrawListSafe = IM_NEW(ImDrawList)(ImGui::GetDrawListSharedData());
	pDrawListRender = IM_NEW(ImDrawList)(ImGui::GetDrawListSharedData());

	// setup styles
	#pragma region draw_setup_style
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildRounding = 4.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4, 2);
	style.FrameRounding = 2.0f;
	style.FrameBorderSize = 1.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.IndentSpacing = 6.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 6.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 0.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 1.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
	style.MouseCursorScale = 0.75f;
	#pragma endregion

	#pragma region draw_setup_colors
	/*
	 * color navigation:
	 * [definition N][purpose]	[color] [rgb]
	 * 1. primitive:
	 * - primtv 0 (text) - white: 255,255,255
	 * - primtv 1 (background) - darkslategrey: 20,20,30
	 * - primtv 2 (disabled) - silver: 192,192,192
	 * - primtv 3 (control bg) - cadetblue: 30,35,50
	 * - primtv 4 (border) - black: 0,0,0
	 * - primtv 5 (hover) - matterhorn: 75,75,75
	 *
	 * 2. accents:
	 * - accent 0 (main) - blueviolet: 140,40,225
	 * - accent 1 (dark) - indigo: 55,0,100
	 * - accent 3 (darker) - darkviolet: 75,50,105
	 */

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);					// primtv 0
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.75f, 0.75f, 0.75f, 0.85f);			// primtv 2
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.22f, 0.00f, 0.40f, 0.85f);			// accent 1

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.14f, 0.20f, 1.00f);				// primtv 3
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.60f);				// primtv 1
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.85f);				// primtv 1

	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);					// primtv 4
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);			// clear

	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.14f, 0.20f, 1.00f);				// primtv 3
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.00f, 0.40f, 1.00f);			// accent 1
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);			// accent 0

	style.Colors[ImGuiCol_ControlBg] = ImVec4(0.11f, 0.14f, 0.20f, 1.00f);				// primtv 3
	style.Colors[ImGuiCol_ControlBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);		// primtv 5
	style.Colors[ImGuiCol_ControlBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 0.10f);		// primtv 2

	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.55f, 0.15f, 0.90f, 0.20f);				// accent 0
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.00f, 0.40f, 0.50f);			// accent 1
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.22f, 0.00f, 0.40f, 0.20f);		// accent 1

	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.70f);				// primtv 1

	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.14f, 0.20f, 0.30f);			// primtv 3
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.20f, 0.40f, 1.00f);			// accent 3
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.90f);	// primtv 5
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.75f, 0.75f, 0.75f, 0.10f);	// primtv 2

	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);				// accent 0

	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);				// accent 0
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.22f, 0.00f, 0.40f, 1.00f);		// accent 1

	style.Colors[ImGuiCol_Button] = ImVec4(0.11f, 0.14f, 0.20f, 1.00f);					// primtv 3
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);			// primtv 5
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);			// accent 0

	style.Colors[ImGuiCol_Header] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);					// accent 0
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);			// primtv 5
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.11f, 0.14f, 0.20f, 1.00f);			// primtv 3

	style.Colors[ImGuiCol_Separator] = ImVec4(0.11f, 0.14f, 0.20f, 1.00f);				// primtv 3
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);		// primtv 5
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);		// accent 0

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);				// accent 0
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.70f);		// primtv 5
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.22f, 0.00f, 0.40f, 1.00f);		// accent 1

	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.12f, 0.80f);					// primtv 1
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.80f);				// primtv 5
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.55f, 0.15f, 0.90f, 0.70f);				// accent 0
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.30f, 0.30f, 0.30f, 0.70f);			// primtv 5
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.55f, 0.15f, 0.90f, 0.60f);		// accent 0

	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);				// accent 0
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.55f, 0.15f, 0.90f, 0.50f);		// accent 0
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);			// accent 0
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.55f, 0.15f, 0.90f, 0.50f);	// accent 0

	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.30f, 0.20f, 0.40f, 0.80f);			// accent 3
	style.Colors[ImGuiCol_Triangle] = ImVec4(0.55f, 0.15f, 0.90f, 1.00f);				// accent 0

	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.25f);		// primtv 4
	#pragma endregion

	// create fonts
	ImGuiIO& io = ImGui::GetIO();

	ImFontConfig imVerdanaConfig;
	imVerdanaConfig.RasterizerFlags = ImGuiFreeType::LightHinting;
	FONT::pMenu = io.Fonts->AddFontFromFileTTF(Q_XOR("C:\\Windows\\Fonts\\Verdana.ttf"), 12.f, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());

	imVerdanaConfig.RasterizerFlags = ImGuiFreeType::Bold;
	FONT::pExtra = io.Fonts->AddFontFromFileTTF(Q_XOR("C:\\Windows\\Fonts\\Verdana.ttf"), 14.f, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());

	ImFontConfig imSmallestPixelConfig;
	imSmallestPixelConfig.RasterizerFlags = ImGuiFreeType::LightHinting;
	FONT::pVisual = io.Fonts->AddFontFromMemoryCompressedTTF(smallest_pixel_compressed_data, smallest_pixel_compressed_size, 40.f, &imSmallestPixelConfig, io.Fonts->GetGlyphRangesCyrillic());

	ImFontConfig imIconsConfig;
	imIconsConfig.RasterizerFlags = ImGuiFreeType::LightHinting;
	constexpr ImWchar wIconRanges[ ] =
	{
		0xE000, 0xF8FF, // Private Use Area
		0
	};

	FONT::pIcons = io.Fonts->AddFontFromMemoryCompressedTTF(qo0icons_compressed_data, qo0icons_compressed_size, 40.f, &imIconsConfig, wIconRanges);

	bInitialized = ImGuiFreeType::BuildFontAtlas(io.Fonts, uFontFlags);
	return bInitialized;
}

void D::Destroy()
{
	// check is it already destroyed or wasn't initialized at all
	if (!bInitialized)
		return;

	// free draw data containers
	IM_DELETE(pDrawListActive);
	IM_DELETE(pDrawListSafe);
	IM_DELETE(pDrawListRender);

	// shutdown imgui directx9 renderer binding
	ImGui_ImplDX9_Shutdown();

	// shutdown imgui win32 platform binding
	ImGui_ImplWin32_Shutdown();

	// destroy imgui context
	ImGui::DestroyContext();

	bInitialized = false;
}

#pragma region draw_callbacks
void D::OnPreReset(IDirect3DDevice9* pDevice)
{
	// check is drawing initialized
	if (!bInitialized)
		return;

	// invalidate vertex & index buffer, release fonts texture
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void D::OnPostReset(IDirect3DDevice9* pDevice)
{
	// check is drawing initialized
	if (!bInitialized)
		return;

	ImGui_ImplDX9_CreateDeviceObjects();
}

bool D::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// check is drawing initialized
	if (!bInitialized)
		return false;

	// switch cheat window state if pressed specified key
	if (C::Get<int>(Vars.iMenuKey) > 0 && IPT::IsKeyReleased(C::Get<int>(Vars.iMenuKey)))
		MENU::bMainOpened = !MENU::bMainOpened;

	// disable game input when menu is opened
	I::InputSystem->EnableInput(!MENU::bMainOpened);

	return (MENU::bMainOpened && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam));
}
#pragma endregion

#pragma region draw_main
void D::RenderDrawData(ImDrawData* pDrawData)
{
	if (::TryAcquireSRWLockExclusive(&drawLock))
	{
		*pDrawListRender = *pDrawListSafe;
		::ReleaseSRWLockExclusive(&drawLock);
	}

	if (pDrawListRender->CmdBuffer.empty())
		return;

	// remove trailing command if unused
	if (const ImDrawCmd& lastCommand = pDrawListRender->CmdBuffer.back(); lastCommand.ElemCount == 0 && lastCommand.UserCallback == nullptr)
	{
		pDrawListRender->CmdBuffer.pop_back();
		if (pDrawListRender->CmdBuffer.empty())
			return;
	}

	ImGuiContext* pContext = ImGui::GetCurrentContext();
	ImVector<ImDrawList*>* vecDrawLists = &pContext->DrawDataBuilder.Layers[0];
	vecDrawLists->push_front(pDrawListRender); // this one being most background

	pDrawData->CmdLists = vecDrawLists->Data;
	pDrawData->CmdListsCount = vecDrawLists->Size;
	pDrawData->TotalVtxCount += pDrawListRender->VtxBuffer.Size;
	pDrawData->TotalIdxCount += pDrawListRender->IdxBuffer.Size;

	pContext->IO.MetricsRenderVertices = pDrawData->TotalVtxCount;
	pContext->IO.MetricsRenderIndices = pDrawData->TotalIdxCount;
}

void D::ResetDrawData()
{
	pDrawListActive->Clear();
	pDrawListActive->PushTextureID(ImGui::GetIO().Fonts->TexID);
	pDrawListActive->PushClipRectFullScreen();
}

void D::SwapDrawData()
{
	::AcquireSRWLockExclusive(&drawLock);

	IM_ASSERT(pDrawListActive->VtxBuffer.Size == 0 || pDrawListActive->_VtxWritePtr == pDrawListActive->VtxBuffer.Data + pDrawListActive->VtxBuffer.Size);
	IM_ASSERT(pDrawListActive->IdxBuffer.Size == 0 || pDrawListActive->_IdxWritePtr == pDrawListActive->IdxBuffer.Data + pDrawListActive->IdxBuffer.Size);

	if (!(pDrawListActive->Flags & ImDrawListFlags_AllowVtxOffset))
		IM_ASSERT(static_cast<int>(pDrawListActive->_VtxCurrentIdx) == pDrawListActive->VtxBuffer.Size);

	*pDrawListSafe = *pDrawListActive;

	::ReleaseSRWLockExclusive(&drawLock);
}
#pragma endregion

#pragma region draw_get
// @todo: after merge of the new renderer, we can completely remove icons font and grab them directly from svgs then add them as custom SDF glyphs to some font
const char8_t* D::GetWeaponIcon(const ItemDefinitionIndex_t nItemDefinitionIndex)
{
	/*
	 * @note: icon code = weapon item definition index in hex
	 * list of other icons:
	 *	"E210" - kevlar
	 *	"E20E" - helmet
	 *	"E20F" - defuser kit
	 *	"E211" - banner
	 *	"E212" - target
	 */
	switch (nItemDefinitionIndex)
	{
	case WEAPON_DEAGLE:
		return u8"\uE001";
	case WEAPON_ELITE:
		return u8"\uE002";
	case WEAPON_FIVESEVEN:
		return u8"\uE003";
	case WEAPON_GLOCK:
		return u8"\uE004";
	case WEAPON_AK47:
		return u8"\uE007";
	case WEAPON_AUG:
		return u8"\uE008";
	case WEAPON_AWP:
		return u8"\uE009";
	case WEAPON_FAMAS:
		return u8"\uE00A";
	case WEAPON_G3SG1:
		return u8"\uE00B";
	case WEAPON_GALILAR:
		return u8"\uE00D";
	case WEAPON_M249:
		return u8"\uE00E";
	case WEAPON_M4A1:
		return u8"\uE010";
	case WEAPON_MAC10:
		return u8"\uE011";
	case WEAPON_P90:
		return u8"\uE013";
	case WEAPON_MP5SD:
		return u8"\uE017";
	case WEAPON_UMP45:
		return u8"\uE018";
	case WEAPON_XM1014:
		return u8"\uE019";
	case WEAPON_BIZON:
		return u8"\uE01A";
	case WEAPON_MAG7:
		return u8"\uE01B";
	case WEAPON_NEGEV:
		return u8"\uE01C";
	case WEAPON_SAWEDOFF:
		return u8"\uE01D";
	case WEAPON_TEC9:
		return u8"\uE01E";
	case WEAPON_TASER:
		return u8"\uE01F";
	case WEAPON_HKP2000:
		return u8"\uE020";
	case WEAPON_MP7:
		return u8"\uE021";
	case WEAPON_MP9:
		return u8"\uE022";
	case WEAPON_NOVA:
		return u8"\uE023";
	case WEAPON_P250:
		return u8"\uE024";
	case WEAPON_SCAR20:
		return u8"\uE026";
	case WEAPON_SG556:
		return u8"\uE027";
	case WEAPON_SSG08:
		return u8"\uE028";
	case WEAPON_KNIFE:
		return u8"\uE02A";
	case WEAPON_FLASHBANG:
		return u8"\uE02B";
	case WEAPON_HEGRENADE:
		return u8"\uE02C";
	case WEAPON_SMOKEGRENADE:
		return u8"\uE02D";
	case WEAPON_MOLOTOV:
		[[fallthrough]];
	case WEAPON_FIREBOMB:
		return u8"\uE02E";
	case WEAPON_DECOY:
		[[fallthrough]];
	case WEAPON_DIVERSION:
		return u8"\uE02F";
	case WEAPON_INCGRENADE:
		return u8"\uE030";
	case WEAPON_C4:
		return u8"\uE031";
	case WEAPON_HEALTHSHOT:
		return u8"\uE039";
	case WEAPON_KNIFE_GG:
		[[fallthrough]];
	case WEAPON_KNIFE_T:
		return u8"\uE03B";
	case WEAPON_M4A1_SILENCER:
		return u8"\uE03C";
	case WEAPON_USP_SILENCER:
		return u8"\uE03D";
	case WEAPON_CZ75A:
		return u8"\uE03F";
	case WEAPON_REVOLVER:
		return u8"\uE040";
	case WEAPON_TAGRENADE:
		return u8"\uE044";
	case WEAPON_FISTS:
		return u8"\uE045";
	case WEAPON_TABLET:
		return u8"\uE048";
	case WEAPON_MELEE:
		return u8"\uE04A";
	case WEAPON_AXE:
		return u8"\uE04B";
	case WEAPON_HAMMER:
		return u8"\uE04C";
	case WEAPON_WRENCH:
		return u8"\uE04E";
	case ITEM_KNIFE_BAYONET:
		return u8"\uE1F4";
	case ITEM_KNIFE_CLASSIC:
		return u8"\uE1F7";
	case ITEM_KNIFE_FLIP:
		return u8"\uE1F9";
	case ITEM_KNIFE_GUT:
		return u8"\uE1FA";
	case ITEM_KNIFE_KARAMBIT:
		return u8"\uE1FB";
	case ITEM_KNIFE_M9_BAYONET:
		return u8"\uE1FC";
	case ITEM_KNIFE_TACTICAL:
		return u8"\uE1FD";
	case ITEM_KNIFE_FALCHION:
		return u8"\uE200";
	case ITEM_KNIFE_SURVIVAL_BOWIE:
		return u8"\uE202";
	case ITEM_KNIFE_BUTTERFLY:
		return u8"\uE203";
	case ITEM_KNIFE_PUSH:
		return u8"\uE204";
	case ITEM_KNIFE_CORD:
		return u8"\uE205";
	case ITEM_KNIFE_CANIS:
		return u8"\uE206";
	case ITEM_KNIFE_URSUS:
		return u8"\uE207";
	case ITEM_KNIFE_GYPSY_JACKKNIFE:
		return u8"\uE208";
	case ITEM_KNIFE_OUTDOOR:
		return u8"\uE209";
	case ITEM_KNIFE_STILETTO:
		return u8"\uE20A";
	case ITEM_KNIFE_WIDOWMAKER:
		return u8"\uE20B";
	case ITEM_KNIFE_SKELETON:
		return u8"\uE20D";
	default:
		return u8"\u003F";
	}
}

bool D::WorldToScreen(const Vector_t& vecOrigin, ImVec2* pvecScreen)
{
	const ViewMatrix_t& matWorldToScreen = I::Engine->WorldToScreenMatrix();
	const float flWidth = matWorldToScreen[3][0] * vecOrigin.x + matWorldToScreen[3][1] * vecOrigin.y + matWorldToScreen[3][2] * vecOrigin.z + matWorldToScreen[3][3];

	// check is point can't fit on screen, because it's behind us
	if (flWidth < 0.001f)
		return false;

	// compute the scene coordinates of a point in 3D
	const float flInverse = 1.0f / flWidth;
	pvecScreen->x = (matWorldToScreen[0][0] * vecOrigin.x + matWorldToScreen[0][1] * vecOrigin.y + matWorldToScreen[0][2] * vecOrigin.z + matWorldToScreen[0][3]) * flInverse;
	pvecScreen->y = (matWorldToScreen[1][0] * vecOrigin.x + matWorldToScreen[1][1] * vecOrigin.y + matWorldToScreen[1][2] * vecOrigin.z + matWorldToScreen[1][3]) * flInverse;

	// screen transform
	// get the screen position in pixels of given point
	const ImVec2 vecDisplaySize = ImGui::GetIO().DisplaySize;
	pvecScreen->x = (vecDisplaySize.x * 0.5f) + (pvecScreen->x * vecDisplaySize.x) * 0.5f;
	pvecScreen->y = (vecDisplaySize.y * 0.5f) - (pvecScreen->y * vecDisplaySize.y) * 0.5f;
	return true;
}
#pragma endregion

#pragma region draw_bindings
void D::AddDrawListRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colRect, const unsigned int uFlags, const Color_t& colOutline, const float flRounding, const ImDrawCornerFlags roundingCorners, float flThickness, const float flOutlineThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colRectPacked = colRect.GetU32();
	const ImU32 colOutlinePacked = colOutline.GetU32();

	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilled(vecMin, vecMax, colRectPacked, flRounding, roundingCorners);
	else
	{
		pDrawList->AddRect(vecMin, vecMax, colRectPacked, flRounding, roundingCorners, flThickness);
		flThickness *= 0.5f;
	}

	const float flHalfOutlineThickness = flOutlineThickness * 0.5f;
	const ImVec2 vecThicknessOffset = { flThickness + flHalfOutlineThickness, flThickness + flHalfOutlineThickness };

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect(vecMin + vecThicknessOffset, vecMax - vecThicknessOffset, colOutlinePacked, flRounding, roundingCorners, flOutlineThickness);

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect(vecMin - vecThicknessOffset, vecMax + vecThicknessOffset, colOutlinePacked, flRounding, roundingCorners, flOutlineThickness);
}

void D::AddDrawListRectMultiColor(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colUpperLeft, const Color_t& colUpperRight, const Color_t& colBottomRight, const Color_t& colBottomLeft)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	pDrawList->AddRectFilledMultiColor(vecMin, vecMax, colUpperLeft.GetU32(), colUpperRight.GetU32(), colBottomRight.GetU32(), colBottomLeft.GetU32());
}

void D::AddDrawListCircle(ImDrawList* pDrawList, const ImVec2& vecCenter, const float flRadius, const Color_t& colCircle, const int nSegments, const unsigned int uFlags, const Color_t& colOutline, float flThickness, const float flOutlineThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colCirclePacked = colCircle.GetU32();

	if (uFlags & DRAW_CIRCLE_FILLED)
	{
		pDrawList->AddCircleFilled(vecCenter, flRadius, colCirclePacked, nSegments);
		flThickness = 0.0f;
	}
	else
		pDrawList->AddCircle(vecCenter, flRadius, colCirclePacked, nSegments, flThickness);

	if (uFlags & DRAW_CIRCLE_OUTLINE)
		pDrawList->AddCircle(vecCenter, flRadius + flOutlineThickness, colOutline.GetU32(), nSegments, flThickness + flOutlineThickness);
}

void D::AddDrawListArc(ImDrawList* pDrawList, const ImVec2& vecPosition, const float flRadius, const float flMinimumAngle, const float flMaximumAngle, const Color_t& colArc, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	pDrawList->PathArcTo(vecPosition, flRadius, M_DEG2RAD(flMinimumAngle), M_DEG2RAD(flMaximumAngle), 32);
	pDrawList->PathStroke(colArc.GetU32(), false, flThickness);
}

void D::AddDrawListLine(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const Color_t& colLine, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	pDrawList->AddLine(vecFirst, vecSecond, colLine.GetU32(), flThickness);
}

void D::AddDrawListTriangle(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color_t& colTriangle, const unsigned int uFlags, const Color_t& colOutline, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colTrianglePacked = colTriangle.GetU32();

	if (uFlags & DRAW_TRIANGLE_FILLED)
		pDrawList->AddTriangleFilled(vecFirst, vecSecond, vecThird, colTrianglePacked);
	else
		pDrawList->AddTriangle(vecFirst, vecSecond, vecThird, colTrianglePacked, flThickness);

	if (uFlags & DRAW_TRIANGLE_OUTLINE)
		pDrawList->AddTriangle(vecFirst, vecSecond, vecThird, colOutline.GetU32(), flThickness + 1.0f);
}

void D::AddDrawListQuad(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const ImVec2& vecFourth, const Color_t& colQuad, const unsigned int uFlags, const Color_t& colOutline, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colQuadPacked = colQuad.GetU32();

	if (uFlags & DRAW_QUAD_FILLED)
		pDrawList->AddQuadFilled(vecFirst, vecSecond, vecThird, vecFourth, colQuadPacked);
	else
		pDrawList->AddQuad(vecFirst, vecSecond, vecThird, vecFourth, colQuadPacked, flThickness);

	if (uFlags & DRAW_QUAD_OUTLINE)
		pDrawList->AddQuad(vecFirst, vecSecond, vecThird, vecFourth, colOutline.GetU32(), flThickness + 1.0f);
}

void D::AddDrawListPolygon(ImDrawList* pDrawList, const ImVec2* vecPoints, const int nPointsCount, const Color_t& colPolygon, unsigned int uFlags, const Color_t& colOutline, const bool bClosed, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colPolygonPacked = colPolygon.GetU32();

	if (uFlags & DRAW_POLYGON_FILLED)
		pDrawList->AddConvexPolyFilled(vecPoints, nPointsCount, colPolygonPacked);
	else
		pDrawList->AddPolyline(vecPoints, nPointsCount, colPolygonPacked, bClosed, flThickness);

	if (uFlags & DRAW_POLYGON_OUTLINE)
		pDrawList->AddPolyline(vecPoints, nPointsCount, colOutline.GetU32(), bClosed, flThickness + 1.0f);
}

void D::AddDrawListText(ImDrawList* pDrawList, const ImFont* pFont, const float flFontSize, const ImVec2& vecPosition, const char* szText, const Color_t& colText, const unsigned int uFlags, const Color_t& colOutline, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	// set font texture
	pDrawList->PushTextureID(pFont->ContainerAtlas->TexID);

	const ImU32 colOutlinePacked = colOutline.GetU32();

	if (uFlags & DRAW_TEXT_DROPSHADOW)
		pDrawList->AddText(pFont, flFontSize, vecPosition + ImVec2(flThickness, -flThickness), colOutlinePacked, szText);
	else if (uFlags & DRAW_TEXT_OUTLINE)
	{
		pDrawList->AddText(pFont, flFontSize, vecPosition + ImVec2(flThickness, -flThickness), colOutlinePacked, szText);
		pDrawList->AddText(pFont, flFontSize, vecPosition + ImVec2(-flThickness, flThickness), colOutlinePacked, szText);
	}

	pDrawList->AddText(pFont, flFontSize, vecPosition, colText.GetU32(), szText);
	pDrawList->PopTextureID();
}
#pragma endregion
