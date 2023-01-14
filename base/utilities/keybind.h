#pragma once
//used: string
#include <string>

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
	CKeyBind(std::string szName, int iKey = 0, EKeyStateType iState = KeyStateHold);

	// constructor for loading config
	CKeyBind(int iKey, EKeyStateType iState);

	 // key number
	int iKey = 0;
	// name of the key so that you can render it on keybind window etc...
	std::string szName = "";
	// type of state
	EKeyStateType iState = KeyStateHold;

	// checking if the key is on
	bool IsActive();

	void DrawKeyBind();
private:
	// converting iState to const char*
	std::string StateToString( );

	// private this so we can check it on IsActive function
	bool bActive = false;
};