#pragma once
// used: [stl] uint32_t
#include <cstdint>

// @source: master/public/basehandle.h

#pragma region basehandle_definitions
// how many bits to use to encode an edict
#define	MAX_EDICT_BITS 11
// max number of edicts in a level
#define	MAX_EDICTS (1 << MAX_EDICT_BITS) // @ida: client.dll -> ["3D ? ? ? ? 73 26" + 0x1]

#define NUM_ENT_ENTRY_BITS (MAX_EDICT_BITS + 2)
#define NUM_ENT_ENTRIES (1 << NUM_ENT_ENTRY_BITS)

#define NUM_SERIAL_NUM_BITS 16
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK ((1 << NUM_SERIAL_NUM_BITS) - 1)

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define STATICPROP_EHANDLE_MASK ((1 << 30) >> NUM_SERIAL_NUM_SHIFT_BITS)
#pragma endregion

class CBaseHandle
{
public:
	CBaseHandle() noexcept :
		nIndex(INVALID_EHANDLE_INDEX) { }

	CBaseHandle(const CBaseHandle& other) noexcept :
		nIndex(other.nIndex) { }

	CBaseHandle(const int iEntry, const int iSerial) noexcept
	{
		Q_ASSERT(iEntry >= 0 && (iEntry & ENT_ENTRY_MASK) == iEntry);
		Q_ASSERT(iSerial >= 0 && iSerial < (1 << NUM_SERIAL_NUM_BITS));

		nIndex = iEntry | (iSerial << NUM_SERIAL_NUM_SHIFT_BITS);
	}

	bool operator!=(const CBaseHandle& other) const noexcept
	{
		return nIndex != other.nIndex;
	}

	bool operator==(const CBaseHandle& other) const noexcept
	{
		return nIndex == other.nIndex;
	}

	bool operator<(const CBaseHandle& other) const noexcept
	{
		return nIndex < other.nIndex;
	}

	[[nodiscard]] bool IsValid() const noexcept
	{
		return nIndex != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex() const noexcept
	{
		/*
		 * there is a hack here: due to a bug in the original implementation of the
		 * entity handle system, an attempt to look up an invalid entity index in
		 * certain cirumstances might fall through to the the mask operation below.
		 * this would mask an invalid index to be in fact a lookup of entity number
		 * NUM_ENT_ENTRIES, so invalid ent indexes end up actually looking up the
		 * last slot in the entities array. Since this slot is always empty, the
		 * lookup returns NULL and the expected behavior occurs through this unexpected
		 * route.
		 * a lot of code actually depends on this behavior, and the bug was only exposed
		 * after a change to NUM_SERIAL_NUM_BITS increased the number of allowable
		 * static props in the world. So the if-stanza below detects this case and
		 * retains the prior (bug-submarining) behavior.
		 *
		 * TL;DR valve crapped their pants
		 */
		if (!IsValid())
			return NUM_ENT_ENTRIES - 1;

		return static_cast<int>(nIndex & ENT_ENTRY_MASK);
	}

	[[nodiscard]] int GetSerialNumber() const noexcept
	{
		return static_cast<int>(nIndex >> NUM_SERIAL_NUM_SHIFT_BITS);
	}

public:
	std::uint32_t nIndex;
};
