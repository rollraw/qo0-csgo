#pragma once
// used: [stl] size_t
#include <cstddef>
// used: [d3d] idirect3ddevice9
#include <d3d9.h>

#include "../common.h"

class CTab
{
public:
	const char* szName = nullptr;
	void (*pRenderFunction)() = nullptr;
};

/*
 * MENU
 */
namespace MENU
{
	/* @section: callbacks */
	void OnEndScene(IDirect3DDevice9* pDevice);

	/* @section: main */
	// main cheat window
	void MainWindow(IDirect3DDevice9* pDevice);

	/* @section: values */
	// open state of main window
	inline bool bMainOpened = false;
	// current selected tab in main window
	inline int iMainTab = 0;
}

/*
 * TABS
 */
namespace T
{
	/* @section: get */
	void Render(const char* szTabBar, const CTab* arrTabs, const std::size_t nTabsCount, int* nCurrentTab, const ImVec4& colActive, ImGuiTabBarFlags flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip);

	/* @section: tabs */
	void RageBot();
	void LegitBot();
	void Visuals();
	void Miscellaneous();
	void Scripts();

	/* @section: values */
	// current selected sub-tab in visuals tab
	inline int iEspTab = 0;
	// user-defined configuration filename in miscellaneous tab
	inline char szConfigFile[MAX_PATH] = { };
	// current selected configuration in miscellaneous tab
	inline std::size_t nSelectedConfig = ~1U;
	// current selected color setting in miscellaneous tab
	inline std::size_t nSelectedColor = 0U;
	// user-defined configuration filename in miscellaneous tab
	inline char szScriptFile[MAX_PATH] = {};
	// current selected configuration in miscellaneous tab
	inline std::size_t nSelectedScript = ~1U;
}
