#pragma once
#include "../datatypes/vector.h"
#include "../datatypes/utlvector.h"

// used: callvfunc
#include "../../utilities/memory.h"

// @source: master/public/engine/IEngineSound.h

#pragma region enginesound_enumerations
enum
{
	CHAN_REPLACE = -1,
	CHAN_AUTO = 0,
	CHAN_WEAPON = 1,
	CHAN_VOICE = 2,
	CHAN_ITEM = 3,
	CHAN_BODY = 4,
	CHAN_STREAM = 5, // allocate stream channel from the static or dynamic area
	CHAN_STATIC = 6, // allocate channel from the static area
	CHAN_VOICE2 = 7,
	CHAN_VOICE_BASE = 8, // allocate channel for network voice data
	CHAN_USER_BASE = (CHAN_VOICE_BASE + 128) // anything >= this number is allocated to game code
};

enum ESoundLevel : int
{
	SNDLVL_NONE = 0,
	SNDLVL_20dB = 20,				// rustling leaves
	SNDLVL_25dB = 25,				// whispering
	SNDLVL_30dB = 30,				// library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,				// refrigerator
	SNDLVL_50dB = 50,		// 3.9	// average home
	SNDLVL_55dB = 55,		// 3.0

	SNDLVL_IDLE = 60,		// 2.0
	SNDLVL_60dB = 60,		// 2.0	// normal conversation, clothes dryer
	SNDLVL_65dB = 65,		// 1.5	// washing machine, dishwasher

	SNDLVL_STATIC = 66,		// 1.25
	SNDLVL_70dB = 70,		// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,		// 0.8	// busy traffic
	SNDLVL_80dB = 80,		// 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile

	SNDLVL_TALKING = 80,	// 0.7
	SNDLVL_85dB = 85,		// 0.6	// average factory, electric shaver
	SNDLVL_90dB = 90,		// 0.5	// screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,		// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105,				// helicopter, power mower
	SNDLVL_110dB = 110,				// snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120,				// auto horn, propeller aircraft
	SNDLVL_130dB = 130,				// air raid siren

	SNDLVL_GUNFIRE = 140,	// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140,		// 0.2
	SNDLVL_150dB = 150,		// 0.2
	SNDLVL_180dB = 180				// rocket launching
};

enum ESoundFlags : int
{
	SND_NOFLAGS = 0,
	SND_CHANGE_VOL = (1 << 0),
	SND_CHANGE_PITCH = (1 << 1),
	SND_STOP = (1 << 2),
	SND_SPAWNING = (1 << 3),
	SND_DELAY = (1 << 4),
	SND_STOP_LOOPING = (1 << 5),
	SND_SPEAKER = (1 << 6),
	SND_SHOULDPAUSE = (1 << 7),
	SND_IGNORE_PHONEMES = (1 << 8),
	SND_IGNORE_NAME = (1 << 9),
	SND_DO_NOT_OVERWRITE_EXISTING_ON_CHANNEL = (1 << 10)
};
#pragma endregion

#pragma region enginesound_definitions
// volume
#define VOL_NORM			1.0f

// attenuation
#define ATTN_NONE			0.0f
#define ATTN_NORM			0.8f
#define ATTN_IDLE			2.0f
#define ATTN_STATIC			1.25f
#define ATTN_RICOCHET		1.5f
#define ATTN_GUNFIRE		0.27f
#define MAX_ATTENUATION		3.98f

// soundlevel limits
#define MAX_SNDLVL_BITS		9
#define MIN_SNDLVL_VALUE	0
#define MAX_SNDLVL_VALUE	((1 << MAX_SNDLVL_BITS) - 1)

// soundlevel
#define SNDLEVEL_TO_COMPATIBILITY_MODE(x) static_cast<ESoundLevel>(static_cast<int>(x + 256))
#define SNDLEVEL_FROM_COMPATIBILITY_MODE(x) static_cast<ESoundLevel>(static_cast<int>(x - 256))
#define SNDLEVEL_IS_COMPATIBILITY_MODE(x) (x) >= 256

// convertation
#define ATTN_TO_SNDLVL(a) static_cast<ESoundLevel>(static_cast<int>(a) ? (50 + 20 / (static_cast<float>(a))) : 0)
#define SNDLVL_TO_ATTN(a) ((a > 50) ? (20.0f / static_cast<float>(a - 50)) : 4.0)

// sound
#define SND_FLAG_BITS_ENCODE 11

#define MAX_SOUND_DELAY_MSEC_ENCODE_BITS 13
#define MAX_SOUND_INDEX_BITS 14
#define MAX_SOUNDS (1 << MAX_SOUND_INDEX_BITS)
#define MAX_SOUND_DELAY_MSEC (1 << (MAX_SOUND_DELAY_MSEC_ENCODE_BITS - 1)) // 4096 msec or about 4 seconds

// sound source
#define SOUND_FROM_UI_PANEL (-2)
#define SOUND_FROM_LOCAL_PLAYER (-1)
#define SOUND_FROM_WORLD (0)

// pitch
#define PITCH_LOW 95
#define	PITCH_NORM 100
#define PITCH_HIGH 120

#define DEFAULT_SOUND_PACKET_VOLUME 1.0f
#define DEFAULT_SOUND_PACKET_PITCH 100
#define DEFAULT_SOUND_PACKET_DELAY 0.0f
#pragma endregion

// forward declarations
class CSfxTable; // not implemented
class IRecipientFilter;

#pragma pack(push, 4)
struct SoundInfo_t
{
	int nGuid; // 0x00
	FileNameHandle_t hFileName; // 0x04
	int nSoundSource; // 0x08
	int iChannel; // 0x0C
	int nSpeakerEntity; // 0x10
	float flVolume; // 0x14
	float flLastSpatializedVolume; // 0x18
	float flRadius; // 0x1C
	int iPitch; // 0x20
	Vector_t* vecOrigin; // 0x24
	Vector_t* vecDirection; // 0x28
	bool bUpdatePositions; // 0x2C
	bool bIsSentence; // 0x2D
	bool bDryMix; // 0x2E
	bool bSpeaker; // 0x2F
	bool bSpecialDSP; // 0x30
	bool bFromServer; // 0x31
};
static_assert(sizeof(SoundInfo_t) == 0x34);

struct StartSoundParams_t
{
	bool bStaticSound; // 0x00
	int iUserData; // 0x04
	int iSoundSource; // 0x08
	int iEntityChannel; // 0x0C
	CSfxTable* pSfx; // 0x10
	Vector_t vecOrigin; // 0x14
	Vector_t vecDirection; // 0x20
	bool bUpdatePositions; // 0x2C
	float flVolume; // 0x30
	ESoundLevel soundLevel; // 0x34
	int iFlags; // 0x38
	int iPitch; // 0x3C
	bool bFromServer; // 0x40
	float flDelay; // 0x44
	int nSpeakerEntity; // 0x48
	bool bSuppressRecording; // 0x4C
	int nInitialStreamPosition; // 0x50
};
static_assert(sizeof(StartSoundParams_t) == 0x54);
#pragma pack(pop)

class IEngineSound : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	void GetActiveSounds(CUtlVector<SoundInfo_t>& vecSoundList)
	{
		CallVFunc<void, 19U>(this, &vecSoundList);
	}
};
