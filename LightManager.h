/***********************************************************************

                           h：NoiseLightManager

************************************************************************/

#pragma once


namespace Noise3D
{
	struct IDirectionalLight
	{
	public:
		IDirectionalLight()
		{
			ZeroMemory(this, sizeof(*this));
			mSpecularIntensity = 1.0f;
			mDirection = NVECTOR3(1.0f, 0, 0);
			mDiffuseIntensity = 0.5;
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

		NVECTOR3 mAmbientColor;	 float		mSpecularIntensity;
		NVECTOR3 mDiffuseColor;	float			mDiffuseIntensity;
		NVECTOR3 mSpecularColor;	 float		mPad2;//用于内存对齐
		NVECTOR3 mDirection;			 float		mPad3;//用于内存对齐
	};

	struct IPointLight
	{
		IPointLight() {
			ZeroMemory(this, sizeof(*this));
			mSpecularIntensity = 1.0f;
			mAttenuationFactor = 0.05f;
			mLightingRange = 100.0f;
			mDiffuseIntensity = 0.5;
		}

		BOOL operator==(IPointLight& Light)
		{
			//two memory fragments are identical
			if (memcmp(this, &Light, sizeof(Light)) == 0)
			{
				return TRUE;
			}
			return FALSE;
		}

		NVECTOR3 mAmbientColor;		float mSpecularIntensity;
		NVECTOR3 mDiffuseColor;		float mLightingRange;
		NVECTOR3 mSpecularColor;		float mAttenuationFactor;
		NVECTOR3 mPosition;				float mDiffuseIntensity;//memory alignment

	};

	struct ISpotLight
	{
		ISpotLight()
		{
			ZeroMemory(this, sizeof(*this));
			mSpecularIntensity = 1.0f;
			mAttenuationFactor = 1.0f;
			mLightingRange = 100.0f;
			mLightingAngle = MATH_PI / 4;
			mDiffuseIntensity = 0.5;
			mLitAt = NVECTOR3(0, 0, 0);
		}

		BOOL operator==(ISpotLight& Light)
		{
			//two memory fragments are identical
			if (memcmp(this, &Light, sizeof(Light)) == 0)
			{
				return TRUE;
			}
			return FALSE;
		}

		NVECTOR3 mAmbientColor;		float mSpecularIntensity;
		NVECTOR3 mDiffuseColor;		float mLightingRange;
		NVECTOR3 mSpecularColor;		float mAttenuationFactor;
		NVECTOR3 mLitAt;					float mLightingAngle;
		NVECTOR3 mPosition;				float mDiffuseIntensity;
	};


	class _declspec(dllexport) ILightManager
	{
	public:
		friend class IScene;
		friend class  IRenderer;

		//构造函数
		ILightManager();

		void		AddDynamicDirLight(IDirectionalLight& refLight);

		void		AddDynamicPointLight(IPointLight& refLight);

		void		AddDynamicSpotLight(ISpotLight& refLight);

		void		RemoveDynamicDirLight(IDirectionalLight& refLight);

		void		RemoveDynamicPointLight(IPointLight& refLight);

		void		RemoveDynamicSpotLight(ISpotLight& refLight);

		void		SetDynamicLightingEnabled(BOOL isEnabled);


		void		AddStaticDirLight(IDirectionalLight iLight);

		void		AddStaticPointLight(IPointLight iLight);

		void		AddStaticSpotLight(ISpotLight iLight);

		void		RemoveStaticDirLight(UINT iLight_Index);

		void		RemoveStaticPointLight(UINT iLight_Index);

		void		RemoveStaticSpotLight(UINT iLight_Index);

		void		SetStaticLightingEnabled(BOOL isEnabled);

		UINT		GetLightCount(NOISE_LIGHT_TYPE lightType);

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
		IScene*		m_pFatherScene;

		std::vector<IDirectionalLight*>*	m_pLightList_Dir_Dynamic;
		std::vector<IPointLight*>*			m_pLightList_Point_Dynamic;
		std::vector<ISpotLight*>*			m_pLightList_Spot_Dynamic;
		BOOL		mIsDynamicLightingEnabled;

		std::vector<IDirectionalLight>*	m_pLightList_Dir_Static;
		std::vector<IPointLight>*			m_pLightList_Point_Static;
		std::vector<ISpotLight>*			m_pLightList_Spot_Static;
		BOOL		mIsStaticLightingEnabled;
		BOOL		mCanUpdateStaticLights;

	};
}