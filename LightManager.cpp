
/***********************************************************************

                           类：NOISE Light Engine

			简述：主要负责管理场景的灯光，但不负责更新到GPU

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

ILightManager::ILightManager()
{
	m_pLightList_Dir_Dynamic	= new std::vector<IDirectionalLight*>;
	m_pLightList_Point_Dynamic	= new std::vector<IPointLight*>;
	m_pLightList_Spot_Dynamic		=	new std::vector<ISpotLight*>;
	mIsDynamicLightingEnabled = TRUE;

	m_pLightList_Dir_Static		= new std::vector<IDirectionalLight>;
	m_pLightList_Point_Static	= new std::vector<IPointLight>;
	m_pLightList_Spot_Static	=	new std::vector<ISpotLight>;
	mIsStaticLightingEnabled = TRUE;
	mCanUpdateStaticLights = FALSE;
}

void ILightManager::Destroy()
{
	m_pFatherScene = nullptr;

};

//这些烂鬼add remove 就用macro吧
void	ILightManager::AddDynamicDirLight(IDirectionalLight& refLight)
{
	mFunction_AddLight<IDirectionalLight*>(m_pLightList_Dir_Dynamic, &refLight,10);
};

void	ILightManager::AddDynamicPointLight(IPointLight& refLight)
{
	mFunction_AddLight<IPointLight*>(m_pLightList_Point_Dynamic,&refLight,10);
};          

void ILightManager::AddDynamicSpotLight(ISpotLight& refLight)
{
	mFunction_AddLight<ISpotLight*>(m_pLightList_Spot_Dynamic, &refLight,10);
};

void	ILightManager::RemoveDynamicDirLight(IDirectionalLight& refLight)
{
	mFunction_RemoveLight_ByAddr<IDirectionalLight*>(m_pLightList_Dir_Dynamic, &refLight);
};

void	ILightManager::RemoveDynamicPointLight(IPointLight& refLight)
{
	mFunction_RemoveLight_ByAddr<IPointLight*>(m_pLightList_Point_Dynamic, &refLight);
};

void	ILightManager::RemoveDynamicSpotLight(ISpotLight& refLight)
{	
	mFunction_RemoveLight_ByAddr<ISpotLight*>(m_pLightList_Spot_Dynamic, &refLight);
};

void	ILightManager::SetDynamicLightingEnabled(BOOL isEnabled)
{
	mIsDynamicLightingEnabled = isEnabled;
};



void	ILightManager::AddStaticDirLight(IDirectionalLight iLight)
{
	mFunction_AddLight<IDirectionalLight>(m_pLightList_Dir_Static,iLight,50);
	mCanUpdateStaticLights = TRUE; 
};

void	ILightManager::AddStaticPointLight(IPointLight iLight)
{
	mFunction_AddLight<IPointLight>(m_pLightList_Point_Static,iLight,50);
	mCanUpdateStaticLights = TRUE; 
};

void	ILightManager::AddStaticSpotLight(ISpotLight iLight)
{
	mFunction_AddLight<ISpotLight>(m_pLightList_Spot_Static,iLight,50);
	mCanUpdateStaticLights = TRUE; 
};

void	ILightManager::RemoveStaticDirLight(UINT iLight_Index)
{
	mFunction_RemoveLight_ByIndex<IDirectionalLight>(m_pLightList_Dir_Static,iLight_Index);
	mCanUpdateStaticLights = TRUE; 
};

void	ILightManager::RemoveStaticPointLight(UINT iLight_Index)
{
	mFunction_RemoveLight_ByIndex<IPointLight>(m_pLightList_Point_Static,iLight_Index);
	mCanUpdateStaticLights = TRUE; 
};

void	ILightManager::RemoveStaticSpotLight(UINT iLight_Index)
{
	mFunction_RemoveLight_ByIndex<ISpotLight>(m_pLightList_Spot_Static,iLight_Index);
	mCanUpdateStaticLights = TRUE; 
};

void	ILightManager::SetStaticLightingEnabled(BOOL isEnabled)
{
	mIsStaticLightingEnabled = isEnabled;
};


UINT	 ILightManager::GetLightCount(NOISE_LIGHT_TYPE lightType)
{
	switch(lightType)
	{
	case NOISE_LIGHT_TYPE_DIRECTIONAL :
		return (m_pLightList_Dir_Dynamic->size()+m_pLightList_Dir_Static->size());		
		break;
	case NOISE_LIGHT_TYPE_POINT :
		return (m_pLightList_Point_Dynamic->size()+m_pLightList_Point_Static->size());		
		break;
	case NOISE_LIGHT_TYPE_SPOT :
		return (m_pLightList_Spot_Dynamic->size()+m_pLightList_Spot_Static->size());		
		break;
	}
	return 0;
};

UINT	 ILightManager::GetTotalLightCount()
{
	return m_pLightList_Dir_Dynamic->size()+m_pLightList_Dir_Static->size()+
			m_pLightList_Point_Dynamic->size()+m_pLightList_Point_Static->size() + 
			m_pLightList_Spot_Dynamic->size()+m_pLightList_Spot_Static->size();
};


/***********************************************************************
								PRIVATE					                    
***********************************************************************/
template <typename T> 
void ILightManager::mFunction_AddLight(std::vector<T>* pList, T iLight,UINT maxElementCount)
{
	std::vector<T>::iterator tmp_iter; 
		for(tmp_iter = pList->begin();	tmp_iter != pList->end(); tmp_iter++) 
		{ 
		//找到重复的灯光就不add了
		if(*tmp_iter == iLight) return; 
		}; 
		if ( pList->size() < maxElementCount) pList->push_back(iLight); 
};

template <typename T> 
void ILightManager::mFunction_RemoveLight_ByAddr(std::vector<T>* pList, T DelLight)
{
	std::vector<T>::iterator tmp_iter; 
	for(tmp_iter = pList->begin();	tmp_iter != pList->end(); tmp_iter++) 
	{ 
	//把vector当前位置的空位和内容都抹掉
	if(*tmp_iter == DelLight){tmp_iter = pList->erase(tmp_iter);break;} 
	}; 
};

template <typename T> 
void ILightManager::mFunction_RemoveLight_ByIndex(std::vector<T>* pList, UINT lightIndex)
{
	std::vector<T>::iterator tmp_iter = pList->begin(); 
		if(pList->size() > lightIndex) 
		{ 
			tmp_iter += lightIndex; 
			pList->erase(tmp_iter);  
		} 
};
