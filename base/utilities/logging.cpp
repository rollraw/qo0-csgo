#include "logging.h"

bool L::Attach(const char* szConsoleTitle)
{
	// allocate memory for console
	if (!AllocConsole())
		return false;

	// attach console to current process
	if (AttachConsole(GetCurrentProcessId()) != 0)
		return false;

	/*
	 * redirect cout stdin - to read / stdout - to write in console window
	 * cuz we doesnt need read anything from console we use only output stream
	 */
	if (freopen_s(&pStream, XorStr("CONOUT$"), XorStr("w"), stdout) != 0) // write
		return false;

	// set console window title
	if (!SetConsoleTitle(szConsoleTitle))
		return false;

	return true;
}

void L::Detach()
{
	// close console stream
	fclose(pStream); // write

	// free allocated memory
	FreeConsole();

	// get console window
	if (const auto hConsoleWnd = GetConsoleWindow(); hConsoleWnd != nullptr)
		// close console window
		PostMessageW(hConsoleWnd, WM_CLOSE, 0U, 0L);
}

void L::Print(std::string_view szText)
{
	// get current time
	tm time = { };
	const std::chrono::system_clock::time_point systemNow = std::chrono::system_clock::now();
	const std::time_t timeNow = std::chrono::system_clock::to_time_t(systemNow);
	localtime_s(&time, &timeNow);

	// format time
	std::string szTime = fmt::format(XorStr("[{:%d-%m-%Y %X}] "), time);

	#if DEBUG_CONSOLE
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
