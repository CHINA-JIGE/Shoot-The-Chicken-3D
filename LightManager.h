/***********************************************************************

                           h£ºNoiseLightManager

************************************************************************/

#pragma once


struct IDirectionalLight
{
public:
	IDirectionalLight()
	{
		ZeroMemory(this, sizeof(*this));
		mSpecularIntensity = 1.0f;
		mDirection = VECTOR3(1.0f, 0, 0);
		mDiffuseIntensity = 0.5;
		mIsEnabled = TRUE;
	}

	BOOL operator==(IDirectionalLight& Light)
	{
		//two memory fragments are identical
		if (memcmp(this, &Light, sizeof(Light)) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	VECTOR3 mAmbientColor;	 float		mSpecularIntensity;
	VECTOR3 mDiffuseColor;		float			mDiffuseIntensity;
	VECTOR3 mSpecularColor;	 BOOL		mIsEnabled;
	VECTOR3 mDirection;			 
};

	class _declspec(dllexport) ILightManager
	{
	public:
		friend class  IRenderer;

		//¹¹Ôìº¯Êý
		ILightManager();

		void		AddLight(IDirectionalLight& refLight);

		void		RemoveLight(IDirectionalLight& refLight);

		IDirectionalLight* GetLight(UINT index);

		UINT		GetTotalLightCount();


	private:

		void		Destroy();

		template <typename T>
		void mFunction_AddLight(std::vector<T>* pList, T iLight, UINT maxElementCount);

		template <typename T>
		void mFunction_RemoveLight_ByAddr(std::vector<T>* pList, T DelLight);

		template <typename T>
		void mFunction_RemoveLight_ByIndex(std::vector<T>* pList, UINT lightIndex);


	private:
		std::vector<IDirectionalLight*>*	m_pLightList;
	};