#pragma once

typedef unsigned char BYTE;

const UINT c_ConsoleCharSize = 3;//size of one char (in pixels)


struct COLOR3
{
	COLOR3() { r = g = b = 255U; }
	COLOR3(BYTE R, BYTE G, BYTE B) { r = R;g = G;b = B; }
	BYTE r;
	BYTE g;
	BYTE b;
};


class IRenderer
{
public:

	IRenderer();

	~IRenderer();

	void Init(UINT bufferWidth,UINT bufferHeight);

	void	Clear(COLOR3 clearColor=COLOR3(0,0,0),BOOL clearZBuff=TRUE);

	void	Render();

	void	Present();

	void	SetWindowTitle(char* titleStr);

private:

	UINT			mFunction_GetIndex(UINT x, UINT y);

	void			mFunction_AdjustWindowSize();

	WORD		mFunction_GetTextAttr(COLOR3 color);

	DWORD	mBufferWidth;
	DWORD	mBufferHeight;

	HANDLE	m_hScreenOutput;
	HANDLE	m_hScreenBuffer;

	std::vector<float>*		m_pZBuffer;
	std::vector<char>*		m_pCharBuffer;
	std::vector<COLOR3>*	m_pColorBuffer;//using 0~255 color
	std::vector<WORD>*	m_pTextAttrBuffer;

};