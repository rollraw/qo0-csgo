#pragma once
// used: angle
#include "../datatypes/qangle.h"

#pragma region clientmode_definitions
#define SIGNONSTATE_NONE		0	// no state yet, about to connect
#define SIGNONSTATE_CHALLENGE	1	// client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED	2	// client is connected to server, netchans ready
#define SIGNONSTATE_NEW			3	// just got serverinfo and string tables
#define SIGNONSTATE_PRESPAWN	4	// received signon buffers
#define SIGNONSTATE_SPAWN		5	// ready to receive entity packets
#define SIGNONSTATE_FULL		6	// we are fully connected, first non-delta packet received (in-game check)
#define SIGNONSTATE_CHANGELEVEL	7	// server is changing level, please wait
#pragma endregion

class CViewSetup
{
public:
	int			x;
	int			iUnscaledX;
	int			y;
	int			iUnscaledY;
	int			iWidth;
	int			iUnscaledWidth;
	int			iHeight;
	int			iUnscaledHeight;
	std::byte	pad0[0x90];
	float		flFOV;
	float		flViewModelFOV;
	Vector		vecOrigin;
	QAngle		angView;
	std::byte	pad1[0x78];
	int			iEdgeBlur;
};

class IHudChat;
class IClientModeShared
{
public:
	std::byte	pad0[0x1B];
	void*		pViewport;
	IHudChat*	pChatElement;
	HCursor		hCursorNone;
	void*		pWeaponSelection;
	int			nRootSize[2];
};

class IAppSystem
{
private:
	virtual void function0() = 0;
	virtual void function1() = 0;
	virtual void function2() = 0;
	virtual void function3() = 0;
	virtual void function4() = 0;
	virtual void function5() = 0;
	virtual void function6() = 0;
	virtual void function7() = 0;
	virtual void function8() = 0;
};

class CEventInfo
{
public:
	std::uint16_t	sClassID;		//0x00 //0 implies not in use
	std::byte		pad0[0x2];		//0x02 
	float			flFireDelay;	//0x04 //if non-zero, the delay time when the event should be fired ( fixed up on the client )
	std::byte		pad1[0x4];		//0x08
	CBaseClient*	pClientClass;	//0x0C //clienclass pointer
	void*			pData;			//0x10 //raw event data
	std::byte		pad2[0x30];		//0x14
}; //Size: 0x0044

class INetChannel;
class IBaseClientState
{
public:
	std::byte		pad0[0x9C];				//0x0000
	std::uint32_t	pNetChannel;			//0x009C
	int				iChallengeNr;			//0x00A0
	std::byte		pad1[0x64];				//0x00A4
	int				iSignonState;			//0x0108
	std::byte		pad2[0x8];				//0x010C
	float			flNextCmdTime;			//0x0114
	int				iServerCount;			//0x0118
	int				iCurrentSequence;		//0x011C
	std::byte		pad3[0x54];				//0x0120
	int				iDeltaTick;				//0x0174
	bool			bPaused;				//0x0178
	std::byte		pad4[0x7];				//0x0179
	int				iViewEntity;			//0x0180
	int				iPlayerSlot;			//0x0184
	char			szLevelName[260];		//0x0188
	char			szLevelNameShort[80];	//0x028C
	char			szGroupName[80];		//0x02DC
	std::byte		pad5[0x5C];				//0x032C
	int				iMaxClients;			//0x0388
	std::byte		pad6[0x4984];			//0x038C
	float			flLastServerTickTime;	//0x4D10
	bool			bInSimulation;			//0x4D14
	std::byte		pad7[0x3];				//0x4D15
	int				iOldTickcount;			//0x4D18
	float			flTickRemainder;		//0x4D1C
	float			flFrameTime;			//0x4D20
	int				nLastOutgoingCommand;	//0x4D24
	int				iChokedCommands;		//0x4D28
	int				nLastCommandAck;		//0x4D2C
	int				iCommandAck;			//0x4D30
	int				iSoundSequence;			//0x4D34
	std::byte		pad8[0x50];				//0x4D38
	QAngle			angViewPoint;			//0x4D88
	std::byte		pad9[0xD0];				//0x4D9A
	CEventInfo*		pEvents;				//0x4E6A
}; //Size: 0x4EAE
