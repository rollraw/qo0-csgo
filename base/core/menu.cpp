#include "menu.h"

// used: global variables
#include "../global.h"
// used: config variables
#include "../core/variables.h"
// used: actions with config
#include "../core/config.h"
// used: configurations error logging
#include "../utilities/logging.h"
// used: render setup, etc
#include "../utilities/draw.h"
// used: engine, inputsystem, convar interfaces
#include "../core/interfaces.h"
// used: render visuals
#include "../features/visuals.h"
// used: skinchanger tab items map
#include "../features/skinchanger.h"
// used: inputtext() wrappers for c++ standard library (stl) type: std::string
#include "../../dependencies/imgui/cpp/imgui_stdlib.h"

#pragma region menu_arrays
const std::pair<const char*, std::uint32_t> arrColors[] =
{
	{ "[esp] box - enemies", Vars.colEspMainBoxEnemies },
	{ "[esp] box - enemies wall", Vars.colEspMainBoxEnemiesWall },
	{ "[esp] box - allies", Vars.colEspMainBoxAllies },
	{ "[esp] box - allies wall", Vars.colEspMainBoxAlliesWall },
	{ "[glow] enemies", Vars.colEspGlowEnemies },
	{ "[glow] enemies wall", Vars.colEspGlowEnemiesWall },
	{ "[glow] allies", Vars.colEspGlowAllies },
	{ "[glow] allies wall", Vars.colEspGlowAlliesWall },
	{ "[glow] weapons", Vars.colEspGlowWeapons },
	{ "[glow] grenades", Vars.colEspGlowGrenades },
	{ "[glow] bomb", Vars.colEspGlowBomb },
	{ "[glow] planted bomb", Vars.colEspGlowBombPlanted },
	{ "[chams] enemies", Vars.colEspChamsEnemies },
	{ "[chams] enemies wall", Vars.colEspChamsEnemiesWall },
	{ "[chams] allies", Vars.colEspChamsAllies },
	{ "[chams] allies wall", Vars.colEspChamsAlliesWall },
	{ "[chams] viewmodel", Vars.colEspChamsViewModel },
	{ "[chams] viewmodel additional", Vars.colEspChamsViewModelAdditional },
	{ "[screen] hitmarker - lines", Vars.colScreenHitMarker },
	{ "[screen] hitmarker - damage", Vars.colScreenHitMarkerDamage }
};

const char* arrVisualsFlags[] =
{
	"helmet",
	"kevlar",
	"defuse kit",
	"zoom"
};

const char* arrVisualsRemovals[] =
{
	"post-processing",
	"punch",
	"smoke",
	"scope"
};
#pragma endregion

// spectator list, radar, other stuff here
#pragma region menu_windows
void W::MainWindow(IDirect3DDevice9* pDevice)
{
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 vecScreenSize = io.DisplaySize;

	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* pForegroundDrawList = ImGui::GetForegroundDrawList();

	#pragma region main_visuals
	if (!I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage())
	{
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 0.03f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.03f));

		// hmm, another one watermark
		ImGui::BeginMainMenuBar();
		{
			ImGui::PushFont(F::Verdana);
			ImGui::Dummy(ImVec2(1, 1));

			#ifdef _DEBUG
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), XorStr("debug"));
			#endif

			if (strstr(GetCommandLine(), XorStr("-insecure")) != nullptr)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), XorStr("insecure"));

			if (I::Engine->IsInGame())
				ImGui::TextColored(G::bSendPacket ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), XorStr("send packets"));

			const char* const szName = XorStr("qo0 base | " __DATE__);
			static ImVec2 vecNameSize = ImGui::CalcTextSize(szName);
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - vecNameSize.x);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), szName);

			ImGui::PopFont();
			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleColor(2);
	}

	ImDrawList* pBackgroundDrawList = ImGui::GetBackgroundDrawList();
	D::RenderDrawData(pBackgroundDrawList);
	#pragma endregion

	#pragma region main_window
	ImGui::PushFont(F::Whitney);
	io.MouseDrawCursor = bMainOpened;

	if (bMainOpened)
	{
		int x, y;
		I::InputSystem->GetCursorPosition(&x, &y);

		// set imgui mouse position
		io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(y));

		ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(500, 327), ImGuiCond_Always);
		ImGui::Begin(XorStr("qo0 base"), &bMainOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
		{
			ImVec2 vecPos = ImGui::GetCursorScreenPos();
			float flWindowWidth = ImGui::GetWindowWidth();

			// header separate line
			pForegroundDrawList->AddRectFilledMultiColor(ImVec2(vecPos.x - 8.f, vecPos.y - 6.f), ImVec2(vecPos.x + flWindowWidth - flWindowWidth / 3.f - 8.f, vecPos.y - 8.f), IM_COL32(75, 50, 105, 255), IM_COL32(110, 100, 130, 255), IM_COL32(110, 100, 130, 255), IM_COL32(75, 50, 105, 255));
			pForegroundDrawList->AddRectFilledMultiColor(ImVec2(vecPos.x + flWindowWidth - flWindowWidth / 3.f - 8.f, vecPos.y - 6.f), ImVec2(vecPos.x + flWindowWidth - 8.f, vecPos.y - 8.f), IM_COL32(110, 100, 130, 255), IM_COL32(75, 50, 105, 255), IM_COL32(75, 50, 105, 255), IM_COL32(110, 100, 130, 255));

			// add tabs
			static std::array<CTab, 4U> const arrTabs =
			{
				CTab{ "rage", &T::RageBot },
				CTab{ "legit", &T::LegitBot },
				CTab{ "visuals", &T::Visuals },
				CTab{ "miscellaneous", &T::Miscellaneous }
				//CTab{ "skinchanger", &T::SkinChanger }
			};

			T::Render<arrTabs.size()>(XorStr("main_tabs"), arrTabs, &iMainTab, style.Colors[ImGuiCol_TabActive]);

			ImGui::End();
		}
	}

	ImGui::PopFont();
	#pragma endregion
}
#pragma endregion

#pragma region menu_tabs
template <std::size_t S>
void T::Render(const char* szTabBar, const std::array<CTab, S> arrTabs, int* nCurrentTab, const ImVec4& colActive, ImGuiTabBarFlags flags)
{
	// is empty check
	if (arrTabs.empty())
		return;

	// set active tab color
	ImGui::PushStyleColor(ImGuiCol_TabActive, colActive);
	if (ImGui::BeginTabBar(szTabBar, flags))
	{
		for (std::size_t i = 0U; i < arrTabs.size(); i++)
		{
			// add tab
			if (ImGui::BeginTabItem(arrTabs.at(i).szName))
			{
				// set current tab index
				*nCurrentTab = i;
				ImGui::EndTabItem();
			}
		}

		// render inner tab
		if (arrTabs.at(*nCurrentTab).pRenderFunction != nullptr)
			arrTabs.at(*nCurrentTab).pRenderFunction();

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
		ImGui::BeginChild(XorStr("ragebot.aimbot"), ImVec2(), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::Selectable(XorStr("aimbot##ragebot"), &C::Get<bool>(Vars.bRage), ImGuiSelectableFlags_None, ImVec2(30, 0));
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));

			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XorStr("ragebot.antiaim"), ImVec2(), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::Selectable(XorStr("anti-aim"), &C::Get<bool>(Vars.bAntiAim), ImGuiSelectableFlags_None, ImVec2(40, 0));
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Combo(XorStr("pitch"), &C::Get<int>(Vars.iAntiAimPitch), XorStr("none\0up\0down\0zero (untrusted)\0\0"));
			ImGui::Combo(XorStr("yaw"), &C::Get<int>(Vars.iAntiAimYaw), XorStr("none\0desync\0\0"));

			if (C::Get<int>(Vars.iAntiAimYaw) == (int)EAntiAimYawType::DESYNC)
				ImGui::HotKey(XorStr("desync switch"), &C::Get<int>(Vars.iAntiAimDesyncKey));
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
		ImGui::BeginChild(XorStr("legitbot.aimbot"), ImVec2(), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::Selectable(XorStr("aimbot##legitbot"), &C::Get<bool>(Vars.bLegit), ImGuiSelectableFlags_None, ImVec2(30, 0));
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));

			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XorStr("legitbot.triggerbot"), ImVec2(), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::Selectable(XorStr("triggerbot##master"), &C::Get<bool>(Vars.bTrigger), ImGuiSelectableFlags_None, ImVec2(45, 0));
				ImGui::PopStyleVar();

				if (ImGui::BeginMenu(XorStr("filters")))
				{
					ImGui::MenuItem(XorStr("head"), nullptr, &C::Get<bool>(Vars.bTriggerHead));
					ImGui::MenuItem(XorStr("chest"), nullptr, &C::Get<bool>(Vars.bTriggerChest));
					ImGui::MenuItem(XorStr("stomach"), nullptr, &C::Get<bool>(Vars.bTriggerStomach));
					ImGui::MenuItem(XorStr("arms"), nullptr, &C::Get<bool>(Vars.bTriggerArms));
					ImGui::MenuItem(XorStr("legs"), nullptr, &C::Get<bool>(Vars.bTriggerLegs));
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::HotKey(XorStr("activation key"), &C::Get<int>(Vars.iTriggerKey));
			ImGui::Separator();

			ImGui::SliderInt(XorStr("reaction delay##trigger"), &C::Get<int>(Vars.iTriggerDelay), 0, 500, "%dms");
			ImGui::Checkbox(XorStr("auto wall##trigger"), &C::Get<bool>(Vars.bTriggerAutoWall));
			ImGui::SliderInt(XorStr("minimal damage##trigger"), &C::Get<int>(Vars.iTriggerMinimalDamage), 1, 100, "%dhp");
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
		ImGui::BeginChild(XorStr("visuals.esp"), ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::Selectable(XorStr("esp##master"), &C::Get<bool>(Vars.bEsp), ImGuiSelectableFlags_None, ImVec2(30, 0));
				ImGui::PopStyleVar();

				if (ImGui::BeginMenu(XorStr("filters")))
				{
					switch (iEspTab)
					{
					case 0:
					{
						ImGui::MenuItem(XorStr("enemies"), nullptr, &C::Get<bool>(Vars.bEspMainEnemies));
						ImGui::MenuItem(XorStr("allies"), nullptr, &C::Get<bool>(Vars.bEspMainAllies));
						ImGui::MenuItem(XorStr("weapons"), nullptr, &C::Get<bool>(Vars.bEspMainWeapons));
						ImGui::MenuItem(XorStr("grenades"), nullptr, &C::Get<bool>(Vars.bEspMainGrenades));
						ImGui::MenuItem(XorStr("bomb"), nullptr, &C::Get<bool>(Vars.bEspMainBomb));
						break;
					}
					case 1:
					{
						ImGui::MenuItem(XorStr("enemies"), nullptr, &C::Get<bool>(Vars.bEspGlowEnemies));
						ImGui::MenuItem(XorStr("allies"), nullptr, &C::Get<bool>(Vars.bEspGlowAllies));
						ImGui::MenuItem(XorStr("weapons"), nullptr, &C::Get<bool>(Vars.bEspGlowWeapons));
						ImGui::MenuItem(XorStr("grenades"), nullptr, &C::Get<bool>(Vars.bEspGlowGrenades));
						ImGui::MenuItem(XorStr("bomb"), nullptr, &C::Get<bool>(Vars.bEspGlowBomb));
						break;
					}
					case 2:
					{
						ImGui::MenuItem(XorStr("enemies"), nullptr, &C::Get<bool>(Vars.bEspChamsEnemies));
						ImGui::MenuItem(XorStr("allies"), nullptr, &C::Get<bool>(Vars.bEspChamsAllies));
						ImGui::MenuItem(XorStr("viewmodel"), nullptr, &C::Get<bool>(Vars.bEspChamsViewModel));
						break;
					}
					default:
						break;
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			static std::array<CTab, 3U> const arrEspTabs =
			{
				CTab{ "main", [&style]()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("enable##main"), &C::Get<bool>(Vars.bEspMain));

				if (C::Get<bool>(Vars.bEspMainEnemies) || C::Get<bool>(Vars.bEspMainAllies))
				{
					ImGui::Separator();
					ImGui::Combo(XorStr("player box"), &C::Get<int>(Vars.iEspMainPlayerBox), XorStr("none\0full\0corners\0\0"));
					ImGui::Checkbox(XorStr("far radar"), &C::Get<bool>(Vars.bEspMainPlayerFarRadar));

					ImGui::Checkbox(XorStr("players info"), &C::Get<bool>(Vars.bEspMainPlayerInfo));
					if (C::Get<bool>(Vars.bEspMainPlayerInfo))
					{
						ImGui::Checkbox(XorStr("health##player"), &C::Get<bool>(Vars.bEspMainPlayerHealth));
						ImGui::Checkbox(XorStr("money##player"), &C::Get<bool>(Vars.bEspMainPlayerMoney));
						//ImGui::Checkbox(XorStr("rank##player"), &C::Get<bool>(Vars.bEspMainPlayerRank));
						ImGui::Checkbox(XorStr("name##player"), &C::Get<bool>(Vars.bEspMainPlayerName));
						ImGui::Checkbox(XorStr("flash##player"), &C::Get<bool>(Vars.bEspMainPlayerFlash));
						ImGui::MultiCombo(XorStr("flags##player"), arrVisualsFlags, C::Get<std::vector<bool>>(Vars.vecEspMainPlayerFlags), IM_ARRAYSIZE(arrVisualsFlags));
						ImGui::Checkbox(XorStr("weapons##player"), &C::Get<bool>(Vars.bEspMainPlayerWeapons));
						ImGui::Checkbox(XorStr("ammo##player"), &C::Get<bool>(Vars.bEspMainPlayerAmmo));
						ImGui::Checkbox(XorStr("distance##player"), &C::Get<bool>(Vars.bEspMainPlayerDistance));
					}
				}

				if (C::Get<bool>(Vars.bEspMainWeapons))
				{
					ImGui::Separator();
					ImGui::Combo(XorStr("weapon box"), &C::Get<int>(Vars.iEspMainWeaponBox), XorStr("none\0full\0corners\0\0"));

					ImGui::Checkbox(XorStr("weapons info"), &C::Get<bool>(Vars.bEspMainWeaponInfo));
					if (C::Get<bool>(Vars.bEspMainWeaponInfo))
					{
						ImGui::Checkbox(XorStr("icon##weapon"), &C::Get<bool>(Vars.bEspMainWeaponIcon));
						ImGui::Checkbox(XorStr("ammo##weapon"), &C::Get<bool>(Vars.bEspMainWeaponAmmo));
						ImGui::Checkbox(XorStr("distance##weapon"), &C::Get<bool>(Vars.bEspMainWeaponDistance));
					}
				}

				ImGui::PopStyleVar();
			}},
				CTab{ "glow", [&style]()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("enable##glow"), &C::Get<bool>(Vars.bEspGlow));
				ImGui::Checkbox(XorStr("bloom"), &C::Get<bool>(Vars.bEspGlowBloom));

				// @note: if u rebuild glow and wanna use styles do like that
				//ImGui::Combo(XorStr("styles example##glow"), XorStr("outer\0rim\0edge\0edge pulse\0\0"));

				ImGui::PopStyleVar();
			}},
				CTab{ "chams", [&style]()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("enable##chams"), &C::Get<bool>(Vars.bEspChams));

				if (C::Get<bool>(Vars.bEspChamsEnemies) || C::Get<bool>(Vars.bEspChamsAllies))
				{
					ImGui::Combo(XorStr("players style##chams"), &C::Get<int>(Vars.iEspChamsPlayer), XorStr("covered\0flat\0wireframe\0reflective\0\0"));
					ImGui::Checkbox(XorStr("xqz"), &C::Get<bool>(Vars.bEspChamsXQZ));
					ImGui::Checkbox(XorStr("disable occlusion"), &C::Get<bool>(Vars.bEspChamsDisableOcclusion));
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XorStr("may cause \"invisible\" players models sometimes"));
				}

				if (C::Get<bool>(Vars.bEspChamsViewModel))
				{
					ImGui::Combo(XorStr("viewmodel style##chams"), &C::Get<int>(Vars.iEspChamsViewModel), XorStr("no draw\0covered\0flat\0wireframe\0glow\0scroll\0chrome\0\0"));
				}

				ImGui::PopStyleVar();
			}}
			};

			T::Render<arrEspTabs.size()>(XorStr("visuals_esp"), arrEspTabs, &iEspTab, style.Colors[ImGuiCol_ScrollbarGrab]);

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		static float flWorldChildSize = 0.f;
		ImGui::BeginChild(XorStr("visuals.world"), ImVec2(0, flWorldChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::Selectable(XorStr("world##master"), &C::Get<bool>(Vars.bWorld), ImGuiSelectableFlags_None, ImVec2(30, 0));
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("night mode"), &C::Get<bool>(Vars.bWorldNightMode));
			ImGui::SliderInt(XorStr("max flash effect"), &C::Get<int>(Vars.iWorldMaxFlash), 0, 100, "%d%%");
			ImGui::MultiCombo(XorStr("removals"), arrVisualsRemovals, C::Get<std::vector<bool>>(Vars.vecWorldRemovals), IM_ARRAYSIZE(arrVisualsRemovals));
			ImGui::Separator();

			ImGui::HotKey(XorStr("thirdperson"), &C::Get<int>(Vars.iWorldThirdPersonKey));
			ImGui::SliderFloat(XorStr("camera offset"), &C::Get<float>(Vars.flWorldThirdPersonOffset), 50.f, 300.f, "%.1f units");
			ImGui::PopStyleVar();

			flWorldChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}

		ImGui::BeginChild(XorStr("visuals.screen"), ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::Selectable(XorStr("screen##master"), &C::Get<bool>(Vars.bScreen), ImGuiSelectableFlags_None, ImVec2(30, 0));
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::SliderFloat(XorStr("camera fov"), &C::Get<float>(Vars.flScreenCameraFOV), -89.f, 89.f, u8"%.1f\u00B0");
			ImGui::SliderFloat(XorStr("viewmodel fov"), &C::Get<float>(Vars.flScreenViewModelFOV), -90.f, 90.f, u8"%.1f\u00B0");
			ImGui::Separator();

			ImGui::Checkbox(XorStr("hitmarker"), &C::Get<bool>(Vars.bScreenHitMarker));
			ImGui::Checkbox(XorStr("damage"), &C::Get<bool>(Vars.bScreenHitMarkerDamage));
			ImGui::Checkbox(XorStr("sound"), &C::Get<bool>(Vars.bScreenHitMarkerSound));
			ImGui::SliderFloat(XorStr("time"), &C::Get<float>(Vars.flScreenHitMarkerTime), 0.5f, 5.f, "%.1fsec");
			ImGui::SliderInt(XorStr("gap"), &C::Get<int>(Vars.iScreenHitMarkerGap), 1, 20, "%d pixels");
			ImGui::SliderInt(XorStr("lenght"), &C::Get<int>(Vars.iScreenHitMarkerLenght), 1, 20, "%d pixels");
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
		ImGui::BeginChild(XorStr("misc.movement"), ImVec2(0, flMovementChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(XorStr("movement"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("bunny hop"), &C::Get<bool>(Vars.bMiscBunnyHop));
			ImGui::SliderInt(XorStr("chance"), &C::Get<int>(Vars.iMiscBunnyHopChance), 0, 100, "%d%%");
			ImGui::Checkbox(XorStr("autostrafe"), &C::Get<bool>(Vars.bMiscAutoStrafe));
			ImGui::Separator();

			ImGui::Checkbox(XorStr("fake lag"), &C::Get<bool>(Vars.bMiscFakeLag));
			ImGui::Checkbox(XorStr("auto accept"), &C::Get<bool>(Vars.bMiscAutoAccept));
			ImGui::Checkbox(XorStr("auto pistol"), &C::Get<bool>(Vars.bMiscAutoPistol));
			ImGui::Checkbox(XorStr("no crouch cooldown"), &C::Get<bool>(Vars.bMiscNoCrouchCooldown));
			ImGui::PopStyleVar();

			flMovementChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}

		ImGui::BeginChild(XorStr("misc.exploits"), ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(XorStr("exploits"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("ping spike"), &C::Get<bool>(Vars.bMiscPingSpike));
			ImGui::SliderFloat(XorStr("latency factor"), &C::Get<float>(Vars.flMiscLatencyFactor), 0.1f, 1.0f, "%.1f second");
			ImGui::Checkbox(XorStr("reveal ranks"), &C::Get<bool>(Vars.bMiscRevealRanks));
			ImGui::Checkbox(XorStr("unlock inventory"), &C::Get<bool>(Vars.bMiscUnlockInventory));
			ImGui::Checkbox(XorStr("anti-untrusted"), &C::Get<bool>(Vars.bMiscAntiUntrusted));
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		// current selected configuration name
		static std::string szCurrentConfig = { };

		static float flConfigChildSize = 0.f;
		ImGui::BeginChild(XorStr("misc.config"), ImVec2(0, flConfigChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(XorStr("configuration"));
				ImGui::EndMenuBar();
			}

			ImGui::Columns(2, XorStr("#CONFIG"), false);
			{
				ImGui::PushItemWidth(-1);

				ImGui::ListBox(XorStr("##config.list"), &iSelectedConfig, [](int nIndex)
					{
						// return current displaying configuration name
						return C::vecFileNames.at(nIndex).c_str();
					}, C::vecFileNames.size(), 5);

				szCurrentConfig = !C::vecFileNames.empty() ? C::vecFileNames.at(iSelectedConfig) : "";
				ImGui::PopItemWidth();
			}
			ImGui::NextColumn();
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::PushItemWidth(-1);
				if (ImGui::InputTextWithHint(XorStr("##config.file"), XorStr("create new..."), &szConfigFile, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (!C::Save(szConfigFile))
					{
						L::PushConsoleColor(FOREGROUND_RED);
						L::Print(fmt::format(XorStr("[error] failed to create \"{}\" config"), szConfigFile));
						L::PopConsoleColor();
					}

					szConfigFile.clear();
					C::Refresh();
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("press enter to create new configuration"));

				if (ImGui::Button(XorStr("save"), ImVec2(-1, 15)))
				{
					if (!C::Save(szCurrentConfig))
					{
						L::PushConsoleColor(FOREGROUND_RED);
						L::Print(fmt::format(XorStr("[error] failed to save \"{}\" config"), szCurrentConfig));
						L::PopConsoleColor();
					}
				}

				if (ImGui::Button(XorStr("load"), ImVec2(-1, 15)))
				{
					if (!C::Load(szCurrentConfig))
					{
						L::PushConsoleColor(FOREGROUND_RED);
						L::Print(fmt::format(XorStr("[error] failed to load \"{}\" config"), szCurrentConfig));
						L::PopConsoleColor();
					}
				}

				if (ImGui::Button(XorStr("remove"), ImVec2(-1, 15)))
					ImGui::OpenPopup(XorStr("confirmation##config.remove"));

				if (ImGui::Button(XorStr("refresh"), ImVec2(-1, 15)))
					C::Refresh();

				ImGui::PopItemWidth();
				ImGui::PopStyleVar();
			}
			ImGui::Columns(1);

			if (ImGui::BeginPopupModal(XorStr("confirmation##config.remove"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(XorStr("are you sure you want to remove \"%s\" configuration?"), szCurrentConfig.c_str());
				ImGui::Spacing();

				if (ImGui::Button(XorStr("no"), ImVec2(30, 0)))
					ImGui::CloseCurrentPopup();

				ImGui::SameLine();

				if (ImGui::Button(XorStr("yes"), ImVec2(30, 0)))
				{
					C::Remove(szCurrentConfig);
					// reset current configuration
					iSelectedConfig = 0;
					C::Refresh();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			flConfigChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}

		ImGui::BeginChild(XorStr("misc.colors"), ImVec2(), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(XorStr("colors"));
				ImGui::EndMenuBar();
			}

			const char* szColorNames[IM_ARRAYSIZE(arrColors)];
			for (int i = 0; i < IM_ARRAYSIZE(arrColors); i++)
				szColorNames[i] = arrColors[i].first;

			ImGui::Spacing();
			ImGui::PushItemWidth(-1);

			ImGui::ListBox(XorStr("##colors.select"), &iSelectedColor, szColorNames, IM_ARRAYSIZE(szColorNames), 4);
			ImGui::ColorEdit4(XorStr("##colors.picker"), &C::Get<Color>(arrColors[iSelectedColor].second), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
			ImGui::PopItemWidth();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::SkinChanger()
{
	ImGui::BeginChild(XorStr("skins"), ImVec2(), true);
	{
		for (const auto& item : mapItemList) //first - itemdefindex, second - skin item struct
		{

		}

		ImGui::EndChild();
	}
}
#pragma endregion
