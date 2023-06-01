#pragma once
#include "../datatypes/usercmd.h"

#define MULTIPLAYER_BACKUP 150

#pragma pack(push, 4)
// functions used to verify offsets:
// @ida: CInput::CreateMove(): client.dll -> "55 8B EC 83 EC 18 56 57 8B F9 8B 4D 04 E8 ? ? ? ? 8B 4D"
// @ida: CInput::CAM_Think(): client.dll -> "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 8B F1 8B 0D ? ? ? ? 57 85 C9" @xref: "sv_cheats", "view angles", "ideal angles", "camera offset", "Pitch: %6.1f   Yaw: %6.1f %38s", "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %19s", "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %16s"
class IInput
{
private:
	void* pVTable; // 0x0000

public:
	std::byte pad0[0x8]; // 0x0004
	bool bTrackIRAvailable; // 0x000C
	bool bMouseInitialized; // 0x000D
	bool bMouseActive; // 0x000E
	std::byte pad1[0x99]; // 0x000F
	bool bCameraInterceptingMouse; // 0x00A8 // @ida: client.dll -> ["88 86 ? ? ? ? 8B 0D ? ? ? ? 66" + 0x2]
	bool bCameraInThirdPerson; // 0x00A9 // @ida: client.dll -> ["38 86 ? ? ? ? 74 28" + 0x2]
	bool bCameraMovingWithMouse; // 0x00AA // @ida: client.dll -> ["80 BE ? ? ? ? ? F3 0F 10 44" + 0x2]
	Vector_t vecCameraOffset; // 0x00AC // @ida: client.dll -> ["F3 0F 7E 87 ? ? ? ? 89 44" + 0x4]
	bool bCameraDistanceMove; // 0x00B8 // @ida: client.dll -> ["C6 86 ? ? ? ? ? EB 3F" + 0x2]
	int nCameraOldX; // 0x00BC
	int nCameraOldY; // 0x00C0
	int nCameraX; // 0x00C4
	int nCameraY; // 0x00C8
	bool bCameraIsOrthographic; // 0x00CC
	bool bCameraIsThirdPersonOverview; // 0x00CD
	QAngle_t angPreviousView; // 0x00D0
	QAngle_t angPreviousViewTilt; // 0x00DC
	float flLastForwardMove; // 0x00E8
	int nClearInputState; // 0x00EC
	CUserCmd* pCommands; // 0x00F0 // @ida: client.dll -> ["8B B7 ? ? ? ? C1 FA" + 0x2]
	CVerifiedUserCmd* pVerifiedCommands; // 0x00F4
	CBaseHandle hSelectedWeapon; // 0x00F8 // @ida: client.dll -> ["8B 8F ? ? ? ? C7 05" + 0x2]
	void* pCameraThirdPersonData; // 0x00FC
	int nCameraCommand; // 0x0100
	std::byte pad2[0x24]; // 0x0104

	[[nodiscard]] CUserCmd* GetUserCmd(const int nSequenceNumber) const
	{
		return &pCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}

	[[nodiscard]] CVerifiedUserCmd* GetVerifiedCmd(const int nSequenceNumber) const
	{
		return &pVerifiedCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}
};
static_assert(sizeof(IInput) == 0x128);
#pragma pack(pop)
