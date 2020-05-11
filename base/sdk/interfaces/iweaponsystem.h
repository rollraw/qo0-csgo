#pragma once

class IWeaponSystem
{
public:
	CCSWeaponData* GetWeaponData(short nItemDefinitionIndex)
	{
		return MEM::CallVFunc<CCSWeaponData*>(this, 2, nItemDefinitionIndex);
	}
};
