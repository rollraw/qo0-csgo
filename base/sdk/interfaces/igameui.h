#pragma once

class ECommandMsgBoxSlot;
class IGameUI
{
public:
	virtual void Initialize(CreateInterfaceFn fnAppFactory) = 0;
	virtual void PostInit() = 0;
	virtual void Connect(CreateInterfaceFn fnGameFactory) = 0;
	virtual void Start() = 0;
	virtual void Shutdown() = 0;
	virtual void RunFrame() = 0;
	virtual void OnGameUIActivated() = 0;
	virtual void OnGameUIHidden() = 0;
	virtual void OnConnectToServer_OLD(const char* szGame, int iIp, int iPort) = 0;
	virtual void OnDisconnectFromServer_OLD(std::uint8_t dSteamLoginFailure, const char* szUsername) = 0;
	virtual void OnLevelLoadingStarted(const char* szLevelName, bool bShowProgressDialog) = 0;
	virtual void OnLevelLoadingFinished(bool bError, const char* szFailureReason, const char* szExtendedReason) = 0;
	virtual void StartLoadingScreenForCommand(const char* szCommand) = 0;
	virtual void StartLoadingScreenForKeyValues(CKeyValues* pKeyValues) = 0;
	virtual void UpdateProgressBar(float flProgress, const char* szStatusText, bool) = 0;
	virtual void SetShowProgressText(bool bShow) = 0;
	virtual void UpdateSecondaryProgressBar(float flProgress, const wchar_t* wStatusText) = 0;
	virtual void SetProgressLevelName(const char* szLevelName) = 0;
	virtual void ShowMessageDialog(const unsigned int uType, IVPanel* pOwner) = 0;
	virtual void ShowMessageDialog(const char*, const char*) = 0;
	virtual void CreateCommandMsgBox(const char* szTitle, const char* szMessage, bool, bool, const char*, const char*, const char*, const char*) = 0;
	virtual void CreateCommandMsgBoxInSlot(ECommandMsgBoxSlot messageSlot, const char* szTitle, const char* szMessage, bool, bool, const char*, const char*, const char*, const char*) = 0;
	virtual void SetLoadingBackgroundDialog(VPANEL uPanel) = 0;
	virtual void OnConnectToServer2(const char* szGame, int nIP, int iConnectionPort, int iQueryPort) = 0;
	virtual void SetProgressOnStart() = 0;
	virtual void OnDisconnectFromServer(std::uint8_t dSteamLoginFailure) = 0;
	virtual void NeedConnectionProblemWaitScreen() = 0;
	virtual void ShowPasswordUI(const char*) = 0;
	virtual void LoadingProgressWantsIsolatedRender(bool) = 0;
	virtual bool IsPlayingFullScreenVideo() = 0;
	virtual bool IsTransitionEffectEnabled() = 0;
	virtual bool IsInLevel() = 0;
	virtual void RestoreTopLevelMenu() = 0;
	virtual void StartProgressBar() = 0;
	virtual void ContinueProgressBar(float, bool) = 0;
	virtual void StopProgressBar(bool, const char*, const char*) = 0;
	virtual void SetProgressBarStatusText(const char*, bool) = 0;
	virtual void SetSecondaryProgressBar(float) = 0;
	virtual void SetSecondaryProgressBarText(const wchar_t*) = 0;
};
