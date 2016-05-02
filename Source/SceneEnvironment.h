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

	private:
			Material				mDefaultMaterial;
			IPicture					mSkyTexture;
			IMesh					mSkyModel;

			IPicture					mAsteroidTexture[10];
			IMesh					mAsteroidMesh[10];

			DirectionalLight	mSceneLight;

	};
}