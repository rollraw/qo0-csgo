#pragma once
// used: cgameeventlistener
#include "igameeventmanager.h"

// @source:
// master/game/client/hud.h
// master/game/client/hud.cpp

// @todo:

#pragma pack(push, 4)
class CHudElement : public CGameEventListener, ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	virtual ~CHudElement();

	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 12U>(this);
	}
};

class CPanoramaHudElement : public CHudElement
{

};

class CBaseHudChat : public CHudElement
{
public:
	template <typename... Args_t>
	void Printf(int iFilter, const char* szFormat, Args_t... argList)
	{
		// @note: don't use 'CallVFunc()' due to variadic args forwarding
		MEM::GetVFunc<void(Q_CDECL*)(CBaseHudChat*, int, const char*, ...)>(this, 26U)(this, iFilter, szFormat, argList...);
	}
};

class CBaseHudWeaponSelection : public CHudElement
{

};

class IHud
{
public:
	/// @returns: class of hud element with given name
	CHudElement* FindElement(const char* szName)
	{
		static auto fnFindElement = reinterpret_cast<CHudElement*(Q_THISCALL*)(IHud*, const char*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"))); // @xref: "[%d] Could not find Hud Element: %s\n"
		return fnFindElement(this, szName);
	}

public:
	int nKeyBits;
	float flMouseSensitivity;
	float flMouseSensitivityFactor;
	float flFOVSensitivityAdjust;
	Color_t colNormal;
	Color_t colCaution;
	Color_t colYellowish;
	CUtlVector<CHudElement*> vecHudList;
	CUtlVector<IPanel*> vecHudPanelList;
};
#pragma pack(pop)
