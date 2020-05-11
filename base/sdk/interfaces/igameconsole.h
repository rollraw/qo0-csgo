#pragma once
// @credits: https://github.com/Nican/swarm-sdk/blob/master/src/common/GameUI/IGameConsole.h

class IGameConsole
{
public:
	virtual			~IGameConsole() { }
	virtual void	Activate() = 0; // activates the console, makes it visible and brings it to the foreground
	virtual void	Initialize() = 0;
	virtual void	Hide() = 0; // hides the console
	virtual void	Clear() = 0; // clears the console
	virtual bool	IsConsoleVisible() = 0; // return true if the console has focus
	virtual void	SetParent(int parent) = 0;
};
