#pragma once
// used: callvfunc
#include "../../utilities/memory.h"
// used: eclassindex
#include "../const.h"

// @source: master/public/cdll_int.h

// forward declarations
class IClientNetworkable;

using CreateClientClassFn_t = IClientNetworkable*(Q_CDECL*)(int, int);
using CreateEventFn_t = IClientNetworkable*(Q_CDECL*)();
using DemoCustomDataCallbackFn_t = void(Q_CDECL*)(unsigned char*, std::size_t);

#pragma pack(push, 4)
// @source: master/public/client_class.h
class CClientClass
{
public:
	CreateClientClassFn_t fnCreate; // 0x00
	CreateEventFn_t fnCreateEvent; // 0x04
	char* szNetworkName; // 0x08
	RecvTable_t* pRecvTable; // 0x0C
	CClientClass* pNext; // 0x10
	EClassIndex nClassID; // 0x14
};
static_assert(sizeof(CClientClass) == 0x18);
#pragma pack(pop)

class IBaseClientDll : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	CClientClass* GetAllClasses()
	{
		return CallVFunc<CClientClass*, 8U>(this);
	}

	bool DispatchUserMessage(int nMessageType, unsigned int nFlags, int nSize, const void* pMessageData)
	{
		return CallVFunc<bool, 38U>(this, nMessageType, nFlags, nSize, pMessageData);
	}

	CStandartRecvProxies* GetStandardRecvProxies()
	{
		return CallVFunc<CStandartRecvProxies*, 48U>(this);
	}
};
