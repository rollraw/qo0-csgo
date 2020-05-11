#pragma once
#define MAX_DLIGHTS 1000

struct color32;
struct Model_t;

enum
{
	DLIGHT_NO_WORLD_ILLUMINATION = 0x1,
	DLIGHT_NO_MODEL_ILLUMINATION = 0x2,
	DLIGHT_ADD_DISPLACEMENT_ALPHA = 0x4,
	DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA = 0x8,
	DLIGHT_DISPLACEMENT_MASK = (DLIGHT_ADD_DISPLACEMENT_ALPHA | DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA)
};

struct dlight_t
{
	int				iFlags;
	Vector			vecOrigin;
	float			flRadius;
	ColorRGBExp32	color;
	float			flDie;
	float			flDecay;
	float			dlMinLight;
	int				iKey;
	int				iStyle;
	Vector			vecDirection;
	float			flInnerAngle;
	float			flOuterAngle;

	float GetRadius() const
	{
		return flRadius;
	}

	float GetRadiusSquared() const
	{
		return flRadius * flRadius;
	}

	float IsRadiusGreaterThanZero() const
	{
		return flRadius > 0.0f;
	}
};

class IVEngineEffects
{
public:
	virtual int Draw_DecalIndexFromName(char* szName) = 0;
	virtual void DecalShoot(int iTexture, int nEntity, const Model_t* pModel, const Vector& vecModelOrigin, const QAngle& angModelView, const Vector& vecPosition, const Vector* saxis, int iFlags) = 0;
	virtual void DecalColorShoot(int iTexture, int nEntity, const Model_t* pModel, const Vector& vecModelOrigin, const QAngle& angModelView, const Vector& vecPosition, const Vector* saxis, int iFlags, const color32& color32) = 0;
	virtual void PlayerDecalShoot(IMaterial* pMaterial, void* pUserdata, int nEntity, const Model_t* pModel, const Vector& vecModelOrigin, const QAngle& angModelView, const Vector& vecPosition, const Vector* saxis, int iFlags, const color32& color32) = 0;
	virtual dlight_t* CL_AllocDlight(int iKey) = 0;
	virtual dlight_t* CL_AllocElight(int iKey) = 0;
	virtual int CL_GetActiveDLights(dlight_t* pList[MAX_DLIGHTS]) = 0;
	virtual const char* Draw_DecalNameFromIndex(int nIndex) = 0;
	virtual dlight_t* GetElightByKey(int iKey) = 0;
};
