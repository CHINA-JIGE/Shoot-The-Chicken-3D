#pragma once

using namespace Math;

enum TIMER_TIMEUINT
{
	TIMER_TIMEUNIT_MILLISECOND = 0,
	TIMER_TIMEUNIT_SECOND = 1,
};

typedef VECTOR3 COLOR3;


struct DirectionalLight
{
public:
	DirectionalLight()
	{
		ZeroMemory(this, sizeof(*this));
		mSpecularIntensity = 1.0f;
		mDirection = VECTOR3(1.0f, 0, 0);
		mDiffuseIntensity = 0.5;
		mIsEnabled = TRUE;
	}

	BOOL operator==(DirectionalLight& Light)
	{
		//two memory fragments are identical
		if (memcmp(this, &Light, sizeof(Light)) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	VECTOR3 mAmbientColor;	 float		mSpecularIntensity;
	VECTOR3 mDiffuseColor;		float			mDiffuseIntensity;
	VECTOR3 mSpecularColor;	 BOOL		mIsEnabled;
	VECTOR3 mDirection;
};

struct Material
{
	Material() 
	{
		ambient = { 0,0,0 };
		diffuse = { 1.0f,0,0 };
		specular = { 1.0f,1.0f,1.0f };
		specularSmoothLevel = 10;
	};
	VECTOR3	ambient;
	VECTOR3	diffuse;
	VECTOR3	specular;
	UINT			specularSmoothLevel;
};

struct Vertex
{
	VECTOR3 pos;
	VECTOR4 color;
	VECTOR3 normal;
	VECTOR2 texcoord;
};

struct VertexShaderOutput_Vertex
{
	VECTOR4 posH;//homogenous position
	VECTOR4 color;
	VECTOR2 texcoord;

	//Gouraud shading don't need to pass down the posW and normalW to pixel shader
	//while Phong shading (per-pixel) need it
};

struct RasterizedFragment
{
	UINT pixelX;
	UINT pixelY;
	VECTOR4 color;
	VECTOR2 texcoord;
	//VECTOR3 normal;
};

struct RenderPipeline_InitData
{
	UINT bufferWidth;
	UINT bufferHeight;

	std::vector<float>*		pZBuffer;//depth buffer
	std::vector<VECTOR3>*	pOutColorBuffer;//output color buffer
};

struct RenderPipeline_DrawCallData
{
	std::vector<Vertex>*		pVertexBuffer;
	std::vector<UINT>*		pIndexBuffer;
	UINT offset;
	UINT VertexCount;
};

//in OBJ file ,vertex info is composed of indices
struct OBJ_vertexInfoIndex
{
	OBJ_vertexInfoIndex()
	{
		vertexID = texcoordID = vertexNormalID = 0;
	};

	OBJ_vertexInfoIndex(int vID, int texcID, int vnID)
	{
		vertexID = vID;
		texcoordID = texcID;
		vertexNormalID = vnID;
	}

	inline BOOL operator==(OBJ_vertexInfoIndex const& v)const
	{
		if (vertexID == v.vertexID && texcoordID == v.texcoordID && vertexNormalID == v.vertexNormalID)
		{
			return TRUE;
		}
		return FALSE;
	}

	UINT vertexID;
	UINT texcoordID;
	UINT vertexNormalID;
};

struct Renderer_Color3ToConsolePixel
{
	COLOR3	color;
	BYTE			asciiChar;
	WORD		textAttr;
};

