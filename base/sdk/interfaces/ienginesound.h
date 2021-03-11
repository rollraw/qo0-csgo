#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/engine/IEngineSound.h

#pragma region enginesound_enumerations
/* channel */
enum
{
	CHAN_REPLACE = -1,
	CHAN_AUTO = 0,
	CHAN_WEAPON = 1,
	CHAN_VOICE = 2,
	CHAN_ITEM = 3,
	CHAN_BODY = 4,
	CHAN_STREAM = 5,		// allocate stream channel from the static or dynamic area
	CHAN_STATIC = 6,		// allocate channel from the static area 
	CHAN_VOICE2 = 7,
	CHAN_VOICE_BASE = 8,	// allocate channel for network voice data
	CHAN_USER_BASE = (CHAN_VOICE_BASE + 128) // anything >= this number is allocated to game code.
};

enum ESoundLevel : int
{
	SNDLVL_NONE = 0,
	SNDLVL_20dB = 20,			// rustling leaves
	SNDLVL_25dB = 25,			// whispering
	SNDLVL_30dB = 30,			// library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,			// refrigerator
	SNDLVL_50dB = 50,	// 3.9	// average home
	SNDLVL_55dB = 55,	// 3.0

	SNDLVL_IDLE = 60,	// 2.0	
	SNDLVL_60dB = 60,	// 2.0	// normal conversation, clothes dryer
	SNDLVL_65dB = 65,	// 1.5	// washing machine, dishwasher

	SNDLVL_STATIC = 66,	// 1.25
	SNDLVL_70dB = 70,	// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,	// 0.8	// busy traffic
	SNDLVL_80dB = 80,	// 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile

	SNDLVL_TALKING = 80,// 0.7
	SNDLVL_85dB = 85,	// 0.6	// average factory, electric shaver
	SNDLVL_90dB = 90,	// 0.5	// screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,	// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105,			// helicopter, power mower
	SNDLVL_110dB = 110,			// snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120,			// auto horn, propeller aircraft
	SNDLVL_130dB = 130,			// air raid siren

	SNDLVL_GUNFIRE = 140,// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140,	// 0.2
	SNDLVL_150dB = 150,	// 0.2
	SNDLVL_180dB = 180,			// rocket launching
};

enum ESoundFlags : int
{
	SND_NOFLAGS = 0,				// to keep the compiler happy
	SND_CHANGE_VOL = (1 << 0),		// change sound vol
	SND_CHANGE_PITCH = (1 << 1),	// change sound pitch
	SND_STOP = (1 << 2),			// stop the sound
	SND_SPAWNING = (1 << 3),		// we're spawning, used in some cases for ambients
									// not sent over net, only a param between dll and server.
	SND_DELAY = (1 << 4),			// sound has an initial delay
	SND_STOP_LOOPING = (1 << 5),	// stop all looping sounds on the entity.
	SND_SPEAKER = (1 << 6),			// being played again by a microphone through a speaker
	SND_SHOULDPAUSE = (1 << 7),		// this sound should be paused if the game is paused
	SND_IGNORE_PHONEMES = (1 << 8),
	SND_IGNORE_NAME = (1 << 9),		// used to change all sounds emitted by an entity, regardless of scriptname
	SND_DO_NOT_OVERWRITE_EXISTING_ON_CHANNEL = (1 << 10)
};
#pragma endregion

#pragma region enginesound_definitions
/* volume */
#define VOL_NORM			1.0f

/* attenuation */
#define ATTN_NONE			0.0f
#define ATTN_NORM			0.8f
#define ATTN_IDLE			2.0f
#define ATTN_STATIC			1.25f 
#define ATTN_RICOCHET		1.5f
#define ATTN_GUNFIRE		0.27f
#define MAX_ATTENUATION		3.98f

/* soundlevel limits */
#define MAX_SNDLVL_BITS		9
#define MIN_SNDLVL_VALUE	0
#define MAX_SNDLVL_VALUE	((1 << MAX_SNDLVL_BITS) - 1)

/* soundlevel */
#define SNDLEVEL_TO_COMPATIBILITY_MODE( x )		static_cast<ESoundLevel>(static_cast<int>(x + 256))
#define SNDLEVEL_FROM_COMPATIBILITY_MODE( x )	static_cast<ESoundLevel>(static_cast<int>(x - 256))
#define SNDLEVEL_IS_COMPATIBILITY_MODE( x )		( x ) >= 256

/* convertation */
#define ATTN_TO_SNDLVL( a ) static_cast<ESoundLevel>(static_cast<int>( a ) ? (50 + 20 / (static_cast<float>( a ))) : 0)
#define SNDLVL_TO_ATTN( a ) ((a > 50) ? (20.0f / static_cast<float>(a - 50)) : 4.0)

/* sound */
#define SND_FLAG_BITS_ENCODE 11

#define MAX_SOUND_DELAY_MSEC_ENCODE_BITS	13
#define MAX_SOUND_INDEX_BITS				14
#define	MAX_SOUNDS							(1 << MAX_SOUND_INDEX_BITS)
#define MAX_SOUND_DELAY_MSEC				(1 << (MAX_SOUND_DELAY_MSEC_ENCODE_BITS - 1))    // 4096 msec or about 4 seconds

/* source */
#define SOUND_FROM_UI_PANEL			-2
#define SOUND_FROM_LOCAL_PLAYER		-1
#define SOUND_FROM_WORLD			0

/* pitch */
#define PITCH_LOW			95
#define	PITCH_NORM			100
#define PITCH_HIGH			120

#define DEFAULT_SOUND_PACKET_VOLUME 1.0f
#define DEFAULT_SOUND_PACKET_PITCH	100
#define DEFAULT_SOUND_PACKET_DELAY	0.0f
#pragma endregion

using FileNameHandle_t = void*;
struct SoundInfo_t
{
	int			nGuid;
	FileNameHandle_t hFileName;
	int			nSoundSource;
	int			iChannel;
	int			nSpeakerEntity;
	float		flVolume;
	float		flLastSpatializedVolume;
	float		flRadius;
	int			iPitch;
	Vector*		vecOrigin;
	Vector*		vecDirection;
	bool		bUpdatePositions;
	bool		bIsSentence;
	bool		bDryMix;
	bool		bSpeaker;
	bool		bSpecialDSP;
	bool		bFromServer;
};

class CSfxTable;
struct StartSoundParams_t
{
	bool			bStaticSound;
	int				iUserData;
	int				iSoundSource;
	int				iEntityChannel;
	CSfxTable*		pSfx;
	Vector			vecOrigin;
	Vector			vecDirection;
	bool			bUpdatePositions;
	float			flVolume;
	ESoundLevel		soundLevel;
	int				iFlags;
	int				iPitch;
	bool			bFromServer;
	float			flDelay;
	int				nSpeakerEntity;
	bool			bSuppressRecording;
	int				iInitialStreamPosition;
};

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() { }

	virtual bool	IsReliable() const = 0;
	virtual bool	IsInitMessage() const = 0;

	virtual int		GetRecipientCount() const = 0;
	virtual int		GetRecipientIndex(int nSlot) const = 0;
};

class IEngineSound
{
public:
	virtual bool	PrecacheSound(const char* szSample, bool bPreload = false, bool bIsUISound = false) = 0;
	virtual bool	IsSoundPrecached(const char* szSample) = 0;
	virtual void	PrefetchSound(const char* szSample) = 0;
	virtual bool	IsLoopingSound(const char* szSample) = 0;
	virtual float	GetSoundDuration(const char* szSample) = 0;

	virtual int EmitSound(IRecipientFilter& filter, int nEntityIndex, int iChannel, const char* szSoundEntry, unsigned int uSoundEntryHash, const char* szSample,
		float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
		const Vector* vecOrigin, const Vector* vecDirection, CUtlVector<Vector>* pUtlVecOrigins, bool bUpdatePositions, float flSoundTime, int nSpeakerEntity, StartSoundParams_t& parameters) = 0;

	virtual int EmitSound(IRecipientFilter& filter, int nEntityIndex, int iChannel, const char* szSoundEntry, unsigned int uSoundEntryHash, const char* szSample,
		float flVolume, ESoundLevel nSoundLevel, int nSeed, int iFlags, int iPitch,
		const Vector* vecOrigin, const Vector* vecDirection, CUtlVector<Vector>* pUtlVecOrigins, bool bUpdatePositions, float flSoundTime, int nSpeakerEntity, StartSoundParams_t& parameters) = 0;

	virtual void EmitSentenceByIndex(IRecipientFilter& filter, int nEntityIndex, int iChannel, int nSentenceIndex,
		float flVolume, ESoundLevel nSoundLevel, int nSeed, int iFlags = 0, int iPitch = PITCH_NORM,
		const Vector* vecOrigin = nullptr, const Vector* vecDirection = nullptr, CUtlVector<Vector>* pUtlVecOrigins = nullptr, bool bUpdatePositions = true, float flSoundTime = 0.0f, int nSpeakerEntity = -1) = 0;

	virtual void	StopSound(int nEntityIndex, int iChannel, const char* szSample, unsigned int uSoundEntryHash) = 0;
	virtual void	StopAllSounds(bool bClearBuffers) = 0;
	virtual void	SetRoomType(IRecipientFilter& filter, int iRoomType) = 0;
	virtual void	SetPlayerDSP(IRecipientFilter& filter, int iDspType, bool bFastReset) = 0;
	virtual void	EmitAmbientSound(const char* szSample, float flVolume, int iPitch = PITCH_NORM, int iFlags = 0, float flSoundTime = 0.0f) = 0;
	virtual float	GetDistGainFromSoundLevel(ESoundLevel soundLevel, float flDistance) = 0;
	virtual int		GetGuidForLastSoundEmitted() = 0;
	virtual bool	IsSoundStillPlaying(int nGUID) = 0;
	virtual void	StopSoundByGuid(int nGUID, bool bForceSync) = 0;
	virtual void	SetVolumeByGuid(int nGUID, float flVolume) = 0;
private:
	virtual void	function0() = 0;
public:
	virtual void	GetActiveSounds(CUtlVector<SoundInfo_t>& vecSoundList) = 0;
	virtual void	PrecacheSentenceGroup(const char* szGroupName) = 0;
	virtual void	NotifyBeginMoviePlayback() = 0;
	virtual void	NotifyEndMoviePlayback() = 0;
	virtual bool	GetSoundChannelVolume(const char* szSound, float& flVolumeLeft, float& flVolumeRight) = 0;
	virtual float	GetElapsedTimeByGuid(int nGUID) = 0;
	virtual bool	GetPreventSound() = 0;
	virtual void	SetReplaySoundFade(float flReplayVolume) = 0;
	virtual float	GetReplaySoundFade() const = 0;
};