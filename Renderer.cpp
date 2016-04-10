
/************************************************************

							Renderer-Core

************************************************************/


#include "MyConsoleEngine.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	CloseHandle(m_hScreenBuffer);
	CloseHandle(m_hScreenOutput);
	delete m_pCharBuffer;
	delete m_pZBuffer;
}

void Renderer::Init(UINT bufferWidth, UINT bufferHeight)
{
	//Get the std stream output handle
	m_hScreenOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	//-----------------FONT-----------------
	//That is to say, we can adjust the real pixel size of console "pixel"!!!!!!!!!!!!!!!!!!!!
	//I can even make it square!!!!!
	CONSOLE_FONT_INFOEX fontInfo = { 0 };
	GetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);
	fontInfo.cbSize = sizeof(fontInfo);
	fontInfo.dwFontSize = { c_ConsoleCharSize,c_ConsoleCharSize };
	fontInfo.FontWeight = 700;		//range from 100~1000, 400 is normal while >400 is bold
	BOOL yes = SetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);


	//Create a Screen buffer (we use double-buffer technique to 
	//present data)(But I don't know if there is a swap chain?????)
	m_hScreenBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
		);

	//hide cursors
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(m_hScreenOutput, &cci);
	SetConsoleCursorInfo(m_hScreenBuffer, &cci);


	//----------INITIALIZATION OF BUFFERS-----------------
	BOOL isSuceeded = FALSE;

	COORD maxWindowSize = GetLargestConsoleWindowSize(m_hScreenOutput);
	mBufferWidth = SHORT(bufferWidth) > maxWindowSize.X ? maxWindowSize.X : bufferWidth;
	mBufferHeight = SHORT(bufferHeight) > maxWindowSize.Y ? maxWindowSize.Y : bufferHeight;
	m_pCharBuffer =new std::vector<char>(mBufferHeight*mBufferWidth);
	m_pZBuffer = new std::vector<float>(mBufferHeight*mBufferWidth);
	m_pColorBuffer =new std::vector<COLOR3>(mBufferHeight*mBufferWidth);
	m_pTextAttrBuffer = new std::vector<WORD>(mBufferHeight*mBufferWidth);

	//set the size of buffer
	COORD dwBuffSize;
	dwBuffSize.X = bufferWidth;
	dwBuffSize.Y = bufferHeight;
	isSuceeded = SetConsoleScreenBufferSize(m_hScreenBuffer, dwBuffSize);

	//set the buffer as an active buffer
	SetConsoleActiveScreenBuffer(m_hScreenBuffer);

}

void Renderer::Clear(COLOR3 clearColor,BOOL clearZBuff)
{
	//clear with SPACE; buffer is a continuous memory block, but it should
	//be regarded as a 2D buffer
	for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
	{
		m_pCharBuffer->at(i)= '@';
		m_pColorBuffer->at(i) = clearColor;
		m_pTextAttrBuffer->at(i) = WORD(mFunction_GetTextAttr(clearColor));
	}

	//Depth Buffer
	if (clearZBuff == TRUE)
	{
		for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
		{
			m_pZBuffer->at(i) = 0.0f;
		}
	}
}

void Renderer::Render()
{
	for (UINT i = 0;i < mBufferWidth*mBufferHeight;i++)
	{
		printf("%c", m_pCharBuffer->at(i));
	}
}

void Renderer::Present()
{
	//to fit and present the whole buffer
	mFunction_AdjustWindowSize();

	COORD topLeftPos = { 0,0 };
	DWORD tmpBytesWritten = 0;

	//Set array of Text Attribute (attribute like COLOR....)
	//The character values at the positions written to are not changed.
	WriteConsoleOutputAttribute(
		m_hScreenBuffer,
		&m_pTextAttrBuffer->at(0),
		mBufferWidth*mBufferHeight,
		topLeftPos,
		&tmpBytesWritten);

	//Set array of Text
	WriteConsoleOutputCharacterA(
		m_hScreenBuffer,						//handle of console screen BUFFER!!!
		&m_pCharBuffer->at(0),							//ptr to buffer memory block
		mBufferWidth*mBufferHeight,	//pixels count
		topLeftPos,								//output start pos
		&tmpBytesWritten);					//return the bytes written???*/

}

void Renderer::SetWindowTitle(char * titleStr)
{
	SetConsoleTitleA(titleStr);
}

/****************************************************
							P R I V A T E
*****************************************************/

inline UINT Renderer::mFunction_GetIndex(UINT x, UINT y)
{
	return y*mBufferWidth+x;
}

void Renderer::mFunction_AdjustWindowSize()
{
	int frameX_Width = GetSystemMetrics(SM_CXFIXEDFRAME);//frame boarder thickness
	int frameY_Height = GetSystemMetrics(SM_CYFIXEDFRAME);//frame boarder thickness
	int frameY_Caption = GetSystemMetrics(SM_CYCAPTION);//caption thickness

	//adjust the size of window to fit the buffer size (directly set the window pixel size)

	HWND hwnd = GetForegroundWindow();
	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);
	MoveWindow(
		hwnd,
		windowRect.left,
		windowRect.top,
		mBufferWidth*c_ConsoleCharSize*2 + 2*frameX_Width,
		mBufferHeight*c_ConsoleCharSize*2 + frameY_Caption+frameY_Height, 
		true);

}

inline WORD Renderer::mFunction_GetTextAttr(COLOR3 color)
{
	/*
	FOREGROUND_BLUE 蓝色
	FOREGROUND_GREEN 绿色
	FOREGROUND_RED 红色
	FOREGROUND_INTENSITY 加强
	BACKGROUND_BLUE 蓝色背景
	BACKGROUND_GREEN 绿色背景
	BACKGROUND_RED 红色背景
	BACKGROUND_INTENSITY 背景色加强
	COMMON_LVB_REVERSE_VIDEO 反色
	*/
	
	//quantization
	UINT rLevel = floor(UINT(2.999f * (float(color.r) / 255.0f)));
	UINT gLevel = floor(UINT(2.999f  * (float(color.g) / 255.0f)));
	UINT bLevel = floor(UINT(2.999f  * (float(color.b) / 255.0f)));

	WORD textAttr = 0;

	switch (rLevel)
	{
	case 1:
		textAttr |= FOREGROUND_RED;
		break;
	case 2:
		textAttr |= (FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	default:
		break;
	}

	switch (gLevel)
	{
	case 1:
		textAttr |= FOREGROUND_GREEN;
		break;
	case 2:
		textAttr |= (FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	default:
		break;
	}

	switch (bLevel)
	{
	case 1:
		textAttr |= FOREGROUND_BLUE;
		break;
	case 2:
		textAttr |= (FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	default:
		break;
	}

	return textAttr;
}
