#pragma once
#include "../datatypes/utlvector.h"
#include "../datatypes/utlmap.h"
#include "../datatypes/utlstring.h"
#include "../datatypes/bitvec.h"
#include "../datatypes/color.h"

// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/game/shared/econ/econ_item_system.h
// master/game/shared/econ/econ_item_schema.h
// master/game/shared/cstrike15/cstrike15_item_system.h
// master/game/shared/cstrike15/cstrike15_item_schema.h

using ItemID_t = std::uint64_t;
using ItemDefinitionIndex_t = std::uint16_t;
using AttributeDefinitionIndex_t = std::uint16_t;
using AttributeValue_t = std::uint32_t;

#pragma region econitemsystem_definitions
// @source: master/game/shared/econ/econ_item_constants.h

// current item level range
#define MIN_ITEM_LEVEL 0
#define MAX_ITEM_LEVEL 100

// maximum number of attributes allowed on a single item
#define MAX_ATTRIBUTES_PER_ITEM 32
// the maximum length of a single attribute's description, divide by locchar_t, so we can ensure 192 bytes, whether that's 128 wchars on client or 256 utf-8 bytes on gc
#define MAX_ATTRIBUTE_DESCRIPTION_LENGTH (256 / sizeof(locchar_t))

// the maximum length of an item's name
#define MAX_ITEM_NAME_LENGTH 128
#define MAX_ITEM_DESC_LENGTH 256
#define MAX_ITEM_LONG_DESC_LENGTH 2048

// for custom user-naming of econ items
#define MAX_ITEM_CUSTOM_NAME_LENGTH 40
#define MAX_ITEM_CUSTOM_NAME_DATABASE_SIZE ((4 * MAX_ITEM_CUSTOM_NAME_LENGTH) + 1) // ensures we can store MAX_ITEM_CUSTOM_NAME_LENGTH characters worth of obscure unicode characters in UTF8
#define MAX_ITEM_CUSTOM_DESC_LENGTH 80
#define MAX_ITEM_CUSTOM_DESC_DATABASE_SIZE ((4 * MAX_ITEM_CUSTOM_DESC_LENGTH) + 1)

// max length in the DB for claim codes
#define MAX_CLAIM_CODE_LENGTH 128

// @source: master/game/shared/cstrike15/cstrike15_item_constants.h

// the total number of loadouts to track for each player
#define LOADOUT_COUNT (2+2)

#define CSGO_EXP_PER_LEVEL 5000
#define CSGO_EXP_MAX_LEVEL 40
#pragma endregion

#pragma region econitemsystem_enumerations
enum EItemCapabilityFlags : int
{
	ITEM_CAP_NONE = 0,
	ITEM_CAP_PAINTABLE = (1 << 0), // some items are tagged in CS:GO schema, but we don't use it
	ITEM_CAP_NAMEABLE = (1 << 1), // used in CS:GO on all weapons that can get a name tag
	ITEM_CAP_DECODABLE = (1 << 2), // used in CS:GO on supply crates containers
	ITEM_CAP_CAN_DELETE = (1 << 3), // used in CS:GO on supply crates containers
	ITEM_CAP_CAN_CUSTOMIZE_TEXTURE = (1 << 4), // NOT USED
	ITEM_CAP_USABLE = (1 << 5), // NOT USED
	ITEM_CAP_USABLE_GC = (1 << 6), // some items are tagged in CS:GO schema, but we don't use it
	ITEM_CAP_CAN_GIFT_WRAP = (1 << 7), // NOT USED
	ITEM_CAP_USABLE_OUT_OF_GAME = (1 << 8), // some items are tagged in CS:GO schema, but we don't use it
	ITEM_CAP_CAN_COLLECT = (1 << 9), // NOT USED
	ITEM_CAP_CAN_CRAFT_COUNT = (1 << 10), // NOT USED
	ITEM_CAP_CAN_CRAFT_MARK = (1 << 11), // NOT USED
	ITEM_CAP_PAINTABLE_TEAM_COLORS = (1 << 12), // NOT USED
	ITEM_CAP_CAN_BE_RESTORED = (1 << 13), // NOT USED
	ITEM_CAP_CAN_USE_STRANGE_PARTS = (1 << 14), // NOT USED
	ITEM_CAP_PAINTABLE_UNUSUAL = (1 << 15), // NOT USED
	ITEM_CAP_CAN_INCREMENT = (1 << 16), // NOT USED
	ITEM_CAP_USES_ESSENCE = (1 << 17), // NOT USED
	ITEM_CAP_AUTOGRAPH = (1 << 18), // NOT USED
	ITEM_CAP_RECIPE = (1 << 19), // NOT USED
	ITEM_CAP_CAN_STICKER = (1 << 20), // used in CS:GO on sticker tools, primary and secondary weapons
	ITEM_CAP_STATTRACK_SWAP = (1 << 21), // used in CS:GO on stattrack items
	ITEM_CAP_CAN_PATCH = (1 << 22), // used in CS:GO on agents
	ITEM_CAP_DEFAULT = ITEM_CAP_NONE,
	ITEM_CAP_TOOL_DEFAULT = ITEM_CAP_NONE
};

enum EAttributeColors : int
{
	ATTRIB_COL_LEVEL = 0,
	ATTRIB_COL_NEUTRAL,
	ATTRIB_COL_POSITIVE,
	ATTRIB_COL_NEGATIVE,
	ATTRIB_COL_ITEMSET_NAME,
	ATTRIB_COL_ITEMSET_EQUIPPED,
	ATTRIB_COL_ITEMSET_MISSING,
	ATTRIB_COL_BUNDLE_ITEM,
	ATTRIB_COL_LIMITED_USE,
	ATTRIB_COL_ITEM_FLAGS,
	ATTRIB_COL_RARITY_DEFAULT,
	ATTRIB_COL_RARITY_COMMON,
	ATTRIB_COL_RARITY_UNCOMMON,
	ATTRIB_COL_RARITY_RARE,
	ATTRIB_COL_RARITY_MYTHICAL,
	ATTRIB_COL_RARITY_LEGENDARY,
	ATTRIB_COL_RARITY_ANCIENT,
	ATTRIB_COL_RARITY_IMMORTAL,
	ATTRIB_COL_RARITY_ARCANA,
	ATTRIB_COL_STRANGE,
	ATTRIB_COL_UNUSUAL,
	ATTRIB_COL_COUNT
};

// coloring for attribute lines
enum EAttributeEffectTypes : int
{
	ATTRIB_EFFECT_NEUTRAL = 0,
	ATTRIB_EFFECT_POSITIVE,
	ATTRIB_EFFECT_NEGATIVE,
	ATTRIB_EFFECT_COUNT
};

// slots for items within loadouts
enum ELoadoutPositions : int
{
	LOADOUT_POSITION_INVALID = -1,

	LOADOUT_POSITION_MELEE = 0,
	LOADOUT_POSITION_C4 = 1,
	LOADOUT_POSITION_FIRST_AUTO_BUY_WEAPON = LOADOUT_POSITION_MELEE,
	LOADOUT_POSITION_LAST_AUTO_BUY_WEAPON = LOADOUT_POSITION_C4,

	LOADOUT_POSITION_SECONDARY0 = 2,
	LOADOUT_POSITION_SECONDARY1 = 3,
	LOADOUT_POSITION_SECONDARY2 = 4,
	LOADOUT_POSITION_SECONDARY3 = 5,
	LOADOUT_POSITION_SECONDARY4 = 6,
	LOADOUT_POSITION_SECONDARY5 = 7,

	LOADOUT_POSITION_SMG0 = 8,
	LOADOUT_POSITION_SMG1 = 9,
	LOADOUT_POSITION_SMG2 = 10,
	LOADOUT_POSITION_SMG3 = 11,
	LOADOUT_POSITION_SMG4 = 12,
	LOADOUT_POSITION_SMG5 = 13,

	LOADOUT_POSITION_RIFLE0 = 14,
	LOADOUT_POSITION_RIFLE1 = 15,
	LOADOUT_POSITION_RIFLE2 = 16,
	LOADOUT_POSITION_RIFLE3 = 17,
	LOADOUT_POSITION_RIFLE4 = 18,
	LOADOUT_POSITION_RIFLE5 = 19,

	LOADOUT_POSITION_HEAVY0 = 20,
	LOADOUT_POSITION_HEAVY1 = 21,
	LOADOUT_POSITION_HEAVY2 = 22,
	LOADOUT_POSITION_HEAVY3 = 23,
	LOADOUT_POSITION_HEAVY4 = 24,
	LOADOUT_POSITION_HEAVY5 = 25,
	LOADOUT_POSITION_FIRST_WHEEL_WEAPON = LOADOUT_POSITION_SECONDARY0,
	LOADOUT_POSITION_LAST_WHEEL_WEAPON = LOADOUT_POSITION_HEAVY5,

	LOADOUT_POSITION_FIRST_WHEEL_GRENADE,
	LOADOUT_POSITION_GRENADE0 = LOADOUT_POSITION_FIRST_WHEEL_GRENADE,
	LOADOUT_POSITION_GRENADE1,
	LOADOUT_POSITION_GRENADE2,
	LOADOUT_POSITION_GRENADE3,
	LOADOUT_POSITION_GRENADE4,
	LOADOUT_POSITION_GRENADE5,
	LOADOUT_POSITION_LAST_WHEEL_GRENADE = LOADOUT_POSITION_GRENADE5,

	LOADOUT_POSITION_EQUIPMENT0,
	LOADOUT_POSITION_EQUIPMENT1,
	LOADOUT_POSITION_EQUIPMENT2,
	LOADOUT_POSITION_EQUIPMENT3,
	LOADOUT_POSITION_EQUIPMENT4,
	LOADOUT_POSITION_EQUIPMENT5,
	LOADOUT_POSITION_FIRST_WHEEL_EQUIPMENT = LOADOUT_POSITION_EQUIPMENT0,
	LOADOUT_POSITION_LAST_WHEEL_EQUIPMENT = LOADOUT_POSITION_EQUIPMENT5,

	LOADOUT_POSITION_CLOTHING_APPEARANCE,
	LOADOUT_POSITION_CLOTHING_TORSO,
	LOADOUT_POSITION_CLOTHING_LOWERBODY,
	LOADOUT_POSITION_CLOTHING_HANDS = 41,
	LOADOUT_POSITION_CLOTHING_HAT,
	LOADOUT_POSITION_CLOTHING_FACEMASK,
	LOADOUT_POSITION_CLOTHING_EYEWEAR,
	LOADOUT_POSITION_CLOTHING_CUSTOMHEAD,
	LOADOUT_POSITION_CLOTHING_CUSTOMPLAYER,
	LOADOUT_POSITION_MISC0,
	LOADOUT_POSITION_MISC1,
	LOADOUT_POSITION_MISC2,
	LOADOUT_POSITION_MISC3,
	LOADOUT_POSITION_MISC4,
	LOADOUT_POSITION_MISC5,
	LOADOUT_POSITION_MISC6,
	LOADOUT_POSITION_FIRST_COSMETIC = LOADOUT_POSITION_CLOTHING_APPEARANCE,
	LOADOUT_POSITION_LAST_COSMETIC = LOADOUT_POSITION_MISC6,

	LOADOUT_POSITION_MUSICKIT = 54,
	LOADOUT_POSITION_FLAIR0 = 55,
	LOADOUT_POSITION_SPRAY0 = 56,
	LOADOUT_POSITION_FIRST_ALL_CHARACTER = LOADOUT_POSITION_MUSICKIT,
	LOADOUT_POSITION_LAST_ALL_CHARACTER = LOADOUT_POSITION_SPRAY0,

	LOADOUT_POSITION_COUNT
};
#pragma endregion

// forward declarations
class CKeyValues;
class CCStrike15ItemDefinition;
class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

// @source: master/game/shared/econ/game_item_schema.h
using GameItemDefinition_t = CCStrike15ItemDefinition;
using GameItemSchema_t = CCStrike15ItemSchema;
using GameItemSystem_t = CCStrike15ItemSystem;

#pragma pack(push, 4)
struct ItemListEntry_t
{
	int nItemDefinitionIndex; // 0x00
	int nPaintKit; // 0x04
	int iPaintKitSeed; // 0x08
	float flPaintKitWear; // 0x0C
	std::uint32_t nStickerKit; // 0x10
	std::uint32_t nMusicKit; // 0x14
	bool bIsNestedList; // 0x18
	bool bIsUnusualList; // 0x19
	mutable bool bAlreadyUsedInRecursiveCreation; // 0x1A
};
static_assert(sizeof(ItemListEntry_t) == 0x1C); // size verify @ida: client.dll -> U8["89 75 FC 83 C3 ? FF 50" + 0x5]

class CEconItemRarityDefinition
{
public:
	int nValue; // 0x00
	EAttributeColors nAttributeColor; // 0x04
	CUtlString strName; // 0x08
	CUtlString strLocKey; // 0x18
	CUtlString strWeaponLocKey; // 0x28
	CUtlString strLootList; // 0x38
	CUtlString strRecycleLootList; // 0x48
	CUtlString strDropSound; // 0x58
	CUtlString strNextRarity; // 0x68
	int iWhiteCount; // 0x78
	int iBlackCount; // 0x7C
	float flWeight; // 0x80
};
static_assert(sizeof(CEconItemRarityDefinition) == 0x84);

class CEconItemQualityDefinition
{
public:
	int nValue; // 0x00
	CUtlConstString strName; // 0x04
	std::uint32_t nWeight; // 0x08
	bool bExplicitMatchesOnly; // 0x0C
	bool bCanSupportSet; // 0xD
	CUtlConstString	strHexColor; // 0x10
};
static_assert(sizeof(CEconItemQualityDefinition) == 0x14);

// functions used to verify offsets:
// @ida CPaintKit::InitFromKeyValues(): client.dll -> "55 8B EC 83 EC 28 53 56 8B 75 0C B8"
class CPaintKit
{
public:
	enum
	{
		NUM_COLORS = 4
	};

	int nIndex; // 0x00
	CUtlString strName; // 0x04
	CUtlString strDescriptionTag; // 0x14
	CUtlString strItemName; // 0x24
	CUtlString strMaterialName; // 0x34
	CUtlString strPattern; // 0x44
	CUtlString strNormal; // 0x54
	CUtlString strLogoMaterial; // 0x64
	bool bBaseDiffuseOverride; // 0x74
	int iRarity; // 0x78
	int iStyle; // 0x7C
	Color_t arrColors[NUM_COLORS]; // 0x80
	Color_t arrLogoColors[NUM_COLORS]; // 0x90
	float flWearDefault; // 0xA0
	float flWearRemapMin; // 0xA4
	float flWearRemapMax; // 0xA8
	unsigned char nFixedSeed; // 0xAC
	unsigned char uchPhongExponent; // 0xAD
	unsigned char uchPhongAlbedoBoost; // 0xAE
	unsigned char uchPhongIntensity; // 0xAF
	float flPatternScale; // 0xB0
	float flPatternOffsetXStart; // 0xB4
	float flPatternOffsetXEnd; // 0xB8
	float flPatternOffsetYStart; // 0xBC
	float flPatternOffsetYEnd; // 0xC0
	float flPatternRotateStart; // 0xC4
	float flPatternRotateEnd; // 0xC8
	float flLogoScale; // 0xCC
	float flLogoOffsetX; // 0xD0
	float flLogoOffsetY; // 0xD4
	float flLogoRotation; // 0xD8
	bool bIgnoreWeaponSizeScale; // 0xDC
	int nViewModelExponentOverrideSize; // 0xE0
	bool bOnlyFirstMaterial; // 0xE4
	float flPearlescent; // 0xE8
	CUtlString strVmtPath; // 0xEC
	CKeyValues* pVmtOverrides; // 0xFC
};
static_assert(sizeof(CPaintKit) == 0x100); // size verify @ida: (CEconItemSchema::BInitPaintKits) client.dll -> ["68 ? ? ? ? 8B 08 8B 01 FF 50 04 85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 8B D8 89 5D F4" + 0x1]

// functions used to verify offsets:
// @ida CStickerKit::InitFromKeyValues(): client.dll -> "55 8B EC 81 EC ? ? ? ? B8 ? ? ? ? 53 56 8B"
class CStickerKit
{
public:
	int nIndex; // 0x00
	int iRarity; // 0x04
	CUtlString strName; // 0x08
	CUtlString strDescriptionTag; // 0x18
	CUtlString strItemName; // 0x28
	CUtlString strMaterialPath; // 0x38
	CUtlString strMaterialPathNoDrips; // 0x48
	CUtlString strInventoryImage; // 0x58
	int nEventID; // 0x68
	int nEventTeamID; // 0x6C
	int nPlayerID; // 0x70
	bool bMaterialPathIsAbsolute; // 0x74
	bool bCannotTrade; // 0x75 // @ida: client.dll -> U8["0F 95 C0 88 47 ? 85 DB" + 0x5] @xref: "cannot_trade"
	float flRotateStart; // 0x78
	float flRotateEnd; // 0x7C
	float flScaleMin; // 0x80
	float flScaleMax; // 0x84
	float flWearMin; // 0x88
	float flWearMax; // 0x8C
	CUtlString strIconURLSmall; // 0x90
	std::byte pad0[0x10]; // 0xA0
	CUtlString strIconURLLarge; // 0xB0
	std::byte pad1[0x10]; // 0xC0
	CKeyValues* pKVItem; // 0xD0
	CKeyValues* pKVMaterial; // 0xD4
};
static_assert(sizeof(CStickerKit) == 0xD8); // size verify @ida: (CEconItemSchema::BInitStickerKits) client.dll -> ["68 ? ? ? ? 8B 08 8B 01 FF 50 04 85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 8B D8 85 DB" + 0x1]

class CMusicKit
{
public:
	int nIndex; // 0x00
	const char* szName; // 0x04
	const char* szItemName; // 0x08
	const char* szDescription; // 0x0C
	const char* szPedestalDisplayModel; // 0x10
	const char* szInventoryImage; // 0x14
	const char* szIconURLSmall; // 0x18
	const char* szIconURLLarge; // 0x1C
};
static_assert(sizeof(CMusicKit) == 0x20);

struct AlternateIconData_t
{
	CUtlString strSimpleName; // 0x00
	CUtlString strLargeSimpleName; // 0x10
	CUtlString strIconURLSmall; // 0x20
	CUtlString strIconURLLarge; // 0x30
	std::byte pad0[0x1C]; // 0x40
};
static_assert(sizeof(AlternateIconData_t) == 0x5C); // @todo: size verify

class IEconItemAttributeDefinition : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	[[nodiscard]] AttributeDefinitionIndex_t GetDefinitionIndex() const
	{
		return CallVFunc<AttributeDefinitionIndex_t, 0U>(this);
	}

	[[nodiscard]] const char* GetDefinitionName() const
	{
		return CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] const char* GetDescriptionString() const
	{
		return CallVFunc<const char*, 2U>(this);
	}

	[[nodiscard]] const char* GetAttributeClass() const
	{
		return CallVFunc<const char*, 3U>(this);
	}

	[[nodiscard]] CKeyValues* GetRawDefinition() const
	{
		return CallVFunc<CKeyValues*, 4U>(this);
	}

public:
	[[nodiscard]] Q_INLINE bool IsStoredAsInteger() const
	{
		return bStoredAsInteger;
	}

	[[nodiscard]] Q_INLINE bool IsStoredAsFloat() const
	{
		return !bStoredAsInteger;
	}

private:
	void* pVTable; // 0x00
public:
	CKeyValues* pKVAttribute; // 0x04 // the raw keyvalues for this attribute definition
	std::uint16_t nDefinitionIndex; // 0x08
	const void* pAttributeType; // 0x0C
	bool bHidden; // 0x10 // if true, this attribute isn't shown in the item description
	bool bWebSchemaOutputForced; // 0x11 // if true, this attribute's description is always output in web api calls regardless of the hidden flag
	bool bStoredAsInteger; // 0x12 // whether or not the value is stored as an integer in the DB
	bool bInstanceData; // 0x13 // if true, the attribute is counted as "instance" data for purposes of asset class in the Steam Economy. non-instance properties are considered things that can differentiate items at a fundamental level (ie., definition index, quality); instance properties are more things like additional customizations -- score for strange items, paint color, etc
	int	nAssetClassAttrExportRule; // 0x14
	std::uint32_t nAssetClassBucket; // 0x18 // if this is set then attribute value is bucketed when exported for asset class
	EAttributeEffectTypes nEffectType; // 0x1C // overall positive/negative effect. used to color the attribute
	int iDescriptionFormat; // 0x20 // contains the description format & string for this attribute
	const char* szDescriptionString; // 0x24
	const char* szDescriptionTag; // 0x28
	const char* szArmoryDescription; // 0x2C // contains information on how to describe items with this attribute in the Armory
	int iScore; // 0x30
	const char* szDefinitionName; // 0x34 // used to allow unique items to specify attributes by name
	const char* szAttributeClass; // 0x38 // the class name of this attribute. used in creation, and to hook the attribute into the actual code that uses it
	mutable const char* iszAttributeClass; // 0x3C // same as the above, but used for fast lookup when applying attributes
};
static_assert(sizeof(IEconItemAttributeDefinition) == 0x40); // size verify @ida: client.dll -> U8["6A ? 8B 08 8B 01 FF 50 04 8B C8 85 C9 74 67" + 0x1]

class IEconLootListDefinition : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 0U>(this);
	}

	[[nodiscard]] const CUtlVector<ItemListEntry_t>& GetLootListContents() const
	{
		return CallVFunc<const CUtlVector<ItemListEntry_t>&, 1U>(this);
	}

	[[nodiscard]] float GetWeight(int nIndex) const
	{
		return CallVFunc<float, 2U>(this, nIndex);
	}

	[[nodiscard]] int GetAdditionalDropCount() const
	{
		return CallVFunc<int, 5U>(this);
	}

	[[nodiscard]] bool GetAdditionalDrop(int nIndex, CUtlString& strLootList, float& flChance) const
	{
		return CallVFunc<bool, 6U>(this, nIndex, &strLootList, &flChance);
	}

	[[nodiscard]] int GetRandomAttributeGroupCount() const
	{
		return CallVFunc<int, 7U>(this);
	}

	[[nodiscard]] bool GetRandomAttributeGroup(int nIndex, float& flChance, float& flTotalWeight) const
	{
		return CallVFunc<bool, 8U>(this, nIndex, &flChance, &flTotalWeight);
	}

	[[nodiscard]] int GetRandomAttributeCount(int iGroup) const
	{
		return CallVFunc<int, 9U>(this, iGroup);
	}

	[[nodiscard]] bool GetRandomAttribute(int iGroup, int nIndex, float& flWeight, int& iValue, int& nDefinitionIndex) const
	{
		return CallVFunc<bool, 10U>(this, iGroup, nIndex, &flWeight, &iValue, &nDefinitionIndex);
	}
};

class IEconItemSetDefinition : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 0U>(this);
	}

	[[nodiscard]] const char* GetLocKey() const
	{
		return CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] const char* GetUnlocalizedName() const
	{
		return CallVFunc<const char*, 2U>(this);
	}

	[[nodiscard]] int GetBundle() const
	{
		return CallVFunc<int, 3U>(this);
	}

	[[nodiscard]] int GetItemCount() const
	{
		return CallVFunc<int, 4U>(this);
	}

	[[nodiscard]] int GetItemDefinitionIndex(int nIndex) const
	{
		return CallVFunc<int, 5U>(this, nIndex);
	}

	[[nodiscard]] int GetItemPaintKit(int nIndex) const
	{
		return CallVFunc<int, 6U>(this, nIndex);
	}

	[[nodiscard]] ItemDefinitionIndex_t GetCraftReward() const
	{
		return CallVFunc<ItemDefinitionIndex_t, 7U>(this);
	}

	[[nodiscard]] int GetItemRarity(int nIndex) const
	{
		return CallVFunc<int, 8U>(this, nIndex);
	}

	[[nodiscard]] int GetHighestItemRarityValue() const
	{
		return CallVFunc<int, 9U>(this);
	}
};

// functions used to verify offsets:
// @ida CEconItemDefinition::BInitFromKV(): "55 8B EC 83 E4 F8 81 EC 44 01 00 00 53 8B 5D"
class IEconItemDefinition : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	[[nodiscard]] ItemDefinitionIndex_t GetDefinitionIndex() const
	{
		return CallVFunc<ItemDefinitionIndex_t, 0U>(this);
	}

	[[nodiscard]] const char* GetItemBaseName() const
	{
		return CallVFunc<const char*, 2U>(this);
	}

	[[nodiscard]] const char* GetItemTypeName() const
	{
		return CallVFunc<const char*, 3U>(this);
	}

	[[nodiscard]] const char* GetItemDescription() const
	{
		return CallVFunc<const char*, 4U>(this);
	}

	[[nodiscard]] const char* GetInventoryImage() const
	{
		return CallVFunc<const char*, 5U>(this);
	}

	[[nodiscard]] const char* GetBasePlayerDisplayModel() const
	{
		return CallVFunc<const char*, 6U>(this);
	}

	[[nodiscard]] const char* GetWorldDisplayModel() const
	{
		return CallVFunc<const char*, 7U>(this);
	}

	[[nodiscard]] const char* GetExtraWearableModel() const
	{
		return CallVFunc<const char*, 8U>(this);
	}

	[[nodiscard]] int GetLoadoutSlot() const
	{
		return CallVFunc<int, 9U>(this);
	}

	[[nodiscard]] CKeyValues* GetRawDefinition() const
	{
		return CallVFunc<CKeyValues*, 10U>(this);
	}

	[[nodiscard]] int GetHeroID() const
	{
		return CallVFunc<int, 11U>(this);
	}

	[[nodiscard]] std::uint8_t GetRarity() const
	{
		return CallVFunc<std::uint8_t, 12U>(this);
	}

	[[nodiscard]] const CUtlVector<int>& GetItemSets() const
	{
		return CallVFunc<const CUtlVector<int>&, 13U>(this);
	}

	[[nodiscard]] int GetBundleItemCount() const
	{
		return CallVFunc<int, 14U>(this);
	}

	[[nodiscard]] int GetBundleItem(int nIndex) const
	{
		return CallVFunc<int, 15U>(this, nIndex);
	}

	[[nodiscard]] bool IsBaseItem() const
	{
		return CallVFunc<bool, 16U>(this);
	}

	[[nodiscard]] bool IsPublicItem() const
	{
		return CallVFunc<bool, 17U>(this);
	}

	[[nodiscard]] bool IsBundle() const
	{
		return CallVFunc<bool, 18U>(this);
	}

	[[nodiscard]] bool IsPackBundle() const
	{
		return CallVFunc<bool, 19U>(this);
	}

	[[nodiscard]] bool IsPackItem() const
	{
		return CallVFunc<bool, 20U>(this);
	}

public:
	std::byte pad0[0x268]; // 0x0000
};
static_assert(sizeof(IEconItemDefinition) == 0x268);

// functions used to verify offsets:
// @ida CCStrike15ItemDefinition::BInitFromKV(): client.dll -> "55 8B EC 83 E4 F8 81 EC 4C 01 00 00 53 56 57 FF"
class CCStrike15ItemDefinition : public IEconItemDefinition
{
public:
	int iDefaultLoadoutSlot; // 0x0268
	int iItemGearSlot; // 0x026C
	int iItemGearSlotPosition; // 0x0270
	int iAnimationSlot; // 0x0274
	const char* szPlayerDisplayModel[LOADOUT_COUNT]; // 0x0278
	CBitVec<LOADOUT_COUNT> vecClassUsability; // 0x0288
	int iLoadoutSlots[LOADOUT_COUNT]; // 0x028C
	bool bIsSupplyCrate : 1; // 0x029C
	bool bItemSharesEquipSlot : 1; // 0x029C
	std::byte pad2[0x8 + 0x3]; // 0x02A0
};
static_assert(sizeof(CCStrike15ItemDefinition) == 0x2A8); // size verify @ida: client.dll -> ["68 ? ? ? ? 8B 08 8B 01 FF 50 04 8B F0 85 F6 74 28 8B CE" + 0x2]

class IEconItemSchema : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	[[nodiscard]] CKeyValues* GetRawDefinition() const
	{
		return CallVFunc<CKeyValues*, 2U>(this);
	}

	[[nodiscard]] int GetItemDefinitionCount()
	{
		return CallVFunc<int, 3U>(this);
	}

	[[nodiscard]] IEconItemDefinition* GetItemDefinitionInterface(int nDefinitionIndex)
	{
		return CallVFunc<IEconItemDefinition*, 4U>(this, nDefinitionIndex);
	}

	[[nodiscard]] IEconItemDefinition* GetDefaultItemDefinitionInterface()
	{
		return CallVFunc<IEconItemDefinition*, 5U>(this);
	}

	[[nodiscard]] int GetLoadoutSlotCount()
	{
		return CallVFunc<int, 6U>(this);
	}

	[[nodiscard]] const char* GetLoadoutSlotName(int iSlot)
	{
		return CallVFunc<const char*, 7U>(this, iSlot);
	}

	[[nodiscard]] int GetLoadoutSlot(const char* szSlotName)
	{
		return CallVFunc<int, 8U>(this, szSlotName);
	}

	[[nodiscard]] int GetCharacterSlotType(int iCharacter, int iSlot)
	{
		return CallVFunc<int, 9U>(this, iCharacter, iSlot);
	}

	[[nodiscard]] int GetCharacterID(const char* szCharacterName)
	{
		return CallVFunc<int, 10U>(this, szCharacterName);
	}

	[[nodiscard]] int GetCharacterCount()
	{
		return CallVFunc<int, 11U>(this);
	}

	[[nodiscard]] const char* GetCharacterName(int iCharacter)
	{
		return CallVFunc<const char*, 12U>(this, iCharacter);
	}

	[[nodiscard]] const char* GetCharacterModel(int iCharacter)
	{
		return CallVFunc<const char*, 13U>(this, iCharacter);
	}

	[[nodiscard]] int GetQualityDefinitionCount()
	{
		return CallVFunc<int, 18U>(this);
	}

	[[nodiscard]] const char* GetQualityName(std::uint8_t iQuality)
	{
		return CallVFunc<const char*, 19U>(this, iQuality);
	}

	[[nodiscard]] int GetQualityIndex(const char* szQualityName)
	{
		return CallVFunc<int, 20U>(this, szQualityName);
	}

	[[nodiscard]] int GetRarityDefinitionCount() const
	{
		return CallVFunc<int, 21U>(this);
	}

	[[nodiscard]] const char* GetRarityName(std::uint8_t iRarity)
	{
		return CallVFunc<const char*, 22U>(this, iRarity);
	}

	[[nodiscard]] const char* GetRarityLocKey(std::uint8_t iRarity)
	{
		return CallVFunc<const char*, 23U>(this, iRarity);
	}

	[[nodiscard]] const char* GetRarityColor(std::uint8_t iRarity)
	{
		return CallVFunc<const char*, 24U>(this, iRarity);
	}

	[[nodiscard]] const char* GetRarityLootList(std::uint8_t iRarity)
	{
		return CallVFunc<const char*, 25U>(this, iRarity);
	}

	[[nodiscard]] int GetRarityLootList(const char* szRarityName)
	{
		return CallVFunc<int, 26U>(this, szRarityName);
	}

	[[nodiscard]] const IEconItemAttributeDefinition* GetAttributeDefinitionInterface(int nAttributeIndex) const
	{
		return CallVFunc<const IEconItemAttributeDefinition*, 27U>(this, nAttributeIndex);
	}

	[[nodiscard]] int GetItemSetCount() const
	{
		return CallVFunc<int, 28U>(this);
	}

	[[nodiscard]] const IEconItemSetDefinition* GetItemSet(int nIndex) const
	{
		return CallVFunc<const IEconItemSetDefinition*, 29U>(this, nIndex);
	}

	[[nodiscard]] const IEconLootListDefinition* GetLootListInterfaceByName(const char* szListName, int* pnIndex = nullptr)
	{
		return CallVFunc<const IEconLootListDefinition*, 31U>(this, szListName, pnIndex);
	}

	[[nodiscard]] const IEconLootListDefinition* GetLootListInterfaceByIndex(int nIndex) const
	{
		return CallVFunc<const IEconLootListDefinition*, 32U>(this, nIndex);
	}

	[[nodiscard]] int GetLootListIndex(const char* szListName)
	{
		return CallVFunc<int, 33U>(this, szListName);
	}

	[[nodiscard]] int GetLootListInterfaceCount() const
	{
		return CallVFunc<int, 34U>(this);
	}

	[[nodiscard]] IEconItemDefinition* GetItemDefinitionByName(const char* szDefinitionName)
	{
		return CallVFunc<IEconItemDefinition*, 41U>(this, szDefinitionName);
	}

	[[nodiscard]] const IEconItemDefinition* GetItemDefinitionByName(const char* szDefinitionName) const
	{
		return CallVFunc<const IEconItemDefinition*, 42U>(this, szDefinitionName);
	}

public:
	// @todo: arg could be replaced with hash | but depends on use case e.g. if u gonna use it with econitemdefiface->getname() then its better to keep it like that
	const IEconItemAttributeDefinition* GetAttributeDefinitionByName(const char* szDefinitionName) const
	{
		// @ida CEconItemSchema::GetAttributeDefinitionByName(): client.dll -> ABS["E8 ? ? ? ? 89 46 F4" + 0x1]

		for (int i = 0; i < vecAttributesContainer.Count(); i++)
		{
			if (const IEconItemAttributeDefinition* pItemAttributeDefinition = vecAttributesContainer[i]; pItemAttributeDefinition != nullptr && CRT::StringCompare(pItemAttributeDefinition->GetDefinitionName(), szDefinitionName) == 0)
				return pItemAttributeDefinition;
		}

		return nullptr;
	}

public:
	std::byte pad0[0x64]; // 0x0000
	CUtlMap<int, CEconItemRarityDefinition, int, CDefLess<int>> mapRarities; // 0x0064 // @ida: client.dll -> U8["8D 78 ? 89 7D 0C" + 0x2] @xref: "Duplicate rarity value (%d)"
	CUtlMap<int, CEconItemQualityDefinition, int, CDefLess<int>> mapQualities; // 0x0088 // @ida: client.dll -> ["8D 99 ? ? ? ? 89 5C 24 14" + 0x2] @xref: "Duplicate quality value (%d)"
	std::byte pad1[0x48]; // 0x00AC
	CUtlMap<int, IEconItemDefinition*, int, CDefLess<int>> mapItemsSorted; // 0x00F4
	IEconItemDefinition* pDefaultItemDefinition; // 0x0118
	CUtlVector<IEconItemAttributeDefinition*> vecAttributesContainer; // 0x011C // @ida: client.dll -> ["8B 87 ? ? ? ? 8B 0C B0 85 C9 74 31" + 0x2]
	std::byte pad2[0x48]; // 0x0130
	CUtlMap<int, const char*, int, CDefLess<int>> mapRevolvingLootLists; // 0x0178
	std::byte pad3[0x80]; // 0x019C
	CUtlMap<std::uint64_t, AlternateIconData_t, int, CDefLess<std::uint64_t>> mapAlternateIcons; // 0x021C // @ida: client.dll -> ["8B 83 ? ? ? ? 8B 75 10 83 C0 18" + 0x2] - 0x4 @xref: "Duplicate alternate icon definition '%s' (%llu)"
	std::byte pad4[0x48]; // 0x0240
	CUtlMap<int, CPaintKit*, int, CDefLess<int>> mapPaintKits; // 0x0288
	CUtlMap<int, CStickerKit*, int, CDefLess<int>> mapStickerKits; // 0x02AC // @ida: client.dll -> ["50 8D 8F ? ? ? ? E8 ? ? ? ? 8D 0C 40" + 0x3] @xref: "Duplicate sticker kit ID %d"
	std::byte pad5[0x11C]; // 0x02D0
	CUtlMap<int, CMusicKit*, int, CDefLess<int>> mapMusicKits; // 0x03EC
	std::byte pad6[0x334]; // 0x0410
	bool bSchemaParsingItems; // 0x0744 // @ida: client.dll -> ["89 86 ? ? ? ? C6 86 ? ? ? ? ? E8 ? ? ? ?" + 0x2]
};
static_assert(sizeof(IEconItemSchema) == 0x748);

class CCStrike15ItemSchema : public IEconItemSchema
{
public:
	std::byte pad7[0x24]; // 0x0748
	CUtlVector<const char*> vecClassUsabilityStrings; // 0x076C
	CUtlVector<const char*> vecLoadoutStrings;// 0x0780
	CUtlVector<const char*> vecLoadoutStringsSubPositions; // 0x0794
	CUtlVector<const char*> vecLoadoutStringsForDisplay; // 0x07A8
	std::byte pad8[0x14]; // 0x07BC
	CUtlVector<const char*> vecWeaponTypeSubstrings; // 0x07D0
};
static_assert(sizeof(CCStrike15ItemSchema) == 0x7E4);

class IEconItemSystem : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	[[nodiscard]] IEconItemSchema* GetItemSchemaInterface()
	{
		return CallVFunc<IEconItemSchema*, 0U>(this);
	}

private:
	void* pVTable; // 0x0000
public:
	GameItemSchema_t itemSchema; // 0x0004
};
static_assert(sizeof(IEconItemSystem) == 0x7E8); // size verify @ida: client.dll -> [ABS["E8 ? ? ? ? 50 56 B9" + 0x1] + 0x1E]

class CCStrike15ItemSystem : public IEconItemSystem
{
public:
};
static_assert(sizeof(CCStrike15ItemSystem) == 0x7E8); // size verify @ida: client.dll -> [ABS["E8 ? ? ? ? 50 56 B9" + 0x1] + 0x1E]
#pragma pack(pop)
