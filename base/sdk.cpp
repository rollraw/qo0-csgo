#include "sdk.h"

#include "sdk/datatypes/utlbuffer.h"

// used: interface handles
#include "core/interfaces.h"
// used: interface declarations
#include "sdk/interfaces/iglobalvars.h"
#include "sdk/interfaces/icliententitylist.h"
#include "sdk/interfaces/ibaseclientdll.h"
#include "sdk/interfaces/inetworkstring.h"
#include "sdk/interfaces/ihud.h"

#pragma region sdk_panorama
bool SDK::DecodeVFONT(CUtlBuffer& bufferFont)
{
	constexpr char szTag[ ] = "VFONT1";

	const int nTotalFontBytes = bufferFont.TellPut();
	const int nTagIndex = nTotalFontBytes - static_cast<int>(sizeof(szTag));

	if (nTagIndex <= 0)
		return false;

	std::uint8_t* pBuffer = static_cast<std::uint8_t*>(bufferFont.Base());

	// compare valve font format header tag
	if (CRT::MemoryCompare(pBuffer + nTagIndex, szTag, sizeof(szTag)) > 0)
		return false;

	unsigned char nSaltBytes = pBuffer[nTagIndex - 1];
	const int nSaltIndex = nTagIndex - nSaltBytes;
	--nSaltBytes;

	unsigned char uSaltKey = 0xA7;

	// recover salt data
	for (const unsigned char* pCurrentSalt = pBuffer + nSaltIndex; nSaltBytes-- > 0; ++pCurrentSalt)
		uSaltKey ^= (*pCurrentSalt + 0xA7) % 0x100;

	// hash the buffer
	for (int nBufferBytes = nSaltIndex; nBufferBytes-- > 0; ++pBuffer)
	{
		const unsigned char uDecoded = *pBuffer ^ uSaltKey;
		uSaltKey = (uDecoded + 0xA7) % 0x100;
		*pBuffer = uDecoded;
	}

	bufferFont.SeekPut(CUtlBuffer::SEEK_HEAD, nSaltIndex);
	return true;
}

// @todo: should be inside hud element class
void SDK::ClearHudWeaponIcons()
{
	static auto fnClearHudWeaponIcon = ROP::MethodInvoker_t<int(Q_THISCALL*)(void*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B"))); // @xref: "WeaponIcon--itemcount"

	// get hud weapons
	if (std::uint8_t* pHudWeapons = reinterpret_cast<std::uint8_t*>(I::Hud->FindElement(Q_XOR("CCSGO_HudWeaponSelection"))) - 0xA0; pHudWeapons != nullptr)
	{
		// go through all weapons
		for (int i = 0; i < *reinterpret_cast<int*>(pHudWeapons + 0x80); i++) // @todo: use its class | size: 0x108 | 55 8B EC 56 57 FF 75 0C 8B F9 FF 75 08 E8 ? ? ? ? 57
			i = fnClearHudWeaponIcon.Invoke<ROP::ClientGadget_t>(pHudWeapons, i);
	}
}

void SDK::SetLocalPlayerReady(const char* szReason)
{
	static auto fnSetLocalPlayerReady = ROP::MethodInvoker_t<void(Q_STDCALL*)(const char*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"))); // @xref: "deferred"
	fnSetLocalPlayerReady.Invoke<ROP::ClientGadget_t>(szReason);
}
#pragma endregion

#pragma region sdk_model
bool SDK::PrecacheModel(const char* szModelName)
{
	if (const auto pModelPrecache = I::NetworkStringTable->FindTable(Q_XOR("modelprecache")); pModelPrecache != nullptr)
	{
		if (I::ModelInfo->FindOrLoadModel(szModelName) == nullptr)
			return false;

		if (pModelPrecache->AddString(false, szModelName) == INVALID_STRING_INDEX)
			return false;
	}

	return true;
}

IClientNetworkable* SDK::CreateDLLEntity(const int nEntity, const EClassIndex nClassID, const int iSerial)
{
	for (const CClientClass* pClass = I::Client->GetAllClasses(); pClass != nullptr; pClass = pClass->pNext)
	{
		if (pClass->nClassID == nClassID)
			return pClass->fnCreate(nEntity, iSerial);
	}

	return nullptr;
}
#pragma endregion

#pragma region sdk_trace
void SDK::ClipTraceToPlayers(const Vector_t& vecAbsStart, const Vector_t& vecAbsEnd, const int nContentsMask, ITraceFilter* pFilter, Trace_t* pTrace, const float flMinRange, const float flMaxRange)
{
	// @source: master/game/shared/util_shared.cpp#L757
	// @ida UTIL_ClipTraceToPlayers(): client.dll -> ABS["E8 ? ? ? ? 0F 28 84 24 68 02 00 00" + 0x1]

	Trace_t trace = { };
	float flSmallestFraction = pTrace->flFraction;

	const Ray_t ray(vecAbsStart, vecAbsEnd);

	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CBasePlayer* pPlayer = I::ClientEntityList->Get<CBasePlayer>(i);

		if (pPlayer == nullptr || !pPlayer->IsAlive() || pPlayer->IsDormant())
			continue;

		if (pFilter != nullptr && !pFilter->ShouldHitEntity(pPlayer, nContentsMask))
			continue;

		const Vector_t& vecPosition = pPlayer->WorldSpaceCenter();

		const Vector_t vecTo = vecPosition - vecAbsStart;
		Vector_t vecDirection = vecAbsEnd - vecAbsStart;
		const float flLength = vecDirection.NormalizeInPlace();
		const float flRangeAlong = vecDirection.DotProduct(vecTo);

		// calculate distance to ray
		float flRange;
		if (flRangeAlong < 0.0f)
			// off start point
			flRange = -vecTo.Length();
		else if (flRangeAlong > flLength)
			// off end point
			flRange = -(vecPosition - vecAbsEnd).Length();
		else
			// within ray bounds
			flRange = (vecPosition - (vecDirection * flRangeAlong + vecAbsStart)).Length();

		if (flRange < flMinRange || flRange > flMaxRange)
			continue;

		I::EngineTrace->ClipRayToEntity(ray, nContentsMask | CONTENTS_HITBOX, pPlayer, &trace);

		if (trace.flFraction < flSmallestFraction)
		{
			// we shortened the ray - save off the trace
			*pTrace = trace;
			flSmallestFraction = trace.flFraction;
		}
	}
}

bool SDK::LineGoesThroughSmoke(const Vector_t& vecStart, const Vector_t& vecEnd, const bool flGrenadeBloat)
{
	static auto fnLineGoesThroughSmoke = ROP::MethodInvoker_t<bool(Q_CDECL*)(Vector_t, Vector_t, bool)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"))); // @xref: "effects/overlaysmoke"
	return fnLineGoesThroughSmoke.Invoke<ROP::ClientGadget_t>(vecStart, vecEnd, flGrenadeBloat);
}
#pragma endregion

#pragma region sdk_network
void SDK::SendClanTag(const char* szClanTag, const char* szIdentifier)
{
	static auto fnSendClanTag = ROP::MethodInvoker_t<void(Q_FASTCALL*)(const char*, const char*)>(MEM::FindPattern(ENGINE_DLL, Q_XOR("53 56 57 8B DA 8B F9 FF 15"))); // @xref: "ClanTagChanged", "tag", "name"
	fnSendClanTag.Invoke<ROP::EngineGadget_t>(szClanTag, szIdentifier);
}
#pragma endregion
