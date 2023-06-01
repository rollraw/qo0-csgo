#pragma once
#include "../datatypes/qangle.h"
#include "../datatypes/utllinkedlist.h"

// used: cenginerecipientfilter
#include "irecipientfilter.h"

using SerializedEntityHandle_t = std::intptr_t;
#define SERIALIZED_ENTITY_HANDLE_INVALID static_cast<SerializedEntityHandle_t>(0)

#pragma region clientstate_definitions
#define MAX_AREA_STATE_BYTES 32
#define MAX_AREA_PORTAL_STATE_BYTES 24

#define	MAX_DEMOS 32
#define	MAX_DEMONAME 32
#pragma endregion

#pragma region clientstate_enumerations
enum ESignonState
{
	SIGNONSTATE_NONE = 0, // no state yet; about to connect
	SIGNONSTATE_CHALLENGE = 1, // client challenging server; all OOB packets
	SIGNONSTATE_CONNECTED = 2, // client is connected to server; netchans ready
	SIGNONSTATE_NEW = 3, // just got serverinfo and string tables
	SIGNONSTATE_PRESPAWN = 4, // received signon buffers
	SIGNONSTATE_SPAWN = 5, // ready to receive entity packets
	SIGNONSTATE_FULL = 6, // we are fully connected; first non-delta packet received
	SIGNONSTATE_CHANGELEVEL = 7 // server is changing level; please wait
};
#pragma endregion

// forward declarations
class INetChannel;
using CRC32_t = std::uint32_t;

#pragma pack(push, 4)
class CClockDriftMgr
{
public:
	enum
	{
		NUM_CLOCKDRIFT_SAMPLES = 16 // this controls how much it smoothes out the samples from the server
	};

	float arrClockOffsets[NUM_CLOCKDRIFT_SAMPLES]; // 0x00 // this holds how many ticks the client is ahead each time we get a server tick. we average these together to get our estimate of how far ahead we are
	int iCurrentClockOffset; // 0x40
	int nServerTick; // 0x44 // last-received tick from the server
	int	nClientTick; // 0x48 // the client's own tick counter (specifically, for interpolation during rendering). the server may be on a slightly different tick and the client will drift towards it
};
static_assert(sizeof(CClockDriftMgr) == 0x4C);

// functions used to verify offsets:
// @ida CBaseClient::IgnoreTempEntity(): engine.dll -> "55 8B EC 8B 41 04 83 C1"
// @ida CClientState::SVCMsg_TempEntities(): engine.dll -> "55 8B EC 83 E4 F8 83 EC 4C A1 ? ? ? ? 80" @xref: "CL_QueueEvent: missing client receive table for %s.\n"
class CEventInfo
{
public:
	short nClassID; // 0x00 // 0 implies not in use
	float flFireDelay; // 0x04 // if non-zero, the delay time when the event should be fired (fixed up on the client)
	const void* pSendTable; // 0x08
	const CClientClass* pClientClass; // 0x0C
	SerializedEntityHandle_t hPacked; // 0x10 // @ida: engine.dll -> U8["8B 4F ? 8B D0 E8" + 0x2]
	int nFlags; // 0x14
	CEngineRecipientFilter filter; // 0x18 // clients that see that event // @ida: engine.dll -> U8["55 8B EC 8B 41 04 83 C1" + 0xC]
};
static_assert(sizeof(CEventInfo) == 0x34); // size verify @ida: engine.dll -> U8["66 83 3F 00 8B 47 ? 89 45 F8" + 0x6] - 0x4

// @source: master/engine/baseclientstate.h
class IBaseClientState
{
public:
	std::byte pad0[0x98]; // 0x0000 // network message binders
	int iSocket; // 0x0098 // network socket
	INetChannel* pNetChannel; // 0x009C // our sequenced channel to the remote server // @ida: engine.dll -> ["8B 8F ? ? ? ? 8D 55 A8" + 0x2]
	bool bSplitScreenUser; // 0x00A0
	unsigned int nChallengeNr; // 0x00A4 // connection challenge number
	double dConnectTime; // 0x00A8
	int nRetryNumber; // 0x00B0
	int nRetryMax; // 0x00B4
	std::byte pad1[0x50]; // 0x00B8
	int nSignonState; // 0x0108
	std::byte pad2[0x4]; // 0x010C
	double dNextCmdTime; // 0x0110
	int nServerCount; // 0x0118
	int iCurrentSequence; // 0x011C
	std::uint64_t ullGameServerSteamID; // 0x0120
	CClockDriftMgr clockDriftMgr; // 0x0128
	int nDeltaTick; // 0x0174 // @ida: engine.dll -> ["83 BF ? ? ? ? ? 0F 84 ? ? ? ? 8D" + 0x2]
	bool bPaused; // 0x0178
	int iViewEntity; // 0x017C
	int iPlayerSlot; // 0x0180
	int iSplitScreenSlot; // 0x0184
	char szLevelName[MAX_PATH]; // 0x0188
	char szLevelNameShort[80]; // 0x028C // removes maps/ and .bsp extension
	char szMapGroupName[80]; // 0x02DC // the name of the map group we are currently playing in
	char szLastLevelNameShort[80]; // 0x032C // stores the previous value of 'szLevelNameShort' when that gets cleared
	std::byte pad3[0x8]; // 0x037C
	int nMaxClients; // 0x0380 // max clients on server
	int nPlayersToConnect; // 0x0384 // number of clients to connect to server
	std::byte pad4[0x494C]; // 0x038C
	//int nServerProtocolVersion; // 0x4CC4 // @ida: engine.dll -> ["8B 87 ? ? ? ? 85 C0 8B" + 0x2]
	//bool bServerConnectionRedirect; // 0x4CCC // @ida: engine.dll -> ["80 BE ? ? ? ? ? 74 13 8B 44" + 0x2]

public:
	// force game to clear cache and reset delta tick
	void ForceFullUpdate()
	{
		// @ida CBaseClientState::ForceFullUpdate(): engine.dll -> "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34" @xref: "Requesting full game update (%s)...\n", "cl_fullupdate command"

		// check wasn't it already called this tick
		if (nDeltaTick != -1)
		{
			FreeEntityBaselines();
			nDeltaTick = -1;
		}
	}

	// clear cached packed entity network data to request full packet update
	void FreeEntityBaselines()
	{
		static auto fnFreeEntityBaselines = reinterpret_cast<void(Q_THISCALL*)(IBaseClientState*)>(MEM::FindPattern(ENGINE_DLL, Q_XOR("55 8B EC 83 EC 08 53 56 57 8B F9 8B 8F ? ? ? ? 85 C9 74")));
		fnFreeEntityBaselines(this);
	}
};
static_assert(sizeof(IBaseClientState) == 0x4CD8); // size verify @ida: engine.dll -> ["E9 ? ? ? ? 8B 01 FF 60 58 83 E9 04" + 0x11]

class CClientFrameManager
{
private:
	std::byte pad5[0x38]; // 0x00
};
static_assert(sizeof(CClientFrameManager) == 0x38);

// @source: master/engine/client.h
// functions used to verify offsets:
// @ida CL_Move(): engine.dll -> "55 8B EC 81 EC ? ? ? ? 53 56 8A F9" @xref: "Requesting full game update (%s)...\n", "@%.03f:choke(%.05f)"
// @ida CL_FireEvents(): engine.dll -> "55 8B EC 83 EC 08 53 8B 1D ? ? ? ? 56 57 83" @xref: "Failed to execute event for classId %i\n"
// @ida CL_DispatchSound(): engine.dll -> "55 8B EC 81 EC ? ? ? ? 56 8B F1 8D 4D 98" @xref: "Entity '%d' created the late load.\n"
class IClientState : public IBaseClientState, public CClientFrameManager
{
public:
	std::byte pad6[0x8]; // 0x4D10
	float flLastServerTickTime; // 0x4D18 // @ida: engine.dll -> ["F3 0F 10 80 ? ? ? ? A1" + 0x4]
	bool bInSimulation; // 0x4D1C
	int nOldTickCount; // 0x4D20
	float flTickRemainder; // 0x4D24
	float flFrameTime; // 0x4D28
	int iLastOutgoingCommand; // 0x4D2C
	int nChokedCommands; // 0x4D30 // @ida: engine.dll -> ["C7 87 ? ? ? ? ? ? ? ? 75 6B" + 0x2]
	int iLastCommandAck; // 0x4D34
	int iLastServerTick; // 0x4D38
	int iCommandAck; // 0x4D3C
	int iSoundSequence; // 0x4D40
	int iLastProgressPercent; // 0x4D44
	bool bIsHLTV; // 0x4D48
	bool bIsReplay; // 0x4D49
	CRC32_t uServerCRC; // 0x4D4C // @ida: engine.dll -> ["FF B0 ? ? ? ? 57 68" + 0x2]
	CRC32_t uServerClientSideDllCRC; // 0x4D50
	unsigned char chAreaBits[32]; // 0x4D54
	unsigned char chAreaPortalBits[24]; // 0x4D74 // @ida: engine.dll -> ["C6 80 ? ? ? ? ? 5D C2" + 0x2]
	bool bAreaBitsValid; // 0x4D8C
	QAngle_t angViewPoint; // 0x4D90
	std::byte pad7[0xC4]; // 0x4D9C
	CUtlFixedLinkedList<CEventInfo> vecEvents; // 0x4E60 // @ida: engine.dll -> ["8D 8B ? ? ? ? E8 ? ? ? ? 8B 45 F8" + 0x2]
	std::byte pad8[0x4A474]; // 0x4E8C
	//bool bCheckCRCsWithServer; // 0x4F2CC // @ida: engine.dll -> ["C6 87 ? ? ? ? ? E8 ? ? ? ? 8D" + 0x2]
	//bool bMarkedCRCsUnverified; // 0x4F2FC // @ida: engine.dll -> ["C6 87 ? ? ? ? ? 8B 01 FF 90 ? ? ? ? C6" + 0x2]
};
static_assert(sizeof(IClientState) == 0x4F300); // size verify @ida: engine.dll -> ["E9 ? ? ? ? 83 E9 04 E9 ? ? ? ? 81 E9" + 0x11]
#pragma pack(pop)
