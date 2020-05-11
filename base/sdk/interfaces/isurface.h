#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/vgui/ISurface.h

// used: color
#include "../datatypes/color.h"
// used: vector
#include "../datatypes/vector.h"

typedef unsigned long HScheme, HPanel, HTexture, HCursor, HFont;

#pragma region surface_enumerations
enum EFontDrawType : int
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2
};

enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};
#pragma endregion

struct Vertex_t
{
	Vertex_t() = default;
	Vertex_t(const Vector2D& pos, const Vector2D& coordinate = Vector2D(0, 0))
	{
		vecPosition = pos;
		vecCoordinate = coordinate;
	}

	void Init(const Vector2D& pos, const Vector2D& coordinate = Vector2D(0, 0))
	{
		vecPosition = pos;
		vecCoordinate = coordinate;
	}

	Vector2D vecPosition;
	Vector2D vecCoordinate;
};

class ISurface
{
public:
	void DrawSetColor(Color color)
	{
		MEM::CallVFunc<void>(this, 14, color);
	}

	void DrawSetColor(int r, int g, int b, int a)
	{
		MEM::CallVFunc<void>(this, 15, r, g, b, a);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		MEM::CallVFunc<void>(this, 16, x0, y0, x1, y1);
	}

	void DrawFilledRectFade(int x0, int y0, int x1, int y1, uint32_t uAlpha0, uint32_t uAlpha1, bool bHorizontal)
	{
		MEM::CallVFunc<void>(this, 123, x0, y0, x1, y1, uAlpha0, uAlpha1, bHorizontal);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		MEM::CallVFunc<void>(this, 18, x0, y0, x1, y1);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		MEM::CallVFunc<void>(this, 19, x0, y0, x1, y1);
	}

	void DrawPolyLine(int* x, int* y, int nPoints)
	{
		MEM::CallVFunc<void>(this, 20, x, y, nPoints);
	}

	void DrawSetTextFont(HFont hFont)
	{
		MEM::CallVFunc<void>(this, 23, hFont);
	}

	void DrawSetTextColor(Color color)
	{
		MEM::CallVFunc<void>(this, 24, color);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		MEM::CallVFunc<void>(this, 25, r, g, b, a);
	}

	void DrawSetTextPos(int x, int y)
	{
		MEM::CallVFunc<void>(this, 26, x, y);
	}

	void DrawPrintText(const wchar_t* wText, int nTextLength, EFontDrawType drawType = FONT_DRAW_DEFAULT)
	{
		MEM::CallVFunc<void>(this, 28, wText, nTextLength, drawType);
	}

	void DrawSetTextureRGBA(int nIndex, const unsigned char* rgba, int iWide, int iTall)
	{
		MEM::CallVFunc<void>(this, 37, nIndex, rgba, iWide, iTall);
	}

	void DrawSetTexture(int nIndex)
	{
		MEM::CallVFunc<void>(this, 38, nIndex);
	}

	int CreateNewTextureID(bool bProcedural = false)
	{
		return MEM::CallVFunc<int>(this, 43, bProcedural);
	}

	void UnLockCursor()
	{
		return MEM::CallVFunc<void>(this, 66);
	}

	void LockCursor()
	{
		return MEM::CallVFunc<void>(this, 67);
	}

	HFont FontCreate()
	{
		return MEM::CallVFunc<HFont>(this, 71);
	}

	bool SetFontGlyphSet(HFont hFont, const char* szWindowsFontName, int iTall, int iWeight, int iBlur, int nScanLines, int iFlags, int nRangeMin = 0, int nRangeMax = 0)
	{
		return MEM::CallVFunc<bool>(this, 72, hFont, szWindowsFontName, iTall, iWeight, iBlur, nScanLines, iFlags, nRangeMin, nRangeMax);
	}

	void GetTextSize(HFont hFont, const wchar_t* wText, int& iWide, int& iTall)
	{
		MEM::CallVFunc<void>(this, 79, hFont, wText, iWide, iTall);
	}

	void PlaySoundSurface(const char* szFileName)
	{
		MEM::CallVFunc<void>(this, 82, szFileName);
	}

	void DrawOutlinedCircle(int x, int y, int iRadius, int nSegments)
	{
		MEM::CallVFunc<void>(this, 103, x, y, iRadius, nSegments);
	}

	void DrawTexturedPolygon(int n, Vertex_t* pVertice, bool bClipVertices = true)
	{
		MEM::CallVFunc<void>(this, 106, n, pVertice, bClipVertices);
	}
};