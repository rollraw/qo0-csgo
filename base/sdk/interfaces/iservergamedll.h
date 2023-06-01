#pragma once
// used: callvfunc
#include "../../utilities/memory.h"

#pragma pack(push, 4)
// @source: master/public/server_class.h
class CServerClass
{
public:
	char* szNetworkName; // 0x00
	void* pTable; // 0x04
	CServerClass* pNext; // 0x08
	int nClassID; // 0x0C
	int nInstanceBaselineIndex; // 0x10
};
static_assert(sizeof(CServerClass) == 0x14);
#pragma pack(pop)

// @source: master/public/eiface.h
class IServerGameDLL : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	// give the list of datatable classes to the engine. the engine matches class names from here with 'edict_t::classname' to figure out how to encode a class's data for networking
	CServerClass* GetAllClasses()
	{
		return CallVFunc<CServerClass*, 11U>(this);
	}

	// let the game module allocate it's own network/shared string tables
	void CreateNetworkStringTables()
	{
		CallVFunc<void, 13U>(this);
	}
};
