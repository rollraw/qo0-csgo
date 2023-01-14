#include "keybind.h"

// used: IsKeyDown and IsKeyReleased
#include "inputsystem.h"

CKeyBind::CKeyBind(const char* szName, int iKey, EKeyStateType iState)
{
	this->szName = szName;
	this->iKey = iKey;
	this->iState = iState;
}

CKeyBind::CKeyBind(int iKey, EKeyStateType iState)
{
	this->iKey = iKey;
	this->iState = iState;
}

bool CKeyBind::IsActive()
{
	switch (this->iState)
	{
	case KeyStateHold:

		this->bActive = IPT::IsKeyDown(this->iKey);

		break;
	case KeyStateToggle:

		if (IPT::IsKeyReleased(this->iKey))
			this->bActive = !this->bActive;

		break;
	case KeyStateAlwaysOn:

		this->bActive = true;

		break;
	}

	return this->bActive;
}
