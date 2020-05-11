#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/usercmd.h

// used: crc32 hashing
#include "../hash/crc32.h"
// used: vector
#include "vector.h"
// used: angle
#include "qangle.h"

#pragma region usercmd_enumerations
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/in_buttons.h
enum ECommandButtons : int
{
	IN_ATTACK =			(1 << 0),
	IN_JUMP =			(1 << 1),
	IN_DUCK =			(1 << 2),
	IN_FORWARD =		(1 << 3),
	IN_BACK =			(1 << 4),
	IN_USE =			(1 << 5),
	IN_CANCEL =			(1 << 6),
	IN_LEFT =			(1 << 7),
	IN_RIGHT =			(1 << 8),
	IN_MOVELEFT =		(1 << 9),
	IN_MOVERIGHT =		(1 << 10),
	IN_SECOND_ATTACK =	(1 << 11),
	IN_RUN =			(1 << 12),
	IN_RELOAD =			(1 << 13),
	IN_LEFT_ALT =		(1 << 14),
	IN_RIGHT_ALT =		(1 << 15),
	IN_SCORE =			(1 << 16),
	IN_SPEED =			(1 << 17),
	IN_WALK =			(1 << 18),
	IN_ZOOM =			(1 << 19),
	IN_FIRST_WEAPON =	(1 << 20),
	IN_SECOND_WEAPON =	(1 << 21),
	IN_BULLRUSH =		(1 << 22),
	IN_FIRST_GRENADE =	(1 << 23),
	IN_SECOND_GRENADE = (1 << 24),
	IN_MIDDLE_ATTACK =	(1 << 25)
};
#pragma endregion

class CUserCmd
{
public:
	virtual	~CUserCmd() { }

	CRC32_t GetChecksum() const
	{
		CRC32_t crc = 0UL;
		CRC32::Init(&crc);
		CRC32::ProcessBuffer(&crc, &iCommandNumber, sizeof(iCommandNumber));
		CRC32::ProcessBuffer(&crc, &iTickCount, sizeof(iTickCount));
		CRC32::ProcessBuffer(&crc, &angViewPoint, sizeof(angViewPoint));
		CRC32::ProcessBuffer(&crc, &vecAimDirection, sizeof(vecAimDirection));
		CRC32::ProcessBuffer(&crc, &flForwardMove, sizeof(flForwardMove));
		CRC32::ProcessBuffer(&crc, &flSideMove, sizeof(flSideMove));
		CRC32::ProcessBuffer(&crc, &flUpMove, sizeof(flUpMove));
		CRC32::ProcessBuffer(&crc, &iButtons, sizeof(iButtons));
		CRC32::ProcessBuffer(&crc, &uImpulse, sizeof(uImpulse));
		CRC32::ProcessBuffer(&crc, &iWeaponSelect, sizeof(iWeaponSelect));
		CRC32::ProcessBuffer(&crc, &iWeaponSubType, sizeof(iWeaponSubType));
		CRC32::ProcessBuffer(&crc, &iRandomSeed, sizeof(iRandomSeed));
		CRC32::ProcessBuffer(&crc, &sMouseDeltaX, sizeof(sMouseDeltaX));
		CRC32::ProcessBuffer(&crc, &sMouseDeltaY, sizeof(sMouseDeltaY));
		CRC32::Final(&crc);
		return crc;
	}

public:
	int				iCommandNumber;
	int				iTickCount;
	QAngle			angViewPoint;
	Vector			vecAimDirection;
	float			flForwardMove;
	float			flSideMove;
	float			flUpMove;
	int				iButtons;
	std::uint8_t	uImpulse;
	int				iWeaponSelect;
	int				iWeaponSubType;
	int				iRandomSeed;
	short			sMouseDeltaX;
	short			sMouseDeltaY;
	bool			bHasBeenPredicted;
	Vector			vecHeadAngles;
	Vector			vecHeadOffset;
};

class CVerifiedUserCmd
{
public:
	CUserCmd	cmd;
	CRC32_t		crc;
};
