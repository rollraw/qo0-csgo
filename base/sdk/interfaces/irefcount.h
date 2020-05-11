#pragma once

class IRefCounted
{
public:
	virtual int AddReference() = 0;
	virtual int Release() = 0;
private:
	volatile long vlRefCount;
};
