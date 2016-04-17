#pragma once




class IRenderer : private IRenderPipeline3D
{
public:

	IRenderer();

	~IRenderer();

	void		Init(UINT bufferWidth,UINT bufferHeight);

	void		Clear(COLOR3 clearColor=COLOR3(0,0,0),BOOL clearZBuff=TRUE);

	BOOL	RenderMesh(IMesh& mesh);

	BOOL	DrawPicture(IPicture& pic, UINT x1, UINT y1, UINT x2, UINT y2);

	void		DrawLine(COLOR3 color, UINT x1, UINT y1, UINT x2, UINT y2);

	void		Present();

	void		SetWindowTitle(const char* titleStr);

private:

	void			mFunction_BlendPixel(UINT x, UINT y,float blendFactor, const COLOR3& newColor);

	void			mFunction_SetPixel(UINT x, UINT y, const COLOR3& color);

	UINT			mFunction_GetIndex(UINT x, UINT y);

	void			mFunction_AdjustWindowSize();

	void			mFunction_UpdateCharAndTextAttrBuffer();

	const short c_ConsoleCharSize = 3;//size of one char (in pixels)


	DWORD	mBufferWidth;
	DWORD	mBufferHeight;

	HANDLE	m_hScreenOutput;
	HANDLE	m_hScreenBuffer;

	std::vector<float>*		m_pZBuffer;
	std::vector<char>*		m_pCharBuffer;
	std::vector<COLOR3>*	m_pColorBuffer;//using 0~255 color
	std::vector<WORD>*	m_pTextAttrBuffer;

};