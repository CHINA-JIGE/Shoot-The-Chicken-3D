#pragma once

	class  IGeometryMeshGenerator
	{
	public:

		void	CreatePlane(float fWidth, float fHeight, UINT iRowCount, UINT iColumnCount, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void CreateBox(float fWidth, float fHeight, float fDepth, UINT iDepthStep, UINT iWidthStep, UINT iHeightStep, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void	CreateSphere(float fRadius, UINT iColumnCount, UINT iRingCount, BOOL bInvertNormal, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void CreateCylinder(float fRadius, float fHeight, UINT iColumnCount, UINT iRingCount, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList);

	private:

		//help creating a box or plane
		void		mFunction_Build_A_Quad(VECTOR3 vOriginPoint, VECTOR3 vBasisVector1, VECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex, std::vector<Vertex>& outVerticeList, std::vector<UINT>& outIndicesList);
	};