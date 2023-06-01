#pragma once
// used: igameeventlistener2
#include "../sdk/interfaces/igameeventmanager.h"

/*
 * EVENT LISTENER
 * - manager to handle our callbacks when certain game events occur
 *   @note: all events list: https://wiki.alliedmods.net/Counter-Strike:_Global_Offensive_Events
 */
namespace EVENT
{
	bool Setup();
	void Destroy();

	/* @section: main */
	/// start listen event with given name
	/// @returns: true if callback has been successfully added to the listener or were listened before, false otherwise
	[[nodiscard]] bool AddToListener(const char* szEventName);
	// stop listen event with given name
	void RemoveFromListener(const char* szEventName);
	/// prioritize our listener callbacks processing for given event over game callbacks
	/// @returns: true if callback has been successfully moved to topmost position or were already topmost, false otherwise
	bool PrioritizeCallback(const char* szEventName);

	/* @section: get */
	/// @returns: pointer to our custom event listener
	[[nodiscard]] IGameEventListener2* GetListener();
}
