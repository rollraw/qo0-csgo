#pragma once
// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: gameitemdefinition_t
#include "interfaces/ieconitemsystem.h"

// @source: master/game/shared/econ/localization_provider.h
using locchar_t = wchar_t;

#pragma region econitem_enumerations
enum EEconItemOrigin : int
{
	ECON_ITEM_ORIGIN_INVALID = -1,
	ECON_ITEM_ORIGIN_DROP = 0,
	ECON_ITEM_ORIGIN_ACHIEVEMENT,
	ECON_ITEM_ORIGIN_PURCHASED,
	ECON_ITEM_ORIGIN_TRADED,
	ECON_ITEM_ORIGIN_CRAFTED,
	ECON_ITEM_ORIGIN_STOREPROMOTION,
	ECON_ITEM_ORIGIN_GIFTED,
	ECON_ITEM_ORIGIN_SUPPORTGRANTED,
	ECON_ITEM_ORIGIN_FOUNDINCRATE,
	ECON_ITEM_ORIGIN_EARNED,
	ECON_ITEM_ORIGIN_THIRDPARTYPROMOTION,
	ECON_ITEM_ORIGIN_GIFTWRAPPED,
	ECON_ITEM_ORIGIN_HALLOWEENDROP,
	ECON_ITEM_ORIGIN_PACKAGEITEM,
	ECON_ITEM_ORIGIN_FOREIGN,
	ECON_ITEM_ORIGIN_CDKEY,
	ECON_ITEM_ORIGIN_COLLECTIONREWARD,
	ECON_ITEM_ORIGIN_PREVIEWITEM,
	ECON_ITEM_ORIGIN_STEAMWORKSHOPCONTRIBUTION,
	ECON_ITEM_ORIGIN_PERIODICSCOREREWARD,
	ECON_ITEM_ORIGIN_RECYCLING,
	ECON_ITEM_ORIGIN_TOURNAMENTDROP,
	ECON_ITEM_ORIGIN_STOCKITEM,
	ECON_ITEM_ORIGIN_QUESTREWARD,
	ECON_ITEM_ORIGIN_LEVELUPREWARD,
	ECON_ITEM_ORIGIN_MAX
};

enum EEconItemQuality : int
{
	ECON_ITEM_QUALITY_INVALID = -1,
	ECON_ITEM_QUALITY_NORMAL = 0,
	ECON_ITEM_QUALITY_GENUINE,
	ECON_ITEM_QUALITY_VINTAGE,
	ECON_ITEM_QUALITY_UNUSUAL,
	ECON_ITEM_QUALITY_UNIQUE,
	ECON_ITEM_QUALITY_COMMUNITY,
	ECON_ITEM_QUALITY_DEVELOPER,
	ECON_ITEM_QUALITY_SELFMADE,
	ECON_ITEM_QUALITY_CUSTOMIZED,
	ECON_ITEM_QUALITY_STRANGE,
	ECON_ITEM_QUALITY_COMPLETED,
	ECON_ITEM_QUALITY_HAUNTED,
	ECON_ITEM_QUALITY_TOURNAMENT,
	ECON_ITEM_QUALITY_FAVORED,
	ECON_ITEM_QUALITY_MAX
};

enum EEconItemRarity : int
{
	ECON_ITEM_RARITY_DEFAULT = 0,
	ECON_ITEM_RARITY_COMMON,
	ECON_ITEM_RARITY_UNCOMMON,
	ECON_ITEM_RARITY_RARE,
	ECON_ITEM_RARITY_MYTHICAL,
	ECON_ITEM_RARITY_LEGENDARY,
	ECON_ITEM_RARITY_ANCIENT,
	ECON_ITEM_RARITY_IMMORTAL
};

enum EEconItemFlags : std::uint8_t
{
	ECON_ITEM_FLAG_CANNOT_TRADE = (1 << 0),
	ECON_ITEM_FLAG_CANNOT_BE_USED_IN_CRAFTING = (1 << 1),
	ECON_ITEM_FLAG_CAN_BE_TRADED_BY_FREE_ACCOUNTS = (1 << 2),
	ECON_ITEM_FLAG_NON_ECONOMY = (1 << 3), // used for items that are meant to not interact in the economy -- these can't be traded, gift-wrapped, crafted, etc.
	ECON_ITEM_FLAG_CLIENT_STORE_ITEM = (1 << 6),
	ECON_ITEM_FLAG_CLIENT_PREVIEW = (1 << 7), // only set on the client; means "this item is being previewed"
};

enum EStickerAttributeType
{
	STICKER_ATTRIBUTE_ID,
	STICKER_ATTRIBUTE_WEAR,
	STICKER_ATTRIBUTE_SCALE,
	STICKER_ATTRIBUTE_ROTATION,
	STICKER_ATTRIBUTE_COUNT,
};
#pragma endregion

using RTime32_t = std::uint32_t;

// forward declarations
class IEconItemAttributeDefinition;

struct Attribute_t
{
	std::uint16_t nDefinitionIndex; // 0x00

	union
	{
		float flValue;
		std::uint32_t uValue;
		std::uint8_t* pValue;
	}; // 0x04
};
static_assert(sizeof(Attribute_t) == 0x8); // size verify @ida: client.dll -> U8["83 C6 ? 3B F3 72 EE 5F 5E 5B" + 0x2]

struct CustomDataOptimizedObject_t
{
	Q_CLASS_NO_INITIALIZER(CustomDataOptimizedObject_t);

	Q_INLINE Attribute_t* GetAttribute(const std::uint32_t nIndex)
	{
		return (reinterpret_cast<Attribute_t*>(this + 1)) + nIndex;
	}

	Q_INLINE const Attribute_t* GetAttribute(const std::uint32_t nIndex) const
	{
		return (reinterpret_cast<const Attribute_t*>(this + 1)) + nIndex;
	}

	std::uint16_t uEquipInstanceSlot1 : 6; // 0x00 // equip instance slot
	std::uint16_t uEquipInstanceClass1 : 3; // 0x00 // equip instance class
	std::uint16_t uEquipInstanceClass2Bit : 1;  // 0x01 // whether the item is equipped for complementary class
	std::uint16_t nAttributeCount : 6; // 0x01 // length of following attributes
};
static_assert(sizeof(CustomDataOptimizedObject_t) == 0x2);

// not return adress checked [30.09.22]
class IEconItemInterface : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	virtual ~IEconItemInterface() { }

	[[nodiscard]] int GetCustomPaintKitIndex() const
	{
		// @xref: "set item texture prefab"
		return CallVFunc<int, 1U>(this);
	}

	[[nodiscard]] int GetCustomPaintKitSeed() const
	{
		// @xref: "set item texture seed"
		return CallVFunc<int, 2U>(this);
	}

	[[nodiscard]] float GetCustomPaintKitWear(float flWearDefault = 0.0f) const
	{
		// @xref: "set item texture wear"
		return CallVFunc<float, 3U>(this, flWearDefault);
	}

	[[nodiscard]] float GetStickerAttributeBySlotIndexFloat(int nSlotIndex, EStickerAttributeType nType, float flDefault) const
	{
		return CallVFunc<float, 4U>(this, nSlotIndex, nType, flDefault);
	}

	[[nodiscard]] std::uint32_t GetStickerAttributeBySlotIndexInt(int nSlotIndex, EStickerAttributeType nType, std::uint32_t uDefault) const
	{
		return CallVFunc<std::uint32_t, 5U>(this, nSlotIndex, nType, uDefault);
	}

	[[nodiscard]] bool IsTradable() const
	{
		return CallVFunc<bool, 6U>(this);
	}

	[[nodiscard]] bool IsMarketable() const
	{
		// @xref: "-perfectworld"
		return CallVFunc<bool, 7U>(this);
	}

	[[nodiscard]] bool IsCommodity() const
	{
		// @xref: "is commodity"
		return CallVFunc<bool, 8U>(this);
	}

	[[nodiscard]] bool IsUsableInCrafting() const
	{
		// @xref: "always tradable"
		return CallVFunc<bool, 8U>(this);
	}

	[[nodiscard]] bool IsHiddenFromDropList() const
	{
		// @xref: "hide from drop list"
		return CallVFunc<bool, 9U>(this);
	}

	[[nodiscard]] RTime32_t GetExpirationDate() const
	{
		// @xref: "expiration date"
		return CallVFunc<RTime32_t, 10U>(this);
	}

	[[nodiscard]] const GameItemDefinition_t* GetItemDefinition() const
	{
		return CallVFunc<const GameItemDefinition_t*, 11U>(this);
	}

	[[nodiscard]] std::uint32_t GetAccountID() const
	{
		return CallVFunc<std::uint32_t, 12U>(this);
	}

	[[nodiscard]] ItemID_t GetItemID() const
	{
		return CallVFunc<ItemID_t, 13U>(this);
	}

	[[nodiscard]] std::int32_t GetQuality() const
	{
		return CallVFunc<std::int32_t, 14U>(this);
	}

	[[nodiscard]] std::int32_t GetRarity() const
	{
		return CallVFunc<std::int32_t, 15U>(this);
	}

	[[nodiscard]] std::uint8_t GetFlags() const
	{
		return CallVFunc<std::uint8_t, 16U>(this);
	}

	[[nodiscard]] EEconItemOrigin GetOrigin() const
	{
		return CallVFunc<EEconItemOrigin, 17U>(this);
	}

	[[nodiscard]] std::uint16_t GetQuantity() const
	{
		return CallVFunc<std::uint16_t, 18U>(this);
	}

	[[nodiscard]] std::uint32_t GetItemLevel() const
	{
		return CallVFunc<std::uint32_t, 19U>(this);
	}

	/// @returns: true if this item in use somewhere in the backend (ie., cross-game trading), false otherwise
	[[nodiscard]] bool GetInUse() const
	{
		return CallVFunc<bool, 20U>(this);
	}

	/// @returns: user-generated name, if present, otherwise null. return value is UTF8
	[[nodiscard]] const char* GetCustomName() const
	{
		return CallVFunc<const char*, 21U>(this);
	}

	/// @returns: user-generated flavor text, if present, otherwise null. return value is UTF8
	[[nodiscard]] const char* GetCustomDesc() const
	{
		return CallVFunc<const char*, 22U>(this);
	}

	[[nodiscard]] int GetItemSetIndex() const
	{
		return CallVFunc<int, 23U>(this);
	}

	/// @remarks: attribute iteration interface. this is not meant to be used for attribute lookup! this is meant for anything that requires iterating over the full attribute list
	void IterateAttributes(/*IEconItemAttributeIterator**/void* pIterator) const
	{
		CallVFunc<void, 24U>(this, pIterator);
	}
};
static_assert(sizeof(IEconItemInterface) == 0x4);

class CEconItem : public IEconItemInterface
{
public:
	virtual ~CEconItem() { }

	CEconItem()
	{
		// @ida: (CInventoryManager::Init) client.dll -> ["55 8B EC 83 EC 1C 8D 45 E4 C7 45" + 0x14] @xref: "CEconItem", "BuildCacheSubscribed(CEconItem)", "Create(CEconItem)", "Update(CEconItem)"
		static auto fnConstructor = reinterpret_cast<CEconItem*(Q_THISCALL*)(CEconItem*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("C7 01 ? ? ? ? 8B C1 C7 41 ? ? ? ? ? C7 01")));
		fnConstructor(this);
	}

	void* operator new(const std::size_t nSize)
	{
		return I::MemAlloc->Alloc(nSize);
	}

	void operator delete(void* pMemory)
	{
		I::MemAlloc->Free(pMemory);
	}

	void SetCustomName(const char* szName)
	{
		static auto fnSetCustomName = reinterpret_cast<void(Q_THISCALL*)(CEconItem*, const char*)>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, Q_XOR("E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34")) + 0x1)); // @xref: "custom name attr"
		fnSetCustomName(this, szName);
	}

	void SetCustomDesciption(const char* szDescription)
	{
		static auto fnSetCustomDesciption = reinterpret_cast<void(Q_THISCALL*)(CEconItem*, const char*)>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, Q_XOR("E8 ? ? ? ? 33 DB 39 5E 3C 7E 5E")) + 0x1)); // @xref: "custom desc attr"
		fnSetCustomDesciption(this, szDescription);
	}

	void SetDynamicAttributeValue(const IEconItemAttributeDefinition* pAttributeDefinition, const void* pValue)
	{
		// @note: this is declared as CEconItem::SetDynamicAttributeValue<unsigned int>, and there is different behaviour for Vector_t: "55 8B EC 83 E4 F8 83 EC 54 53 8B 5D 08 56 57" or strings: "55 8B EC 83 E4 F8 83 EC 5C 53 8B 5D 08 56 57 6A 00"
		static auto fnSetDynamicAttributeValue = reinterpret_cast<void(Q_THISCALL*)(CEconItem*, const IEconItemAttributeDefinition*, const void*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A 00")));
		fnSetDynamicAttributeValue(this, pAttributeDefinition, pValue);
	}

	// @todo: remove due to iface dependency?
	template <typename T>
	void SetDynamicAttributeValue(const int nAttributeIndex, const T& value)
	{
		if (const IEconItemAttributeDefinition* pAttributeDefinition = I::ItemSystem->GetItemSchemaInterface()->GetAttributeDefinitionInterface(nAttributeIndex))
			SetDynamicAttributeValue(pAttributeDefinition, value);
	}

	void RemoveDynamicAttribute(const IEconItemAttributeDefinition* pAttributeDefinition)
	{
		static auto fnRemoveDynamicAttribute = reinterpret_cast<void(Q_THISCALL*)(CEconItem*, const IEconItemAttributeDefinition*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 08 8B C1 89")));
		fnRemoveDynamicAttribute(this, pAttributeDefinition);
	}

public:
	ItemID_t ullIndex; // 0x0008
	ItemID_t ullOriginalIndex; // 0x0010
	CustomDataOptimizedObject_t* pCustomDataOptimizedObject; // 0x0018
	std::uint32_t nAccountID; // 0x001C
	std::uint32_t uInventory; // 0x0020
	std::uint16_t nDefinitionIndex; // 0x0024
	std::uint16_t uOrigin : 5; // 0x0026
	std::uint16_t uQuality : 4; // 0x0027
	std::uint16_t uLevel : 2; // 0x0027
	std::uint16_t uRarity : 4; // 0x0028
	std::uint16_t bInUse : 1; // 0x0028
	mutable std::int16_t nItemSet; // 0x002A
	mutable int iSOUpdateFrame; // 0x002C
	std::uint8_t nFlags; // 0x0030
};
static_assert(sizeof(CEconItem) == 0x38); // size verify @ida: [stack allocated] client.dll -> U8["8B 55 0C 83 EC ? 53 8B 5D 08 8B C3" + 0x5] - 0x14 | client.dll -> U8[["55 8B EC 83 EC 1C 8D 45 E4 C7 45" + 0x14] + 0x6]
