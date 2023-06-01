#pragma once
// used: bf_read, bf_write
#include "../bitbuf.h"

enum : int
{
	FLOW_OUTGOING = 0,
	FLOW_INCOMING,
	MAX_FLOWS
};

// forward declarations
class INetMessage;
class INetMessageBinder;
class INetChannelHandler;
using netpacket_t = struct netpacket_s; // not implemented

// @source: master/public/inetchannelinfo.h
class INetChannelInfo : protected ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	enum : int
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		PAINTMAP,		// paintmap data
		ENCRYPTED,		// encrypted data
		TOTAL			// must be last and is not a real group
	};

	// get channel name
	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 0U>(this);
	}

	// get channel IP address as string
	[[nodiscard]] const char* GetAddress() const
	{
		return CallVFunc<const char*, 1U>(this);
	}

	// current net time
	[[nodiscard]] float GetTime() const
	{
		return CallVFunc<float, 2U>(this);
	}

	// get connection time in seconds
	[[nodiscard]] float GetTimeConnected() const
	{
		return CallVFunc<float, 3U>(this);
	}

	// netchannel packet history size
	[[nodiscard]] int GetBufferSize() const
	{
		return CallVFunc<int, 4U>(this);
	}

	// send data rate in byte/sec
	[[nodiscard]] int GetDataRate() const
	{
		return CallVFunc<int, 5U>(this);
	}

	// true if loopback channel
	[[nodiscard]] bool IsLoopback() const
	{
		return CallVFunc<bool, 6U>(this);
	}

	// true if timing out
	[[nodiscard]] bool IsTimingOut() const
	{
		return CallVFunc<bool, 7U>(this);
	}

	// true if demo playback
	[[nodiscard]] bool IsPlayback() const
	{
		return CallVFunc<bool, 8U>(this);
	}

	// current latency (RTT), more accurate but jittering
	[[nodiscard]] float GetLatency(int iFlow) const
	{
		return CallVFunc<float, 9U>(this, iFlow);
	}

	// average packet latency in seconds
	[[nodiscard]] float GetAvgLatency(int iFlow) const
	{
		// @ida: engine.dll -> U8["8B 40 ? FF D0 D8 44" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<float, 10U>(this, iFlow);
	}

	// average packet loss in range [0.0 .. 1.0]
	[[nodiscard]] float GetAvgLoss(int iFlow) const
	{
		// @ida: engine.dll -> U8["8B 40 ? D9 54 24 6C" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<float, 11U>(this, iFlow);
	}

	// average packet choke in range [0.0 .. 1.0]
	[[nodiscard]] float GetAvgChoke(int iFlow) const
	{
		return CallVFunc<float, 12U>(this, iFlow);
	}

	// average data flow in bytes/sec
	[[nodiscard]] float GetAvgData(int iFlow) const
	{
		// @ida: engine.dll -> U8["8B 40 ? D9 54 24 7C" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<float, 13U>(this, iFlow);
	}

	// average packets/sec
	[[nodiscard]] float GetAvgPackets(int iFlow) const
	{
		// @ida: engine.dll -> U8["8B 40 ? D9 54 24 74" + 0x2] / sizeof(std::uintptr_t)
		return CallVFunc<float, 14U>(this, iFlow);
	}

	// total flow in/out in bytes
	[[nodiscard]] int GetTotalData(int iFlow) const
	{
		return CallVFunc<int, 15U>(this, iFlow);
	}

	[[nodiscard]] int GetTotalPackets(int iFlow) const
	{
		return CallVFunc<int, 16U>(this, iFlow);
	}

	// last send sequence number
	[[nodiscard]] int GetSequenceNr(int iFlow) const
	{
		return CallVFunc<int, 17U>(this, iFlow);
	}

	// true if packet was not lost/dropped/chocked/flushed
	[[nodiscard]] bool IsValidPacket(int iFlow, int nFrame) const
	{
		return CallVFunc<bool, 18U>(this, iFlow, nFrame);
	}

	// time when packet was send
	[[nodiscard]] float GetPacketTime(int iFlow, int nFrame) const
	{
		return CallVFunc<float, 19U>(this, iFlow, nFrame);
	}

	// group size of this packet
	[[nodiscard]] int GetPacketBytes(int iFlow, int nFrame) const
	{
		return CallVFunc<int, 20U>(this, iFlow, nFrame);
	}

	// TCP progress if transmitting
	[[nodiscard]] bool GetStreamProgress(int iFlow, int nFrame) const
	{
		return CallVFunc<bool, 21U>(this, iFlow, nFrame);
	}

	// get time since last received packet in seconds
	[[nodiscard]] float GetTimeSinceLastReceived(int iFlow, int nFrame) const
	{
		return CallVFunc<float, 22U>(this, iFlow, nFrame);
	}

	[[nodiscard]] float GetCommandInterpolationAmount(int iFlow, int nFrame) const
	{
		return CallVFunc<float, 23U>(this, iFlow, nFrame);
	}

	void GetPacketResponseLatency(int iFlow, int nFrame, int* pnLatencyMsecs, int* pnChoke) const
	{
		CallVFunc<void, 24U>(this, iFlow, nFrame, pnLatencyMsecs, pnChoke);
	}

	void GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation, float* pflFrameStartTimeStdDeviation) const
	{
		CallVFunc<void, 25U>(this, pflFrameTime, pflFrameTimeStdDeviation, pflFrameStartTimeStdDeviation);
	}

	[[nodiscard]] float GetTimeoutSeconds() const
	{
		return CallVFunc<float, 26U>(this);
	}

private:
	void* pVTable; // 0x00
};
static_assert(sizeof(INetChannelInfo) == 0x4);

// @source: master/public/inetchannel.h
// master/engine/net_chan.h
#pragma pack(push, 4)
class INetChannel : public INetChannelInfo
{
public:
	bool SendNetMsg(INetMessage& message, bool bForceReliable = false, bool bVoice = false)
	{
		return CallVFunc<bool, 40U>(this, &message, bForceReliable, bVoice);
	}

	int	SendDatagram(bf_write* pDatagram)
	{
		return CallVFunc<int, 46U>(this, pDatagram);
	}

	bool Transmit(bool bOnlyReliable = false)
	{
		return CallVFunc<bool, 49U>(this, bOnlyReliable);
	}

private:
	std::byte pad0[0x10]; // 0x0000
public:
	bool bProcessingMessages; // 0x0014
	bool bShouldDelete; // 0x0015
	bool bStopProcessing; // 0x0016
	int nOutSequenceNr; // 0x0018 // last send outgoing sequence number
	int nInSequenceNr; // 0x001C // last received incoming sequence number
	int nOutSequenceNrAck; // 0x0020 // last received acknowledge outgoing sequence number
	int nOutReliableState; // 0x0024 // state of outgoing reliable data (0/1) flip flop used for loss detection
	int nInReliableState; // 0x0028 // state of incoming reliable data
	int nChokedPackets; // 0x002C // number of choked packets
private:
	std::byte pad1[0x4104]; // 0x0030
public:
	int iPacketDrop; // 0x4134 // packets lost before getting last update // @ida: engine.dll -> ["FF B3 ? ? ? ? FF 50 04" + 0x2] @xref: "%s:Dropped %i packets at %i\n"
	char szName[32]; // 0x4138 // channel name
	unsigned int nChallengeNr; // 0x4158 // unique, random challenge number
	float flTimeout; // 0x415C
	INetChannelHandler* pMessageHandler; // 0x4160 // who registers and processes messages
	CUtlVector<CUtlVector<INetMessageBinder*>> vecNetMessages; // 0x4164 // list of registered messages // @ida: engine.dll -> ["8B 9E ? ? ? ? 03" + 0x2] @xref: "shutdown netchan"
	void* pDemoRecorder; // 0x4178
	int nQueuedPackets; // 0x417C
	float flInterpolationAmount; // 0x4180
	float flRemoteFrameTime; // 0x4184
	float flRemoteFrameTimeStdDeviation; // 0x4188
	float flRemoteFrameStartTimeStdDeviation; // 0x418C
	int nMaxRoutablePayloadSize; // 0x4190
	int nSplitPacketSequence; // 0x4194
	INetChannel* pActiveChannel; // 0x4198 // for split screen support, if we get a message saying we're focusing on another client, then we need to switch to the appropriate handler objects
private:
	std::byte pad2[0x94]; // 0x419C
};
static_assert(sizeof(INetChannel) == 0x4230); // size verify @ida NET_CreateNetChannel(): engine.dll -> ["68 ? ? ? ? 8B 08 8B 01 8B 40 04 FF D0 85" + 0x1]
#pragma pack(pop)

// @credits: master/public/inetmessage.h
class INetMessage : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	virtual ~INetMessage() { }

	void SetNetChannel(INetChannel* pNetChannel)
	{
		CallVFunc<void, 1U>(this, pNetChannel);
	}

	void SetReliable(bool bState)
	{
		CallVFunc<void, 2U>(this, bState);
	}

	[[nodiscard]] bool Process()
	{
		return CallVFunc<bool, 3U>(this);
	}

	/// @returns: true if parsing was completed successfully, false otherwise
	[[nodiscard]] bool ReadFromBuffer(bf_read& buffer)
	{
		return CallVFunc<bool, 4U>(this, &buffer);
	}

	/// @returns: true if writing was completed successfully, false otherwise
	[[nodiscard]] bool WriteToBuffer(bf_write& buffer)
	{
		return CallVFunc<bool, 5U>(this, &buffer);
	}

	/// @returns: true if message needs reliable handling, false otherwise
	[[nodiscard]] bool IsReliable() const
	{
		return CallVFunc<bool, 6U>(this);
	}

	/// @note: to find those tags look through RTTI class names inside 'engine.dll' with base of 'CNetMessagePB<>' they contains message name and also type id
	/// @returns: module specific header tag, e.g. 'svc_serverinfo'
	[[nodiscard]] int GetType() const
	{
		return CallVFunc<int, 7U>(this);
	}

	/// @returns: net message group of this message
	[[nodiscard]] int GetGroup() const
	{
		return CallVFunc<int, 8U>(this);
	}

	/// @returns: network message name, e.g. "svc_serverinfo"
	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 9U>(this);
	}

	[[nodiscard]] INetChannel* GetNetChannel() const
	{
		return CallVFunc<INetChannel*, 10U>(this);
	}

	/// @returns: human readable string about message content
	[[nodiscard]] const char* ToString() const
	{
		return CallVFunc<const char*, 11U>(this);
	}

	[[nodiscard]] std::size_t GetSize() const
	{
		return CallVFunc<std::size_t, 12U>(this);
	}
};

template <typename T>
class CNetMessagePB : public INetMessage, public T { };

// @source: master/public/inetmessage.h
class INetMessageBinder
{
public:
	virtual ~INetMessageBinder() { }
	virtual int GetType() const = 0; // returns module specific header tag eg svc_serverinfo
	virtual void SetNetChannel(INetChannel* pNetChannel) = 0; // netchannel this message is from/for
	virtual INetMessage* CreateFromBuffer(bf_read& buffer) = 0;
	virtual bool Process(const INetMessage& src) = 0;
};

class INetChannelHandler
{
public:
	virtual ~INetChannelHandler() { }
	virtual void ConnectionStart(INetChannel* pChannel) = 0; // called first time network channel is established
	virtual void ConnectionStop() = 0; // called first time network channel is established
	virtual void ConnectionClosing(const char* szReason) = 0; // network channel is being closed by remote site
	virtual void ConnectionCrashed(const char* szReason) = 0; // network error occured
	virtual void PacketStart(int nIncomingSequence, int nOutgoingAcknowledged) = 0; // called each time a new packet arrived
	virtual void PacketEnd() = 0; // all messages has been parsed
	virtual void FileRequested(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // other side request a file for download
	virtual void FileReceived(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // we received a file
	virtual void FileDenied(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // a file request was denied by other side
	virtual void FileSent(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // we sent a file
	virtual bool ChangeSplitscreenUser(int nSplitScreenUserSlot) = 0; // interleaved networking used by SS system is changing the SS player slot that the subsequent messages pertain to
};

// @source: master/public/inetmsghandler.h
class IConnectionlessPacketHandler
{
public:
	virtual ~IConnectionlessPacketHandler() { }
	virtual bool ProcessConnectionlessPacket(netpacket_t* pNetPacket) = 0; // process a connectionless packet
};
