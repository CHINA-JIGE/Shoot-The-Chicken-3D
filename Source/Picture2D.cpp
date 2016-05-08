
/************************************************************

				Picture Interface , providing interfaces to 
				2D picture loading and storage.

************************************************************/

#include "MyConsoleEngine.h"

IPicture::IPicture()
{
	m_pColorBuffer = new std::vector<COLOR3>;
}

IPicture::~IPicture()
{
	delete m_pColorBuffer;
}

BOOL IPicture::LoadPPM(std::string ppmFilePath)
{
	BOOL yes=IFileManager::ImportFile_PPM(ppmFilePath, mWidth, mHeight, *m_pColorBuffer);
	if (yes == TRUE)
	{
		return TRUE;
	}
	else
	{
		mWidth = 0;
		mHeight = 0;
		DEBUG_MSG1("IPicture: Load PPM failed!!");
		return FALSE;
	}
}

UINT IPicture::GetWidth()
{
	return mWidth;
}

UINT IPicture::GetHeight()
{
	return mHeight;
}

inline void IPicture::SetPixel(UINT x, UINT y,const COLOR3& color)
{
	if (x<mWidth && y<mHeight)
	{
		UINT index = y*mWidth + x;
		m_pColorBuffer->at(index) = color;
	}
}

COLOR3 IPicture::GetPixel(UINT x, UINT y)
{
	if (x<mWidth && y<mHeight)
	{
		UINT index = y*mWidth + x;
		return m_pColorBuffer->at(index);
	}
	return COLOR3(255, 0, 0);
}
