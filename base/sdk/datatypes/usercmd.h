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
	IN_MIDDLE_ATTACK =	(1 << 25),
	IN_USE_OR_RELOAD =	(1 << 26)
};
#pragma endregion

#pragma pack(push, 4)
class CUserCmd
{
public:
	virtual			~CUserCmd() { }		// 0x00
	int				iCommandNumber;		// 0x04
	int				iTickCount;			// 0x08
	QAngle			angViewPoint;		// 0x0C
	Vector			vecAimDirection;	// 0x18
	float			flForwardMove;		// 0x24
	float			flSideMove;			// 0x28
	float			flUpMove;			// 0x2C
	int				iButtons;			// 0x30
	std::uint8_t	uImpulse;			// 0x34
	int				iWeaponSelect;		// 0x38
	int				iWeaponSubType;		// 0x3C
	int				iRandomSeed;		// 0x40
	short			sMouseDeltaX;		// 0x44
	short			sMouseDeltaY;		// 0x46
	bool			bHasBeenPredicted;	// 0x48
	Vector			vecHeadAngles;		// 0x4C
	Vector			vecHeadOffset;		// 0x58

	[[nodiscard]] CRC32_t GetChecksum() const
	{
		CRC32_t uHashCRC = 0UL;

		CRC32::Init(&uHashCRC);
		CRC32::ProcessBuffer(&uHashCRC, &iCommandNumber, sizeof(iCommandNumber));
		CRC32::ProcessBuffer(&uHashCRC, &iTickCount, sizeof(iTickCount));
		CRC32::ProcessBuffer(&uHashCRC, &angViewPoint, sizeof(angViewPoint));
		CRC32::ProcessBuffer(&uHashCRC, &vecAimDirection, sizeof(vecAimDirection));
		CRC32::ProcessBuffer(&uHashCRC, &flForwardMove, sizeof(flForwardMove));
		CRC32::ProcessBuffer(&uHashCRC, &flSideMove, sizeof(flSideMove));
		CRC32::ProcessBuffer(&uHashCRC, &flUpMove, sizeof(flUpMove));
		CRC32::ProcessBuffer(&uHashCRC, &iButtons, sizeof(iButtons));
		CRC32::ProcessBuffer(&uHashCRC, &uImpulse, sizeof(uImpulse));
		CRC32::ProcessBuffer(&uHashCRC, &iWeaponSelect, sizeof(iWeaponSelect));
		CRC32::ProcessBuffer(&uHashCRC, &iWeaponSubType, sizeof(iWeaponSubType));
		CRC32::ProcessBuffer(&uHashCRC, &iRandomSeed, sizeof(iRandomSeed));
		CRC32::ProcessBuffer(&uHashCRC, &sMouseDeltaX, sizeof(sMouseDeltaX));
		CRC32::ProcessBuffer(&uHashCRC, &sMouseDeltaY, sizeof(sMouseDeltaY));
		CRC32::Final(&uHashCRC);

		return uHashCRC;
	}
};
static_assert(sizeof(CUserCmd) == 0x64);

class CVerifiedUserCmd
{
public:
	CUserCmd	userCmd;	// 0x00
	CRC32_t		uHashCRC;	// 0x64
};
static_assert(sizeof(CVerifiedUserCmd) == 0x68);
#pragma pack(pop)
