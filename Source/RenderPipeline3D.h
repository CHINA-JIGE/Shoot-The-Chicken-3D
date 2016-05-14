#pragma once



class IRenderPipeline3D
{
public:
	IRenderPipeline3D();

	BOOL	Init(RenderPipeline_InitData& initData);

	void		DrawTriangles(RenderPipeline_DrawCallData& drawCallData);

	void		DrawPoint(RenderPipeline_DrawCallData& drawCallData);

	//so many SET functions provide interface to modify this render state machine
	void		SetWorldMatrix(const MATRIX4x4& mat);

	void		SetViewMatrix(const MATRIX4x4& mat);

	void		SetProjMatrix(const MATRIX4x4& mat);

	void		SetCameraPos(const VECTOR3& vec);

	void		SetLightingEnabled(BOOL enabled=TRUE);

	void		SetLight(UINT index,const DirectionalLight& light);

	void		SetTexcoordTransform(float dx, float dy, float scale);

	void		SetMaterial(const Material& mat);

	void		SetTexture(IPicture* pTex);

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
	BOOL					mLightEnabled;
	DirectionalLight	mDirLight[c_maxLightCount];//"IsEnabled"control whether to enable a light in one draw call
	Material				mMaterial;//current using material
	IPicture*				m_pTexture;//current using texture
	VECTOR3				mCameraPos;

	//------------------pipeline stage-----------------

	void VertexShader(Vertex& inVertex);//draw triangles & points share one VS

	std::vector<VertexShaderOutput_Vertex>*		m_pVB_HomoSpace;//vertices in homogeous clipping space

	void	HomoSpaceClipping_Triangles(std::vector<UINT>* const pIB);//polygon clipping in homo space

	void	HomoSpaceClipping_Points(std::vector<UINT>* pIB);

	std::vector<VertexShaderOutput_Vertex>*	 m_pVB_HomoSpace_Clipped;//after clipping

	std::vector<UINT>*										m_pIB_HomoSpace_Clipped;

	void RasterizeTriangles();//rasterize triangles to get discrete pixels

	void	RasterizePoints();

	std::vector<RasterizedFragment>*		m_pVB_Rasterized;//vertices attribute have been interpolated

	void	PixelShader_DrawTriangles(RasterizedFragment& inVertex);//compute colors of each rasterized "pixel"

	void	PixelShader_DrawPoints(RasterizedFragment& inVertex);

	std::vector<COLOR3>*		m_pOutColorBuffer;//output color buffer

	//----------------------Helper function-----------------
	BOOL		mFunction_HorizontalIntersect(float y,const VECTOR2& v1, const VECTOR2& v2, const VECTOR2& v3, UINT& outX1,UINT& outX2);

	void			mFunction_SetZ(UINT x, UINT y,float z);

	float			mFunction_GetZ(UINT x, UINT y);

	BOOL		mFunction_DepthTest(UINT x,UINT y,float testZ);

	VECTOR4	mFunction_VertexLighting(const VECTOR3& vPosW,const VECTOR3& vNormalW);

	VECTOR3	mFunction_SampleTexture(float x, float y);

};