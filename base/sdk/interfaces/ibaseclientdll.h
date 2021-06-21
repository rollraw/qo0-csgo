#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/cdll_int.h

class IClientNetworkable;
using CreateClientClassFn = IClientNetworkable*(__cdecl*)(int, int);
using CreateEventFn = IClientNetworkable*(__cdecl*)();

class CStandartRecvProxies;
struct RecvTable_t;
enum class EClassIndex;

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/client_class.h
class CBaseClient
{
public:
	CreateClientClassFn		pCreateFn;
	CreateEventFn			pCreateEventFn;
	char*					szNetworkName;
	RecvTable_t*			pRecvTable;
	CBaseClient*			pNext;
	EClassIndex				nClassID;
};

using CreateInterfaceFn = void*(__cdecl*)(const char*, int*);
using InstantiateInterfaceFn = void*(__cdecl*)();
using DemoCustomDataCallbackFn = void(__cdecl*)(unsigned char*, std::size_t);

class IBaseClientDll
{
public:
	CBaseClient* GetAllClasses()
	{
		return MEM::CallVFunc<CBaseClient*>(this, 8);
	}

	bool DispatchUserMessage(int iMessageType, unsigned int nFlags, int iSize, const void* pMessageData)
	{
		return MEM::CallVFunc<bool>(this, 38, iMessageType, nFlags, iSize, pMessageData);
	}

	CStandartRecvProxies* GetStandardRecvProxies()
	{
		return MEM::CallVFunc<CStandartRecvProxies*>(this, 48);
	}
};
