#include "screen.h"

// used: cheat variables
#include "../../core/variables.h"
// used: render functions
#include "../../utilities/draw.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../../sdk/interfaces/iglobalvars.h"
#include "../../sdk/interfaces/iengineclient.h"
#include "../../sdk/interfaces/icliententitylist.h"
#include "../../sdk/interfaces/iweaponsystem.h"

using namespace F::VISUAL;

#pragma region visual_screen_callbacks
void SCREEN::OnDraw(CCSPlayer* pLocal)
{
	if (!C::Get<bool>(Vars.bVisualScreen))
		return;

	HitMarker(C::Get<Color_t>(Vars.colVisualScreenHitMarker), C::Get<Color_t>(Vars.colVisualScreenHitMarkerDamage));
}

void SCREEN::OnEvent(const FNV1A_t uEventHash, IGameEvent& gameEvent)
{
	if (uEventHash != FNV1A::HashConst("player_hurt"))
		return;

	CCSPlayer* pLocal = CCSPlayer::GetLocalPlayer();

	if (pLocal == nullptr || !pLocal->IsAlive())
		return;

	if (!C::Get<bool>(Vars.bVisualScreen) || !C::Get<bool>(Vars.bVisualScreenHitMarker))
		return;

	// get hitmarker info
	if (const CBasePlayer* pAttacker = I::ClientEntityList->Get<CBasePlayer>(I::Engine->GetPlayerForUserID(gameEvent.GetInt(Q_XOR("attacker")))); pAttacker == pLocal)
	{
		if (CBasePlayer* pPlayer = I::ClientEntityList->Get<CBasePlayer>(I::Engine->GetPlayerForUserID(gameEvent.GetInt(Q_XOR("userid")))); pPlayer != nullptr && pPlayer != pLocal)
		{
			// play hit sound
			if (C::Get<bool>(Vars.bVisualScreenHitMarkerSound))
				I::Surface->PlaySound(Q_XOR("buttons\\arena_switch_press_02.wav"));

			// add hit info
			vecHitMarks.emplace_back(pPlayer->GetHitGroupPosition(gameEvent.GetInt(Q_XOR("hitgroup"))), gameEvent.GetInt(Q_XOR("dmg_health")), I::Globals->flRealTime + C::Get<float>(Vars.flVisualScreenHitMarkerTime));
		}
	}
}
#pragma endregion

#pragma region visual_screen_main
void SCREEN::OverrideFOV(CCSPlayer* pLocal, float* pflFieldOfViewOut)
{
	if (!C::Get<bool>(Vars.bVisualScreen) || C::Get<float>(Vars.flVisualScreenCameraFOV) == 0.0f)
		return;

	bool bIsSniperScoped = false;
	if (pLocal->IsAlive())
	{
		if (CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon(); pWeapon != nullptr)
		{
			if (const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeapon->GetEconItemView()->GetItemDefinitionIndex()); pWeaponData != nullptr)
				bIsSniperScoped = (pWeaponData->nWeaponType == WEAPONTYPE_SNIPER && pLocal->IsScoped());
		}
	}

	if (!bIsSniperScoped)
		*pflFieldOfViewOut += C::Get<float>(Vars.flVisualScreenCameraFOV);
}

void SCREEN::OverrideViewModelFOV(CCSPlayer* pLocal, float* pflFieldOfViewOut)
{
	if (!C::Get<bool>(Vars.bVisualScreen) || C::Get<float>(Vars.flVisualScreenViewModelFOV) == 0.0f)
		return;

	if (!pLocal->IsAlive())
		return;

	*pflFieldOfViewOut += C::Get<float>(Vars.flVisualScreenViewModelFOV);
}

void SCREEN::HitMarker(const Color_t& colLines, const Color_t& colDamage)
{
	if (!C::Get<bool>(Vars.bVisualScreenHitMarker))
	{
		vecHitMarks.clear();
		return;
	}

	if (vecHitMarks.empty())
		return;

	// get most recent marker time
	const float flLastDelta = vecHitMarks.back().flExpireTime - I::Globals->flRealTime;

	if (flLastDelta <= 0.0f)
	{
		vecHitMarks.clear();
		return;
	}

	const ImVec2& vecScreenSize = ImGui::GetIO().DisplaySize;
	const float flMaxLinesAlpha = colLines.Base<COLOR_A>();

	// draw marker cross
	for (constexpr float arrSideDirections[4][2] = { { -1.0f, -1.0f }, { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f } }; const float (&arrDirection)[2] : arrSideDirections)
		D::AddDrawListLine(nullptr, ImVec2(vecScreenSize.x * 0.5f + C::Get<int>(Vars.iVisualScreenHitMarkerGap) * arrDirection[0], vecScreenSize.y * 0.5f + C::Get<int>(Vars.iVisualScreenHitMarkerGap) * arrDirection[1]), ImVec2(vecScreenSize.x * 0.5f + C::Get<int>(Vars.iVisualScreenHitMarkerLength) * arrDirection[0], vecScreenSize.y * 0.5f + C::Get<int>(Vars.iVisualScreenHitMarkerLength) * arrDirection[1]), colLines.Set<COLOR_A>(static_cast<std::uint8_t>(CRT::Min(flMaxLinesAlpha, flLastDelta / C::Get<float>(Vars.flVisualScreenHitMarkerTime)) * 255.f)));

	if (C::Get<bool>(Vars.bVisualScreenHitMarkerDamage))
	{
		const float flMaxDamageAlpha = colDamage.Base<COLOR_A>();

		for (std::size_t i = 0U; i < vecHitMarks.size(); i++)
		{
			const HitMarkerObject_t& hitMarker = vecHitMarks[i];

			const float flDelta = hitMarker.flExpireTime - I::Globals->flRealTime;

			if (flDelta <= 0.0f)
			{
				vecHitMarks.erase(vecHitMarks.begin() + i);
				continue;
			}

			if (ImVec2 vecScreen = { }; D::WorldToScreen(hitMarker.vecPosition, &vecScreen))
			{
				// max distance for floating damage
				constexpr float flDistance = 40.f;
				const float flRatio = 1.0f - (flDelta / C::Get<float>(Vars.flVisualScreenHitMarkerTime));

				// calculate fade out alpha
				const int iAlpha = static_cast<int>(CRT::Min(flMaxDamageAlpha, flDelta / C::Get<float>(Vars.flVisualScreenHitMarkerTime)) * 255.f);

				char szDamageBuffer[CRT::IntegerToString_t<int, 10U>::MaxCount()];
				const char* szDealtDamage = CRT::IntegerToString(hitMarker.iDamage, szDamageBuffer, Q_ARRAYSIZE(szDamageBuffer));

				// draw dealt damage
				D::AddDrawListText(nullptr, FONT::pVisual, 24.f, ImVec2(vecScreen.x, vecScreen.y - flRatio * flDistance), szDealtDamage, colDamage.Set<COLOR_A>(static_cast<std::uint8_t>(iAlpha)), DRAW_TEXT_OUTLINE, Color_t(0, 0, 0, iAlpha));
			}
		}
	}
}
#pragma endregion
