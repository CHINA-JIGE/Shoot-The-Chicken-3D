
/************************************************************

							Render Pipeline

				It's just like a D3D9 Fixed Render Pipeline

				But a simplified one....

************************************************************/

#include "MyConsoleEngine.h"

IRenderPipeline3D::IRenderPipeline3D()
{
	m_pVB_HomoSpace = new std::vector<VertexShaderOutput_Vertex>;
	m_pVB_Rasterized = new std::vector<RasterizedVertex>;
	mWorldMatrix.Identity();
	mViewMatrix.Identity();
	mProjMatrix.Identity();
	mTexCoord_offsetX=0.0f;
	mTexCoord_offsetY=0.0f;
	mTexCoord_scale=1.0f;

	for (UINT i = 0;i < const_maxLightCount;++i)memset(&mDirLight[i],0,sizeof(IDirectionalLight));
}

BOOL IRenderPipeline3D::Init(RenderPipeline_InitData & initData)
{
	mBufferWidth = initData.bufferWidth;
	mBufferHeight = initData.bufferHeight;
	if (initData.pOutColorBuffer != nullptr)
	{
		m_pOutColorBuffer = initData.pOutColorBuffer;
	}
	else
	{
		return FALSE;
		DEBUG_MSG1("Render PipeLine: color buffer ptr invalid!!");
	}


	if (initData.pZBuffer != nullptr)
	{
		m_pZBuffer = initData.pZBuffer;
	}
	else
	{
		return FALSE;
		DEBUG_MSG1("Render PipeLine: Z buffer ptr invalid!!");
	}
	return TRUE;
}

void IRenderPipeline3D::DrawTriangles(RenderPipeline_DrawCallData & drawCallData)
{
	//clear the last Draw Call ruins...
	m_pVB_HomoSpace->clear();
	m_pVB_Rasterized->clear();

	//..............
	UINT offset = drawCallData.offset;
	UINT vCount = drawCallData.VertexCount;
	auto pVB = drawCallData.pVertexBuffer;
	auto pIB = drawCallData.pIndexBuffer;
	UINT triangleCount = pIB->size() / 3;

	//reserve space for vector, because they need to push_back later
	m_pVB_HomoSpace->reserve(vCount);
	m_pVB_Rasterized->reserve(mBufferWidth*mBufferHeight / 3);//approximate estimation

	//------------------------------VERTEX SHADER-----------------------------
	//in case vertex index is invalid
	for (UINT i = offset;i < (offset+vCount>pVB->size()?pVB->size():offset+vCount);++i)
	{

		Vertex& currVertex = drawCallData.pVertexBuffer->at(offset + i);

		//Use Vertex Shader To Deal with Every vertex
		VertexShader(currVertex);
	}

	//------------------------------RASTERIZER-----------------------------
	for (UINT i = 0;i < pIB->size() - 2;i += 3)
	{
		Rasterize(pIB->at(i), pIB->at(i + 1), pIB->at(i + 2));
	}

	//------------------------------PIXEL SHADER-----------------------------
	for (auto& rasterizedVertex: *m_pVB_Rasterized)
	{
		PixelShader(rasterizedVertex);
	}

}

void IRenderPipeline3D::SetWorldMatrix(const MATRIX4x4 & mat)
{
	mWorldMatrix = mat;
}

void IRenderPipeline3D::SetViewMatrix(const MATRIX4x4 & mat)
{
	mViewMatrix = mat;
}

void IRenderPipeline3D::SetProjMatrix(const MATRIX4x4 & mat)
{
	mProjMatrix = mat;
}

void IRenderPipeline3D::SetLight(UINT index, const IDirectionalLight & light)
{
	if (index < const_maxLightCount)
	{
		mDirLight[index] = light;
	}
}

void IRenderPipeline3D::SetTexcoordTransform(float dx, float dy, float scale)
{
	mTexCoord_offsetX = dx;
	mTexCoord_offsetY = dy;
	mTexCoord_scale = scale;
}

void IRenderPipeline3D::SetMaterial(const Material & mat)
{
	mMaterial = mat;
}


/************************************************************
						
								P R I V A T E

************************************************************/
void IRenderPipeline3D::VertexShader(Vertex& inVertex)
{
	VertexShaderOutput_Vertex outVertex;

	//vertex position
	VECTOR4 pos(inVertex.pos.x, inVertex.pos.y, inVertex.pos.z, 1.0f);
	pos = Matrix_Multiply(mWorldMatrix, pos);
	pos = Matrix_Multiply(mViewMatrix, pos);
	pos = Matrix_Multiply(mProjMatrix, pos);
	outVertex.posH = pos;

	//Normal also need transformation,actually it need a World-Inverse-Transpose
	//But I rule out all non-Orthogonal Transformation, so inverse can just use Transpose to substitude
	//thus inverse-transpose yields world matrix itself
	VECTOR4 norm(inVertex.normal.x, inVertex.normal.y, inVertex.normal.z, 1.0f);
	norm = Matrix_Multiply(mWorldMatrix, norm);
	outVertex.normalW = VECTOR3(norm.x, norm.y, norm.z);

	//-----TexCoord
	outVertex.texcoord = VECTOR2(
		inVertex.texcoord.x*mTexCoord_scale + mTexCoord_offsetX, 
		inVertex.texcoord.y*mTexCoord_scale + mTexCoord_offsetY);


	//-------Should I Perform Gouraud Shading??
	outVertex.color = inVertex.color;

	m_pVB_HomoSpace->push_back(outVertex);
}

void IRenderPipeline3D::Rasterize(UINT idx1, UINT idx2, UINT idx3)
{
	RasterizedVertex outVertex;

	//3 vertices of triangles ( in homogeneous space , [-1,1]x[-1,1])
	const auto& v1 = m_pVB_HomoSpace->at(idx1);
	const auto& v2 = m_pVB_HomoSpace->at(idx2);
	const auto& v3 = m_pVB_HomoSpace->at(idx3);


	//convert to pixel space
	auto convertToPixelSpace=[&](const VertexShaderOutput_Vertex& v,VECTOR2& outV)
	{
		outV.x =float(mBufferWidth) * (v.posH.x + 1.0f) / 2.0f;
		outV.y =float(mBufferHeight) * (-v.posH.y + 1.0f) / 2.0f;
	};

	VECTOR2 v1_pixel, v2_pixel, v3_pixel;//pixel space
	convertToPixelSpace(v1, v1_pixel);
	convertToPixelSpace(v2, v2_pixel);
	convertToPixelSpace(v3, v3_pixel);

	//scanline rasterization , generate pixels ROW-BY-ROW
	float minY = min(min(v1_pixel.y, v2_pixel.y), v3_pixel.y);
	float maxY = max(max(v1_pixel.y, v2_pixel.y), v3_pixel.y);

	//------------ horizontal scan line Intersection ------------
	for (UINT j = UINT(minY);j < UINT(maxY) + 1;++j)
	{
		BOOL intersectSucceeded = FALSE;
		UINT x1 = 0, x2 = 0;
		intersectSucceeded = 
			mFunction_HorizontalIntersect(float(j),v1_pixel,v2_pixel,v3_pixel,x1,x2);

		if (intersectSucceeded == TRUE)
		{
			for (UINT i = x1;i <= x2;++i)
			{

			}

		}


	}


	m_pVB_Rasterized->push_back(outVertex);
}

void IRenderPipeline3D::PixelShader(RasterizedVertex& inVertex)
{
	COLOR3 outColor;

	m_pOutColorBuffer->push_back(outColor);
}

/************************************************************
						
					OTHER  MEMBER  FUNCTION

************************************************************/

BOOL IRenderPipeline3D::mFunction_HorizontalIntersect(float y, const VECTOR2 & v1, const VECTOR2 & v2, const VECTOR2 & v3, UINT & outX1, UINT & outX2)
{
	const VECTOR2 triangleVertices[3] = { v1,v2,v3 };

	//step1, find out how many vertices have the same Y Coord with test Y Coord
	std::vector<UINT> indexList_sameYCoord;
	if (v1.y == y) indexList_sameYCoord.push_back(1); 
	if (v2.y == y) indexList_sameYCoord.push_back(2); 
	if (v3.y == y) indexList_sameYCoord.push_back(3); 


	//line-line intersection
	const auto func_intersect = [](float y, const VECTOR2& v1, const VECTOR2& v2, VECTOR2& outVec)->BOOL
	{
		bool b1 = v1.y > y && v2.y < y;
		bool b2 = v1.y < y && v2.y > y;
		// if two points lies on two sides of line Y=y
		if (b1||b2)
		{
			outVec = Lerp(v1, v2, (y - v1.y) / (v2.y - v1.y));
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	};


	//step2, discuss every circumstances and compute intersect result[x1, x2]
	VECTOR2 outVec;
	switch (indexList_sameYCoord.size())
	{
		//------------0 : no points lie on line Y=y-----------------
		case 0:
		{
			//then we'll have check all of 3 segments
			BOOL canIntersect = FALSE;
			std::vector<VECTOR2> intersectPointList;

			VECTOR2 tmpVec;
			canIntersect = func_intersect(y, v1, v2, tmpVec);
			if (canIntersect)intersectPointList.push_back(tmpVec);

			canIntersect = func_intersect(y, v2, v3, tmpVec);
			if (canIntersect)intersectPointList.push_back(tmpVec);

			canIntersect = func_intersect(y, v1, v3, tmpVec);
			if (canIntersect)intersectPointList.push_back(tmpVec);

			//there must be exactly 2 intersect point
			if (intersectPointList.size() == 2) 
			{
				outX1 = UINT(intersectPointList.at(0).x);
				outX2 = UINT(intersectPointList.at(1).x);
				if (outX1 > outX2)std::swap(outX1, outX2);
				return TRUE;
			}
			else
			{
				//intersection failed, less than 2 points are intersected.
				return FALSE;
			}

			break;
		}


		//------------1 : one point lie on the line Y=y-----------------
		case 1:
		{
			VECTOR2 tmpVec;
			BOOL canOppositeLineSegmentIntersect = FALSE;
			switch (indexList_sameYCoord.at(0))
			{
			case 1:
				canOppositeLineSegmentIntersect = func_intersect(y, v2, v3, tmpVec);
				break;
			case 2:
				canOppositeLineSegmentIntersect = func_intersect(y, v1, v3, tmpVec);
				break;
			case 3:
				canOppositeLineSegmentIntersect = func_intersect(y, v1, v2, tmpVec);
				break;
			}

			if (canOppositeLineSegmentIntersect==TRUE)
			{
				outX1 = UINT(tmpVec.x);//new intersect point
				outX2 = UINT(triangleVertices[indexList_sameYCoord.at(0)].x);//the one on the tested-y line
				if (outX1 > outX2)std::swap(outX1, outX2);
				return TRUE;
			}
			else
			{
				//the opposite line segment didn't intersect
				return FALSE;
			}

			break;
		}


		//------------2 : 2 vertices lie on line Y=y-----------------
		case 2:
		{
			outX1 = UINT(triangleVertices[indexList_sameYCoord.at(0)].x);//new intersect point
			outX2 = UINT(triangleVertices[indexList_sameYCoord.at(1)].x);//the one on the tested-y line
			if (outX1 > outX2)std::swap(outX1, outX2);
			return TRUE;
			break;
		}
	}

}
