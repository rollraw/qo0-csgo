#include "menu.h"

// used: global variables
#include "../global.h"
// used: config variables
#include "../core/variables.h"
// used: actions with config
#include "../core/config.h"
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
	{ XorStr("[esp] box - enemies"), Vars.colEspMainBoxEnemies },
	{ XorStr("[esp] box - enemies wall"), Vars.colEspMainBoxEnemiesWall },
	{ XorStr("[esp] box - allies"), Vars.colEspMainBoxAllies },
	{ XorStr("[esp] box - allies wall"), Vars.colEspMainBoxAlliesWall },
	{ XorStr("[glow] enemies"), Vars.colEspGlowEnemies },
	{ XorStr("[glow] enemies wall"), Vars.colEspGlowEnemiesWall },
	{ XorStr("[glow] allies"), Vars.colEspGlowAllies },
	{ XorStr("[glow] allies wall"), Vars.colEspGlowAlliesWall },
	{ XorStr("[glow] weapons"), Vars.colEspGlowWeapons },
	{ XorStr("[glow] grenades"), Vars.colEspGlowGrenades },
	{ XorStr("[glow] bomb"), Vars.colEspGlowBomb },
	{ XorStr("[glow] planted bomb"), Vars.colEspGlowBombPlanted },
	{ XorStr("[chams] enemies"), Vars.colEspChamsEnemies },
	{ XorStr("[chams] enemies wall"), Vars.colEspChamsEnemiesWall },
	{ XorStr("[chams] allies"), Vars.colEspChamsAllies },
	{ XorStr("[chams] allies wall"), Vars.colEspChamsAlliesWall },
	{ XorStr("[chams] viewmodel"), Vars.colEspChamsViewModel },
	{ XorStr("[chams] viewmodel additional"), Vars.colEspChamsViewModelAdditional },
	{ XorStr("[screen] hitmarker - lines"), Vars.colScreenHitMarker },
	{ XorStr("[screen] hitmarker - damage"), Vars.colScreenHitMarkerDamage }
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
	ImDrawList* pDrawList = ImGui::GetForegroundDrawList();

	#pragma region main_visuals
	if (!I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage())
	{
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 0.03f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.03f));

		// hmm, another one watermark
		ImGui::BeginMainMenuBar();
		{
			ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();

			// our debug text or something other
			static const char* szInsecure = XorStr("insecure");
			static ImVec2 vecInsecureSize = F::SmallestPixel->CalcTextSizeA(16.f, FLT_MAX, 0.0f, szInsecure);

			if (strstr(GetCommandLine(), XorStr("-insecure")) != nullptr)
				ImGui::AddText(pWindowDrawList, F::SmallestPixel, 16.f, ImVec2(0, 0), szInsecure, IM_COL32(255, 255, 0, 255));

			static const char* szSendPackets = XorStr("send packets");
			static ImVec2 vecSendPacketsSize = F::SmallestPixel->CalcTextSizeA(16.f, FLT_MAX, 0.0f, szSendPackets);

			if (I::Engine->IsInGame())
				ImGui::AddText(pWindowDrawList, F::SmallestPixel, 16.f, ImVec2(vecInsecureSize.x + 10.f, 0), szSendPackets, G::bSendPacket ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255));

			static const char* szName = XorStr("qo0 base | " __DATE__);
			static ImVec2 vecNameSize = F::SmallestPixel->CalcTextSizeA(16.f, FLT_MAX, 0.0f, szName);
			ImGui::AddText(pWindowDrawList, F::SmallestPixel, 16.f, ImGui::GetWindowContentRegionMax() - ImVec2(vecNameSize.x, vecNameSize.y * 0.5f), szName, IM_COL32(255, 255, 255, 255));

			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleColor(2);
	}

	CVisuals::Get().Run(pDrawList, vecScreenSize);
	#pragma endregion

	#pragma region main_window
	ImGui::PushFont(F::Whitney);
	io.MouseDrawCursor = bMainOpened;

	if (bMainOpened)
	{
		int x, y;
		I::InputSystem->GetCursorPosition(&x, &y);

		// set imgui mouse position
		io.MousePos.x = (float)x;
		io.MousePos.y = (float)y;

		ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(500, 320), ImGuiCond_Always);
		ImGui::Begin(XorStr("qo0 base"), &bMainOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
		{
			ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
			ImVec2 vecPos = ImGui::GetCursorScreenPos();
			float flWindowWidth = ImGui::GetWindowWidth();

			// header separate line
			pDrawList->AddRectFilledMultiColor(ImVec2(vecPos.x - 8.f, vecPos.y - 6.f), ImVec2(vecPos.x + flWindowWidth - flWindowWidth / 3.f - 8.f, vecPos.y - 8.f), IM_COL32(75, 50, 105, 255), IM_COL32(110, 100, 130, 255), IM_COL32(110, 100, 130, 255), IM_COL32(75, 50, 105, 255));
			pDrawList->AddRectFilledMultiColor(ImVec2(vecPos.x + flWindowWidth - flWindowWidth / 3.f - 8.f, vecPos.y - 6.f), ImVec2(vecPos.x + flWindowWidth - 8.f, vecPos.y - 8.f), IM_COL32(110, 100, 130, 255), IM_COL32(75, 50, 105, 255), IM_COL32(75, 50, 105, 255), IM_COL32(110, 100, 130, 255));

			// add tabs
			static std::array<CTab, 4U> const arrTabs =
			{
				CTab{ XorStr("rage"), &T::RageBot },
				CTab{ XorStr("legit"), &T::LegitBot },
				CTab{ XorStr("visuals"), &T::Visuals },
				CTab{ XorStr("miscellaneous"), &T::Miscellaneous }
				//CTab{ XorStr("skinchanger"), &T::SkinChanger }
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
		for (auto i = 0U; i < arrTabs.size(); i++)
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
				CTab{ XorStr("main"), [&style]()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("enable##main"), &C::Get<bool>(Vars.bEspMain));

				if (C::Get<bool>(Vars.bEspMainEnemies) || C::Get<bool>(Vars.bEspMainAllies))
				{
					ImGui::Checkbox(XorStr("far radar"), &C::Get<bool>(Vars.bEspMainFarRadar));
					ImGui::Combo(XorStr("box"), &C::Get<int>(Vars.iEspMainBox), XorStr("none\0full\0corners\0\0"));
					ImGui::Separator();

					// @todo: very later after some changes in visual's rendering system move it to single modal window or popup and add preview
					ImGui::Checkbox(XorStr("info"), &C::Get<bool>(Vars.bEspMainInfo));
					if (C::Get<bool>(Vars.bEspMainInfo))
					{
						ImGui::Checkbox(XorStr("health"), &C::Get<bool>(Vars.bEspMainInfoHealth));
						ImGui::Checkbox(XorStr("money"), &C::Get<bool>(Vars.bEspMainInfoMoney));
						//ImGui::Checkbox(XorStr("rank"), &C::Get<bool>(Vars.bEspMainInfoRank));
						ImGui::Checkbox(XorStr("name"), &C::Get<bool>(Vars.bEspMainInfoName));
						ImGui::Checkbox(XorStr("flash"), &C::Get<bool>(Vars.bEspMainInfoFlash));
						ImGui::MultiCombo(XorStr("flags"), arrVisualsFlags, C::Get<std::vector<bool>>(Vars.vecEspMainInfoFlags), IM_ARRAYSIZE(arrVisualsFlags));
						ImGui::Checkbox(XorStr("weapons"), &C::Get<bool>(Vars.bEspMainInfoWeapons));
						ImGui::Checkbox(XorStr("ammo"), &C::Get<bool>(Vars.bEspMainInfoAmmo));
						ImGui::Checkbox(XorStr("distance"), &C::Get<bool>(Vars.bEspMainInfoDistance));
					}
				}

				ImGui::PopStyleVar();
			}},
				CTab{ XorStr("glow"), [&style]()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("enable##glow"), &C::Get<bool>(Vars.bEspGlow));
				ImGui::Checkbox(XorStr("bloom"), &C::Get<bool>(Vars.bEspGlowBloom));

				// @note: if u rebuild glow and wanna use styles do like that
				//ImGui::Combo(XorStr("styles example##glow"), XorStr("outer\0rim\0edge\0edge pulse\0\0"));

				ImGui::PopStyleVar();
			}},
				CTab{ XorStr("chams"), [&style]()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("enable##chams"), &C::Get<bool>(Vars.bEspChams));

				if (C::Get<bool>(Vars.bEspChamsEnemies) || C::Get<bool>(Vars.bEspChamsAllies))
				{
					ImGui::Checkbox(XorStr("xqz"), &C::Get<bool>(Vars.bEspChamsXQZ));

					if (ImGui::Combo(XorStr("players style##chams"), &C::Get<int>(Vars.iEspChamsPlayers), XorStr("covered\0flat\0wireframe\0reflective\0\0")))
						CVisuals::Get().bUpdatePlayersChams = true;
				}

				if (C::Get<bool>(Vars.bEspChamsViewModel))
				{
					if (ImGui::Combo(XorStr("viewmodel style##chams"), &C::Get<int>(Vars.iEspChamsViewModel), XorStr("no draw\0covered\0flat\0wireframe\0glow\0scroll\0\0")))
						CVisuals::Get().bUpdateViewModelChams = true;
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
		ImGui::BeginChild(XorStr("misc.movement"), ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(XorStr("movement"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("bunny hop"), &C::Get<bool>(Vars.bBunnyHop));
			ImGui::SliderInt(XorStr("chance"), &C::Get<int>(Vars.iBunnyHopChance), 0, 100, "%d%%");
			ImGui::Checkbox(XorStr("autostrafe"), &C::Get<bool>(Vars.bAutoStrafe));
			ImGui::Separator();

			ImGui::Checkbox(XorStr("fake lag"), &C::Get<bool>(Vars.bFakeLag));
			ImGui::Checkbox(XorStr("ping spike"), &C::Get<bool>(Vars.bPingSpike));
			ImGui::Checkbox(XorStr("auto pistol"), &C::Get<bool>(Vars.bAutoPistol));
			ImGui::Checkbox(XorStr("no crouch cooldown"), &C::Get<bool>(Vars.bNoCrouchCooldown));
			ImGui::Checkbox(XorStr("auto accept"), &C::Get<bool>(Vars.bAutoAccept));
			ImGui::Checkbox(XorStr("reveal ranks"), &C::Get<bool>(Vars.bRankReveal));
			ImGui::Checkbox(XorStr("unlock inventory"), &C::Get<bool>(Vars.bUnlockInventory));
			ImGui::Checkbox(XorStr("anti-untrusted"), &C::Get<bool>(Vars.bAntiUntrusted));
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		// current displaying configuration name
		static char szDisplayConfig[FILENAME_MAX] = { };
		// current selected configuration name
		static std::string szCurrentConfig;

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
						strcpy_s(szDisplayConfig, sizeof(szDisplayConfig), C::vecFileNames.at(nIndex).c_str());
						return szDisplayConfig;
					}, C::vecFileNames.size(), 5);

				szCurrentConfig = C::vecFileNames.at(iSelectedConfig);
				ImGui::PopItemWidth();
			}
			ImGui::NextColumn();
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::PushItemWidth(-1);
				ImGui::InputTextWithHint(XorStr("##config.file"), XorStr("enter filename..."), &szConfigFile);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("write file name to create new config\nor clear input to load choosed file"));

				if (!szConfigFile.empty())
				{
					if (ImGui::Button(XorStr("create"), ImVec2(-1, 15)))
					{
						C::Save(szConfigFile);
						szConfigFile.clear();
						C::Refresh();
					}
				}
				else
				{
					if (ImGui::Button(XorStr("save"), ImVec2(-1, 15)))
						C::Save(szCurrentConfig);
				}

				if (ImGui::Button(XorStr("load"), ImVec2(-1, 15)))
					C::Load(szCurrentConfig);

				if (ImGui::Button(XorStr("remove"), ImVec2(-1, 15)))
					ImGui::OpenPopup(XorStr("confirmation##config.remove"));

				if (ImGui::Button(XorStr("refresh"), ImVec2(-1, 15)))
					C::Refresh();

				ImGui::PopItemWidth();
				ImGui::PopStyleVar();
			}
			ImGui::Columns(1);

			if (ImGui::BeginPopupModal(XorStr("confirmation##config.remove"), false, ImGuiWindowFlags_NoResize))
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
			// @todo: resets hue if 255, 255, 255
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
