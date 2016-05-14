
/************************************************************

							Render Pipeline

				It's just like a D3D9 Fixed Render Pipeline

				But a simplified one....

************************************************************/

#include "MyConsoleEngine.h"

IRenderPipeline3D::IRenderPipeline3D()
{
	m_pVB_HomoSpace = new std::vector<VertexShaderOutput_Vertex>;
	m_pVB_HomoSpace_Clipped = new std::vector<VertexShaderOutput_Vertex>;
	m_pIB_HomoSpace_Clipped = new std::vector<UINT>;
	m_pVB_Rasterized = new std::vector<RasterizedFragment>;
	m_pTexture = nullptr;//wait for user to set
	mWorldMatrix.Identity();
	mViewMatrix.Identity();
	mProjMatrix.Identity();
	mTexCoord_offsetX=0.0f;
	mTexCoord_offsetY=0.0f;
	mTexCoord_scale=1.0f;
	mCameraPos = { 0,0,0 };
	mLightEnabled = TRUE;
	for (UINT i = 0;i < c_maxLightCount;++i)memset(&mDirLight[i],0,sizeof(DirectionalLight));
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
		DEBUG_MSG1("Render PipeLine: color buffer ptr invalid!!");
		return FALSE;
	}


	if (initData.pZBuffer != nullptr)
	{
		m_pZBuffer = initData.pZBuffer;
	}
	else
	{
		DEBUG_MSG1("Render PipeLine: Z buffer ptr invalid!!");
		return FALSE;

	}
	return TRUE;
}

void IRenderPipeline3D::DrawTriangles(RenderPipeline_DrawCallData & drawCallData)
{
	//clear the last Draw Call ruins...
	m_pVB_HomoSpace->clear();
	m_pVB_HomoSpace_Clipped->clear();
	m_pIB_HomoSpace_Clipped->clear();
	m_pVB_Rasterized->clear();

	//..............
	UINT offset = drawCallData.offset;
	UINT vCount = drawCallData.VertexCount;
	auto const pVB = drawCallData.pVertexBuffer;//the data the ptr point to can't be modified
	auto const pIB = drawCallData.pIndexBuffer;

	//reserve space for vector, because they need to push_back later
	m_pVB_HomoSpace->reserve(vCount);
	m_pVB_Rasterized->reserve(mBufferWidth*mBufferHeight / 3);// /3 is approximate estimation

	//------------------------------VERTEX SHADER-----------------------------
	//in case vertex index is invalid
	for (UINT i = offset;i < (offset+vCount>pVB->size()?pVB->size():offset+vCount);++i)
	{

		Vertex& currVertex = drawCallData.pVertexBuffer->at(offset + i);

		//Use Vertex Shader To Deal with Every vertex
		VertexShader(currVertex);
	}

	//--------------------HOMO SPACE CLIPPING-------------------
	HomoSpaceClipping_Triangles(pIB);

	//------------------------------RASTERIZER-----------------------------
	RasterizeTriangles();

	//------------------------------PIXEL SHADER-----------------------------
	for (auto& rasterizedVertex: *m_pVB_Rasterized)
	{
		PixelShader_DrawTriangles(rasterizedVertex);
	}

}

void IRenderPipeline3D::DrawPoint(RenderPipeline_DrawCallData & drawCallData)
{
	//clear the last Draw Call ruins...
	m_pVB_HomoSpace->clear();
	m_pVB_HomoSpace_Clipped->clear();
	m_pIB_HomoSpace_Clipped->clear();
	m_pVB_Rasterized->clear();

	//..............
	UINT offset = drawCallData.offset;
	UINT vCount = drawCallData.VertexCount;
	auto const pVB = drawCallData.pVertexBuffer;//the data the ptr point to can't be modified
	auto const pIB = drawCallData.pIndexBuffer;

	//reserve space for vector, because they need to push_back later
	m_pVB_HomoSpace->reserve(vCount);
	m_pVB_Rasterized->reserve(mBufferWidth*mBufferHeight / 3);// /3 is approximate estimation

	//------------------------------VERTEX SHADER-----------------------------
	//---Point drawing use the same VS as triangle drawing
	//in case vertex index is invalid
	for (UINT i = offset;i < vCount;++i)
	{

		Vertex& currVertex = pVB->at(offset + i);

		//Use Vertex Shader To Deal with Every vertex
		VertexShader(currVertex);
	}

	//--------------------HOMO SPACE CLIPPING-------------------
	HomoSpaceClipping_Points(pIB);

	//------------------------------RASTERIZER-----------------------------
	RasterizePoints();

	//------------------------------PIXEL SHADER-----------------------------
	for (auto& rasterizedVertex: *m_pVB_Rasterized)
	{
		PixelShader_DrawPoints(rasterizedVertex);
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

void IRenderPipeline3D::SetCameraPos(const VECTOR3 & vec)
{
	mCameraPos = vec;
}

void IRenderPipeline3D::SetLightingEnabled(BOOL enabled)
{
	mLightEnabled = enabled;
}

void IRenderPipeline3D::SetLight(UINT index, const DirectionalLight & light)
{
	if (index < c_maxLightCount)
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

void IRenderPipeline3D::SetTexture(IPicture * pTex)
{
	m_pTexture = pTex;
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

	//non-linear operation
	float Z_ViewSpace = pos.z;
	pos = Matrix_Multiply(mProjMatrix, pos);
	if (Z_ViewSpace >= 0)
	{
		pos.x /= (Z_ViewSpace);
		pos.y /= (Z_ViewSpace);
	}

	outVertex.posH = pos;

	//Normal also need transformation,actually it need a World-Inverse-Transpose
	//But I rule out all non-Orthogonal Transformation, so inverse can just use Transpose to substitude
	//thus inverse-transpose yields world matrix itself(without translation)
	MATRIX4x4 WorldMat_NoTrans = mWorldMatrix;
	WorldMat_NoTrans.SetColumn(3, { 0,0,0,1.0f });
	VECTOR4 normW(inVertex.normal.x, inVertex.normal.y, inVertex.normal.z, 1.0f);
	normW = Matrix_Multiply(WorldMat_NoTrans, normW);


	//-----TexCoord
	outVertex.texcoord = VECTOR2(
		inVertex.texcoord.x*mTexCoord_scale + mTexCoord_offsetX, 
		inVertex.texcoord.y*mTexCoord_scale + mTexCoord_offsetY);


	//...Vertex Light or directly use vertex color
	if (mLightEnabled)
	{
		outVertex.color = mFunction_VertexLighting(inVertex.pos, VECTOR3(normW.x,normW.y,normW.z));
	}
	else
	{
		outVertex.color = inVertex.color;
	}

	m_pVB_HomoSpace->push_back(outVertex);
}

//Draw Triangles
void IRenderPipeline3D::HomoSpaceClipping_Triangles(std::vector<UINT>* const pIB)
{
	//copy VB/IB into new list, then erase (or std::remove() );
	*m_pIB_HomoSpace_Clipped = (*pIB);
	*m_pVB_HomoSpace_Clipped = std::move(*m_pVB_HomoSpace);

	UINT i = 0;
	while(i < m_pIB_HomoSpace_Clipped->size()-3)
	{
		UINT idx1 = m_pIB_HomoSpace_Clipped->at(i);
		UINT idx2 = m_pIB_HomoSpace_Clipped->at(i + 1);
		UINT idx3 = m_pIB_HomoSpace_Clipped->at(i + 2);

		auto const v1 = m_pVB_HomoSpace_Clipped->at(idx1);
		auto const v2 = m_pVB_HomoSpace_Clipped->at(idx2);
		auto const v3 = m_pVB_HomoSpace_Clipped->at(idx3);

		bool b1 = v1.posH.x <= -1.0f || v1.posH.x >= 1.0f ||
			v1.posH.y <= -1.0f || v1.posH.y >= 1.0f ||
			v1.posH.z <= 0.0f || v1.posH.z >= 1.0f;

		bool b2 = v2.posH.x <= -1.0f || v2.posH.x >= 1.0f ||
			v2.posH.y <= -1.0f || v2.posH.y >= 1.0f ||
			v2.posH.z <= 0.0f || v2.posH.z >= 1.0f;

		bool b3 = v3.posH.x <= -1.0f || v3.posH.x >= 1.0f ||
			v3.posH.y <= -1.0f || v3.posH.y >= 1.0f ||
			v3.posH.z <= 0.0f || v3.posH.z >= 1.0f;

		//violently and roughly clip triangles ::: only when 3 vertices out of boundary
		//will we exclude this triangle
		//THIS IS NOT A COMPLETE CORRECT SOLUTION, BUT USABLE

		if (b1 && b2 && b3==true)
		{
			//swap the clipped index to the tail
			UINT rubbishFragmentStartIndex = m_pIB_HomoSpace_Clipped->size() - 3;
			std::swap(m_pIB_HomoSpace_Clipped->at(i), m_pIB_HomoSpace_Clipped->at(rubbishFragmentStartIndex));
			std::swap(m_pIB_HomoSpace_Clipped->at(i+1), m_pIB_HomoSpace_Clipped->at(rubbishFragmentStartIndex+1));
			std::swap(m_pIB_HomoSpace_Clipped->at(i+2), m_pIB_HomoSpace_Clipped->at(rubbishFragmentStartIndex+2));
		
			//then pop back the rubbish indices
			for (UINT j = 0;j < 3;j++)m_pIB_HomoSpace_Clipped->pop_back();
		}
		else
		{
			//because when triangle is clipped,another triangle at the tail of list
			//will be swapped with current triangle, thus can't move on to process
			//next triangle
			i += 3;
		}
	}
}

void IRenderPipeline3D::HomoSpaceClipping_Points(std::vector<UINT>* pIB)
{
		//copy VB/IB into new list, then erase (or std::remove() );
	*m_pIB_HomoSpace_Clipped = (*pIB);
	*m_pVB_HomoSpace_Clipped = std::move(*m_pVB_HomoSpace);

	UINT i = 0;
	while(i < m_pIB_HomoSpace_Clipped->size())
	{
		UINT idx = m_pIB_HomoSpace_Clipped->at(i);

		//refer to vertex
		auto const v1 = m_pVB_HomoSpace_Clipped->at(idx);

		bool bOutOfBox = v1.posH.x <= -1.0f || v1.posH.x >= 1.0f ||
			v1.posH.y <= -1.0f || v1.posH.y >= 1.0f ||
			v1.posH.z <= 0.0f || v1.posH.z >= 1.0f;

		//violently and roughly clip triangles ::: only when 3 vertices out of boundary
		//will we exclude this triangle
		//THIS IS NOT A COMPLETE CORRECT SOLUTION, BUT USABLE

		if (bOutOfBox)
		{
			//swap the clipped index to the tail
			UINT rubbishFragmentStartIndex = m_pIB_HomoSpace_Clipped->size() - 1;
			std::swap(m_pIB_HomoSpace_Clipped->at(i), m_pIB_HomoSpace_Clipped->at(rubbishFragmentStartIndex));
			//then pop back the rubbish indices
			m_pIB_HomoSpace_Clipped->pop_back();
		}
		else
		{
			++i;
		}
	}
}

void IRenderPipeline3D::RasterizeTriangles()
{
	for (UINT tri = 0;tri < m_pIB_HomoSpace_Clipped->size() - 2;tri += 3)
	{
		UINT idx1 = m_pIB_HomoSpace_Clipped->at(tri);
		UINT idx2 = m_pIB_HomoSpace_Clipped->at(tri + 1);
		UINT idx3 = m_pIB_HomoSpace_Clipped->at(tri + 2);

		RasterizedFragment outVertex;

		//3 vertices of triangles ( in homogeneous space , [-1,1]x[-1,1])
		const auto& v1 = m_pVB_HomoSpace_Clipped->at(idx1);
		const auto& v2 = m_pVB_HomoSpace_Clipped->at(idx2);
		const auto& v3 = m_pVB_HomoSpace_Clipped->at(idx3);


		//convert to pixel space
		auto convertToPixelSpace = [&](const VertexShaderOutput_Vertex& v, VECTOR2& outV)
		{
			outV.x = float(mBufferWidth) * (v.posH.x + 1.0f) / 2.0f;
			outV.y = float(mBufferHeight) * (-v.posH.y + 1.0f) / 2.0f;
		};

		VECTOR2 v1_pixel, v2_pixel, v3_pixel;//pixel space
		convertToPixelSpace(v1, v1_pixel);
		convertToPixelSpace(v2, v2_pixel);
		convertToPixelSpace(v3, v3_pixel);

		//Basis Vector, used to compute the bilinear interpolation coord (s,t) of current pixel
		VECTOR2 basisVector1 = v2_pixel - v1_pixel;
		VECTOR2 basisVector2 = v3_pixel - v1_pixel;

		//a determinant to solve B-Lerp Coord equation
		//refer to doc for more math detail.
		float D = basisVector1.x*basisVector2.y - basisVector2.x*basisVector1.y;

		//in such circumstances,A B C lie on the same line.
		//------well , use THE SIGN OF D can implement BACKFACE CULLING--------
		if (D == 0)return;

		//scanline rasterization , generate pixels ROW-BY-ROW
		float minY = Clamp(min(min(v1_pixel.y, v2_pixel.y), v3_pixel.y), 0, float(mBufferHeight - 1));
		float maxY = Clamp(max(max(v1_pixel.y, v2_pixel.y), v3_pixel.y), 0, float(mBufferHeight - 1));

		//------------ horizontal scan line Intersection ------------
		for (int j = int(minY);j < int(maxY) + 1;++j)
		{
			BOOL intersectSucceeded = FALSE;
			UINT x1 = 0, x2 = 0;
			intersectSucceeded =
				mFunction_HorizontalIntersect(float(j), v1_pixel, v2_pixel, v3_pixel, x1, x2);

			//x1 = Clamp(x1, 0, mBufferWidth - 1);
			//x2 = Clamp(x2, 0, mBufferWidth - 1);

			//if intersect succeed, we will get X region [x1,x2] which indicate the range of pixels to fill
			if (intersectSucceeded == TRUE)
			{
				//-----------------FOR EACH RASTERIZED FRAGMENT----------------
				for (UINT i = x1;i <= x2;++i)
				{
					if (i >= mBufferWidth || j >= mBufferHeight)
					{
						break;
					}

					//pixel coord of current processing pixel
					VECTOR2 currentPoint_pixel = VECTOR2(float(i) + 0.5f, float(j) + 0.5f);

					//v1 (A) is the orginal point of basis, calculate the relative pixel coordinate
					VECTOR2 currentPointLocal_pixel = currentPoint_pixel - v1_pixel;

					//calculate the bilinear interpolation ratio coordinate (s,t)
					// (->localP) = s (->V1) + t(->V2)
					float s = (currentPointLocal_pixel.x*basisVector2.y - currentPointLocal_pixel.y*basisVector2.x) / D;
					float t = (basisVector1.x*currentPointLocal_pixel.y - basisVector1.y*currentPointLocal_pixel.x) / D;

					//depth correct interpolation ,then perform depth test
					float depth = 1.0f / (s / v2.posH.z + t / v3.posH.z + (1 - s - t) / v1.posH.z);
					if (mFunction_DepthTest(i, j, depth) == FALSE)goto label_nextPixel;

					//write to  depth buffer
					mFunction_SetZ(i, j, depth);

					//I will use normal bilinear interpolation to see the result first
					outVertex.pixelX = i;
					outVertex.pixelY = j;

					//perspective correct interpolation
					outVertex.color =
						(s / v2.posH.z* v2.color +
							t / v3.posH.z *v3.color +
							(1 - s - t) / v1.posH.z *v1.color)*depth;

					outVertex.texcoord =
						(s / v2.posH.z * v2.texcoord +
							t / v3.posH.z * v3.texcoord +
							(1 - s - t) / v1.posH.z * v1.texcoord)*depth;


					m_pVB_Rasterized->push_back(outVertex);
				label_nextPixel:;
				}//for each x (column)
			}//if horizontal intersect succeed

		}//for each y (row)
	}//for each homogeneous clip space triangle
}

void IRenderPipeline3D::RasterizePoints()
{
	for (UINT i = 0;i < m_pIB_HomoSpace_Clipped->size() ;++i)
	{
		RasterizedFragment outVertex;


		//get vertex
		UINT idx = m_pIB_HomoSpace_Clipped->at(i);
		const auto& v1 = m_pVB_HomoSpace_Clipped->at(idx);

		//convert to pixel space
		auto convertToPixelSpace = [&](const VertexShaderOutput_Vertex& v, VECTOR2& outV)
		{
			outV.x = float(mBufferWidth) * (v.posH.x + 1.0f) / 2.0f;
			outV.y = float(mBufferHeight) * (-v.posH.y + 1.0f) / 2.0f;
		};

		VECTOR2 v1_pixel;//pixel space
		convertToPixelSpace(v1, v1_pixel);

		//clip in scr space 
		if (v1_pixel.x >= mBufferWidth || v1_pixel.y >= mBufferHeight)
		{
			goto label_nextPixel;
		}

		//perform depth test
		float depth = v1.posH.z;
		if (mFunction_DepthTest(UINT(v1_pixel.x), UINT(v1_pixel.y), depth) == FALSE)goto label_nextPixel;


		//I will use normal bilinear interpolation to see the result first
		outVertex.pixelX = UINT(v1_pixel.x);
		outVertex.pixelY = UINT(v1_pixel.y);

		//write to  depth buffer
		mFunction_SetZ(outVertex.pixelX, outVertex.pixelY, depth);

		//perspective correct interpolation
		outVertex.color = v1.color;
		outVertex.texcoord = v1.texcoord;

		m_pVB_Rasterized->push_back(outVertex);
		label_nextPixel:;
	}//for each homogeneous space point
}

void IRenderPipeline3D::PixelShader_DrawTriangles(RasterizedFragment& inVertex)
{
	COLOR3 outColor;
	//if texture mapping is disabled, (1,1,1) will be returned to apply multiplication
	COLOR3 texSampleColor = mFunction_SampleTexture(inVertex.texcoord.x, inVertex.texcoord.y);
	outColor =COLOR3(inVertex.color.x, inVertex.color.y, inVertex.color.z) * texSampleColor;
	m_pOutColorBuffer->at(inVertex.pixelY*mBufferWidth + inVertex.pixelX) = outColor;
}

void IRenderPipeline3D::PixelShader_DrawPoints(RasterizedFragment & inVertex)
{
	COLOR3 outColor;
	outColor = COLOR3(inVertex.color.x, inVertex.color.y, inVertex.color.z);
	
	//draw a bigger point (2x2 pixel)
	int px1 = Clamp(inVertex.pixelX - 1, 0, mBufferWidth);
	int px2 = Clamp(inVertex.pixelX + 1, 0, mBufferWidth);
	int py1 = Clamp(inVertex.pixelY - 1, 0, mBufferHeight);
	int py2 = Clamp(inVertex.pixelY +1, 0, mBufferHeight);
	for (int i = px1;i < px2;i++)
		for (int j = py1;j < py2;j++)
			m_pOutColorBuffer->at(j*mBufferWidth +i) = outColor;
}

//Draw Points

/************************************************************
						
					OTHER  MEMBER  FUNCTION

************************************************************/

BOOL IRenderPipeline3D::mFunction_HorizontalIntersect(float y, const VECTOR2 & v1, const VECTOR2 & v2, const VECTOR2 & v3, UINT & outX1, UINT & outX2)
{
	const VECTOR2 triangleVertices[3] = { v1,v2,v3 };

	//step1, find out how many vertices have the same Y Coord with test Y Coord
	std::vector<UINT> indexList_sameYCoord;
	if (v1.y == y) indexList_sameYCoord.push_back(0); 
	if (v2.y == y) indexList_sameYCoord.push_back(1); 
	if (v3.y == y) indexList_sameYCoord.push_back(2); 


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
				outX1 = UINT(Clamp(intersectPointList.at(0).x,0,float(mBufferWidth)));
				outX2 = UINT(Clamp(intersectPointList.at(1).x,0,float(mBufferWidth)));
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
			case 0:
				canOppositeLineSegmentIntersect = func_intersect(y, v2, v3, tmpVec);
				break;
			case 1:
				canOppositeLineSegmentIntersect = func_intersect(y, v1, v3, tmpVec);
				break;
			case 2:
				canOppositeLineSegmentIntersect = func_intersect(y, v1, v2, tmpVec);
				break;
			}

			if (canOppositeLineSegmentIntersect==TRUE)
			{
				outX1 = UINT(Clamp(tmpVec.x,0,float(mBufferWidth)));//new intersect point
				outX2 = UINT(Clamp(triangleVertices[indexList_sameYCoord.at(0)].x,0,float(mBufferWidth)));//the one on the tested-y line
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
			outX1 = UINT(Clamp(triangleVertices[indexList_sameYCoord.at(0)].x,0,float(mBufferWidth)));//new intersect point
			outX2 = UINT(Clamp(triangleVertices[indexList_sameYCoord.at(1)].x,0,float(mBufferWidth)));//the one on the tested-y line
			if (outX1 > outX2)std::swap(outX1, outX2);
			return TRUE;
			break;
		}

		default:
			return FALSE;
	}

}

void IRenderPipeline3D::mFunction_SetZ(UINT x, UINT y, float z)
{
	m_pZBuffer->at(y*mBufferWidth + x)=z;
}

inline float IRenderPipeline3D::mFunction_GetZ(UINT x, UINT y)
{
	return m_pZBuffer->at(y*mBufferWidth + x);
}

inline BOOL IRenderPipeline3D::mFunction_DepthTest(UINT x, UINT y, float testZ)
{
	float& targetZ = m_pZBuffer->at(y*mBufferWidth + x);
	if (testZ <= targetZ&&testZ>0.0f)
	{
		targetZ = testZ;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VECTOR4 IRenderPipeline3D::mFunction_VertexLighting(const VECTOR3& vPosW, const VECTOR3& vNormalW)
{
	//---------For Each Vertex, Perform Gouraud Shading------------

	VECTOR4 outColor = { 0.0f,0.0f,0.0f,1.0f };

	//traverse every lights
	for (UINT i = 0;i < c_maxLightCount;++i)
	{
		if (mDirLight[i].mIsEnabled == TRUE)
		{
			//normalized light vector
			VECTOR3 unitIncomingLightVec = mDirLight[i].mDirection;
			unitIncomingLightVec.Normalize();

			//vector from current vertex to Camera(Eye),used when compute specular
			VECTOR3 toEye = mCameraPos - vPosW;
			toEye.Normalize();

			//unit vertex normal
			VECTOR3 unitNormal = vNormalW;
			unitNormal.Normalize();

			//Ambient Color
			VECTOR3 currentAmbient = mMaterial.ambient* mDirLight[i].mAmbientColor * mMaterial.diffuse;

			//diffuse Factor (first make sure that angle <normal,light> is less than PI/2
			VECTOR3 currentDiffuse = { 0,0,0 };
			VECTOR3 currentSpecular = { 0,0,0 };

			float diffuseFactor = mDirLight[i].mDiffuseIntensity*Math::Vec3_Dot((-1)*unitIncomingLightVec, unitNormal);
			if (diffuseFactor > 0.0f)
			{
				//diffuse color (eye pos independent)
				currentDiffuse = diffuseFactor * mDirLight[i].mDiffuseColor;

				//if Texture Mapping is disabled, then use pure diffuse color of material
				if (m_pTexture == nullptr)
				{
					//component-wise
					currentDiffuse = currentDiffuse* mMaterial.diffuse;
				}
				//else the color will be passed down to pixel shader to multiply by 
				//per-pixel sample diffuse color


				//Specular color - eye position dependent
				/*VECTOR3 unitOutgoingLightVec = Vec3_Reflect(unitIncomingLightVec, unitNormal);
				float specFactor =
					mDirLight[i].mSpecularIntensity *
					pow(max(Vec3_Dot(unitOutgoingLightVec, toEye), 0.0f), mMaterial.specularSmoothLevel);

				//Vector3 * vector3 means component-wise mult , return vec3(x1*x2,y1*y2,z1*z2)
				currentSpecular = specFactor* mMaterial.specular * mDirLight[i].mSpecularColor;*/

			}

			VECTOR3 outColor3 = currentAmbient+currentDiffuse+currentSpecular;
			outColor += VECTOR4(outColor3.x, outColor3.y, outColor3.z, 0.0f);
		}
	}

	return outColor;
}

inline VECTOR3 IRenderPipeline3D::mFunction_SampleTexture(float x, float y)
{
	//texture mapping disabled, diffuse color will be taken from material
	if (m_pTexture == nullptr)return VECTOR3(1.0f, 1.0f, 1.0f);

	//wrap-mode
	UINT width = m_pTexture->GetWidth();
	UINT height = m_pTexture->GetHeight();
	float pixelX= abs(width * (float(x - UINT(x))));
	float pixelY = abs(height * (float(y - UINT(y))));
	return m_pTexture->GetPixel(UINT(pixelX), UINT(pixelY));
}
