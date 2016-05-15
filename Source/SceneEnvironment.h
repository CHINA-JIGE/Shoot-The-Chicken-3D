#pragma once

namespace GamePlay
{
	enum SCENE_TYPE
	{
		SCENE_COSMOS1 = 1,
		SCENE_CHECKERBOARD=2,
	};

	class ISceneModelManager
	{
	public:

		void		Init(SCENE_TYPE type);

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