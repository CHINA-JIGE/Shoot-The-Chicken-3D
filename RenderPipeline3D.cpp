
/************************************************************

							Render Pipeline

				It's just like a D3D9 Fixed Render Pipeline

				But a simplified one....

************************************************************/

#include "MyConsoleEngine.h"

IRenderPipeline3D::IRenderPipeline3D()
{
	m_pVB_HomoSpace = new std::vector<VertexShaderOutput_Vertex>;
	m_pVB_Rasterized = new std::vector<RasterizedFragment>;
	mWorldMatrix.Identity();
	mViewMatrix.Identity();
	mProjMatrix.Identity();
	mTexCoord_offsetX=0.0f;
	mTexCoord_offsetY=0.0f;
	mTexCoord_scale=1.0f;
	mCameraPos = { 0,0,0 };

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
	m_pVB_Rasterized->clear();

	//..............
	UINT offset = drawCallData.offset;
	UINT vCount = drawCallData.VertexCount;
	auto pVB = drawCallData.pVertexBuffer;
	auto pIB = drawCallData.pIndexBuffer;

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

void IRenderPipeline3D::SetCameraPos(const VECTOR3 & vec)
{
	mCameraPos = vec;
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
	if (Z_ViewSpace != 0)
	{
		pos.x /= (Z_ViewSpace);
		pos.y /= (Z_ViewSpace);
	}

	outVertex.posH = pos;

	//Normal also need transformation,actually it need a World-Inverse-Transpose
	//But I rule out all non-Orthogonal Transformation, so inverse can just use Transpose to substitude
	//thus inverse-transpose yields world matrix itself
	VECTOR4 normW(inVertex.normal.x, inVertex.normal.y, inVertex.normal.z, 1.0f);
	normW = Matrix_Multiply(mWorldMatrix, normW);


	//-----TexCoord
	outVertex.texcoord = VECTOR2(
		inVertex.texcoord.x*mTexCoord_scale + mTexCoord_offsetX, 
		inVertex.texcoord.y*mTexCoord_scale + mTexCoord_offsetY);


	//-------Should I Perform Gouraud Shading??Yes...
	outVertex.color = mFunction_VertexLighting(inVertex.pos,inVertex.normal);

	m_pVB_HomoSpace->push_back(outVertex);
}

void IRenderPipeline3D::HomoSpaceClipping()
{
}

void IRenderPipeline3D::Rasterize(UINT idx1, UINT idx2, UINT idx3)
{
	RasterizedFragment outVertex;

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
	float minY = Clamp(min(min(v1_pixel.y, v2_pixel.y), v3_pixel.y),0,float(mBufferHeight-1));
	float maxY = Clamp(max(max(v1_pixel.y, v2_pixel.y), v3_pixel.y),0,float(mBufferHeight-1));

	//------------ horizontal scan line Intersection ------------
	for (int j = int(minY);j < int(maxY) + 1;++j)
	{
		BOOL intersectSucceeded = FALSE;
		UINT x1 = 0, x2 = 0;
		intersectSucceeded = 
			mFunction_HorizontalIntersect(float(j),v1_pixel,v2_pixel,v3_pixel,x1,x2);

		x1 = Clamp(x1, 0, mBufferWidth-1);
		x2 = Clamp(x2, 0, mBufferWidth-1);

		//if intersect succeed, we will get X region [x1,x2] which indicate the range of pixels to fill
		if (intersectSucceeded == TRUE)
		{
			//-----------------FOR EACH RASTERIZED FRAGMENT----------------
			for (UINT i = x1;i <= x2;++i)
			{
				//pixel coord of current processing pixel
				VECTOR2 currentPoint_pixel= VECTOR2(float(i)+0.5f, float(j)+0.5f);

				//v1 (A) is the orginal point of basis, calculate the relative pixel coordinate
				VECTOR2 currentPointLocal_pixel = currentPoint_pixel - v1_pixel;

				//calculate the bilinear interpolation ratio coordinate (s,t)
				// (->localP) = s (->V1) + t(->V2)
				float s = (currentPointLocal_pixel.x*basisVector2.y - currentPointLocal_pixel.y*basisVector2.x) / D;
				float t = (basisVector1.x*currentPointLocal_pixel.y - basisVector1.y*currentPointLocal_pixel.x) / D;

				//depth correct interpolation ,then perform depth test
				float depth =1.0f/( s/v2.posH.z + t /v3.posH.z + (1 - s - t)/v1.posH.z);
				if (mFunction_DepthTest(i, j, depth) == FALSE)goto label_nextPixel;

				//I will use normal bilinear interpolation to see the result first
				outVertex.pixelX = i;
				outVertex.pixelY = j;
				outVertex.color = s * v2.color + t*v3.color + (1-s - t)*v1.color;
				outVertex.texcoord = s*v2.texcoord + t*v3.texcoord + (1-s - t)*v1.texcoord;


				m_pVB_Rasterized->push_back(outVertex);
			label_nextPixel:;
			}
		}

	}

}

void IRenderPipeline3D::PixelShader(RasterizedFragment& inVertex)
{
	COLOR3 outColor;
	outColor =COLOR3(inVertex.color.x, inVertex.color.y, inVertex.color.z);
	m_pOutColorBuffer->at(inVertex.pixelY*mBufferWidth + inVertex.pixelX) = outColor;
}

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
				currentDiffuse = diffuseFactor * mMaterial.diffuse*mDirLight[i].mDiffuseColor;


				//Specular color - eye position dependent
				/*
				VECTOR3 unitOutgoingLightVec = Vec3_Reflect(unitIncomingLightVec, unitNormal);
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
