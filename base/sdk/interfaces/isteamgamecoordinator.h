#pragma once
// @credits: https://partner.steamgames.com/doc/sdk

enum EGCResults
{
	EGCResultOK = 0,
	EGCResultNoMessage = 1,			// there is no message in the queue
	EGCResultBufferTooSmall = 2,	// the buffer is too small for the requested message
	EGCResultNotLoggedOn = 3,		// the client is not logged onto steam
	EGCResultInvalidMessage = 4,	// something was wrong with the message being sent with sendmessage
};

class ISteamGameCoordinator
{
public:
	virtual EGCResults SendMessageGC(std::uint32_t uMsgType, const void* pData, std::uint32_t uData) = 0;
	virtual bool IsMessageAvailable(std::uint32_t* puMsgSize) = 0;
	virtual EGCResults RetrieveMessageGC(std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize) = 0;
};
