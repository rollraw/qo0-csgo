#pragma once
// @source: master/public/engine/IClientLeafSystem.h
// master/game/client/clientleafsystem.cpp
// master/game/client/clientleafsystem.h

#pragma region clientleafsystem_enumerations
enum ERenderGroup : int
{
	RENDER_GROUP_OPAQUE = 0,
	RENDER_GROUP_TRANSLUCENT,
	RENDER_GROUP_TRANSLUCENT_IGNOREZ,
	RENDER_GROUP_COUNT
};

enum ERenderFlags : unsigned int
{
	RENDER_FLAGS_DISABLE_RENDERING = 0x01,
	RENDER_FLAGS_HASCHANGED = 0x02,
	RENDER_FLAGS_ALTERNATE_SORTING = 0x04,
	RENDER_FLAGS_RENDER_WITH_VIEWMODELS = 0x08,
	RENDER_FLAGS_BLOAT_BOUNDS = 0x10,
	RENDER_FLAGS_BOUNDS_VALID = 0x20,
	RENDER_FLAGS_BOUNDS_ALWAYS_RECOMPUTE = 0x40,
	RENDER_FLAGS_IS_SPRITE = 0x80,
	RENDER_FLAGS_FORCE_OPAQUE_PASS = 0x100
};

enum ERenderableModelType : int
{
	RENDERABLE_MODEL_UNKNOWN_TYPE = -1,
	RENDERABLE_MODEL_ENTITY = 0,
	RENDERABLE_MODEL_STUDIOMDL,
	RENDERABLE_MODEL_STATIC_PROP,
	RENDERABLE_MODEL_BRUSH,
};
#pragma endregion

// forward declarations
enum ERenderableTranslucencyType : int;
class IClientRenderable;
class IClientAlphaProperty;

#pragma pack(push, 4)
struct RenderableInfo_t
{
	IClientRenderable* pRenderable; // 0x00
	IClientAlphaProperty* pAlphaProperty; // 0x04
	int nEnumCount; // 0x08
	int nRenderFrame; // 0x0C
	unsigned short uFirstShadow; // 0x10
	unsigned short uLeafList; // 0x12
	short shArea; // 0x14
	std::uint16_t uFlags; // 0x16
	std::uint16_t bRenderInFastReflection : 1; // 0x18
	std::uint16_t bDisableShadowDepthRendering : 1; // 0x18
	std::uint16_t bDisableCSMRendering : 1; // 0x18
	std::uint16_t bDisableShadowDepthCaching : 1; // 0x18
	std::uint16_t nSplitscreenEnabled : 2; // 0x18
	std::uint16_t nTranslucencyType : 2; // 0x18 // ERenderableTranslucencyType
	std::uint16_t nModelType : 8; // 0x19 // ERenderableModelType // @ida: client.dll -> ["8B 4E 24 8A 44 F9 ? 3C 04" + 0x7]
	Vector_t vecBloatedAbsMins; // 0x1C
	Vector_t vecBloatedAbsMaxs; // 0x28
	Vector_t vecAbsMins; // 0x34
	Vector_t vecAbsMaxs; // 0x40
};
static_assert(sizeof(RenderableInfo_t) == 0x4C);
#pragma pack(pop)

class IClientLeafSystem : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	void CreateRenderableHandle(IClientRenderable* pRenderable, bool bRenderWithViewModels, ERenderableTranslucencyType nTranslucencyType, ERenderableModelType nModelType, std::uint32_t uSplitscreenEnabled = 0xFFFFFFFF)
	{
		CallVFunc<void, 0U>(this, pRenderable, bRenderWithViewModels, nTranslucencyType, nModelType, uSplitscreenEnabled);
	}

	void AddRenderable(IClientRenderable* pRenderable, bool bIsStaticProp, ERenderableTranslucencyType nTranslucencyType, ERenderableModelType nModelType, std::uint32_t uSplitscreenEnabled = 0xFFFFFFFF)
	{
		CallVFunc<void, 7U>(this, pRenderable, bIsStaticProp, nTranslucencyType, nModelType, uSplitscreenEnabled);
	}
};
