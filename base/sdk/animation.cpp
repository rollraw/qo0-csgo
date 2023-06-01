#include "animation.h"

// used: ccsplayer, cweaponcsbase
#include "entity.h"

// used: interface handles
#include "../core/interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/iengineclient.h"

CCSGOPlayerAnimState::CCSGOPlayerAnimState(CCSPlayer* pCSPlayer)
{
	static auto fnConstructor = reinterpret_cast<void(Q_THISCALL*)(CCSGOPlayerAnimState*, CCSPlayer*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"))); // @xref: "ggprogressive_player_levelup"
	fnConstructor(this, pCSPlayer);
}

void CCSGOPlayerAnimState::Update(const QAngle_t& angView)
{
	static auto fnUpdate = reinterpret_cast<void(Q_VECTORCALL*)(CCSGOPlayerAnimState*, void*, float, float, float, void*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"))); // @xref: "%s_aim"
	fnUpdate(this, nullptr, angView.z, angView.y, angView.x, nullptr);
}

void CCSGOPlayerAnimState::Reset()
{
	static auto fnReset = reinterpret_cast<void(Q_THISCALL*)(CCSGOPlayerAnimState*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("56 6A 01 68 ? ? ? ? 8B F1"))); // @xref: "player_spawn"
	fnReset(this);
}

void CCSGOPlayerAnimState::ModifyEyePosition(Vector_t& vecInputEyePosition) const
{
	/*
	 * @ida CCSGOPlayerAnimState::ModifyEyePosition():
	 * client.dll -> "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14" @xref: "head_0"
	 * server.dll -> "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 8B 4F" @xref: "head_0"
	 */

	if (pPlayer == nullptr || I::Engine->IsHLTV() || I::Engine->IsPlayingDemo())
		return;

	if (!bLanding && flDuckAmount == 0.0f && pPlayer->GetGroundEntityHandle().IsValid())
		return;

	const int iHeadBone = pPlayer->GetBoneByHash(FNV1A::HashConst("head_0"));
	if (iHeadBone == BONE_INVALID)
		return;

	Vector_t vecHead = pPlayer->GetBonePosition(iHeadBone);
	vecHead.z += 1.7f;

	if (vecInputEyePosition.z > vecHead.z)
	{
		const float flHeightFactor = CRT::Clamp((std::fabsf(vecInputEyePosition.z - vecHead.z) - 4.0f) / 6.0f, 0.0f, 1.0f);

		// SimpleSplineRemapValClamped
		const float flHeightFactorSqr = (flHeightFactor * flHeightFactor);
		vecInputEyePosition.z += ((vecHead.z - vecInputEyePosition.z) * ((flHeightFactorSqr * 3.0f) - ((flHeightFactorSqr * 2.0f) * flHeightFactor)));
	}
}
