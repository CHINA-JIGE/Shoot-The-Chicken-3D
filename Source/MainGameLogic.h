/***********************************************************************

                           h£ºMain Game Logic 

************************************************************************/

#pragma once

namespace GamePlay
{

	//restrict rocks and player movement
	const float c_halfMovementRestrictBoxWidth = 800.0f;

	class IMainGame
	{
	public:

		IMainGame();

		void	Init();

		void	UpdateLogic();

		void	Render();

	private:

		//update logic
		void					mFunction_UpdateLogic_Playing();

		void					mFunction_UpdateLogic_GameOverMenu();

		void					mFunction_UpdateLogic_PauseMenu();

		//render function 
		void					mFunction_Render_Playing();

		void					mFunction_Render_GameOverMenu();

		void					mFunction_Render_PauseMenu();

		//render only after all other stuffs have been rendered
		void					mFunction_Render_HUD();

		void					mFunction_CollisionDetectionAndInteract();

		IChickenMonster			mChickenBoss;
		IPlayer							mPlayer;
		ISceneModelManager	mSceneMgr;
		IBulletManager				mBulletMgr;
		int								mMainGameState;
	};
};