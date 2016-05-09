#pragma once

namespace GamePlay
{
	enum SCENE_MODEL_ID
	{
		SCENEMODEL_COSMOS1 = 1
	};

	class ISceneModelManager
	{
	public:

		void		Init(SCENE_MODEL_ID model);

		void		Update();

		void		Render();

		BOOL	CollisionDetection_Rocks(const VECTOR3& rayStart, const VECTOR3& rayEnd,VECTOR3& outCollidePoint);

	private:
			Material				mDefaultMaterial;
			IPicture					mSkyTexture;
			IMesh					mSkyModel;

			static const  UINT  c_asteroidCount = 20;
			IAsteroid				mAsteroid[c_asteroidCount];

			DirectionalLight	mSceneLight;

	};
}