#pragma once

class OverlayText_t;
class IVDebugOverlay
{
public:
	virtual void			AddEntityTextOverlay(int iEntityIndex, int iLineOffset, float flDuration, int r, int g, int b, int a, const char* fmt, ...) = 0;
	virtual void			AddBoxOverlay(const Vector& vecOrigin, const Vector& vecAbsMin, const Vector& vecAbsMax, const QAngle& angOrientation, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			AddSphereOverlay(const Vector& vecOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool bNoDepthTest, float flDuration) = 0;
	virtual void			AddLineOverlay(const Vector& vecOrigin, const Vector& vecDest, int r, int g, int b, bool bNoDepthTest, float flDuration) = 0;
	virtual void			AddTextOverlay(const Vector& vecOrigin, float flDuration, const char* fmt, ...) = 0;
	virtual void			AddTextOverlay(const Vector& vecOrigin, int iLineOffset, float flDuration, const char* fmt, ...) = 0;
	virtual void			AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* szText) = 0;
	virtual void			AddSweptBoxOverlay(const Vector& vecStart, const Vector& vecEnd, const Vector& vecMin, const Vector& vecMax, const QAngle& angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			AddGridOverlay(const Vector& vecOrigin) = 0;
	virtual void			AddCoordFrameOverlay(const matrix3x4_t& matFrame, float flScale, int vColorTable[3][3] = nullptr) = 0;
	virtual int				ScreenPosition(const Vector& vecPoint, Vector& vecScreen) = 0;
	virtual int				ScreenPosition(float flXPos, float flYPos, Vector& vecScreen) = 0;
	virtual OverlayText_t*	GetFirst() = 0;
	virtual OverlayText_t*	GetNext(OverlayText_t* pCurrent) = 0;
	virtual void			ClearDeadOverlays() = 0;
	virtual void			ClearAllOverlays() = 0;
	virtual void			AddTextOverlayRGB(const Vector& vecOrigin, int iLineOffset, float flDuration, float r, float g, float b, float a, const char* fmt, ...) = 0;
	virtual void			AddTextOverlayRGB(const Vector& vecOrigin, int iLineOffset, float flDuration, int r, int g, int b, int a, const char* fmt, ...) = 0;
	virtual void			AddLineOverlayAlpha(const Vector& vecOrigin, const Vector& dest, int r, int g, int b, int a, bool bNoDepthTest, float flDuration) = 0;
	virtual void			AddBoxOverlay2(const Vector& vecOrigin, const Vector& vecAbsMin, const Vector& vecAbsMax, const QAngle& angOrientation, const Color& faceColor, const Color& edgeColor, float flDuration) = 0;
	virtual void			AddLineOverlay(const Vector& vecOrigin, const Vector& vecDest, int r, int g, int b, int a, float flThickness, float flDuration) = 0;
	virtual void			PurgeTextOverlays() = 0;
	virtual void			AddCapsuleOverlay(const Vector& vecAbsMin, const Vector& vecAbsMax, const float& flRadius, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			DrawPill(Vector& vecAbsMin, Vector& vecAbsMax, float flRadius, int r, int g, int b, int a, float flDuration) = 0;
};
