#include "convar.h"

// used: [crt] time_t, time, localtime_s
#include <ctime>

// used: getworkingpath
#include "../core.h"
// used: l_print
#include "../utilities/log.h"

// used: interface handles
#include "interfaces.h"

bool CONVAR::Setup()
{
	bool bSuccess = true;

	name = Find(FNV1A::HashConst("name"));
	bSuccess &= (name != nullptr);

	sensitivity = Find(FNV1A::HashConst("sensitivity"));
	bSuccess &= (sensitivity != nullptr);

	game_mode = Find(FNV1A::HashConst("game_mode"));
	bSuccess &= (game_mode != nullptr);

	game_type = Find(FNV1A::HashConst("game_type"));
	bSuccess &= (game_type != nullptr);

	inferno_flame_lifetime = Find(FNV1A::HashConst("inferno_flame_lifetime"));
	bSuccess &= (inferno_flame_lifetime != nullptr);

	m_pitch = Find(FNV1A::HashConst("m_pitch"));
	bSuccess &= (m_pitch != nullptr);

	m_yaw = Find(FNV1A::HashConst("m_yaw"));
	bSuccess &= (m_yaw != nullptr);

	sv_autobunnyhopping = Find(FNV1A::HashConst("sv_autobunnyhopping"));
	bSuccess &= (sv_autobunnyhopping != nullptr);

	sv_coaching_enabled = Find(FNV1A::HashConst("sv_coaching_enabled"));
	bSuccess &= (sv_coaching_enabled != nullptr);

	sv_clip_penetration_traces_to_players = Find(FNV1A::HashConst("sv_clip_penetration_traces_to_players"));
	bSuccess &= (sv_clip_penetration_traces_to_players != nullptr);

	sv_maxunlag = Find(FNV1A::HashConst("sv_maxunlag"));
	bSuccess &= (sv_maxunlag != nullptr);

	cl_forwardspeed = Find(FNV1A::HashConst("cl_forwardspeed"));
	bSuccess &= (cl_forwardspeed != nullptr);

	cl_sidespeed = Find(FNV1A::HashConst("cl_sidespeed"));
	bSuccess &= (cl_sidespeed != nullptr);

	cl_upspeed = Find(FNV1A::HashConst("cl_upspeed"));
	bSuccess &= (cl_upspeed != nullptr);

	weapon_recoil_scale = Find(FNV1A::HashConst("weapon_recoil_scale"));
	bSuccess &= (weapon_recoil_scale != nullptr);

	ff_damage_reduction_bullets = Find(FNV1A::HashConst("ff_damage_reduction_bullets"));
	bSuccess &= (ff_damage_reduction_bullets != nullptr);

	ff_damage_bullet_penetration = Find(FNV1A::HashConst("ff_damage_bullet_penetration"));
	bSuccess &= (ff_damage_bullet_penetration != nullptr);

	mp_teammates_are_enemies = Find(FNV1A::HashConst("mp_teammates_are_enemies"));
	bSuccess &= (mp_teammates_are_enemies != nullptr);

	mp_damage_headshot_only = Find(FNV1A::HashConst("mp_damage_headshot_only"));
	bSuccess &= (mp_damage_headshot_only != nullptr);

	mp_damage_scale_ct_head = Find(FNV1A::HashConst("mp_damage_scale_ct_head"));
	bSuccess &= (mp_damage_scale_ct_head != nullptr);

	mp_damage_scale_ct_body = Find(FNV1A::HashConst("mp_damage_scale_ct_body"));
	bSuccess &= (mp_damage_scale_ct_body != nullptr);

	mp_damage_scale_t_head = Find(FNV1A::HashConst("mp_damage_scale_t_head"));
	bSuccess &= (mp_damage_scale_t_head != nullptr);

	mp_damage_scale_t_body = Find(FNV1A::HashConst("mp_damage_scale_t_body"));
	bSuccess &= (mp_damage_scale_t_body != nullptr);

	return bSuccess;
}

void CONVAR::Dump(const wchar_t* wszFileName)
{
	wchar_t wszDumpFilePath[MAX_PATH];
	if (!CORE::GetWorkingPath(wszDumpFilePath))
		return;

	CRT::StringCat(wszDumpFilePath, wszFileName);

	// open our dump file to write in
	const HANDLE hFileOut = ::CreateFileW(wszDumpFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFileOut == INVALID_HANDLE_VALUE)
		return;

	const std::time_t time = std::time(nullptr);
	std::tm timePoint;
	localtime_s(&timePoint, &time);

	char szInfoBuffer[64];
	const std::size_t nInfoSize = CRT::TimeToString(szInfoBuffer, sizeof(szInfoBuffer), "[%d-%m-%Y %T] qo0 | convars dump\n\n", &timePoint);

	// write current date, time and info
	::WriteFile(hFileOut, szInfoBuffer, nInfoSize, nullptr, nullptr);

	IConVar::ICVarIteratorInternal* pIterator = I::ConVar->FactoryInternalIterator();
	for (pIterator->SetFirst(); pIterator->IsValid(); pIterator->Next())
	{
		if (const CConCommandBase* pConCommand = pIterator->Get(); pConCommand != nullptr && pConCommand->szName != nullptr)
		{
			char szBuffer[512];
			char* szBufferEnd = CRT::StringCopy(szBuffer, pConCommand->szName);

			// check is not a callback
			if (!pConCommand->IsCommand())
			{
				const CConVar* pConVar = static_cast<const CConVar*>(pConCommand);
				szBufferEnd = CRT::StringCopy(CRT::StringCopy(CRT::StringCopy(szBufferEnd, " = \""), pConVar->szDefaultValue), "\"; ");
			}
			else
				szBufferEnd = CRT::StringCopy(szBufferEnd, "; [callback] ");

			// accumulate flags
			if (pConCommand->nFlags & FCVAR_HIDDEN)
				szBufferEnd = CRT::StringCopy(szBufferEnd, "[hidden] ");
			if (pConCommand->nFlags & FCVAR_REPLICATED)
				szBufferEnd = CRT::StringCopy(szBufferEnd, "[replicated] ");
			if (pConCommand->nFlags & FCVAR_CHEAT)
				szBufferEnd = CRT::StringCopy(szBufferEnd, "[cheat] ");

			// check does it have description text
			if (const char* szHelpText = pConCommand->GetHelpText(); szHelpText != nullptr)
				szBufferEnd = CRT::StringCopy(szBufferEnd, szHelpText);

			// insert line feed if it isn't set already
			if (szBufferEnd[-1] != '\n')
			{
				*szBufferEnd++ = '\n';
				*szBufferEnd = '\0';
			}

			Q_ASSERT(szBufferEnd - szBuffer < sizeof(szBuffer)); // stack overflow
			::WriteFile(hFileOut, szBuffer, szBufferEnd - szBuffer, nullptr, nullptr);
		}
	}

	::CloseHandle(hFileOut);
}

#pragma region convar_get
CConVar* CONVAR::Find(const FNV1A_t uNameHash)
{
	IConVar::ICVarIteratorInternal* pIterator = I::ConVar->FactoryInternalIterator();
	for (pIterator->SetFirst(); pIterator->IsValid(); pIterator->Next())
	{
		if (CConCommandBase* pConCommand = pIterator->Get(); FNV1A::Hash(pConCommand->szName) == uNameHash)
			return static_cast<CConVar*>(pConCommand);
	}

	L_PRINT(LOG_ERROR) << Q_XOR("failed to find convar");
	return nullptr;
}
#pragma endregion
