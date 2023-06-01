#pragma once
// @source: master/public/networkstringtabledefs.h

#define INVALID_STRING_TABLE (-1)
#define INVALID_STRING_INDEX static_cast<std::uint16_t>(~0)

class INetworkStringTable : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	const char* GetTableName()
	{
		return CallVFunc<const char*, 1U>(this);
	}

	int GetTableID()
	{
		return CallVFunc<int, 2U>(this);
	}

	int GetNumStrings()
	{
		return CallVFunc<int, 3U>(this);
	}

	int GetMaxStrings()
	{
		return CallVFunc<int, 4U>(this);
	}

	int GetEntryBits()
	{
		return CallVFunc<int, 5U>(this);
	}

	int AddString(bool bIsServer, const char* szValue, int iLength = -1, const void* pUserData = nullptr)
	{
		return CallVFunc<int, 8U>(this, bIsServer, szValue, iLength, pUserData);
	}

	const char* GetString(int nString)
	{
		return CallVFunc<const char*, 9U>(this, nString);
	}

	void SetStringUserData(int nString, int iLength, const void* pUserData)
	{
		CallVFunc<void, 10U>(this, nString, iLength, pUserData);
	}

	const void* GetStringUserData(int nString, int* iLength)
	{
		return CallVFunc<const void*, 11U>(this, nString, iLength);
	}

	int FindStringIndex(const char* szString)
	{
		return CallVFunc<int, 12U>(this, szString);
	}
};

class INetworkStringTableContainer : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	INetworkStringTable* FindTable(const char* szTableName)
	{
		return CallVFunc<INetworkStringTable*, 3U>(this, szTableName);
	}

	INetworkStringTable* GetTable(int nTableIndex)
	{
		return CallVFunc<INetworkStringTable*, 4U>(this, nTableIndex);
	}
};
