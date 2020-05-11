#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/networkstringtabledefs.h

#define INVALID_STRING_TABLE -1
#define INVALID_STRING_INDEX (std::uint16_t)~0

class INetworkStringTable;
using StringChangedFn = void(__cdecl*)(void*, INetworkStringTable*, int, char const*, void const*);

class INetworkStringTable
{
public:
	virtual					~INetworkStringTable() { }

	// Table Info
	virtual const char*		GetTableName() const = 0;
	virtual int				GetTableId() const = 0;
	virtual int				GetNumStrings() const = 0;
	virtual int				GetMaxStrings() const = 0;
	virtual int				GetEntryBits() const = 0;

	// Networking
	virtual void			SetTick(int iTick) = 0;
	virtual bool			ChangedSinceTick(int iTick) const = 0;

	// Accessors
	virtual int				AddString(bool bIsServer, const char* szValue, int iLength = -1, const void* pUserData = 0) = 0;

	virtual const char*		GetString(int nString) = 0;
	virtual void			SetStringUserData(int nString, int iLength, const void* pUserData) = 0;
	virtual const void*		GetStringUserData(int nString, int* iLength) = 0;
	virtual int				FindStringIndex(char const* szString) = 0;

	virtual void			SetStringChangedCallback(void* pObject, StringChangedFn changeFunc) = 0;
};

class INetworkContainer
{
public:
	INetworkStringTable* FindTable(const char* szTableName)
	{
		return MEM::CallVFunc<INetworkStringTable*>(this, 3, szTableName);
	}
};
