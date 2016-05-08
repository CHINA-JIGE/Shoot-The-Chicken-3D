#pragma once

class IPicture :private IFileManager
{
public:
		
	friend class IRenderer;

	IPicture();

	~IPicture();

	BOOL	LoadPPM(std::string ppmFilePath);

	UINT		GetWidth();

	UINT		GetHeight();

	void			SetPixel(UINT x, UINT y,const COLOR3& color);

	COLOR3	GetPixel(UINT x, UINT y);

private:
	std::vector<COLOR3>* m_pColorBuffer;
	UINT mWidth;
	UINT mHeight;

};