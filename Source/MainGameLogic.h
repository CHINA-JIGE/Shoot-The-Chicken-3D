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

		void	Init(SCENE_TYPE modelID);

		void	UpdateAndRenderMainGame();//including Menus

	private:
		
		void					mFunction_UpdateAndRenderStartAnimaton();

		void					mFunction_UpdateAndRenderPlaying();

		void					mFunction_UpdateAndRender_GameOverMenu(BOOL hasWon);

		void					mFunction_UpdateAndRender_PauseMenu();

		void					mFunction_UpdateAndRenderDeathExplode();//chicken die

		//..............OTHERS
		void					mFunction_CollisionDetectionAndInteract();

		void					mFunction_GameOverAnimationInit(BOOL hasPlayerWon);

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