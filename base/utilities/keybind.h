#pragma once

// state type
enum EKeyStateType : unsigned
{
	KeyStateHold = 0,
	KeyStateToggle,
	KeyStateAlwaysOn
};

class CKeyBind
{
public:
	// constructor for adding variable to config
	CKeyBind(const char* szName, int iKey = 0, EKeyStateType iState = KeyStateHold);

	// constructor for loading config
	CKeyBind(int iKey, EKeyStateType iState);

	 // key number
	int iKey = 0;
	// name of the key so that you can render it on keybind window etc...
	const char* szName = "";
	// type of state
	EKeyStateType iState = KeyStateHold;

	// checking if the key is on
	bool IsActive();
private:

	// private this so we can check it on IsActive function
	bool bActive = false;
};