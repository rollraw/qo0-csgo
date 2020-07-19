#include "logging.h"

bool L::Attach(const char* szConsoleTitle)
{
	// allocate memory for console
	if (!AllocConsole())
		return false;

	// attach console to current process
	AttachConsole(ATTACH_PARENT_PROCESS);

	/*
	 * redirect cout stdin - to read / stdout - to write in console window
	 * cuz we doesn't need read anything from console we use only write stream
	 */
	if (freopen_s(&pStream, XorStr("CONOUT$"), XorStr("w"), stdout) != 0)
		return false;

	// set console window title
	if (!SetConsoleTitle(szConsoleTitle))
		return false;

	return true;
}

void L::Detach()
{
	// close write console stream
	fclose(pStream);

	// free allocated memory
	FreeConsole();

	// get console window
	if (const auto hConsoleWnd = GetConsoleWindow(); hConsoleWnd != nullptr)
		// close console window
		PostMessageW(hConsoleWnd, WM_CLOSE, 0U, 0L);
}

void L::Print(std::string_view szText)
{
	// format time
	std::string szTime = fmt::format(XorStr("[{:%d-%m-%Y %X}] "), fmt::localtime(std::time(nullptr)));

	#ifdef DEBUG_CONSOLE
	// print to console
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSE_GREEN);
	fmt::print(szTime);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wConsoleColor);
	fmt::print(XorStr("{}\n"), szText);
	#else
	// print to file
	if (ofsFile.is_open())
		ofsFile << szTime << szText << std::endl;
	#endif
}
