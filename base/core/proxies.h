#pragma once
#include "../common.h"

// used: recvprop hook setup/destroy, recvproxydata
#include "netvar.h"

/*
 * PROXIES
 * - networkable property proxy function swap
 */
namespace P
{
	bool Setup();
	void Destroy();

	/* @section: handlers */
	void BaseViewModelSequence(const CRecvProxyData* pData, void* pStruct, void* pOut);

	/* @section: managers */
	inline CRecvPropHook hkBaseViewModelSequence;
}
