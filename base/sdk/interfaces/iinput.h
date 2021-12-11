#pragma once
// used: usercmd
#include "../datatypes/usercmd.h"

#define MULTIPLAYER_BACKUP 150

class IInput
{
public:
	std::byte			pad0[0xC];				//0x0000
	bool				bTrackIRAvailable;		//0x000C
	bool				bMouseInitialized;		//0x000D
	bool				bMouseActive;			//0x000E
	std::byte			pad1[0xB2];				//0x000F
	bool				bCameraInThirdPerson;	//0x00C1
	std::byte			pad2[0x2];				//0x00C2
	Vector				vecCameraOffset;		//0x00C4
	std::byte			pad3[0x38];				//0x00D0
	CUserCmd*			pCommands;				//0x0108
	CVerifiedUserCmd*	pVerifiedCommands;		//0x010C

	CUserCmd* GetUserCmd(const int nSequenceNumber) const
	{
		return &pCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}

	CVerifiedUserCmd* GetVerifiedCmd(const int nSequenceNumber) const
	{
		return &pVerifiedCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}
};
static_assert(sizeof(IInput) == 0x0110);
