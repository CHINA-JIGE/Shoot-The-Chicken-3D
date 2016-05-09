#include "Game.h"

using namespace GamePlay;

IMainGame::IMainGame()
	:mChickenBoss(&mBulletMgr),
	mPlayer(&mBulletMgr),
	mIsPlayerVictorious(FALSE)
{
}

void IMainGame::Init()
{
	mSceneMgr.Init(SCENE_MODEL_ID::SCENEMODEL_COSMOS1);
	mChickenBoss.Init();
	mPlayer.Init();
	mMainGameState = GameState::MainGame::GS_Playing;

	mPauseMenuBgr_Continue.LoadPPM("Media/GUI/PauseMenu_Continue.ppm");
	mPauseMenuBgr_Back.LoadPPM("Media/GUI/PauseMenu_Back.ppm");
	mGameOverMenuBgr_Win.LoadPPM("Media/GUI/GameOver_Win.ppm");
	mGameOverMenuBgr_Lose.LoadPPM("Media/GUI/GameOver_Lose.ppm");
}

void IMainGame::UpdateAndRenderMainGame()
{
	switch (mMainGameState)
	{
	case GameState::MainGame::GS_Playing:
	{
		gRenderer.Clear();
		mFunction_UpdateLogic_Playing();
		mFunction_Render_Playing();
		mFunction_Render_Playing_HUD();
		gRenderer.Present();
		break;
	}

	case GameState::MainGame::GS_PauseMenu:
	{
		mFunction_UpdateAndRender_PauseMenu();
		break;
	}

	case GameState::MainGame::GS_GameOverMenu:
	{
		mFunction_UpdateAndRender_GameOverMenu(mIsPlayerVictorious);
		break;
	}

	default:
		DEBUG_MSG1("game state error!!");
		break;
	}
}

/**********************************************************
								PRIVATE
*********************************************************/
void IMainGame::mFunction_UpdateLogic_Playing()
{

	mSceneMgr.Update();
	mPlayer.Update();
	mChickenBoss.Update(mPlayer.GetPosition());

	//collision detection
	mFunction_CollisionDetectionAndInteract();

	mBulletMgr.UpdateBullets();

	if (IS_KEY_DOWN(VK_ESCAPE))
	{
		mMainGameState = GameState::MainGame::GS_PauseMenu;
		Sleep(300);
	}

	//state transition-  switch to Game Over Menu
	if (mPlayer.IsDead() == TRUE)
	{
		mMainGameState = GameState::MainGame::GS_GameOverMenu;
		mIsPlayerVictorious = FALSE;
	}

	if (mChickenBoss.IsDead() == TRUE)
	{
		mMainGameState = GameState::MainGame::GS_GameOverMenu;
		mIsPlayerVictorious = TRUE;
	}
}

void IMainGame::mFunction_Render_Playing()
{
	mSceneMgr.Render();
	mChickenBoss.Render();
	mPlayer.Render();
	mBulletMgr.Render();
}

void IMainGame::mFunction_Render_Playing_HUD()
{
	//blood bar
	UINT bloodBarWidth = UINT(50.0f* (mPlayer.GetHP() / c_playerInitalHealth));
	gRenderer.DrawRect({ 1.0f,0,0 }, 5, 3, 5 + bloodBarWidth, 5);

}

void IMainGame::mFunction_CollisionDetectionAndInteract()
{
	BOUNDINGBOX playerAABB, chickenAABB;
	mPlayer.GetBoundingBox(playerAABB);
	mChickenBoss.GetBoundingBox(chickenAABB);

	//----------First, bullet-player collision
	std::vector<VECTOR3> collidePointList;
	IPointCollection tmpPC;

	//player get hit
	mBulletMgr.CollisionDetection(playerAABB,TRUE, collidePointList);
	if (collidePointList.size() != 0)
	{
		mPlayer.ReduceHP(5.0f);
	}

	//chicken get hit
	mBulletMgr.CollisionDetection(chickenAABB, TRUE, collidePointList);
	if(collidePointList.size()!=0)
	{
		mChickenBoss.ReduceHP(5.0f);
		mChickenBoss.BeHitAndChangeColor();
	}

}


//------------------------------MENUS------------------------------
void IMainGame::mFunction_UpdateAndRender_GameOverMenu(BOOL hasWon)
{
	//draw to the whole screen
	UINT width = gRenderer.GetBufferWidth();
	UINT height = gRenderer.GetBufferHeight();

	gRenderer.Clear();

	if (hasWon == TRUE)
	{
		//WIN
		gRenderer.DrawPicture(mGameOverMenuBgr_Win, 0, 0, width, height);
		if (IS_KEY_DOWN(VK_RETURN))
		{
			gRootGameState = GameState::GS_StartMenu;
			mMainGameState = GameState::MainGame::GS_Playing;
			Sleep(300);
		}
	}
	else
	{
		//LOSE
		gRenderer.DrawPicture(mGameOverMenuBgr_Lose, 0, 0, width, height);
		if (IS_KEY_DOWN(VK_RETURN))
		{
			gRootGameState = GameState::GS_StartMenu;
			mMainGameState = GameState::MainGame::GS_Playing;
			Sleep(300);
		}
	}

	gRenderer.Present();
}

void IMainGame::mFunction_UpdateAndRender_PauseMenu()
{
	static int pauseMenuState = GameState::MainGame::PauseMenu::GS_Choose_Continue;

	//draw to the whole screen
	UINT width = gRenderer.GetBufferWidth();
	UINT height = gRenderer.GetBufferHeight();

	gRenderer.Clear();

	//state transition
	switch (pauseMenuState)
	{
	case GameState::MainGame::PauseMenu::GS_Choose_Continue:
	{
		//ENTER or ESCAPE can go back to main game
		if (IS_KEY_DOWN(VK_RETURN) == TRUE || IS_KEY_DOWN(VK_ESCAPE) == TRUE)
		{
			//go back to main game
			gRootGameState = GameState::GS_MainGame;
			mMainGameState = GameState::MainGame::GS_Playing;

			//prevent that key are consistently pressed
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_DOWN))
		{
			//cursor move to choose "quit"
			pauseMenuState = GameState::MainGame::PauseMenu::GS_Choose_Quit;
		}

		//draw picture
		gRenderer.DrawPicture(mPauseMenuBgr_Continue, 0, 0, width, height);

		break;
	}

	case GameState::MainGame::PauseMenu::GS_Choose_Quit:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			gRootGameState = GameState::GS_StartMenu;
			//prevent that key are consistently pressed
			Sleep(300);
		}

		//ESCAPE can go back to main game
		if (IS_KEY_DOWN(VK_ESCAPE))
		{
			//go back to main game
			gRootGameState = GameState::GS_MainGame;
			mMainGameState = GameState::MainGame::GS_Playing;

			//prevent that key are consistently pressed
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_UP))
		{
			//cursor move to choose "quit"
			pauseMenuState = GameState::MainGame::PauseMenu::GS_Choose_Continue;
		}

		//draw picture
		gRenderer.DrawPicture(mPauseMenuBgr_Back, 0, 0, width, height);
		break;
	}

	default:
		DEBUG_MSG1("game state error!!");
		break;

	}
	gRenderer.Present();
}
