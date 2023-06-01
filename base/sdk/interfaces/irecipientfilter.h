#pragma once
#include "../datatypes/utlvector.h"

// @source: master/engine/enginesingleuserfilter.h

class IRecipientFilter
{
public:
	virtual ~IRecipientFilter() { }

	virtual bool IsReliable() const = 0;
	virtual bool IsInitMessage() const = 0;
	virtual int GetRecipientCount() const = 0;
	virtual int GetRecipientIndex(int iSlot) const = 0;
};
static_assert(sizeof(IRecipientFilter) == 0x4);

#pragma pack(push, 4)
class CEngineRecipientFilter : public IRecipientFilter
{
public:
	[[nodiscard]] bool IsReliable() const override
	{
		return bReliable;
	}

	[[nodiscard]] bool IsInitMessage() const override
	{
		return bInit;
	}

	[[nodiscard]] int GetRecipientCount() const override
	{
		return vecRecipients.Count();
	}

	[[nodiscard]] int GetRecipientIndex(int iSlot) const override
	{
		if (iSlot < 0 || iSlot >= GetRecipientCount())
			return -1;

		return vecRecipients[iSlot];
	}

public:
	bool bInit; // 0x04
	bool bReliable; // 0x05
	CUtlVector<int> vecRecipients; // 0x08
};
static_assert(sizeof(CEngineRecipientFilter) == 0x1C); // size verify @ida: (CEngineSoundServer::StopSound) [stack allocated] engine.dll -> U8["83 EC ? 56 8B F1 C7 45 E4" + 0x3]
#pragma pack(pop)
