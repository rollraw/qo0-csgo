#pragma once
// used: usercmd
#include "../datatypes/usercmd.h"

#define MULTIPLAYER_BACKUP 150

class IInput
{
public:
	std::byte			pad0[0xC];				//0x00
	bool				bTrackIRAvailable;		//0x0C
	bool				bMouseInitialized;		//0x0D
	bool				bMouseActive;			//0x0E
	std::byte			pad1[0x9E];				//0x0F
	bool				bCameraInThirdPerson;	//0xAD
	std::byte			pad2[0x2];				//0xAE
	Vector				vecCameraOffset;		//0xB0
	std::byte			pad3[0x38];				//0xBC
	CUserCmd*			pCommands;				//0xF4
	CVerifiedUserCmd*	pVerifiedCommands;		//0xF8

	CUserCmd* GetUserCmd(int nSequenceNumber)
	{
		return &pCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}

	CVerifiedUserCmd* GetVerifiedCmd(int nSequenceNumber)
	{
		return &pVerifiedCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}
};
