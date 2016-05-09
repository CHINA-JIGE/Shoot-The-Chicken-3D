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

		void	UpdateAndRenderMainGame();//including Menus

	private:

		//update logic
		void					mFunction_UpdateLogic_Playing();

		void					mFunction_UpdateAndRender_GameOverMenu(BOOL hasWon);

		void					mFunction_UpdateAndRender_PauseMenu();

		//render function 
		void					mFunction_Render_Playing();

		//render only after all other stuffs have been rendered
		void					mFunction_Render_Playing_HUD();

		void					mFunction_CollisionDetectionAndInteract();

		IChickenMonster			mChickenBoss;
		IPlayer							mPlayer;
		ISceneModelManager	mSceneMgr;
		IBulletManager				mBulletMgr;
		int								mMainGameState;
		BOOL							mIsPlayerVictorious;//has player won??

		IPicture				mPauseMenuBgr_Continue;
		IPicture				mPauseMenuBgr_Back;

		IPicture				mGameOverMenuBgr_Win;
		IPicture				mGameOverMenuBgr_Lose;
	};
};