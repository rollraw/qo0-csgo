// used: std::ofstream
#include <fstream>

#include "utilities.h"
// used: logging
#include "utilities/logging.h"
// used: interfacereg class and convar, clients, globals, engine, trace, materialsystem, model/view render, modelinfo, clientstate interfaces
#include "core/interfaces.h"

#pragma region utilities_get
template <class C>
C* U::FindHudElement(const char* szName)
{
	using FindHudElementFn = std::uintptr_t(__thiscall*)(void*, const char*);
	static auto oFindHudElement = (FindHudElementFn)MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	static auto pThis = *(std::uintptr_t**)(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89")) + 0x1);

	return (C*)oFindHudElement(pThis, szName);
}
#pragma endregion

#pragma region utilities_game
CBaseEntity* U::GetLocalPlayer()
{
	return I::ClientEntityList->Get<CBaseEntity>(I::Engine->GetLocalPlayer());
}

void U::TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int fMask, IHandleEntity* pSkip, Trace_t* pTrace)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilterSkipEntity filter(pSkip);
	I::EngineTrace->TraceRay(ray, fMask, &filter, pTrace);
}

void U::ForceFullUpdate()
{
	// update hud weapon icon
	using ClearHudWeaponIconFn = int(__thiscall*)(void*, int);
	static auto oClearHudWeaponIcon = (ClearHudWeaponIconFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C"))); // @xref: https://www.unknowncheats.me/forum/counterstrike-global-offensive/342743-finding-sigging-chud-pointer-chud-findelement.html

	if (auto dwHudWeaponSelection = FindHudElement<std::uintptr_t*>(XorStr("CCSGO_HudWeaponSelection")); dwHudWeaponSelection != nullptr)
	{
		if (auto pHudWeapons = (int*)(dwHudWeaponSelection - 0xA0); pHudWeapons != nullptr && *pHudWeapons) // get by weapons count (pHudWeapons + 0x80)
		{
			for (int i = 0; i < *pHudWeapons; i++)
				i = oClearHudWeaponIcon(pHudWeapons, i);

			*pHudWeapons = 0;
		}
	}

	I::ClientState->iDeltaTick = -1;
}

bool U::LineGoesThroughSmoke(Vector vecStartPos, Vector vecEndPos)
{
	using LineGoesThroughSmokeFn = bool(__thiscall*)(Vector, Vector, std::int16_t);
	static auto oLineGoesThroughSmoke = (LineGoesThroughSmokeFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"))); // @xref: "effects/overlaysmoke"
	return oLineGoesThroughSmoke(vecStartPos, vecEndPos, 1);
}

void U::SetLocalPlayerReady()
{
	using SetLocalPlayerReadyFn = void(__stdcall*)(const char*);
	static auto oSetLocalPlayerReady = (SetLocalPlayerReadyFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"))); // @xref: "deffered"

	if (oSetLocalPlayerReady != nullptr)
		oSetLocalPlayerReady("");
}

void U::SendName(const char* szName)
{
	static CConVar* name = I::ConVar->FindVar(XorStr("name"));
	name->fnChangeCallbacks.Size() = NULL;

	if (name != nullptr)
		name->SetValue(szName);
}

void U::SendClanTag(const char* szClanTag, const char* szIdentifier)
{
	using SendClanTagFn = void(__fastcall*)(const char*, const char*);
	static auto oSendClanTag = (SendClanTagFn)(MEM::FindPattern(ENGINE_DLL, XorStr("53 56 57 8B DA 8B F9 FF 15"))); // @xref: "ClanTagChanged"

	if (oSendClanTag != nullptr)
		oSendClanTag(szClanTag, szIdentifier);
}

int U::GetChokedTicks(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	float flSimulationTime = pEntity->GetSimulationTime();
	float flDifference = pLocal->GetTickBase() * I::Globals->flIntervalPerTick - flSimulationTime;
	return TIME_TO_TICKS(std::max<float>(0.f, flDifference));
}

bool U::PrecacheModel(const char* szModelName)
{
	if (auto pModelPrecache = I::StringContainer->FindTable(XorStr("modelprecache")); pModelPrecache != nullptr)
	{
		I::ModelInfo->FindOrLoadModel(szModelName);
		if (pModelPrecache->AddString(false, szModelName) == INVALID_STRING_INDEX)
			return false;
	}

	return true;
}

const char* U::GetWeaponIcon(short nItemDefinitionIndex)
{
	/*
	 * @note: icon code = weapon item definition index in hex
	 * list of other icons:
	 *	"E210" - kevlar
	 *	"E20E" - helmet
	 *	"E20F" - defuser kit
	 *	"E211" - banner
	 *	"E212" - target
	 */
	switch (nItemDefinitionIndex)
	{
	case WEAPON_DEAGLE:
		return u8"\uE001";
	case WEAPON_ELITE:
		return u8"\uE002";
	case WEAPON_FIVESEVEN:
		return u8"\uE003";
	case WEAPON_GLOCK:
		return u8"\uE004";
	case WEAPON_AK47:
		return u8"\uE007";
	case WEAPON_AUG:
		return u8"\uE008";
	case WEAPON_AWP:
		return u8"\uE009";
	case WEAPON_FAMAS:
		return u8"\uE00A";
	case WEAPON_G3SG1:
		return u8"\uE00B";
	case WEAPON_GALILAR:
		return u8"\uE00D";
	case WEAPON_M249:
		return u8"\uE00E";
	case WEAPON_M4A1:
		return u8"\uE010";
	case WEAPON_MAC10:
		return u8"\uE011";
	case WEAPON_P90:
		return u8"\uE013";
	case WEAPON_MP5SD:
		return u8"\uE017";
	case WEAPON_UMP45:
		return u8"\uE018";
	case WEAPON_XM1014:
		return u8"\uE019";
	case WEAPON_BIZON:
		return u8"\uE01A";
	case WEAPON_MAG7:
		return u8"\uE01B";
	case WEAPON_NEGEV:
		return u8"\uE01C";
	case WEAPON_SAWEDOFF:
		return u8"\uE01D";
	case WEAPON_TEC9:
		return u8"\uE01E";
	case WEAPON_TASER:
		return u8"\uE01F";
	case WEAPON_HKP2000:
		return u8"\uE020";
	case WEAPON_MP7:
		return u8"\uE021";
	case WEAPON_MP9:
		return u8"\uE022";
	case WEAPON_NOVA:
		return u8"\uE023";
	case WEAPON_P250:
		return u8"\uE024";
	case WEAPON_SCAR20:
		return u8"\uE026";
	case WEAPON_SG556:
		return u8"\uE027";
	case WEAPON_SSG08:
		return u8"\uE028";
	case WEAPON_KNIFE:
		return u8"\uE02A";
	case WEAPON_FLASHBANG:
		return u8"\uE02B";
	case WEAPON_HEGRENADE:
		return u8"\uE02C";
	case WEAPON_SMOKEGRENADE:
		return u8"\uE02D";
	case WEAPON_MOLOTOV:
		return u8"\uE02E";
	case WEAPON_DECOY:
		return u8"\uE02F";
	case WEAPON_INCGRENADE:
		return u8"\uE030";
	case WEAPON_C4:
		return u8"\uE031";
	case WEAPON_HEALTHSHOT:
		return u8"\uE039";
	case WEAPON_KNIFE_GG:
	case WEAPON_KNIFE_T:
		return u8"\uE03B";
	case WEAPON_M4A1_SILENCER:
		return u8"\uE03C";
	case WEAPON_USP_SILENCER:
		return u8"\uE03D";
	case WEAPON_CZ75A:
		return u8"\uE03F";
	case WEAPON_REVOLVER:
		return u8"\uE040";
	case WEAPON_KNIFE_BAYONET:
		return u8"\uE1F4";
	case WEAPON_KNIFE_CSS:
		return u8"\uE1F7";
	case WEAPON_KNIFE_FLIP:
		return u8"\uE1F9";
	case WEAPON_KNIFE_GUT:
		return u8"\uE1FA";
	case WEAPON_KNIFE_KARAMBIT:
		return u8"\uE1FB";
	case WEAPON_KNIFE_M9_BAYONET:
		return u8"\uE1FC";
	case WEAPON_KNIFE_TACTICAL:
		return u8"\uE1FD";
	case WEAPON_KNIFE_FALCHION:
		return u8"\uE200";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return u8"\uE202";
	case WEAPON_KNIFE_BUTTERFLY:
		return u8"\uE203";
	case WEAPON_KNIFE_PUSH:
		return u8"\uE204";
	case WEAPON_KNIFE_CORD:
		return u8"\uE205";
	case WEAPON_KNIFE_CANIS:
		return u8"\uE206";
	case WEAPON_KNIFE_URSUS:
		return u8"\uE207";
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return u8"\uE208";
	case WEAPON_KNIFE_OUTDOOR:
		return u8"\uE209";
	case WEAPON_KNIFE_STILETTO:
		return u8"\uE20A";
	case WEAPON_KNIFE_WIDOWMAKER:
		return u8"\uE20B";
	case WEAPON_KNIFE_SKELETON:
		return u8"\uE20D";
	default:
		return "?";
	}
}
#pragma endregion

#pragma region utilities_extra
void U::FlashWindow(HWND pWindow)
{
	FLASHWINFO fwInfo;
	fwInfo.cbSize = sizeof(FLASHWINFO);
	fwInfo.hwnd = pWindow;
	fwInfo.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	fwInfo.uCount = 0;
	fwInfo.dwTimeout = 0;
	FlashWindowEx(&fwInfo);
}
#pragma endregion

#pragma region utilities_string
std::string U::UnicodeAscii(const std::wstring& wszUnicode)
{
	std::string szOutput(wszUnicode.cbegin(), wszUnicode.cend());
	return szOutput;
}

std::wstring U::AsciiUnicode(const std::string& szAscii)
{
	std::wstring wszOutput(szAscii.cbegin(), szAscii.cend());
	return wszOutput;
}
#pragma endregion
