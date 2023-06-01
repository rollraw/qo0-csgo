#pragma once
#include "vector.h"
#include "qangle.h"
#include "../hash/crc32.h"

// @source: master/game/shared/usercmd.h

#pragma region usercmd_enumerations
// @source: master/game/shared/in_buttons.h
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
	Q_CLASS_NO_ALLOC();

	CUserCmd& operator=(const CUserCmd& other)
	{
		iCommandNumber = other.iCommandNumber;
		nTickCount = other.nTickCount;
		angViewPoint = other.angViewPoint;
		vecAimDirection = other.vecAimDirection;
		flForwardMove = other.flForwardMove;
		flSideMove = other.flSideMove;
		flUpMove = other.flUpMove;
		nButtons = other.nButtons;
		uImpulse = other.uImpulse;
		iWeaponSelect = other.iWeaponSelect;
		iWeaponSubType = other.iWeaponSubType;
		iRandomSeed = other.iRandomSeed;
		shMouseDeltaX = other.shMouseDeltaX;
		shMouseDeltaY = other.shMouseDeltaY;
		bHasBeenPredicted = other.bHasBeenPredicted;
		angViewPointBackup = other.angViewPointBackup;
		nButtonsBackup = other.nButtonsBackup;
		iUnknown0 = other.iUnknown0;
		iUnknown1 = other.iUnknown1;
		return *this;
	}

	[[nodiscard]] CRC32_t GetChecksum() const
	{
		// @ida CUserCmd::GetChecksum(): client.dll -> "53 8B D9 83 C8"
		CRC32_t uHashCRC = 0U;

		CRC32::Init(&uHashCRC);
		CRC32::ProcessBuffer(&uHashCRC, &iCommandNumber, sizeof(iCommandNumber));
		CRC32::ProcessBuffer(&uHashCRC, &nTickCount, sizeof(nTickCount));
		CRC32::ProcessBuffer(&uHashCRC, &angViewPoint, sizeof(angViewPoint));
		CRC32::ProcessBuffer(&uHashCRC, &vecAimDirection, sizeof(vecAimDirection));
		CRC32::ProcessBuffer(&uHashCRC, &flForwardMove, sizeof(flForwardMove));
		CRC32::ProcessBuffer(&uHashCRC, &flSideMove, sizeof(flSideMove));
		CRC32::ProcessBuffer(&uHashCRC, &flUpMove, sizeof(flUpMove));
		CRC32::ProcessBuffer(&uHashCRC, &nButtons, sizeof(nButtons));
		CRC32::ProcessBuffer(&uHashCRC, &uImpulse, sizeof(uImpulse));
		CRC32::ProcessBuffer(&uHashCRC, &iWeaponSelect, sizeof(iWeaponSelect));
		CRC32::ProcessBuffer(&uHashCRC, &iWeaponSubType, sizeof(iWeaponSubType));
		CRC32::ProcessBuffer(&uHashCRC, &iRandomSeed, sizeof(iRandomSeed));
		CRC32::ProcessBuffer(&uHashCRC, &shMouseDeltaX, sizeof(shMouseDeltaX));
		CRC32::ProcessBuffer(&uHashCRC, &shMouseDeltaY, sizeof(shMouseDeltaY));
		CRC32::Final(&uHashCRC);

		return uHashCRC;
	}

private:
	void* pVTable; // 0x00
public:
	int iCommandNumber; // 0x04
	int nTickCount; // 0x08
	QAngle_t angViewPoint; // 0x0C
	Vector_t vecAimDirection; // 0x18
	float flForwardMove; // 0x24
	float flSideMove; // 0x28
	float flUpMove; // 0x2C
	int nButtons; // 0x30
	std::uint8_t uImpulse; // 0x34
	int iWeaponSelect; // 0x38
	int iWeaponSubType; // 0x3C
	int iRandomSeed; // 0x40
	short shMouseDeltaX; // 0x44
	short shMouseDeltaY; // 0x46
	bool bHasBeenPredicted; // 0x48
	QAngle_t angViewPointBackup; // 0x4C // @note: instead of 'QAngle angHeadView' there's backup value of view angles that used to detect their change, changed since ~11.06.22 (version 1.38.3.7, build 1490)
	int nButtonsBackup; // 0x58 // @note: instead of 'Vector vecHeadOffset' there's single backup value of buttons that used to detect their change, 0x5C/0x60 used for something else but still there, changed since ~11.06.22 (version 1.38.3.7, build 1490) // @ida: (WriteUsercmd) client.dll -> U8["FF 76 ? E8 ? ? ? ? 83 C4 1C" + 0x2] @xref: "headoffset"
private:
	int iUnknown0; // 0x5C
	int iUnknown1; // 0x60
};
static_assert(sizeof(CUserCmd) == 0x64); // size verify @ida: (CInput::Init_All) client.dll -> U8["83 C1 ? 83 EA 01 75 ED EB 02 33 FF A1" + 0x2]

class CVerifiedUserCmd
{
public:
	CUserCmd userCmd; // 0x00
	CRC32_t uHashCRC; // 0x64
};
static_assert(sizeof(CVerifiedUserCmd) == 0x68); // size verify @ida: (CInput::Init_All) client.dll -> U8["83 C1 ? 83 EA 01 75 ED EB 02 33 FF 89 BE" + 0x2]
#pragma pack(pop)
