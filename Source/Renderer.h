#pragma once




class IRenderer : private IRenderPipeline3D
{
public:

	IRenderer();

	~IRenderer();

	void		Init(UINT bufferWidth,UINT bufferHeight);

	void		Clear(COLOR3 clearColor=COLOR3(0,0,0),BOOL clearZBuff=TRUE);

	void		SetCamera(ICamera& cam);

	void		SetLight(UINT index,const  DirectionalLight& light);

	void		RenderMesh(IMesh& mesh);

	void		RenderPointCollection(IPointCollection& collection);

	BOOL	DrawPicture(IPicture& pic, UINT x1, UINT y1, UINT x2, UINT y2);

	void		DrawLine(COLOR3 color, UINT x1, UINT y1, UINT x2, UINT y2);

	void		DrawTriangle(COLOR3 color, const VECTOR2& v1_pixel, const VECTOR2& v2_pixel, const VECTOR2& v3_pixel);
	
	void		Present();

	void		SetWindowTitle(const char* titleStr);

private:

	void			mFunction_GeneratePalette();

	void			mFunction_BlendPixel(UINT x, UINT y,float blendFactor, const COLOR3& newColor);

	void			mFunction_SetPixel(UINT x, UINT y, const COLOR3& color);

	UINT			mFunction_GetIndex(UINT x, UINT y);

	void			mFunction_AdjustWindowSize();

	void			mFunction_UpdateCharAndTextAttrBuffer();

	DWORD	mBufferWidth;
	DWORD	mBufferHeight;
	HANDLE	m_hScreenOutput;
	HANDLE	m_hScreenBuffer;
	std::string* m_pConsoleWindowTitle;

	ICamera*						m_pCamera;

	std::vector<float>*		m_pZBuffer;
	std::vector<char>*		m_pCharBuffer;
	std::vector<COLOR3>*	m_pColorBuffer;//using 0~255 color
	std::vector<WORD>*	m_pTextAttrBuffer;

	//store preset color that could be expressed with console
	//each palette for one 0.5x0.5x0.5 block (for optimization)
	std::vector<Renderer_Color3ToConsolePixel>*		m_pPalette[8];
};