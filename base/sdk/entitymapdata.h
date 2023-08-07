#pragma once
// @source: master/game/shared/mapentities_shared.h
// master/game/shared/mapentities_shared.cpp

#define MAPKEY_MAXLENGTH 2048

/// parse a token out of a data block
/// @remarks: the token gets fully read no matter what the length, but only 'MAPKEY_MAXLENGTH' characters are written into @a'szNewToken'
/// @param[in] szData the data to parse
/// @param[out] szNewToken the buffer into which the new token is written
/// @returns: a pointer to the position in the data following the @a'szNewToken'
inline const char* MapEntity_ParseToken(const char* szData, char* szNewToken)
{
	// @ida MapEntity_ParseToken(): client.dll -> "53 56 8B DA 8B F1 57 33"

	szNewToken[0] = '\0';

	if (szData == nullptr)
		return nullptr;

	int nLength = 0;
	unsigned char uChar;

SKIP_WHITESPACE:
	while ((uChar = *szData) <= ' ')
	{
		if (uChar == '\0')
			return nullptr;

		szData++;
	}

	// skip // comments
	if (uChar == '/' && szData[1] == '/')
	{
		while (*szData != '\0' && *szData != '\n')
			szData++;
		goto SKIP_WHITESPACE;
	}

	// handle quoted strings specially
	if (uChar == '\"')
	{
		szData++;
		while (nLength < MAPKEY_MAXLENGTH)
		{
			uChar = *szData++;
			if (uChar == '\"' || uChar == '\0')
			{
				szNewToken[nLength] = '\0';
				return szData;
			}
			szNewToken[nLength] = static_cast<char>(uChar);
			nLength++;
		}

		if (nLength >= MAPKEY_MAXLENGTH)
		{
			nLength--;
			szNewToken[nLength] = '\0';
		}
	}

	// parse single characters
	if (uChar == '{' || uChar == '}' || uChar == ')' || uChar == '(' || uChar == '\'')
	{
		szNewToken[nLength] = static_cast<char>(uChar);
		nLength++;
		szNewToken[nLength] = '\0';
		return szData + 1;
	}

	// parse a regular word
	do
	{
		szNewToken[nLength] = static_cast<char>(uChar);
		szData++;
		nLength++;

		uChar = *szData;
		if (uChar == '{' || uChar == '}' || uChar == ')' || uChar == '(' || uChar == '\'')
			break;

		if (nLength >= MAPKEY_MAXLENGTH)
		{
			nLength--;
			szNewToken[nLength] = '\0';
		}
	} while (uChar > 32U);

	szNewToken[nLength] = '\0';
	return szData;
}

/// skip to the beginning of the next entity in the data block
/// @returns: pointer to the next character and null if no more entities leave
inline const char* MapEntity_SkipToNextEntity(const char* szMapData, char* szWorkBuffer)
{
	// @ida MapEntity_SkipToNextEntity(): client.dll -> "53 57 8B F9 8B DA 85 FF 75"

	if (szMapData == nullptr)
		return nullptr;

	// search through the map string for the next matching opening brace
	int nOpenBraceCount = 1;
	while (szMapData != nullptr)
	{
		szMapData = MapEntity_ParseToken(szMapData, szWorkBuffer);

		if (*szWorkBuffer == '{')
			nOpenBraceCount++;
		else if (*szWorkBuffer == '}')
		{
			if (--nOpenBraceCount == 0)
				// we've found the closing brace, so return the next character
				return szMapData;
		}
	}

	return nullptr;
}

class CEntityMapData
{
public:
	explicit CEntityMapData(char* szEntityBlock, const int nEntityBlockSize = -1) :
		szEntityData(szEntityBlock), nEntityDataSize(nEntityBlockSize), szCurrentKey(szEntityBlock) { }

	[[nodiscard]] const char* CurrentBufferPosition() const
	{
		return szCurrentKey;
	}

	bool GetFirstKey(char* szKeyName, char* szValue)
	{
		szCurrentKey = szEntityData; // reset the status pointer
		return GetNextKey(szKeyName, szValue);
	}

	bool GetNextKey(char* szKeyName, char* szValue)
	{
		// @ida CEntityMapData::GetNextKey(): client.dll -> "55 8B EC 81 EC ? ? ? ? 53 8B D9 8D 95 ? ? ? ? 56 8B"

		char szToken[MAPKEY_MAXLENGTH];

		// parse key
		char* pPrevKey = szCurrentKey;
		szCurrentKey = const_cast<char*>(MapEntity_ParseToken(szCurrentKey, szToken));
		if (szToken[0] == '}')
		{
			// step back
			szCurrentKey = pPrevKey;
			return false;
		}

		if (!szCurrentKey)
		{
			Q_ASSERT(false); // EOF without closing brace
			return false;
		}

		char* szKeyNameEnd = CRT::StringCopyN(szKeyName, szToken, MAPKEY_MAXLENGTH - 1U);
		*szKeyNameEnd = '\0';

		// fix up keynames with trailing spaces
		std::size_t nLength = CRT::StringLength(szKeyName);
		while (nLength > 0U && szKeyName[nLength - 1U] == ' ')
		{
			szKeyName[nLength - 1U] = '\0';
			nLength--;
		}

		// parse value
		szCurrentKey = const_cast<char*>(MapEntity_ParseToken(szCurrentKey, szToken));
		if (!szCurrentKey)
		{
			Q_ASSERT(false); // EOF without closing brace
			return false;
		}
		if (szToken[0] == '}')
		{
			Q_ASSERT(false); // closing brace without data
			return false;
		}

		// value successfully found
		char* szValueEnd = CRT::StringCopyN(szValue, szToken, MAPKEY_MAXLENGTH - 1U);
		*szValueEnd = '\0';
		return true;
	}

private:
	char* szEntityData;
	int nEntityDataSize;
	char* szCurrentKey;
};
