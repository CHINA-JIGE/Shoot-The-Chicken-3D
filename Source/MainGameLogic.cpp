#include "Game.h"

using namespace GamePlay;

//I just arbitarily decide
const VECTOR3 c_CameraInitalPos_Cosmos = VECTOR3(1000.0f, 1000.0f, 1000.0f);
const VECTOR3 c_CameraIntialLookat_Cosmos = VECTOR3(0.0f, 1000.0f, 1000.0f);

const VECTOR3 c_CameraInitalPos_CheckerBoard= VECTOR3(-800.0f, 800.0f, 1000.0f);
const VECTOR3 c_CameraIntialLookat_CheckerBoard = VECTOR3(0.0f, 1000.0f, 1000.0f);

const VECTOR3 c_CameraTargetPos = VECTOR3(200.0f, 200.0f, 200.0f);
const VECTOR3 c_CameraTargetLookat = VECTOR3(0, 0, 0);


IMainGame::IMainGame()
	:mChickenBoss(&mBulletMgr),
	mPlayer(&mBulletMgr),
	mIsPlayerVictorious(FALSE)
{
}

void IMainGame::Init(SCENE_TYPE modelID)
{
	mSceneMgr.Init(modelID);
	mChickenBoss.Init(modelID);
	mPlayer.Init();
	mMainGameState = GameState::MainGame::GS_MainGameStartAnimation;
	//set camera pos
	switch (modelID)
	{
	case SCENE_TYPE::SCENE_COSMOS1:
		gCamera.SetPosition(c_CameraInitalPos_Cosmos);
		gCamera.SetLookAt(c_CameraIntialLookat_Cosmos);
		break;
		
	case SCENE_TYPE::SCENE_CHECKERBOARD:
		gCamera.SetPosition(c_CameraInitalPos_CheckerBoard);
		gCamera.SetLookAt(c_CameraIntialLookat_CheckerBoard);
		break;

	default:
		break;
	}


	mPauseMenuBgr_Continue.LoadPPM("Media/GUI/PauseMenu_Continue.ppm");
	mPauseMenuBgr_Back.LoadPPM("Media/GUI/PauseMenu_Back.ppm");
	mGameOverMenuBgr_Win.LoadPPM("Media/GUI/GameOver_Win.ppm");
	mGameOverMenuBgr_Lose.LoadPPM("Media/GUI/GameOver_Lose.ppm");
}

void IMainGame::UpdateAndRenderMainGame()
{
	switch (mMainGameState)
	{
	case GameState::MainGame::GS_MainGameStartAnimation:
	{
		mFunction_UpdateAndRenderStartAnimaton();
		break;
	}

	case GameState::MainGame::GS_Playing:
	{
		mFunction_UpdateAndRenderPlaying();
		break;
	}

	case GameState::MainGame::GS_DeathExplode:
	{
		//chicken/player die, boom
		mFunction_UpdateAndRenderDeathExplode();
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

void IMainGame::mFunction_UpdateAndRenderStartAnimaton()
{
	//timer of start animation
	static float startAnimationTimeCounter = 0.0f;
	startAnimationTimeCounter += gTimeElapsed;

	//move to target position
	VECTOR3 camPos = gCamera.GetPosition();
	VECTOR3 camLookat = gCamera.GetLookAt();
	float restDistance = (camPos - c_CameraTargetPos).Length();

	//proceed until the camera reach preset location
	if (restDistance > 10.0f)
	{
		gRenderer.Clear();

		//1 second proceed approximatly 30% of the planned route
		gCamera.SetPosition(Lerp(camPos, c_CameraTargetPos, gTimeElapsed*0.001f));
		gCamera.SetLookAt(Lerp(camLookat, c_CameraTargetLookat, gTimeElapsed*0.001f));

		//render without HUD
		mSceneMgr.Render();
		mChickenBoss.Render();

		gRenderer.Present();
	}
	else
	{
		//reach destination
		mMainGameState = GameState::MainGame::GS_Playing;
		startAnimationTimeCounter = 0.0f;
	}
}

void IMainGame::mFunction_UpdateAndRenderPlaying()
{
	//------------------------------LOGICS-----------------
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
		mFunction_GameOverAnimationInit(FALSE);
	}

	if (mChickenBoss.IsDead() == TRUE)
	{
		mFunction_GameOverAnimationInit(TRUE);
	}

	gRenderer.Clear();
	//------------------------render 3D----------------
	mSceneMgr.Render();
	mChickenBoss.Render();
	mPlayer.Render();
	mBulletMgr.Render();

	//-------------------------HUD-----------------------
	//player blood bar
	UINT bloodBarWidth = UINT(50.0f* (mPlayer.GetHP() / mPlayer.GetInitialHealth()));
	gRenderer.DrawRect({ 1.0f,0,0 }, 5, 3, 5 + bloodBarWidth, 5);

	//chicken boss blood bar
	UINT chickenBloodBarWidth = UINT(80.0f*(mChickenBoss.GetHP() / mChickenBoss.GetInitialHealth()));
	gRenderer.DrawRect({ 1.0f,1.0f,0 }, 60, 3, 60 + chickenBloodBarWidth, 5);

	//cross in the middle
	UINT width = gRenderer.GetBufferWidth();
	UINT height = gRenderer.GetBufferHeight();
	gRenderer.DrawLine({ 1.0f,1.0f,1.0f }, width / 2 - 4, height / 2, width / 2 + 4, height / 2);
	gRenderer.DrawLine({ 1.0f,1.0f,1.0f }, width / 2, height / 2 -4, width / 2, height / 2 +4);

	gRenderer.Present();
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

	collidePointList.clear();
	for (UINT i = 0;i < mSceneMgr.GetAsteroidCount();++i)
	{
		//retrieve bounding box of each asteroid
		BOUNDINGBOX rockBox;
		mSceneMgr.GetAsteroidBoundingBox(i, rockBox);

		//do nothing except killing collided bullets
		mBulletMgr.CollisionDetection(rockBox, TRUE, collidePointList);
	}

	collidePointList.clear();
}

void IMainGame::mFunction_GameOverAnimationInit(BOOL hasPlayerWon)
{
	static std::default_random_engine rndEngine;
	static std::uniform_real_distribution<float> unitDist(-1.0f, 1.0f);

	if (hasPlayerWon)
	{
		mMainGameState = GameState::MainGame::GS_DeathExplode;
		//clear bullets
		mBulletMgr.KillAllBullet();
		//player WIN
		mIsPlayerVictorious = TRUE;
		//set camera to look at chicken
		gCamera.SetLookAt(mChickenBoss.GetPosition());
		//..explode fireworks
		for (int i = 0;i < 2000;++i)
		{
			//shoot direction (add some random offset)
			VECTOR3 dir = { unitDist(rndEngine),unitDist(rndEngine) ,unitDist(rndEngine) };
			//Y direction offset ( a whole column of bullets)
			dir.Normalize();
			mBulletMgr.SpawnBullet(mChickenBoss.GetPosition(), dir, VECTOR3(1, 0,0));
		}
	}
	else
	{
		mMainGameState = GameState::MainGame::GS_DeathExplode;
		//clear bullets
		mBulletMgr.KillAllBullet();
		//player LOSE
		mIsPlayerVictorious = FALSE;
		//..explode fireworks
		for (int i = 0;i < 2000;++i)
		{
			//shoot direction (add some random offset)
			VECTOR3 dir = { unitDist(rndEngine),unitDist(rndEngine) ,unitDist(rndEngine) };
			//Y direction offset ( a whole column of bullets)
			dir.Normalize();
			mBulletMgr.SpawnBullet(mPlayer.GetPosition(), dir, VECTOR3(1, 0, 0));
		}

		//move to another position to watch the explosion
		gCamera.SetPosition(mPlayer.GetPosition() + VECTOR3(300.0f,300.0f,300.0f));
		gCamera.SetLookAt(mPlayer.GetPosition());
	}
}

void IMainGame::mFunction_UpdateAndRenderDeathExplode()
{
	static float deathExplodeTimeCounter = 0.0f;
	deathExplodeTimeCounter += gTimeElapsed;


	if (deathExplodeTimeCounter < 1000.0f)
	{
		gRenderer.Clear();

		mChickenBoss.Render();
		mSceneMgr.Render();

		gRenderer.Present();
	}

	//time to enjoy watching the explosion!!
	if (deathExplodeTimeCounter>=1000.0f && deathExplodeTimeCounter < 3000.0f)
	{
		gRenderer.Clear();

		if(mIsPlayerVictorious==FALSE)
		{
			//if player die, chicken lives, then chicken should be rendered
			mChickenBoss.Render();
		}

		mSceneMgr.Render();
		mBulletMgr.UpdateBullets();
		mBulletMgr.Render();

		gRenderer.Present();
	}

	//time to switch to death menu
	if (deathExplodeTimeCounter >= 3000.0f)
	{
		mMainGameState = GameState::MainGame::GS_GameOverMenu;
		deathExplodeTimeCounter = 0.0f;
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
