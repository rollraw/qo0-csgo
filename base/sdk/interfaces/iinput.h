#pragma once
// used: usercmd
#include "../datatypes/usercmd.h"

#define MULTIPLAYER_BACKUP 150

class IInput
{
public:
	std::byte			pad0[0xC];				// 0x00
	bool				bTrackIRAvailable;		// 0x0C
	bool				bMouseInitialized;		// 0x0D
	bool				bMouseActive;			// 0x0E
	std::byte			pad1[0x9A];				// 0x0F
	bool				bCameraInThirdPerson;	// 0xA9
	std::byte			pad2[0x2];				// 0xAA
	Vector				vecCameraOffset;		// 0xAC
	std::byte			pad3[0x38];				// 0xB8
	CUserCmd*			pCommands;				// 0xF0
	CVerifiedUserCmd*	pVerifiedCommands;		// 0xF4

	[[nodiscard]] CUserCmd* GetUserCmd(const int nSequenceNumber) const
	{
		return &pCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}

	[[nodiscard]] CVerifiedUserCmd* GetVerifiedCmd(const int nSequenceNumber) const
	{
		return &pVerifiedCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}
};
static_assert(sizeof(IInput) == 0xF8);
