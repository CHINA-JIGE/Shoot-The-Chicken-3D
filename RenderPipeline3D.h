#pragma once



class IRenderPipeline3D
{
public:
	IRenderPipeline3D();

	BOOL	Init(RenderPipeline_InitData& initData);

	void		DrawTriangles(RenderPipeline_DrawCallData& drawCallData);


	//so many SET functions provide interface to modify this render state machine
	void		SetWorldMatrix(const MATRIX4x4& mat);

	void		SetViewMatrix(const MATRIX4x4& mat);

	void		SetProjMatrix(const MATRIX4x4& mat);

	void		SetCameraPos(const VECTOR3& vec);

	void		SetLight(UINT index,const DirectionalLight& light);

	void		SetTexcoordTransform(float dx, float dy, float scale);

	void		SetMaterial(const Material& mat);

private:

	UINT mBufferWidth;
	UINT mBufferHeight;
	std::vector<float>*		m_pZBuffer;//depth buffer


	//-----------------------Pipeline Variable------------------
	MATRIX4x4			mWorldMatrix;//used in Vertex Shader
	MATRIX4x4			mViewMatrix;
	MATRIX4x4			mProjMatrix;
	float						mTexCoord_offsetX;//texcoord transformation info
	float						mTexCoord_offsetY;
	float						mTexCoord_scale;
	static const UINT	c_maxLightCount = 8;
	DirectionalLight	mDirLight[c_maxLightCount];//"IsEnabled"control whether to enable a light in one draw call
	Material				mMaterial;//current using material
	VECTOR3				mCameraPos;

	//------------------pipeline stage-----------------

	void VertexShader(Vertex& inVertex);

	std::vector<VertexShaderOutput_Vertex>*	m_pVB_HomoSpace;//vertices in homogeous clipping space

	void	HomoSpaceClipping();//polygon clipping in homo space

	void Rasterize(UINT idx1,UINT idx2,UINT idx3);//rasterize triangles to get discrete pixels

	std::vector<RasterizedFragment>*		m_pVB_Rasterized;//vertices attribute have been interpolated

	void PixelShader(RasterizedFragment& inVertex);//compute colors of each rasterized "pixel"

	std::vector<COLOR3>*		m_pOutColorBuffer;//output color buffer

	//----------------------Helper function-----------------
	BOOL		mFunction_HorizontalIntersect(float y,const VECTOR2& v1, const VECTOR2& v2, const VECTOR2& v3, UINT& outX1,UINT& outX2);

	float			mFunction_GetZ(UINT x, UINT y);

	BOOL		mFunction_DepthTest(UINT x,UINT y,float testZ);

	VECTOR4	mFunction_VertexLighting(const VECTOR3& vPosW,const VECTOR3& vNormalW);

};