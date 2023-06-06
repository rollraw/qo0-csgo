// used: [ext] imgui
#include "../../dependencies/imgui/imgui.h"

#include "menu.h"

// used: config variables
#include "../core/variables.h"
// used: stringstring
#include "../utilities/crt.h"
// used: setup, fonts
#include "../utilities/draw.h"
// used: last send packet state
#include "../features.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/iengineclient.h"

// used: [ext] imgui
#include "../../dependencies/imgui/dx9/imgui_impl_dx9.h"
#include "../../dependencies/imgui/win32/imgui_impl_win32.h"

// @todo: untouched, wait for new gui merge

static const std::pair<const char*, std::size_t> arrColors[ ] =
{
	{ "[esp] box - enemies", Vars.colVisualOverlayBoxEnemies },
	{ "[esp] box - enemies hidden", Vars.colVisualOverlayBoxEnemiesHidden},
	{ "[esp] box - allies", Vars.colVisualOverlayBoxAllies },
	{ "[esp] box - allies hidden", Vars.colVisualOverlayBoxAlliesHidden },
	{ "[esp] box - local", Vars.colVisualOverlayBoxLocal },
	{ "[esp] box - local hidden", Vars.colVisualOverlayBoxLocalHidden },
	{ "[glow] enemies", Vars.colVisualGlowEnemies },
	{ "[glow] enemies hidden", Vars.colVisualGlowEnemiesHidden },
	{ "[glow] allies", Vars.colVisualGlowAllies },
	{ "[glow] allies hidden", Vars.colVisualGlowAlliesHidden },
	{ "[glow] local", Vars.colVisualGlowLocal },
	{ "[glow] local hidden", Vars.colVisualGlowLocalHidden },
	{ "[glow] weapons", Vars.colVisualGlowWeapons },
	{ "[glow] grenades", Vars.colVisualGlowGrenades },
	{ "[glow] bomb", Vars.colVisualGlowBomb },
	{ "[glow] planted bomb", Vars.colVisualGlowBombPlanted },
	{ "[chams] enemies", Vars.colVisualChamsEnemies },
	{ "[chams] enemies hidden", Vars.colVisualChamsEnemiesHidden },
	{ "[chams] allies", Vars.colVisualChamsAllies },
	{ "[chams] allies hidden", Vars.colVisualChamsAlliesHidden },
	{ "[chams] local", Vars.colVisualChamsLocal },
	{ "[chams] local hidden", Vars.colVisualChamsLocalHidden },
	{ "[chams] local desync", Vars.colVisualChamsLocalDesync },
	{ "[chams] local desync hidden", Vars.colVisualChamsLocalDesyncHidden },
	{ "[chams] viewmodel", Vars.colVisualChamsViewModel },
	{ "[chams] viewmodel hidden", Vars.colVisualChamsViewModelHidden },
	{ "[screen] hitmarker - lines", Vars.colVisualScreenHitMarker },
	{ "[screen] hitmarker - damage", Vars.colVisualScreenHitMarkerDamage }
};

#pragma region menu_array_entries
static constexpr const char* arrVisualsFlags[ ] =
{
	"helmet",
	"kevlar",
	"defuse kit",
	"zoom"
};

static constexpr const char* arrVisualsRemovals[ ] =
{
	"post-processing",
	"punch",
	"flash",
	"smoke",
	"scope"
};
#pragma endregion

#pragma region menu_callbacks
void MENU::OnEndScene(IDirect3DDevice9* pDevice)
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	// render cheat menu
	MainWindow(pDevice);

	ImGui::EndFrame();

	ImGui::Render();

	// render thread-safe data
	D::RenderDrawData(ImGui::GetDrawData());

	// render draw lists from draw data
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
#pragma endregion

#pragma region menu_main
void MENU::MainWindow(IDirect3DDevice9* pDevice)
{
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 vecScreenSize = io.DisplaySize;

	ImGuiStyle& style = ImGui::GetStyle();

	#pragma region main_header
	if (!I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage())
	{
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 0.03f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.03f));

		// hmm, another one watermark
		ImGui::BeginMainMenuBar();
		{
			ImGui::PushFont(FONT::pExtra);
			ImGui::Dummy(ImVec2(1, 1));

		#ifdef _DEBUG
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), Q_XOR("debug"));
		#endif

			if (CRT::StringString(GetCommandLineW(), Q_XOR(L"-insecure")) != nullptr)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), Q_XOR("insecure"));

			if (I::Engine->IsInGame())
				ImGui::TextColored(F::bLastSendPacket ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), Q_XOR("send packets"));

			const char* const szName = Q_XOR("qo0 base | " __DATE__);
			static ImVec2 vecNameSize = ImGui::CalcTextSize(szName);
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - vecNameSize.x);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), szName);

			ImGui::PopFont();
			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleColor(2);
	}
	#pragma endregion

	#pragma region main_window
	io.MouseDrawCursor = bMainOpened;

	if (bMainOpened)
	{
		ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(500, 327), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(500, 327), ImVec2(1000, 327*2));
		ImGui::Begin(Q_XOR("qo0 base"), &bMainOpened, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
		{
			const ImVec2 vecPosition = ImGui::GetCursorScreenPos();
			const float flWindowWidth = ImGui::GetWindowWidth();
			ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();

			// push clip so separator will be drawn at maximal window bounds
			ImGui::PushClipRect(ImVec2(vecPosition.x - 8.f, vecPosition.y - 8.f), ImVec2(vecPosition.x + flWindowWidth - 8.f, vecPosition.y - 6.f), false);

			// header separate line
			pWindowDrawList->AddRectFilledMultiColor(ImVec2(vecPosition.x - 8.f, vecPosition.y - 6.f), ImVec2(vecPosition.x + flWindowWidth - flWindowWidth / 3.f - 8.f, vecPosition.y - 8.f), IM_COL32(75, 50, 105, 255), IM_COL32(110, 100, 130, 255), IM_COL32(110, 100, 130, 255), IM_COL32(75, 50, 105, 255));
			pWindowDrawList->AddRectFilledMultiColor(ImVec2(vecPosition.x + flWindowWidth - flWindowWidth / 3.f - 8.f, vecPosition.y - 6.f), ImVec2(vecPosition.x + flWindowWidth - 8.f, vecPosition.y - 8.f), IM_COL32(110, 100, 130, 255), IM_COL32(75, 50, 105, 255), IM_COL32(75, 50, 105, 255), IM_COL32(110, 100, 130, 255));

			// restore cliprect
			ImGui::PopClipRect();

			// add tabs
			static const CTab arrTabs[ ] =
			{
				CTab{ "rage", &T::RageBot },
				CTab{ "legit", &T::LegitBot },
				CTab{ "visuals", &T::Visuals },
				CTab{ "miscellaneous", &T::Miscellaneous }
			};

			T::Render(Q_XOR("main_tabs"), arrTabs, 4U, &iMainTab, style.Colors[ImGuiCol_TabActive]);

			ImGui::End();
		}
	}
	#pragma endregion
}
#pragma endregion

#pragma region menu_tabs
void T::Render(const char* szTabBar, const CTab* arrTabs, const std::size_t nTabsCount, int* nCurrentTab, const ImVec4& colActive, ImGuiTabBarFlags flags)
{
	// set active tab color
	ImGui::PushStyleColor(ImGuiCol_TabActive, colActive);
	if (ImGui::BeginTabBar(szTabBar, flags))
	{
		for (std::size_t i = 0U; i < nTabsCount; i++)
		{
			// add tab
			if (ImGui::BeginTabItem(arrTabs[i].szName))
			{
				// set current tab index
				*nCurrentTab = i;
				ImGui::EndTabItem();
			}
		}

		// render inner tab
		if (arrTabs[*nCurrentTab].pRenderFunction != nullptr)
			arrTabs[*nCurrentTab].pRenderFunction();

		ImGui::EndTabBar();
	}
	ImGui::PopStyleColor();
}
#pragma endregion

#pragma region menu_tabs_main
void T::RageBot()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(
		Q_XOR("ragebot.aimbot"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(Q_XOR("aimbot##master"), C::Get<bool>(Vars.bRage)))
					ImGui::EndMenu();
				if (ImGui::IsItemClicked())
					C::Get<bool>(Vars.bRage) ^= true;

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(Q_XOR("ragebot.antiaim"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(Q_XOR("anti-aim##master"), C::Get<bool>(Vars.bAntiAim)))
					ImGui::EndMenu();
				if (ImGui::IsItemClicked())
					C::Get<bool>(Vars.bAntiAim) ^= true;

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));
			ImGui::Checkbox(Q_XOR("pitch"), &C::Get<bool>(Vars.bAntiAimPitch));
			if (C::Get<bool>(Vars.bAntiAimPitch))
				ImGui::SliderInt(Q_XOR("angle"), &C::Get<int>(Vars.iAntiAimPitch), -89, 89, "%d\xC2\xB0");
			ImGui::Checkbox(Q_XOR("yaw real"), &C::Get<bool>(Vars.bAntiAimYawReal));
			if (C::Get<bool>(Vars.bAntiAimYawReal))
				ImGui::SliderInt(Q_XOR("offset"), &C::Get<int>(Vars.iAntiAimYawRealOffset), -180, 180, "%d\xC2\xB0");

			ImGui::Checkbox(Q_XOR("yaw fake"), &C::Get<bool>(Vars.bAntiAimYawFake));
			if (C::Get<bool>(Vars.bAntiAimYawFake))
				ImGui::HotKey(Q_XOR("inverter"), &C::Get<KeyBind_t>(Vars.keyAntiAimYawFakeInverter));
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::LegitBot()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(Q_XOR("legitbot.aimbot"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(Q_XOR("aimbot##master"), C::Get<bool>(Vars.bLegit)))
					ImGui::EndMenu();
				if (ImGui::IsItemClicked())
					C::Get<bool>(Vars.bLegit) ^= true;

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(Q_XOR("legitbot.triggerbot"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(Q_XOR("triggerbot##master"), C::Get<bool>(Vars.bTrigger)))
					ImGui::EndMenu();
				if (ImGui::IsItemClicked())
					C::Get<bool>(Vars.bTrigger) ^= true;

				if (ImGui::BeginMenu(Q_XOR("filters")))
				{
					ImGui::MenuItem(Q_XOR("head"), nullptr, &C::Get<bool>(Vars.bTriggerHead));
					ImGui::MenuItem(Q_XOR("chest"), nullptr, &C::Get<bool>(Vars.bTriggerChest));
					ImGui::MenuItem(Q_XOR("stomach"), nullptr, &C::Get<bool>(Vars.bTriggerStomach));
					ImGui::MenuItem(Q_XOR("arms"), nullptr, &C::Get<bool>(Vars.bTriggerArms));
					ImGui::MenuItem(Q_XOR("legs"), nullptr, &C::Get<bool>(Vars.bTriggerLegs));
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));
			ImGui::HotKey(Q_XOR("activation key"), &C::Get<KeyBind_t>(Vars.keyTrigger));
			ImGui::Separator();

			ImGui::SliderInt(Q_XOR("reaction delay##trigger"), &C::Get<int>(Vars.iTriggerDelay), 0, 500, "%dms");
			ImGui::Checkbox(Q_XOR("auto wall##trigger"), &C::Get<bool>(Vars.bTriggerAutoWall));
			ImGui::SliderInt(Q_XOR("minimal damage##trigger"), &C::Get<int>(Vars.iTriggerMinimalDamage), 1, 100, "%dhp");
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::Visuals()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(Q_XOR("visuals.esp"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(Q_XOR("visual##master"), C::Get<bool>(Vars.bVisual)))
					ImGui::EndMenu();
				if (ImGui::IsItemClicked())
					C::Get<bool>(Vars.bVisual) ^= true;

				if (ImGui::BeginMenu(Q_XOR("filters")))
				{
					switch (iEspTab)
					{
					case 0:
					{
						ImGui::MenuItem(Q_XOR("enemies"), nullptr, &C::Get<bool>(Vars.bVisualOverlayEnemies));
						ImGui::MenuItem(Q_XOR("allies"), nullptr, &C::Get<bool>(Vars.bVisualOverlayAllies));
						ImGui::MenuItem(Q_XOR("local"), nullptr, &C::Get<bool>(Vars.bVisualOverlayLocal));
						ImGui::MenuItem(Q_XOR("weapons"), nullptr, &C::Get<bool>(Vars.bVisualOverlayWeapons));
						ImGui::MenuItem(Q_XOR("grenades"), nullptr, &C::Get<bool>(Vars.bVisualOverlayGrenades));
						ImGui::MenuItem(Q_XOR("bomb"), nullptr, &C::Get<bool>(Vars.bVisualOverlayBomb));
						break;
					}
					case 1:
					{
						ImGui::MenuItem(Q_XOR("enemies"), nullptr, &C::Get<bool>(Vars.bVisualGlowEnemies));
						ImGui::MenuItem(Q_XOR("allies"), nullptr, &C::Get<bool>(Vars.bVisualGlowAllies));
						ImGui::MenuItem(Q_XOR("local"), nullptr, &C::Get<bool>(Vars.bVisualGlowLocal));
						ImGui::MenuItem(Q_XOR("weapons"), nullptr, &C::Get<bool>(Vars.bVisualGlowWeapons));
						ImGui::MenuItem(Q_XOR("grenades"), nullptr, &C::Get<bool>(Vars.bVisualGlowGrenades));
						ImGui::MenuItem(Q_XOR("bomb"), nullptr, &C::Get<bool>(Vars.bVisualGlowBomb));
						break;
					}
					case 2:
					{
						ImGui::MenuItem(Q_XOR("enemies"), nullptr, &C::Get<bool>(Vars.bVisualChamsEnemies));
						ImGui::MenuItem(Q_XOR("allies"), nullptr, &C::Get<bool>(Vars.bVisualChamsAllies));
						ImGui::MenuItem(Q_XOR("local"), nullptr, &C::Get<bool>(Vars.bVisualChamsLocal));
						ImGui::MenuItem(Q_XOR("viewmodel"), nullptr, &C::Get<bool>(Vars.bVisualChamsViewModel));
						break;
					}
					default:
						break;
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			static const CTab arrEspTabs[ ] =
			{
				CTab{ "overlay", []()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, -1));
				ImGui::Checkbox(Q_XOR("enable##main"), &C::Get<bool>(Vars.bVisualOverlay));

				if (C::Get<bool>(Vars.bVisualOverlayEnemies) || C::Get<bool>(Vars.bVisualOverlayAllies))
				{
					ImGui::Separator();
					ImGui::Combo(Q_XOR("player box"), &C::Get<int>(Vars.iVisualOverlayPlayerBox), Q_XOR("none\0full\0corners\0\0"));

					ImGui::Checkbox(Q_XOR("players info"), &C::Get<bool>(Vars.bVisualOverlayPlayerInfo));
					if (C::Get<bool>(Vars.bVisualOverlayPlayerInfo))
					{
						ImGui::Checkbox(Q_XOR("health##player"), &C::Get<bool>(Vars.bVisualOverlayPlayerHealth));
						ImGui::Checkbox(Q_XOR("money##player"), &C::Get<bool>(Vars.bVisualOverlayPlayerMoney));
						ImGui::Checkbox(Q_XOR("name##player"), &C::Get<bool>(Vars.bVisualOverlayPlayerName));
						ImGui::Checkbox(Q_XOR("flash##player"), &C::Get<bool>(Vars.bVisualOverlayPlayerFlash));
						ImGui::MultiCombo(Q_XOR("flags##player"), &C::Get<unsigned int>(Vars.nVisualOverlayPlayerFlags), arrVisualsFlags, ARRAYSIZE(arrVisualsFlags));
						ImGui::Checkbox(Q_XOR("weapons##player"), &C::Get<bool>(Vars.bVisualOverlayPlayerWeapons));
						ImGui::Checkbox(Q_XOR("ammo##player"), &C::Get<bool>(Vars.bVisualOverlayPlayerAmmo));
						ImGui::Checkbox(Q_XOR("distance##player"), &C::Get<bool>(Vars.bVisualOverlayPlayerDistance));
					}
				}

				if (C::Get<bool>(Vars.bVisualOverlayWeapons))
				{
					ImGui::Separator();
					ImGui::Combo(Q_XOR("weapon box"), &C::Get<int>(Vars.iVisualOverlayWeaponBox), Q_XOR("none\0full\0corners\0\0"));

					ImGui::Checkbox(Q_XOR("weapons info"), &C::Get<bool>(Vars.bVisualOverlayWeaponInfo));
					if (C::Get<bool>(Vars.bVisualOverlayWeaponInfo))
					{
						ImGui::Checkbox(Q_XOR("icon##weapon"), &C::Get<bool>(Vars.bVisualOverlayWeaponIcon));
						ImGui::Checkbox(Q_XOR("ammo##weapon"), &C::Get<bool>(Vars.bVisualOverlayWeaponAmmo));
						ImGui::Checkbox(Q_XOR("distance##weapon"), &C::Get<bool>(Vars.bVisualOverlayWeaponDistance));
					}
				}

				ImGui::PopStyleVar();
			}},
				CTab{ "glow", []()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, -1));
				ImGui::Checkbox(Q_XOR("enable##glow"), &C::Get<bool>(Vars.bVisualGlow));
				ImGui::Checkbox(Q_XOR("bloom"), &C::Get<bool>(Vars.bVisualGlowBloom));

				// @note: if u rebuild glow and wanna use styles do like that
				//ImGui::Combo(Q_XOR("styles example##glow"), Q_XOR("outer\0rim\0edge\0edge pulse\0\0"));

				ImGui::PopStyleVar();
			}},
				CTab{ "chams", []()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, -1));
				ImGui::Checkbox(Q_XOR("enable##chams"), &C::Get<bool>(Vars.bVisualChams));

				if (C::Get<bool>(Vars.bVisualChamsEnemies))
				{
					ImGui::Combo(Q_XOR("enemies style##enemies"), &C::Get<int>(Vars.iVisualChamsEnemies), Q_XOR("none\0covered\0flat\0glow\0reflective\0scroll\0\0"));
					ImGui::Checkbox(Q_XOR("xqz##enemies"), &C::Get<bool>(Vars.bVisualChamsEnemiesXQZ));
					ImGui::Checkbox(Q_XOR("wireframe##enemies"), &C::Get<bool>(Vars.bVisualChamsEnemiesWireframe));
					ImGui::Separator();
				}

				if (C::Get<bool>(Vars.bVisualChamsAllies))
				{
					ImGui::Combo(Q_XOR("allies style##allies"), &C::Get<int>(Vars.iVisualChamsAllies), Q_XOR("none\0covered\0flat\0glow\0reflective\0scroll\0\0"));
					ImGui::Checkbox(Q_XOR("xqz##allies"), &C::Get<bool>(Vars.bVisualChamsAlliesXQZ));
					ImGui::Checkbox(Q_XOR("wireframe##allies"), &C::Get<bool>(Vars.bVisualChamsAlliesWireframe));
					ImGui::Separator();
				}

				if (C::Get<bool>(Vars.bVisualChamsLocal))
				{
					ImGui::Combo(Q_XOR("local style##local"), &C::Get<int>(Vars.iVisualChamsLocal), Q_XOR("none\0covered\0flat\0glow\0reflective\0scroll\0\0"));
					ImGui::Checkbox(Q_XOR("xqz##local"), &C::Get<bool>(Vars.bVisualChamsLocalXQZ));
					ImGui::Checkbox(Q_XOR("wireframe##local"), &C::Get<bool>(Vars.bVisualChamsLocalWireframe));
					ImGui::Combo(Q_XOR("desync style##local"), &C::Get<int>(Vars.iVisualChamsLocalDesync), Q_XOR("none\0covered\0flat\0glow\0reflective\0scroll\0\0"));
					ImGui::Checkbox(Q_XOR("desync xqz##local"), &C::Get<bool>(Vars.bVisualChamsLocalDesyncXQZ));
					ImGui::Checkbox(Q_XOR("desync wireframe##local"), &C::Get<bool>(Vars.bVisualChamsLocalDesyncWireframe));
					ImGui::Separator();
				}

				if (C::Get<bool>(Vars.bVisualChamsViewModel))
				{
					ImGui::Combo(Q_XOR("viewmodel style##viewmodel"), &C::Get<int>(Vars.iVisualChamsViewModel), Q_XOR("none\0covered\0flat\0glow\0reflective\0scroll\0\0"));
					ImGui::Checkbox(Q_XOR("xqz##viewmodel"), &C::Get<bool>(Vars.bVisualChamsViewModelXQZ));
					ImGui::Checkbox(Q_XOR("wireframe##viewmodel"), &C::Get<bool>(Vars.bVisualChamsViewModelWireframe));
				}

				ImGui::PopStyleVar();
			}}
			};

			Render(Q_XOR("visuals_esp"), arrEspTabs, 3U, &iEspTab, style.Colors[ImGuiCol_ScrollbarGrab]);

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		static float flWorldChildSize = 0.f;
		ImGui::BeginChild(Q_XOR("visuals.world"), ImVec2(0, flWorldChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(Q_XOR("world##master"), C::Get<bool>(Vars.bVisualWorld)))
					ImGui::EndMenu();
				if (ImGui::IsItemClicked())
					C::Get<bool>(Vars.bVisualWorld) ^= true;

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));
			ImGui::Checkbox(Q_XOR("night mode"), &C::Get<bool>(Vars.bVisualWorldNightMode));
			ImGui::MultiCombo(Q_XOR("removals"), &C::Get<unsigned int>(Vars.nVisualWorldRemovals), arrVisualsRemovals, ARRAYSIZE(arrVisualsRemovals));
			ImGui::Separator();

			ImGui::HotKey(Q_XOR("thirdperson"), &C::Get<KeyBind_t>(Vars.keyVisualWorldThirdPerson));
			ImGui::SliderFloat(Q_XOR("camera offset"), &C::Get<float>(Vars.flVisualWorldThirdPersonOffset), 50.f, 300.f, "%.1f units");
			ImGui::PopStyleVar();

			flWorldChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}

		ImGui::BeginChild(Q_XOR("visuals.screen"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(Q_XOR("screen##master"), C::Get<bool>(Vars.bVisualScreen)))
					ImGui::EndMenu();
				if (ImGui::IsItemClicked())
					C::Get<bool>(Vars.bVisualScreen) ^= true;

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));
			ImGui::SliderFloat(Q_XOR("camera fov"), &C::Get<float>(Vars.flVisualScreenCameraFOV), -89.f, 89.f, "%.1f\xC2\xB0");
			ImGui::SliderFloat(Q_XOR("viewmodel fov"), &C::Get<float>(Vars.flVisualScreenViewModelFOV), -90.f, 90.f, "%.1f\xC2\xB0");
			ImGui::Separator();

			ImGui::Checkbox(Q_XOR("hitmarker"), &C::Get<bool>(Vars.bVisualScreenHitMarker));
			ImGui::Checkbox(Q_XOR("damage"), &C::Get<bool>(Vars.bVisualScreenHitMarkerDamage));
			ImGui::Checkbox(Q_XOR("sound"), &C::Get<bool>(Vars.bVisualScreenHitMarkerSound));
			ImGui::SliderFloat(Q_XOR("time"), &C::Get<float>(Vars.flVisualScreenHitMarkerTime), 0.5f, 5.f, "%.1fsec");
			ImGui::SliderInt(Q_XOR("gap"), &C::Get<int>(Vars.iVisualScreenHitMarkerGap), 1, 20, "%d pixels");
			ImGui::SliderInt(Q_XOR("length"), &C::Get<int>(Vars.iVisualScreenHitMarkerLength), 1, 20, "%d pixels");
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::Miscellaneous()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, nullptr, false);
	{
		static float flMovementChildSize = 0.f;
		ImGui::BeginChild(Q_XOR("misc.movement"), ImVec2(0, flMovementChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(Q_XOR("movement"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));
			ImGui::Checkbox(Q_XOR("bunny hop"), &C::Get<bool>(Vars.bMiscBunnyHop));
			ImGui::SliderInt(Q_XOR("chance"), &C::Get<int>(Vars.iMiscBunnyHopChance), 0, 100, "%d%%");
			ImGui::Checkbox(Q_XOR("autostrafe"), &C::Get<bool>(Vars.bMiscAutoStrafe));
			ImGui::Separator();

			ImGui::Checkbox(Q_XOR("fake lag"), &C::Get<bool>(Vars.bMiscFakeLag));
			ImGui::SliderInt(Q_XOR("ticks"), &C::Get<int>(Vars.iMiscFakeLagTicks), 1, 17, "%d");
			ImGui::Checkbox(Q_XOR("auto accept"), &C::Get<bool>(Vars.bMiscAutoAccept));
			ImGui::Checkbox(Q_XOR("auto pistol"), &C::Get<bool>(Vars.bMiscAutoPistol));
			ImGui::Checkbox(Q_XOR("no crouch cooldown"), &C::Get<bool>(Vars.bMiscNoCrouchCooldown));
			ImGui::PopStyleVar();

			flMovementChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}

		ImGui::BeginChild(Q_XOR("misc.exploits"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(Q_XOR("exploits"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));
			ImGui::Checkbox(Q_XOR("fake latency"), &C::Get<bool>(Vars.bMiscFakeLatency));
			ImGui::SliderInt(Q_XOR("amount##fakelatency"), &C::Get<int>(Vars.iMiscFakeLatencyAmount), 1, 1000, "%dms");
			ImGui::Checkbox(Q_XOR("reveal ranks"), &C::Get<bool>(Vars.bMiscRevealRanks));
			ImGui::Checkbox(Q_XOR("unlock inventory"), &C::Get<bool>(Vars.bMiscUnlockInventory));
			ImGui::Checkbox(Q_XOR("anti-smac"), &C::Get<bool>(Vars.bMiscAntiSMAC));
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		static float flConfigChildSize = 0.f;
		ImGui::BeginChild(Q_XOR("misc.config"), ImVec2(0, flConfigChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(Q_XOR("configuration"));
				ImGui::EndMenuBar();
			}

			ImGui::Columns(2, Q_XOR("#CONFIG"), false);
			{
				ImGui::PushItemWidth(-1);

				if (ImGui::ListBoxHeader(Q_XOR("##config.list"), C::vecFileNames.size(), 5))
				{
					for (std::size_t i = 0U; i < C::vecFileNames.size(); i++)
					{
						// @todo: imgui cant work with wstring, wait for change to other gui
						const wchar_t* wszFileName = C::vecFileNames[i];

						char szFileName[MAX_PATH] = { };
						CRT::StringUnicodeToMultiByte(szFileName, sizeof(szFileName), wszFileName);

						if (ImGui::Selectable(szFileName, (nSelectedConfig == i)))
							nSelectedConfig = i;
					}

					ImGui::ListBoxFooter();
				}

				ImGui::PopItemWidth();
			}
			ImGui::NextColumn();
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));
				ImGui::PushItemWidth(-1);
				if (ImGui::InputTextWithHint(Q_XOR("##config.file"), Q_XOR("create new..."), szConfigFile, sizeof(szConfigFile), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					// @todo: imgui cant work with wstring, wait for change to other gui
					wchar_t wszConfigFile[MAX_PATH] = { };
					CRT::StringMultiByteToUnicode(wszConfigFile, MAX_PATH, szConfigFile, szConfigFile + sizeof(szConfigFile));

					C::CreateFile(wszConfigFile);

					// clear string
					CRT::MemorySet(szConfigFile, 0U, sizeof(szConfigFile));
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(Q_XOR("press enter to create new configuration"));

				if (ImGui::Button(Q_XOR("save"), ImVec2(-1, 15)))
					C::SaveFile(nSelectedConfig);

				if (ImGui::Button(Q_XOR("load"), ImVec2(-1, 15)))
					C::LoadFile(nSelectedConfig);

				if (ImGui::Button(Q_XOR("remove"), ImVec2(-1, 15)))
					ImGui::OpenPopup(Q_XOR("confirmation##config.remove"));

				if (ImGui::Button(Q_XOR("refresh"), ImVec2(-1, 15)))
					C::Refresh();

				ImGui::PopItemWidth();
				ImGui::PopStyleVar();
			}
			ImGui::Columns(1);

			if (ImGui::BeginPopupModal(Q_XOR("confirmation##config.remove"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
			{
				// @todo: imgui cant work with wstring, wait for change to other gui
				char szCurrentConfig[MAX_PATH] = { };
				CRT::StringUnicodeToMultiByte(szCurrentConfig, MAX_PATH, C::vecFileNames[nSelectedConfig]);

				ImGui::Text(Q_XOR("are you sure you want to remove \"%s\" configuration?"), szCurrentConfig);
				ImGui::Spacing();

				if (ImGui::Button(Q_XOR("no"), ImVec2(30, 0)))
					ImGui::CloseCurrentPopup();

				ImGui::SameLine();

				if (ImGui::Button(Q_XOR("yes"), ImVec2(30, 0)))
				{
					C::RemoveFile(nSelectedConfig);

					// reset current configuration
					nSelectedConfig = 0U;

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			flConfigChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}

		ImGui::BeginChild(Q_XOR("misc.colors"), ImVec2{ }, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(Q_XOR("colors"));
				ImGui::EndMenuBar();
			}

			ImGui::Spacing();
			ImGui::PushItemWidth(-1);
			
			const int nColorsCount = IM_ARRAYSIZE(arrColors);
			static ImGuiTextFilter filter;
			filter.Draw(Q_XOR("##colors.filter"));
			ImGui::ColorEdit4(Q_XOR("##colors.picker"), &C::Get<Color_t>(arrColors[iSelectedColor].second), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
			if (ImGui::ListBoxHeader(Q_XOR("##colors.select"), ImVec2(-1, ImGui::GetContentRegionAvail( ).y)))
			{
				for (std::size_t i = 0U; i < nColorsCount; i++)
				{
					if (filter.PassFilter(arrColors[i].first))
					{
						if (ImGui::Selectable(arrColors[i].first, (i == iSelectedColor)))
							iSelectedColor = i;
					}
				}

				ImGui::ListBoxFooter();
			}

			ImGui::PopItemWidth();
			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}
#pragma endregion
