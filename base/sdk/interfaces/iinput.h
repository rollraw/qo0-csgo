#pragma once
// used: usercmd
#include "../datatypes/usercmd.h"

#define MULTIPLAYER_BACKUP 150

class IInput
{
public:
	std::byte			pad0[0xC];
	bool				bTrackIRAvailable;
	bool				bMouseInitialized;
	bool				bMouseActive;
	std::byte			pad1[0x9E];
	bool				bCameraInThirdPerson;
	std::byte			pad2[0x2];
	Vector				vecCameraOffset;
	std::byte			pad3[0x38];
	CUserCmd*			pCommands;
	CVerifiedUserCmd*	pVerifiedCommands;

	CUserCmd* GetUserCmd(const int nSequenceNumber) const
	{
		return &pCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}

	CVerifiedUserCmd* GetVerifiedCmd(const int nSequenceNumber) const
	{
		return &pVerifiedCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}
};
static_assert(sizeof(IInput) == 0xFC);
