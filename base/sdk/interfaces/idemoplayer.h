#pragma once

class DemoPlaybackParameter_t
{
private:
	std::byte		pad0[0x10];		// 0x0000
public:
	std::uint32_t	bIsOverwatch;	// 0x0010
private:
	std::byte		pad1[0x88];		// 0x0014
}; // size = 0x009C

class CDemoFile
{
public:
	char* GetHeaderID()
	{
		return (char*)((std::uintptr_t)this + 0x108);
	}
};

class IDemoPlayer
{
public:
	CDemoFile* GetDemoFile()
	{
		return *(CDemoFile**)((std::uintptr_t)this + 0x4);
	}

	DemoPlaybackParameter_t* GetPlaybackParameter()
	{
		return *(DemoPlaybackParameter_t**)((std::uintptr_t)this + 0x5C8);
	}

	bool IsInOverwatch()
	{
		DemoPlaybackParameter_t* pPlaybackParameter = GetPlaybackParameter();
		return (pPlaybackParameter != nullptr && pPlaybackParameter->bIsOverwatch);
	}

	void SetOverwatchState(bool bState)
	{
		DemoPlaybackParameter_t* pPlaybackParameter = GetPlaybackParameter();
		if (pPlaybackParameter != nullptr)
			pPlaybackParameter->bIsOverwatch = bState;
	}

	bool IsPlayingDemo()
	{
		return MEM::CallVFunc<bool>(this, 4);
	}

	bool IsPlayingTimeDemo()
	{
		return MEM::CallVFunc<bool>(this, 6);
	}
};
