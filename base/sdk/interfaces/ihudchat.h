#pragma once

class IHudChat
{
public:
	//template <typename ... Args_t>
	void ChatPrintf(int nPlayerIndex, int iFilter, const char* szFormat, /*Args_t*/... /*args*/)
	{
		return call_vfunc<26, void>(this, nPlayerIndex, iFilter, szFormat/*, args*/);
	}

	//template <typename ... Args_t>
	void ChatPrintfW(int nPlayerIndex, int iFilter, const wchar_t* wFormat, /*Args_t*/... /*args*/)
	{
		return call_vfunc<27, void>(this, nPlayerIndex, iFilter, wFormat/*, args*/);
	}
};