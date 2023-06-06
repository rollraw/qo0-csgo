#pragma once
// used: ccsgamerulesproxy, ccsplayerresource
#include "../sdk/entity.h"

/*
 * ENTITY LISTENER
 * - manager to handle our callbacks and collect data when a certain entity is created or destroyed
 */
namespace ENTITY
{
	struct BreakableObject_t
	{
		char szModelPath[MAX_PATH] = { };
		char szClassName[256] = { };
	};

	void Setup();
	void Destroy();

	/* @section: main */
	// parse potentially breakable entities from BSP data
	// @note: same could be done with bsp parser
	void ParseMapBreakableEntities();

	/* @section: values */
	// game rules proxy entity, should be always checked for null
	inline CCSGameRulesProxy* pCSGameRules = nullptr;
	// player resource entity, should be always checked for null
	inline CCSPlayerResource* pCSPlayerResource = nullptr;
	// breakable entities of the map that client doesn't handle as breakable
	inline std::vector<BreakableObject_t> vecBreakableEntities = { }; // @todo: stl
}
