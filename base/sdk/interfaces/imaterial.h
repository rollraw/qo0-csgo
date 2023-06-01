#pragma once
#include "../datatypes/matrix.h"
#include "../datatypes/keyvalues.h"
#include "../datatypes/utlsymbol.h"

using VertexFormat_t = std::uint64_t;

#pragma region material_enumerations
enum EMaterialVarFlags : int
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_PSEUDO_TRANSLUCENT = (1 << 9), // used to mark water materials for rendering after opaques but before translucents (with alpha blending but also with depth writes)
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	MATERIAL_VAR_AOPREPASS = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_MULTIPLY = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31)
};

enum EShaderParameterType : int
{
	SHADER_PARAM_TYPE_TEXTURE,
	SHADER_PARAM_TYPE_INTEGER,
	SHADER_PARAM_TYPE_COLOR,
	SHADER_PARAM_TYPE_VEC2,
	SHADER_PARAM_TYPE_VEC3,
	SHADER_PARAM_TYPE_VEC4,
	SHADER_PARAM_TYPE_ENVMAP, // obsolete
	SHADER_PARAM_TYPE_FLOAT,
	SHADER_PARAM_TYPE_BOOL,
	SHADER_PARAM_TYPE_FOURCC,
	SHADER_PARAM_TYPE_MATRIX,
	SHADER_PARAM_TYPE_MATERIAL,
	SHADER_PARAM_TYPE_STRING,
};

enum EMaterialVarType : int
{
	MATERIAL_VAR_TYPE_FLOAT = 0,
	MATERIAL_VAR_TYPE_STRING,
	MATERIAL_VAR_TYPE_VECTOR,
	MATERIAL_VAR_TYPE_TEXTURE,
	MATERIAL_VAR_TYPE_INT,
	MATERIAL_VAR_TYPE_FOURCC,
	MATERIAL_VAR_TYPE_UNDEFINED,
	MATERIAL_VAR_TYPE_MATRIX,
	MATERIAL_VAR_TYPE_MATERIAL
};

enum EImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,	// note: D3D9 does not have this format
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,

	IMAGE_FORMAT_NULL,			// dummy format which takes no video memory

	// compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// one-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// two-surface ATI1N format

	IMAGE_FORMAT_RGBA1010102,	// 10 bit-per component render targets
	IMAGE_FORMAT_BGRA1010102,
	IMAGE_FORMAT_R16F,			// 16 bit FP format

	// depth-stencil texture formats
	IMAGE_FORMAT_D16,
	IMAGE_FORMAT_D15S1,
	IMAGE_FORMAT_D32,
	IMAGE_FORMAT_D24S8,
	IMAGE_FORMAT_LINEAR_D24S8,
	IMAGE_FORMAT_D24X8,
	IMAGE_FORMAT_D24X4S4,
	IMAGE_FORMAT_D24FS8,
	IMAGE_FORMAT_D16_SHADOW,	// specific formats for shadow mapping
	IMAGE_FORMAT_D24X8_SHADOW,	// specific formats for shadow mapping

	// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,

	IMAGE_FORMAT_MAX
};
#pragma endregion

// forward declarations
class IMaterial;

#pragma pack(push, 4)
// functions used to verify indexes:
// @ida CMaterialVar::CopyFrom(): materialsystem.dll -> "55 8B EC A1 ? ? ? ? 83 EC 08 53"
// functions used to verify offsets:
// @ida CMaterialVar::GetStringValue(): materialsystem.dll -> "55 8B EC A1 ? ? ? ? 83 EC 0C 53" @xref: "env_cubemap", "<UNDEFINED>"
class IMaterialVar : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	[[nodiscard]] ITexture* GetTexture()
	{
		return CallVFunc<ITexture*, 0U>(this);
	}

	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 3U>(this);
	}

	void SetFloat(float flNewValue)
	{
		CallVFunc<void, 4U>(this, flNewValue);
	}

	void SetInt(int iNewValue)
	{
		CallVFunc<void, 5U>(this, iNewValue);
	}

	void SetString(const char* szNewValue)
	{
		CallVFunc<void, 6U>(this, szNewValue);
	}

	[[nodiscard]] const char* GetString() const
	{
		return CallVFunc<const char*, 7U>(this);
	}

	void SetVector(const float* arrNewVector, int nComponentCount)
	{
		CallVFunc<void, 9U>(this, arrNewVector, nComponentCount);
	}

	void SetVector(float x, float y)
	{
		CallVFunc<void, 10U>(this, x, y);
	}

	void SetVector(float x, float y, float z)
	{
		CallVFunc<void, 11U>(this, x, y, z);
	}

	void SetVector(float x, float y, float z, float w)
	{
		CallVFunc<void, 12U>(this, x, y, z, w);
	}

	void SetTexture(ITexture* pNewTexture)
	{
		CallVFunc<void, 15U>(this, pNewTexture);
	}

	[[nodiscard]] IMaterial* GetMaterial()
	{
		return CallVFunc<IMaterial*, 16U>(this);
	}

	void SetMaterial(IMaterial* pMaterial)
	{
		CallVFunc<void, 17U>(this, pMaterial);
	}

	void SetVectorComponent(float flComponentValue, int nComponentIndex)
	{
		CallVFunc<void, 26U>(this, flComponentValue, nComponentIndex);
	}

	[[nodiscard]] int GetInt() const
	{
		return CallVFunc<int, 27U>(this);
	}

	[[nodiscard]] float GetFloat() const
	{
		return CallVFunc<float, 28U>(this);
	}

	[[nodiscard]] const float* GetVector() const
	{
		return CallVFunc<const float*, 30U>(this);
	}

	[[nodiscard]] int GetVectorSize() const
	{
		return CallVFunc<int, 31U>(this);
	}

private:
	void* pVTable; // 0x00
public:
	struct Matrix_t
	{
		ViewMatrix_t matView; // 0x00
		bool bIsIdent; // 0x40
	};
	static_assert(sizeof(Matrix_t) == 0x44);

	char* szValue; // 0x04
	int iValue; // 0x08
	Vector4D_t vecValue; // 0x0C // @ida: materialsystem.dll -> U8["F3 0F 10 43 ? 83" + 0x4]
	std::uint8_t nType : 4; // 0x18
	std::uint8_t nNumVectorComps : 3; // 0x18
	std::uint8_t bFakeMaterialVar : 1; // 0x18
	// 0x19 // valve screwed up with those bitfields and made it even worse uhh.. leads to 4 byte padding
	std::uint8_t nTempIndex; // 0x1D // @ida: materialsystem.dll -> U8["8A 43 ? 3C FF 74 21 0F B6 C0 B9" + 0x2]
	CUtlSymbol name; // 0x1E
	IMaterial* pMaterialInternal; // 0x20 // owning material

	union
	{
		IMaterial* pMaterialValue;
		ITexture* pTextureValue;
		Matrix_t* pMatrixValue;
	}; // 0x24
};
static_assert(sizeof(IMaterialVar) == 0x28);
#pragma pack(pop)

class IMaterial : ROP::VirtualCallable_t<ROP::EngineGadget_t>
{
public:
	[[nodiscard]] const char* GetName() const
	{
		return CallVFunc<const char*, 0U>(this);
	}

	[[nodiscard]] const char* GetTextureGroupName() const
	{
		return CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] int GetMappingWidth()
	{
		return CallVFunc<int, 4U>(this);
	}

	[[nodiscard]] int GetMappingHeight()
	{
		return CallVFunc<int, 5U>(this);
	}

	[[nodiscard]] int GetNumAnimationFrames()
	{
		return CallVFunc<int, 6U>(this);
	}

	/// @param[in] szName name of shader variable
	/// @param[out] pbFound [optional] true if shader variable was found, false otherwise
	/// @param[in] bComplain accumulate failures with error message
	/// @returns: shader variable of current material on success, dummy variable otherwise (not null!)
	[[nodiscard]] IMaterialVar* FindVar(const char* szName, bool* pbFound, bool bComplain = true)
	{
		return CallVFunc<IMaterialVar*, 11U>(this, szName, pbFound, bComplain);
	}

	void IncrementReferenceCount()
	{
		CallVFunc<void, 12U>(this);
	}

	void DecrementReferenceCount()
	{
		CallVFunc<void, 13U>(this);
	}

	[[nodiscard]] int GetEnumerationID()
	{
		return CallVFunc<int, 14U>(this);
	}

	[[nodiscard]] bool IsTranslucent()
	{
		return CallVFunc<bool, 17U>(this);
	}

	[[nodiscard]] bool IsAlphaTested()
	{
		return CallVFunc<bool, 18U>(this);
	}

	[[nodiscard]] bool IsVertexLit()
	{
		return CallVFunc<bool, 19U>(this);
	}

	[[nodiscard]] VertexFormat_t GetVertexFormat() const
	{
		return CallVFunc<VertexFormat_t, 20U>(this);
	}

	[[nodiscard]] bool HasProxy() const
	{
		return CallVFunc<bool, 21U>(this);
	}

	[[nodiscard]] bool UsesEnvCubemap()
	{
		return CallVFunc<bool, 22U>(this);
	}

	void AlphaModulate(float flAlpha)
	{
		CallVFunc<void, 27U>(this, flAlpha);
	}

	void ColorModulate(float flRed, float flGreen, float flBlue)
	{
		CallVFunc<void, 28U>(this, flRed, flGreen, flBlue);
	}

	void SetMaterialVarFlag(EMaterialVarFlags nVarFlag, bool bEnable)
	{
		CallVFunc<void, 29U>(this, nVarFlag, bEnable);
	}

	[[nodiscard]] bool GetMaterialVarFlag(EMaterialVarFlags nVarFlag)
	{
		return CallVFunc<bool, 30U>(this, nVarFlag);
	}

	void GetReflectivity(Vector_t& vecReflectivity)
	{
		CallVFunc<void, 31U>(this, &vecReflectivity);
	}

	[[nodiscard]] bool IsTwoSided()
	{
		return CallVFunc<bool, 33U>(this);
	}

	void SetShader(const char* szShaderName)
	{
		CallVFunc<void, 34U>(this, szShaderName);
	}

	[[nodiscard]] int GetTextureMemoryBytes()
	{
		return CallVFunc<int, 36U>(this);
	}

	void Refresh()
	{
		CallVFunc<void, 37U>(this);
	}

	[[nodiscard]] int GetShaderParametersCount() const
	{
		return CallVFunc<int, 40U>(this);
	}

	[[nodiscard]] IMaterialVar** GetShaderParameters()
	{
		return CallVFunc<IMaterialVar**, 41U>(this);
	}

	[[nodiscard]] bool IsErrorMaterial() const
	{
		return CallVFunc<bool, 42U>(this);
	}

	[[nodiscard]] float GetAlphaModulation()
	{
		return CallVFunc<float, 44U>(this);
	}

	void GetColorModulation(float* pflRed, float* pflGreen, float* pflBlue)
	{
		CallVFunc<void, 45U>(this, pflRed, pflGreen, pflBlue);
	}

	void SetShaderAndParams(CKeyValues* pKeyValues)
	{
		CallVFunc<void, 48U>(this, pKeyValues);
	}

	[[nodiscard]] const char* GetShaderName() const
	{
		return CallVFunc<const char*, 49U>(this);
	}

	[[nodiscard]] int GetReferenceCount() const
	{
		return CallVFunc<int, 56U>(this);
	}
};
