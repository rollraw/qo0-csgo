#pragma once
// used: winapi includes
#include "../common.h"
// used: event's hashing
#include "../sdk/hash/fnv1a.h"
// used: listener event function
#include "../sdk/interfaces/igameeventmanager.h"

class CResolver : public CSingleton<CResolver>
{
public:
	// Get
	void Run();
	/* check for hit */
	void Event(IGameEvent* pEvent, const FNV1A_t uNameHash);

	// Info
	/* angles, lby states, shots counters etc*/
private:
	// Main
	/* ur resolver */
};
