#pragma once
// @source: master/public/materialsystem/itexture.h

// forward declarations
struct IDirect3DTexture9;

struct Texture_t
{
	std::byte pad0[0xC]; // 0x00
	IDirect3DTexture9* lpRawTexture; // 0x0C
};
static_assert(sizeof(Texture_t) == 0x10);

class ITexture : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
private:
	std::byte pad0[0x50]; // 0x00
public:
	Texture_t** pTextureHandles; // 0x50

	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 0U>(this);
	}

	[[nodiscard]] int GetActualWidth() const
	{
		return CallVFunc<int, 3U>(this);
	}

	[[nodiscard]] int GetActualHeight() const
	{
		return CallVFunc<int, 4U>(this);
	}

	void IncrementReferenceCount() const
	{
		CallVFunc<void, 10U>(this);
	}

	void DecrementReferenceCount() const
	{
		CallVFunc<void, 11U>(this);
	}
};
static_assert(sizeof(ITexture) == 0x54);
