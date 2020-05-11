#pragma once
// used: winapi includes
#include "../common.h"
// used: listener event function
#include "../sdk/interfaces/igameeventmanager.h"

class CResolver : public CSingleton<CResolver>
{
public:
	// Get
	void Run();
	/* check for hit */
	void Event(IGameEvent* pEvent);

	// Info
	/* angles, lby states, shots counters etc*/
private:
	// Main
	/* ur resolver */
};
