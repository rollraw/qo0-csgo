#include "draw.h"

// used: engine interface
#include "../core/interfaces.h"
// used: current window
#include "inputsystem.h"

/* font resources */
#include "../../resources/whitney.h"
#include "../../resources/smallest_pixel.h"
#include "../../resources/qo0icons.h"

#pragma region imgui_extended
/*
 * what is changed in imgui framework files (currently used imgui version is v1.75):
 * changed things can be found with "modified by qo0" commentary
 *
 1	changed type of imgui dx stateblock creation to fix in-game artefacts and create additional shader to compensate it
 *
 2	removed unused GetStyleColorName function
 *
 3	changed sliderscale grab lenght to range from frame start pos to current value pos
 *
 4	modified combo, slider, inputtext bounding boxes and text position
 *	also pushed frame padding to decrease frame size on menu
 *  ocornut should add global scaling later and this will be obsolete (i've seen that in somewhat issue)
 *
 5	added triangle direction switch when popup is open for combo and change arrow size scale
 *
 6	added and used ImGuiCol_ControlBg, ImGuiCol_ControlBgHovered, ImGuiCol_ControlBgActive color entries for next control's widgets: Checkbox, BeginCombo, SliderScalar, InputTextEx
 *	added and used ImGuiCol_Triangle color entry for combo etc triangle and make changes in RenderArrow function
 *
 7	changed cursor color to (140, 40, 225, 100)
 *
 8	now used freetype font rasterizier instead stb truetype to make small fonts clear and readable (https://github.com/ocornut/imgui/tree/master/misc/freetype)
 *
 9	removed default imgui font (proggyclean) and GetDefaultCompressedFontDataTTFBase85 function cos we dont need it, then using windows tahoma.ttf
 *
 10	used inputtext() wrappers for c++ standard library (stl) type: std::string (https://github.com/ocornut/imgui/tree/master/misc/cpp)
 *
 */

constexpr std::array<const char*, 166U> arrKeyNames =
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
	TextDisabled(XorStr("(?)"));
	if (IsItemHovered())
	{
		BeginTooltip();
		PushTextWrapPos(450.f);
		TextUnformatted(szDescription);
		PopTextWrapPos();
		EndTooltip();
	}
}

bool ImGui::ListBox(const char* szLabel, int* iCurrentItem, std::function<const char* (int)> pLambda, int nItemsCount, int iHeightInItems)
{
	return ListBox(szLabel, iCurrentItem, [](void* pData, int nIndex, const char** szOutText)
		{
			*szOutText = (*(std::function<const char* (int)>*)pData)(nIndex);
			return true;
		}, &pLambda, nItemsCount, iHeightInItems);
}

bool ImGui::HotKey(const char* szLabel, int* v)
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

	const ImRect frame_bb(pWindow->DC.CursorPos + ImVec2(vecLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + GetFrameHeight() : 0.0f, 0.0f), pWindow->DC.CursorPos + ImVec2(flWidth, vecLabelSize.x > 0.0f ? vecLabelSize.y + style.FramePadding.y : 0.f));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max);

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, nIndex, &frame_bb))
		return false;

	const bool bHovered = ItemHoverable(frame_bb, nIndex);
	if (bHovered)
	{
		SetHoveredID(nIndex);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool bFocusRequested = FocusableItemRegister(pWindow, nIndex);
	const bool bClicked = bHovered && io.MouseClicked[0];
	const bool bDoubleClicked = bHovered && g.IO.MouseDoubleClicked[0];
	if (bFocusRequested || bClicked || bDoubleClicked)
	{
		if (g.ActiveId != nIndex)
		{
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			memset(io.KeysDown, 0, sizeof(io.KeysDown));
			*v = 0;
		}

		SetActiveID(nIndex, pWindow);
		FocusWindow(pWindow);
	}

	bool bPressed = false;
	if (int nKey = *v; g.ActiveId == nIndex)
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

				bPressed = true;
				ClearActiveID();
			}
		}

		if (!bPressed)
		{
			for (int n = VK_BACK; n <= VK_RMENU; n++)
			{
				if (IsKeyDown(n))
				{
					nKey = n;
					bPressed = true;
					ClearActiveID();
				}
			}
		}
		
		if (IsKeyPressed(io.KeyMap[ImGuiKey_Escape]))
		{
			*v = 0;
			ClearActiveID();
		}
		else
			*v = nKey;
	}

	char chBuffer[64];
	XorCompileTime::w_sprintf_s(chBuffer, sizeof(chBuffer), XorStr("[ %s ]"), *v != 0 && g.ActiveId != nIndex ? arrKeyNames.at(*v) : g.ActiveId == nIndex ? XorStr("press") : XorStr("none"));
	
	// modified by qo0
	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
	pWindow->DrawList->AddText(ImVec2(frame_bb.Max.x - CalcTextSize(chBuffer).x, total_bb.Min.y + style.FramePadding.y), GetColorU32(g.ActiveId == nIndex ? ImGuiCol_Text : ImGuiCol_TextDisabled), chBuffer);

	if (vecLabelSize.x > 0.f)
		RenderText(ImVec2(total_bb.Min.x, total_bb.Min.y + style.FramePadding.y), szLabel);

	PopStyleVar();
	return bPressed;
}

bool ImGui::MultiCombo(const char* szLabel, const char** szDisplayName, std::vector<bool>& v, int nHeightInItems)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* pWindow = g.CurrentWindow;
	if (pWindow->SkipItems)
		return false;

	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	std::string szBuffer;
	const ImVec2 vecLabelSize = CalcTextSize(szLabel);
	float flActiveWidth = CalcItemWidth() - (style.ItemInnerSpacing.x + GetFrameHeight()) - 40.f;
	
	for (int i = 0; i < nHeightInItems; i++)
	{
		if (v[i])
		{
			ImVec2 vecTextSize = CalcTextSize(szBuffer.c_str());

			if (szBuffer.empty())
				szBuffer.assign(szDisplayName[i]);
			else
				szBuffer.append(", ").append(szDisplayName[i]);

			if (vecTextSize.x > flActiveWidth)
				szBuffer.erase(szBuffer.find_last_of(",")).append("...");
		}
	}

	if (szBuffer.empty())
		szBuffer.assign("-");

	bool bValueChanged = false;
	if (BeginCombo(szLabel, szBuffer.c_str()))
	{
		for (int i = 0; i < nHeightInItems; i++)
		{
			if (Selectable(szDisplayName[i], v[i], ImGuiSelectableFlags_DontClosePopups))
			{
				v[i] = !v[i];
				bValueChanged = true;
			}
		}

		EndCombo();
	}

	return bValueChanged;
}

bool ImGui::ColorEdit3(const char* szLabel, Color* v, ImGuiColorEditFlags flags)
{
	return ColorEdit4(szLabel, v, flags);
}

bool ImGui::ColorEdit4(const char* szLabel, Color* v, ImGuiColorEditFlags flags)
{
	ImVec4 vecColor = ImVec4{ v->rBase(), v->gBase(), v->bBase(), v->aBase() };

	if (ImGui::ColorEdit4(szLabel, &vecColor.x, flags))
	{
		v->Set(vecColor.x, vecColor.y, vecColor.z, vecColor.w);
		return true;
	}

	return false;
}

void ImGui::AddText(ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const char* szText, ImU32 colText, bool bOutline, ImU32 colOutline)
{
	if (pFont->ContainerAtlas == nullptr)
		return;

	pDrawList->PushTextureID(pFont->ContainerAtlas->TexID);

	if (bOutline)
	{
		pDrawList->AddText(pFont, flFontSize, ImVec2(vecPosition.x + 1.0f, vecPosition.y + 1.0f), colOutline, szText);
		pDrawList->AddText(pFont, flFontSize, ImVec2(vecPosition.x + 1.0f, vecPosition.y - 1.0f), colOutline, szText);
	}

	pDrawList->AddText(pFont, flFontSize, vecPosition, colText, szText);
	pDrawList->PopTextureID();
}

void ImGui::AddText(ImDrawList* pDrawList, const ImVec2& vecPosition, const char* szText, ImU32 colText, bool bOutline, ImU32 colOutline)
{
	AddText(pDrawList, nullptr, 0.f, vecPosition, szText, colText, colOutline);
}
#pragma endregion

#pragma region draw_get
void D::Setup(IDirect3DDevice9* pDevice, unsigned int uFontFlags)
{
	ImGui::CreateContext();

	// setup platform and renderer bindings
	ImGui_ImplWin32_Init(IPT::hWindow);
	ImGui_ImplDX9_Init(pDevice);

	// setup styles
	#pragma region draw_style
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

	#pragma region draw_style_color
	/*
	 * color navigation:
	 * [definition N][purpose]	[color] [rgb]
	 1 primitive:
	 -	primtv 0 (text) - white: 255,255,255
	 -	primtv 1 (background) - darkslategrey: 20,20,30
	 -	primtv 2 (disabled) - silver: 192,192,192
	 -	primtv 3 (control bg) - cadetblue: 30,35,50
	 -	primtv 4 (border) - black: 0,0,0
	 -	primtv 5 (hover) - matterhorn: 75,75,75
	 *
	 2 accents:
	 -	accent 0 (main) - blueviolet: 140,40,225
	 *	accent 1 (dark) - indigo: 55,0,100
	 *	accent 3 (darker) - darkviolet: 75,50,105
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
	io.Fonts->AddFontDefault();

	ImFontConfig imWhitneyConfig;
	imWhitneyConfig.RasterizerFlags = ImGuiFreeType::ForceAutoHint;
	F::Whitney = io.Fonts->AddFontFromMemoryCompressedTTF(whitney_compressed_data, whitney_compressed_size, 13.f, &imWhitneyConfig, io.Fonts->GetGlyphRangesDefault());

	ImFontConfig imVerdanaConfig;
	imVerdanaConfig.RasterizerFlags = ImGuiFreeType::Bold;
	F::Verdana = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\Verdana.ttf"), 14.f, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());

	ImFontConfig imSmallestPixelConfig;
	imSmallestPixelConfig.RasterizerFlags = ImGuiFreeType::LightHinting;
	F::SmallestPixel = io.Fonts->AddFontFromMemoryCompressedTTF(smallest_pixel_compressed_data, smallest_pixel_compressed_size, 40.f, &imSmallestPixelConfig, io.Fonts->GetGlyphRangesCyrillic());

	ImFontConfig imIconsConfig;
	imIconsConfig.RasterizerFlags = ImGuiFreeType::LightHinting;
	const ImWchar wIconRanges[] = { 0xE000, 0xF000, 0 };
	F::Icons = io.Fonts->AddFontFromMemoryCompressedTTF(qo0icons_compressed_data, qo0icons_compressed_size, 40.f, &imIconsConfig, wIconRanges);

	if (ImGuiFreeType::BuildFontAtlas(io.Fonts, uFontFlags))
		bInitialized = true;
	else
		bInitialized = false;
}

void D::Destroy()
{
	// shutdown imgui directx9 renderer binding
	ImGui_ImplDX9_Shutdown();

	// shutdown imgui win32 platform binding
	ImGui_ImplWin32_Shutdown();

	// destroy imgui context
	ImGui::DestroyContext();
}
#pragma endregion

#pragma region draw_extra
bool D::WorldToScreen(const Vector& vecOrigin, Vector2D& vecScreen)
{
	static std::uintptr_t uViewMatrixPtr = 0U;

	// get crender::viewmatrix once
	if (uViewMatrixPtr == 0U)
	{
		uViewMatrixPtr = (MEM::FindPattern(CLIENT_DLL, XorStr("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")));
		uViewMatrixPtr = *(std::uintptr_t*)(uViewMatrixPtr + 0x3) + 0xB0;
	}

	const ViewMatrix_t& matWorldToScreen = *(ViewMatrix_t*)uViewMatrixPtr;
	const float flWidth = matWorldToScreen[3][0] * vecOrigin.x + matWorldToScreen[3][1] * vecOrigin.y + matWorldToScreen[3][2] * vecOrigin.z + matWorldToScreen[3][3];

	if (flWidth > 0.01f)
	{
		// compute the scene coordinates of a point in 3d
		const float flInverse = 1.f / flWidth;
		vecScreen.x = (matWorldToScreen[0][0] * vecOrigin.x + matWorldToScreen[0][1] * vecOrigin.y + matWorldToScreen[0][2] * vecOrigin.z + matWorldToScreen[0][3]) * flInverse;
		vecScreen.y = (matWorldToScreen[1][0] * vecOrigin.x + matWorldToScreen[1][1] * vecOrigin.y + matWorldToScreen[1][2] * vecOrigin.z + matWorldToScreen[1][3]) * flInverse;

		// screen transform
		// get the screen position in pixels of given point
		const ImVec2 vecDisplaySize = ImGui::GetIO().DisplaySize;
		vecScreen.x = (vecDisplaySize.x * 0.5f) + (vecScreen.x * vecDisplaySize.x) * 0.5f;
		vecScreen.y = (vecDisplaySize.y * 0.5f) - (vecScreen.y * vecDisplaySize.y) * 0.5f;
		return true;
	}

	return false;
}
#pragma endregion
