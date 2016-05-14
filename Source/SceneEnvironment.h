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

		void		GetAsteroidBoundingBox(UINT index,BOUNDINGBOX& outBox);

		UINT		GetAsteroidCount() const;

	private:
			Material				mDefaultMaterial;
			IPicture					mSkyTexture;
			IMesh					mSkyModel;

			static const  UINT  c_asteroidCount = 15;
			IAsteroid				mAsteroid[c_asteroidCount];

			DirectionalLight	mSceneLight;

	};
}