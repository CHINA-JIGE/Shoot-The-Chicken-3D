#pragma once

using namespace Math;

enum TIMER_TIMEUINT
{
	TIMER_TIMEUNIT_MILLISECOND = 0,
	TIMER_TIMEUNIT_SECOND = 1,
};


struct COLOR3
{
	COLOR3() { r = g = b = 255U; }
	COLOR3(BYTE R, BYTE G, BYTE B) { r = R;g = G;b = B; }

	COLOR3& operator*=(float scale)
	{
		r =	BYTE(r * scale);
		g =	BYTE(g* scale);
		b =	BYTE(b*scale);
		return *this;
	};

	COLOR3 operator *(float scale)
	{
		return COLOR3(scale*r, scale*g, scale*b);
	}

	friend COLOR3 operator*(float scale, const COLOR3& color)
	{
		return COLOR3(color.r*scale, color.g*scale, color.b*scale);
	}

	BYTE r;
	BYTE g;
	BYTE b;
};

struct Material
{
	VECTOR4 ambient;
	VECTOR4 diffuse;
	VECTOR4 specular;
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
	VECTOR3 normalW;//world coord normal
	VECTOR2 texcoord;
};

struct RasterizedVertex
{
	UINT pixelX;
	UINT pixelY;
	VECTOR3 pos;
	VECTOR3 color;
	VECTOR3 normal;
	VECTOR2 texcoord;
};

struct RenderPipeline_InitData
{
	UINT bufferWidth;
	UINT bufferHeight;

	std::vector<float>*		pZBuffer;//depth buffer
	std::vector<COLOR3>*	pOutColorBuffer;//output color buffer
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

