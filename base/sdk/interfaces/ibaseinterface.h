#pragma once
#include "../../common.h"

// all interfaces derive from this
class IBaseInterface
{
public:
	virtual	~IBaseInterface() { }
};
static_assert(sizeof(IBaseInterface) == 0x4);

using CreateInterfaceFn_t = void*(Q_CDECL*)(const char*, int*);
using InstantiateInterfaceFn_t = void*(Q_CDECL*)();

// used internally to register interface classes
class CInterfaceRegister
{
public:
	InstantiateInterfaceFn_t fnCreate; // 0x00
	const char* szName; // 0x04
	CInterfaceRegister* pNext; // 0x08
};
static_assert(sizeof(CInterfaceRegister) == 0xC);
