
/***********************************************************************

					Class : Geometry Mesh Generator

			Desc: Procedural geometry generator

***********************************************************************/

#include "MyConsoleEngine.h"

void IGeometryMeshGenerator::CreatePlane(float fWidth, float fDepth, UINT iRowCount, UINT iColumnCount, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{
	mFunction_Build_A_Quad(
		VECTOR3(-fWidth / 2, 0, fDepth / 2),
		VECTOR3(fWidth / (float)(iColumnCount - 1), 0, 0),//tangent
		VECTOR3(0, 0, -fDepth / (float)(iRowCount - 1)),
		iRowCount,
		iColumnCount, 
		0,
		outVerticeList,
		outIndicesList);
}

void IGeometryMeshGenerator::CreateBox(float fWidth, float fHeight, float fDepth, UINT iDepthStep, UINT iWidthStep, UINT iHeightStep, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{
	/*
	Y  |
	|      /  Z
	|	 /
	|  /
	|/___________ X

	*/

	//Build 6 Quad
	int tmpBaseIndex;
	//BOTTOM- NORMAL√
	float tmpStep1 = fWidth / (float)(iWidthStep - 1);
	float tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = 0;
	mFunction_Build_A_Quad(
		VECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		VECTOR3(tmpStep1, 0, 0),
		VECTOR3(0, 0, tmpStep2),
		iWidthStep,
		iDepthStep,
		tmpBaseIndex, 
		outVerticeList,
		outIndicesList);

	//TOP- NORMAL√
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpStep1 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		VECTOR3(-fWidth / 2, fHeight / 2, fDepth / 2),
		VECTOR3(tmpStep1, 0, 0),
		VECTOR3(0, 0, -tmpStep2),
		iDepthStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//LEFT- NORMAL√
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		VECTOR3(-fWidth / 2, -fHeight / 2, fDepth / 2),
		VECTOR3(0, tmpStep1, 0),
		VECTOR3(0, 0, -tmpStep2),
		iDepthStep,
		iHeightStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//RIGHT- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		VECTOR3(fWidth / 2, -fHeight / 2, -fDepth / 2),
		VECTOR3(0, tmpStep1, 0),
		VECTOR3(0, 0, tmpStep2),
		iHeightStep,
		iDepthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);


	//FRONT- NORMAL√
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		VECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		VECTOR3(0, tmpStep1, 0),
		VECTOR3(tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//BACK- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		VECTOR3(fWidth / 2, -fHeight / 2, fDepth / 2),
		VECTOR3(0, tmpStep1, 0),
		VECTOR3(-tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);
}

void IGeometryMeshGenerator::CreateSphere(float fRadius, UINT iColumnCount, UINT iRingCount, BOOL bInvertNormal, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{


	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be restored in the last 2 position in this array
	//the first column will be duplicated to achieve adequate texture mapping
	VECTOR3* tmpV;
	VECTOR2* tmpTexCoord;
	UINT tmpVertexCount = (iColumnCount+1) * iRingCount +2;
	tmpV			  = new VECTOR3[tmpVertexCount];
	tmpTexCoord = new VECTOR2[tmpVertexCount];
	tmpV[tmpVertexCount-2] = VECTOR3(VECTOR3(0,fRadius,0));			//TOP vertex
	tmpV[tmpVertexCount-1] = VECTOR3(VECTOR3(0,-fRadius,0));		//BOTTOM vertex
	tmpTexCoord[tmpVertexCount-2] = VECTOR2(0.5f,0);			//TOP vertex
	tmpTexCoord[tmpVertexCount-1] = VECTOR2(0.5f,1.0f);			//BOTTOM vertex


#pragma region GenerateVertex

	float	tmpX,tmpY,tmpZ,tmpRingRadius;

	//Calculate the Step length (步长)
	float	StepLength_AngleY =		Math::CONST_PI / (iRingCount +1); // distances between each level (ring)
	float StepLength_AngleXZ =		2* Math::CONST_PI / iColumnCount;

	UINT k = 0;//for iteration
	//start to iterate
	for(UINT i = 0;i < iRingCount ;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for(UINT j = 0; j <	iColumnCount+1 ; j++)
		{
			//the Y coord of  current ring 
			tmpY = fRadius *sin(Math::CONST_PI /2 - (i+1) *StepLength_AngleY);

			////Pythagoras theorem(勾股定理)
			tmpRingRadius = sqrtf(fRadius*fRadius - tmpY * tmpY); 

			////trigonometric function(三角函数)
			tmpX = tmpRingRadius * cos( j*StepLength_AngleXZ);

			//...
			tmpZ = tmpRingRadius * sin( j*StepLength_AngleXZ);

			//...
			tmpV[k] = VECTOR3(tmpX,tmpY,tmpZ);

			//map the i,j to closed interval [0,1] respectively , to proceed a spheric texture wrapping
			tmpTexCoord[k] = VECTOR2( (float)j/(iColumnCount),(float)i /(iRingCount-1));

			k++;
		}
	}


	//add to Memory
	Vertex tmpCompleteV;
	for(UINT i =0;i<tmpVertexCount;i++)
	{

		tmpCompleteV.pos			= tmpV[i];
		tmpCompleteV.normal		= VECTOR3(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius);
		tmpCompleteV.color			= 	VECTOR4(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius,1.0f);
		tmpCompleteV.texcoord	= tmpTexCoord[i];
		if (bInvertNormal == TRUE)tmpCompleteV.normal = tmpCompleteV.normal*(-1);
		outVerticeList.push_back(tmpCompleteV);
	}

#pragma endregion GenerateVertex

#pragma region GenerateIndices
	//Generate Indices of a ball
	//every Ring grows a triangle net with lower level ring
	for(UINT i=0; i<iRingCount-1; i++)
	{
		for(UINT j=0; j<iColumnCount; j++)
		{
			/*	
					v1	_____ v2
						|    /
						|  /
						|/		v3
		
			*/
			//one ring owns (iColumnCount +1) vertices, 'Cos the first column is copied
			outIndicesList.push_back(	i*			(iColumnCount+1)		+j		+0);
			outIndicesList.push_back(	i*			(iColumnCount+1)		+j		+1);
			outIndicesList.push_back(	(i+1)*	(iColumnCount	+1)		+j		+0);

			/*
						v4
					    /|
					  /  |
			v6		/___|	v5

			*/
			outIndicesList.push_back(	i*			(iColumnCount+1)		+j		+1);
			outIndicesList.push_back(	(i+1)*	(iColumnCount+1)		+j		+1);
			outIndicesList.push_back(	(i+1)*	(iColumnCount+1)		+j		+0);
		}
	}


	//deal with the TOP/BOTTOM
	
	for(UINT j =0;j<iColumnCount;j++)
	{
		outIndicesList.push_back(j+1);
		outIndicesList.push_back(j) ;
		outIndicesList.push_back(tmpVertexCount-2);	//index of top vertex

		outIndicesList.push_back((iColumnCount+1)* (iRingCount-1) + j);
		outIndicesList.push_back((iColumnCount+1) * (iRingCount-1) + j+1);
		outIndicesList.push_back(tmpVertexCount -1); //index of bottom vertex
	}
	
#pragma endregion GenerateIndices
}

void IGeometryMeshGenerator::CreateCylinder(float fRadius, float fHeight, UINT iColumnCount, UINT iRingCount, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{

	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the last "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be restored in the last 2 position in this array
	//the first column will be duplicated to achieve adequate texture mapping
	VECTOR3* tmpV;
	VECTOR2* tmpTexCoord;
	UINT tmpVertexCount = (iColumnCount + 1) * (iRingCount + 2) + 2;
	tmpV = new VECTOR3[tmpVertexCount];
	tmpTexCoord = new VECTOR2[tmpVertexCount];
	tmpV[tmpVertexCount - 2] = VECTOR3(VECTOR3(0, fHeight / 2, 0));		//TOP vertex
	tmpV[tmpVertexCount - 1] = VECTOR3(VECTOR3(0, -fHeight / 2, 0));		//BOTTOM vertex
	tmpTexCoord[tmpVertexCount - 2] = VECTOR2(0.5f, 0);			//TOP vertex
	tmpTexCoord[tmpVertexCount - 1] = VECTOR2(0.5f, 1.0f);			//BOTTOM vertex
	UINT tmpVertexArrayIter = 0;

	float	tmpX, tmpY, tmpZ;

	//Calculate the Step length (步长)
	//the RINGS include "the top ring" and "the bottom ring"
	float	StepLength_Y = fHeight / (iRingCount - 1); // distances between each level (ring)
	float StepLength_Angle = 2 * Math::CONST_PI / iColumnCount;


#pragma region GenerateVertex

	//start to iterate
	for (UINT i = 0;i < iRingCount;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for (UINT j = 0; j < iColumnCount + 1; j++)
		{

			tmpY = (fHeight / 2) - i *StepLength_Y;
			tmpX = fRadius * cos(j*StepLength_Angle);
			tmpZ = fRadius * sin(j*StepLength_Angle);
			tmpV[tmpVertexArrayIter] = VECTOR3(tmpX, tmpY, tmpZ);

			//TexCoord generation, look for more detail in tech doc
			tmpTexCoord[tmpVertexArrayIter] = VECTOR2((float)j / (iColumnCount - 1), tmpY / (fRadius * 2 + fHeight));

			++tmpVertexArrayIter;
		}
	}

	//要增加TOP/BOTTOM两个RING，因为NORMAL不一样，这个要指着上面
	for (UINT j = 0; j < iColumnCount + 1; j++)
	{
		tmpY = (fHeight / 2);
		tmpX = fRadius * cos(j*StepLength_Angle);
		tmpZ = fRadius * sin(j*StepLength_Angle);
		tmpV[tmpVertexArrayIter] = VECTOR3(tmpX, tmpY, tmpZ);
		tmpTexCoord[tmpVertexArrayIter] = VECTOR2((float)j / (iColumnCount - 1), tmpY / (fRadius * 2 + fHeight));
		++tmpVertexArrayIter;
	}

	for (UINT j = 0; j < iColumnCount + 1; j++)
	{
		tmpY = (-fHeight / 2);
		tmpX = fRadius * cos(j*StepLength_Angle);
		tmpZ = fRadius * sin(j*StepLength_Angle);
		tmpV[tmpVertexArrayIter] = VECTOR3(tmpX, tmpY, tmpZ);
		tmpTexCoord[tmpVertexArrayIter] = VECTOR2((float)j / (iColumnCount - 1), tmpY / (fRadius * 2 + fHeight));
		++tmpVertexArrayIter;
	}




	//------------Complete Vertex Format-----------
	//侧面(side Face) along with their normals
	for (UINT i = 0;i<(iColumnCount + 1)*iRingCount;i++)
	{
		Vertex tmpCompleteV;
		tmpCompleteV.pos = tmpV[i];
		tmpCompleteV.normal = VECTOR3(tmpV[i].x / fRadius, 0, tmpV[i].z / fRadius);
		tmpCompleteV.color = VECTOR4(tmpV[i].x / fRadius, tmpV[i].y / fRadius, tmpV[i].z / fRadius,1.0f);
		tmpCompleteV.texcoord = tmpTexCoord[i];
		outVerticeList.push_back(tmpCompleteV);
	}

	//TOP/BOTTOM ring along with their normals
	for (UINT i = (iColumnCount + 1)*iRingCount;i<(iColumnCount + 1)*(iRingCount + 2);i++)
	{
		Vertex tmpCompleteV;
		tmpCompleteV.pos = tmpV[i];
		tmpCompleteV.normal = VECTOR3(0.0f, (tmpV[i].y>0 ? 1.0f : -1.0f), 0);
		tmpCompleteV.color = VECTOR4(tmpV[i].x / fRadius, tmpV[i].y / fRadius, tmpV[i].z / fRadius,1.0f);
		tmpCompleteV.texcoord = tmpTexCoord[i];
		outVerticeList.push_back(tmpCompleteV);
	}

	
	Vertex tmpCompleteV;
	//TOP/BOTTOM Vertex
	tmpCompleteV.pos = tmpV[tmpVertexCount - 2];
	tmpCompleteV.normal = VECTOR3(0, 1.0f, 0);
	tmpCompleteV.color = VECTOR4(1.0f, 1.0f, 1.0f,1.0f);
	tmpCompleteV.texcoord = tmpTexCoord[tmpVertexCount - 2];
	outVerticeList.push_back(tmpCompleteV);

	tmpCompleteV.pos = tmpV[tmpVertexCount - 1];
	tmpCompleteV.normal = VECTOR3(0, -1.0f, 0);
	tmpCompleteV.color = VECTOR4(1.0f, 1.0f, 1.0f,1.0f);
	tmpCompleteV.texcoord = tmpTexCoord[tmpVertexCount - 1];
	outVerticeList.push_back(tmpCompleteV);

#pragma endregion GenerateVertex


#pragma region GenerateIndex

	//Generate Indices of a ball
	//every Ring grows a triangle net with lower level ring
	for (UINT i = 0; i<iRingCount - 1; i++)
	{
		for (UINT j = 0; j<iColumnCount; j++)
		{

			/*
			k	_____ k+1
			|    /
			|  /
			|/		k+2

			*/
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 0);
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 1);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 0);

			/*
			k+3
			/|
			/  |
			k+5	/___|	k+4

			*/
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 1);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 1);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 0);
		}
	}


	//deal with the TOP/BOTTOM
	for (UINT j = 0;j<iColumnCount;j++)
	{
		outIndicesList.push_back((iColumnCount + 1)*iRingCount + j);
		outIndicesList.push_back((iColumnCount + 1)*iRingCount + j + 1);
		outIndicesList.push_back(tmpVertexCount - 2);	//index of top vertex

		outIndicesList.push_back((iColumnCount + 1) * (iRingCount + 1) + j);
		outIndicesList.push_back((iColumnCount + 1) * (iRingCount + 1) + j + 1);
		outIndicesList.push_back(tmpVertexCount - 1); //index of bottom vertex
	}

#pragma endregion GenerateIndex

	delete tmpV;
	delete tmpTexCoord;
}

/***********************************************************************
								P R I V A T E					                    
***********************************************************************/

inline void IGeometryMeshGenerator::mFunction_Build_A_Quad
(VECTOR3 vOriginPoint, VECTOR3 vBasisVector1, VECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{
	VECTOR3 tmpNormal;
	Vertex tmpCompleteV;
	tmpNormal=Vec3_Cross( vBasisVector1, vBasisVector2);
	tmpNormal.Normalize();

	for (UINT i = 0;i < StepCount1;i++)
	{
		for (UINT j = 0;j < StepCount2;j++)
		{
			tmpCompleteV.normal = tmpNormal;
			tmpCompleteV.pos = VECTOR3(vOriginPoint + (float)i*vBasisVector1 + (float)j*vBasisVector2);
			tmpCompleteV.color = VECTOR4(((float)i / StepCount1), ((float)j / StepCount2), 0.5f,1.0f);
			tmpCompleteV.texcoord = VECTOR2((float)i / (StepCount1 - 1), ((float)j / StepCount2));
			outVerticeList.push_back(tmpCompleteV);
		}
	}

	for (UINT i = 0;i<StepCount1 - 1;i++)
	{
		for (UINT j = 0;j<StepCount2 - 1;j++)
		{
			//why use iBaseIndex : when we build things like a box , we need build 6 quads ,
			//thus inde offset is needed
			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j);
			outIndicesList.push_back(iBaseIndex + (i + 1)* StepCount2 + j);
			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j + 1);

			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j + 1);
			outIndicesList.push_back(iBaseIndex + (i + 1) *StepCount2 + j);
			outIndicesList.push_back(iBaseIndex + (i + 1)* StepCount2 + j + 1);
		}
	}

}
