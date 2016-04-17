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

	void		SetLight(UINT index,const IDirectionalLight& light);

	void		SetTexcoordTransform(float dx, float dy, float scale);

	void		SetMaterial(const Material& mat);

private:

	UINT mBufferWidth;
	UINT mBufferHeight;

	std::vector<float>*		m_pZBuffer;//depth buffer


	//-----------------------Pipeline Variable
	MATRIX4x4			mWorldMatrix;//used in Vertex Shader
	MATRIX4x4			mViewMatrix;
	MATRIX4x4			mProjMatrix;

	float						mTexCoord_offsetX;//texcoord transformation info
	float						mTexCoord_offsetY;
	float						mTexCoord_scale;

	static const UINT	const_maxLightCount = 8;
	IDirectionalLight	mDirLight[const_maxLightCount];//"IsEnabled"control whether to enable a light in one draw call

	Material				mMaterial;//current using material

	//------------------pipeline stage-----------------

	void VertexShader(Vertex& inVertex);

	std::vector<VertexShaderOutput_Vertex>*	m_pVB_HomoSpace;//vertices in homogeous clipping space

	void Rasterize(UINT idx1,UINT idx2,UINT idx3);//rasterize triangles to get discrete pixels

	std::vector<RasterizedVertex>*		m_pVB_Rasterized;//vertices attribute have been interpolated

	void PixelShader(RasterizedVertex& inVertex);//compute colors of each rasterized "pixel"

	std::vector<COLOR3>*		m_pOutColorBuffer;//output color buffer

	//----------------------Helper function-----------------
	BOOL		mFunction_HorizontalIntersect(float y,const VECTOR2& v1, const VECTOR2& v2, const VECTOR2& v3, UINT& outX1,UINT& outX2);

};