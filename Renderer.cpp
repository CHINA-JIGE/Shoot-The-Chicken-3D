
/************************************************************

				Renderer-Interfaces exposed to user

************************************************************/


#include "MyConsoleEngine.h"

IRenderer::IRenderer()
{

}

IRenderer::~IRenderer()
{
	CloseHandle(m_hScreenBuffer);
	CloseHandle(m_hScreenOutput);
	delete m_pCharBuffer;
	delete m_pZBuffer;
}

void IRenderer::Init(UINT bufferWidth, UINT bufferHeight)
{
	//Get the std stream output handle
	m_hScreenOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);

	//-----------------FONT-----------------
	//That is to say, we can adjust the real pixel size of console "pixel"!!!!!!!!!!!!!!!!!!!!
	//I can even make it square!!!!!
	CONSOLE_FONT_INFOEX fontInfo = { 0 };
	::GetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);
	fontInfo.cbSize = sizeof(fontInfo);
	fontInfo.dwFontSize = { c_ConsoleCharSize,c_ConsoleCharSize };
	fontInfo.FontWeight = 700;		//range from 100~1000, 400 is normal while >400 is bold
	::SetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);


	//Create a Screen buffer (we use double-buffer technique to 
	//present data)(But I don't know if there is a swap chain?????)
	m_hScreenBuffer = ::CreateConsoleScreenBuffer(
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
	::SetConsoleCursorInfo(m_hScreenOutput, &cci);
	::SetConsoleCursorInfo(m_hScreenBuffer, &cci);


	//----------INITIALIZATION OF BUFFERS-----------------
	BOOL isSuceeded = FALSE;

	COORD maxWindowSize = ::GetLargestConsoleWindowSize(m_hScreenOutput);
	mBufferWidth = SHORT(bufferWidth) > maxWindowSize.X ? maxWindowSize.X : bufferWidth;
	mBufferHeight = SHORT(bufferHeight) > maxWindowSize.Y ? maxWindowSize.Y : bufferHeight;

	//set the size of buffer
	COORD dwBuffSize;
	dwBuffSize.X = bufferWidth;
	dwBuffSize.Y = bufferHeight;
	isSuceeded = ::SetConsoleScreenBufferSize(m_hScreenBuffer, dwBuffSize);

	m_pCharBuffer = new std::vector<char>(mBufferHeight*mBufferWidth);
	m_pZBuffer = new std::vector<float>(mBufferHeight*mBufferWidth);
	m_pColorBuffer = new std::vector<COLOR3>(mBufferHeight*mBufferWidth);
	m_pTextAttrBuffer = new std::vector<WORD>(mBufferHeight*mBufferWidth);

	//set the buffer as an active buffer
	::SetConsoleActiveScreenBuffer(m_hScreenBuffer);

	//----------------------------INIT RENDER PIPELINE-----------------------
	RenderPipeline_InitData initData;
	initData.bufferWidth = mBufferWidth;
	initData.bufferHeight = mBufferHeight;
	initData.pOutColorBuffer = m_pColorBuffer;
	initData.pZBuffer = m_pZBuffer;
	IRenderPipeline3D::Init(initData);

}

void IRenderer::Clear(COLOR3 clearColor,BOOL clearZBuff)
{
	//clear with SPACE; buffer is a continuous memory block, but it should
	//be regarded as a 2D buffer
	for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
	{
		//clear the RGB buffer
		m_pColorBuffer->at(i) = clearColor;
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

BOOL IRenderer::DrawPicture(IPicture & pic, UINT x1, UINT y1, UINT x2, UINT y2)
{
	x1 = Clamp(x1, 0, mBufferWidth-1);
	x2 = Clamp(x2, 0, mBufferWidth-1);
	y1 = Clamp(y1, 0, mBufferHeight-1);
	y2 = Clamp(y2, 0, mBufferHeight-1);


	if (x1 >= x2 || y1 >= y2)
	{
		DEBUG_MSG1("Render Picture: region info error!");
		return FALSE;
	}

	UINT drawRegionWidth = x2 - x1+1;
	UINT drawRegionHeight = y2 - y1+1;

	//because Minification/Magnification could occur, we must find the 
	//x/y ratio coord first to filter
	for (UINT i = x1;i <= x2;++i)
	{
		float x_ratio = float(i - x1) / drawRegionWidth;
		for (UINT j = y1;j <= y2;++j)
		{
			float y_ratio = float(j - y1) / drawRegionHeight;
			UINT picCoordX = UINT(pic.mWidth*x_ratio);
			UINT picCoordY = UINT(pic.mHeight*y_ratio);
			mFunction_SetPixel(i, j, pic.GetPixel(picCoordX, picCoordY));
		}
	}
	return TRUE;
}

void IRenderer::DrawLine(COLOR3 color, UINT x1, UINT y1, UINT x2, UINT y2)
{
	auto fractionPart = [](float f)->float 
	{
		return f - float(UINT(f));
	};

	if (x1==x2)
	{
		//if slope doesn't exist
		for (UINT j = y1;j <= y2;j++)
		{
				mFunction_SetPixel(x1, j, color);//pixel outside the boundary won't be drawn
		}
	}
	else
	{
		//.........It should a signed distance
		float	k = float(int(y2)-int(y1)) / float(int(x2)-int(x1));
		float k_inv = 1 / k;
		//bresenham-like line drawing

		//two circumstances
		//1. abs(slope)>=1; 
		//2. abs(slope)<=1;
		float offset = 0.0f;
		if (abs(k) <= 1.0f)
		{
			UINT i = x1;

			while(i!=x2)
			{
				//UINT() forced type conversion will truncate the fraction part
				mFunction_SetPixel(i, y1 + UINT(offset),color);

				//anti-alising
				float frac = fractionPart(offset);
				mFunction_BlendPixel(i, y1 + UINT(offset)+ 1, frac,color);
				offset += k;//dy = dx * k;

				if (x2 > x1)++i;else --i;
			}
		}
		else
		{
			UINT j = y1;
			while(j!=y2)
			{
				mFunction_SetPixel(x1+UINT(offset), j, color);

				//anti-alising
				mFunction_BlendPixel(x1+UINT(offset) +1,j, fractionPart(offset),color);
				offset += (k_inv);

				if (y2 > y1) 
					{ ++j; }
				else
					{ --j; }
			}
		}
	}

};

BOOL IRenderer::RenderMesh(IMesh& mesh)
{
	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = mesh.m_pIB_Mem;
	drawCallData.pVertexBuffer = mesh.m_pVB_Mem;
	drawCallData.VertexCount = mesh.GetVertexCount();

	//Pipeline will directly draw to ColorBuffer and ZBuffer......
	IRenderPipeline3D::DrawTriangles(drawCallData);

	return TRUE;
}

void IRenderer::Present()
{
	//to fit and present the whole buffer
	mFunction_AdjustWindowSize();

	mFunction_UpdateCharAndTextAttrBuffer();

	COORD topLeftPos = { 0,0 };
	DWORD tmpBytesWritten = 0;

	//Set array of Text Attribute (attribute like COLOR....)
	//The character values at the positions written to are not changed.
	::WriteConsoleOutputAttribute(
		m_hScreenBuffer,
		&m_pTextAttrBuffer->at(0),
		mBufferWidth*mBufferHeight,
		topLeftPos,
		&tmpBytesWritten);

	//Set array of Text
	::WriteConsoleOutputCharacterA(
		m_hScreenBuffer,						//handle of console screen BUFFER!!!
		&m_pCharBuffer->at(0),							//ptr to buffer memory block
		mBufferWidth*mBufferHeight,	//pixels count
		topLeftPos,								//output start pos
		&tmpBytesWritten);					//return the bytes written???
	
}

void IRenderer::SetWindowTitle(const char * titleStr)
{
	::SetConsoleTitleA(titleStr);
}

/****************************************************
							P R I V A T E
*****************************************************/

void IRenderer::mFunction_BlendPixel(UINT x, UINT y, float blendFactor, const COLOR3 & newColor)
{
	if (x < mBufferWidth && y < mBufferHeight)
	{
		COLOR3& c = m_pColorBuffer->at(y*mBufferWidth + x);
		c.r = Lerp(c.r, newColor.r, blendFactor);
		c.g = Lerp(c.g, newColor.g, blendFactor);
		c.b = Lerp(c.b, newColor.b, blendFactor);
	}

}

inline void IRenderer::mFunction_SetPixel(UINT x, UINT y, const COLOR3 & color)
{
	if(x<mBufferWidth && y <mBufferHeight)
		m_pColorBuffer->at(y*mBufferWidth + x) = color;
}

inline UINT IRenderer::mFunction_GetIndex(UINT x, UINT y)
{
	return y*mBufferWidth+x;
}

void IRenderer::mFunction_AdjustWindowSize()
{
	int frameX_Width = GetSystemMetrics(SM_CXFIXEDFRAME);//frame boarder thickness
	int frameY_Height = GetSystemMetrics(SM_CYFIXEDFRAME);//frame boarder thickness
	int frameY_Caption = GetSystemMetrics(SM_CYCAPTION);//caption thickness

	//adjust the size of window to fit the buffer size (directly set the window pixel size)

	HWND hwnd = ::GetForegroundWindow();
	RECT windowRect;
	::GetWindowRect(hwnd, &windowRect);
	::MoveWindow(
		hwnd,
		windowRect.left,
		windowRect.top,
		mBufferWidth*c_ConsoleCharSize*2 + 2*frameX_Width,
		mBufferHeight*c_ConsoleCharSize*2 + frameY_Caption+frameY_Height, 
		true);

}

void IRenderer::mFunction_UpdateCharAndTextAttrBuffer()
{
	//Update Char And Text Attr Buffer according to Color Buffer,
	//almost the last operation before using Win API to draw colored chars
	//to screen


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

	//define colors...
	const WORD lowR = FOREGROUND_RED;
	const WORD highR = FOREGROUND_RED | FOREGROUND_INTENSITY;
	const WORD lowG = FOREGROUND_GREEN;
	const WORD highG = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	const WORD lowB = FOREGROUND_BLUE;
	const WORD highB = FOREGROUND_BLUE | FOREGROUND_INTENSITY;


	//define what char & color & textAttr it will present of each color level
	const UINT c_color_level_Count = 9;

	/*const char color_level_to_char[c_color_level_Count] = {
	' ',	'.',';','s','@'		,'i','s', '@'
	};*/
	const char color_level_to_char[c_color_level_Count] = {
		' ',	'.',';','s','#'		,'i','s','&','#'
	};
	const WORD color_level_R_to_textAttr[c_color_level_Count] = {
		0,lowR,lowR,lowR,lowR,highR,highR,highR,highR
	};

	const WORD color_level_G_to_textAttr[c_color_level_Count] = {
		0,lowG,lowG,lowG,lowG,highG,highG,highG,highG
	};

	const WORD color_level_B_to_textAttr[c_color_level_Count] = {
		0,lowB,lowB,lowB,lowB,highB,highB,highB,highB
	};

	for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
	{
		COLOR3& color = m_pColorBuffer->at(i);

		//quantization
		//the .01 is in case the r/g/b=255, I want the rLevel = [0,7]
		UINT rLevel = UINT(c_color_level_Count	 * (color.r) / 255.01f);
		UINT gLevel = UINT(c_color_level_Count * (color.g) / 255.01f);
		UINT bLevel = UINT(c_color_level_Count * (color.b) / 255.01f);
		UINT maxLevel = max(max(rLevel, gLevel), bLevel);

		m_pCharBuffer->at(i) = color_level_to_char[maxLevel];
		WORD textAttr = (color_level_R_to_textAttr[rLevel] |
			color_level_G_to_textAttr[gLevel] |
			color_level_B_to_textAttr[bLevel]);
		m_pTextAttrBuffer->at(i) = textAttr;
	}
}

/*inline void IRenderer::mFunction_SetAllColorBuffer(UINT offset,const COLOR3& color)
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
	*\/

	//I think I should make full use of the foreGround and BackGround color,
	//control the percentage of foreground coverage, 
	//to simulate linear interpolation between foreground and background color,
	//in order to "GENERATE" more preset color sample in color space
	//if I only use console's color , I'll only got 15 colors in total, which is a severe shortage.
	const WORD lowR = FOREGROUND_RED;
	const WORD highR = FOREGROUND_RED | FOREGROUND_INTENSITY;
	const WORD lowG = FOREGROUND_GREEN;
	const WORD highG = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	const WORD lowB = FOREGROUND_BLUE;
	const WORD highB = FOREGROUND_BLUE | FOREGROUND_INTENSITY;


	//define what char & color & textAttr it will present of each color level
	const UINT c_color_level_Count = 8;

	const char color_level_to_char[c_color_level_Count] = {
	' ',	'.',';','s','@'		,'i','s', '@'
	};

	const WORD color_level_R_to_textAttr[c_color_level_Count] = {
		lowR,lowR,lowR,lowR,lowR,highR,highR,highR
	};

	const WORD color_level_G_to_textAttr[c_color_level_Count] = {
		lowR,lowG,lowG,lowG,lowG,highG,highG,highG
	};

	const WORD color_level_B_to_textAttr[c_color_level_Count] = {
		lowR,lowB,lowB,lowB,lowB,highB,highB,highB
	};

	//quantization
	//the .01 is in case the r/g/b=255, I want the rLevel = [0,7]
	UINT rLevel = c_color_level_Count	 * (color.r) / 255.01f;
	UINT gLevel = c_color_level_Count * (color.g) / 255.01f;
	UINT bLevel = c_color_level_Count * (color.b) / 255.01f;
	UINT maxLevel = max(max(rLevel, gLevel), bLevel);

	m_pCharBuffer->at(offset) = color_level_to_char[maxLevel];
	m_pColorBuffer->at(offset) = std::move(color);//std::move -> faster "copy"
	m_pTextAttrBuffer->at(offset) =
		color_level_R_to_textAttr[rLevel] |
		color_level_G_to_textAttr[gLevel] |
		color_level_B_to_textAttr[bLevel];
}*/