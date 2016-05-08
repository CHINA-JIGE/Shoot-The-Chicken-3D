#include "Game.h"

using namespace GamePlay;

IMainGame::IMainGame()
	:mChickenBoss(&mBulletMgr),
	mPlayer(&mBulletMgr)
{
}

void IMainGame::Init()
{
	mSceneMgr.Init(SCENE_MODEL_ID::SCENEMODEL_COSMOS1);
	mChickenBoss.Init();
	mPlayer.Init();
	mMainGameState = GameState::MainGame::GS_Playing;
}

void IMainGame::UpdateLogic()
{
	switch (mMainGameState)
	{
	case GameState::MainGame::GS_Playing:
		mFunction_UpdateLogic_Playing();
		break;

	case GameState::MainGame::GS_PauseMenu:
		mFunction_UpdateLogic_PauseMenu();
		break;

	case GameState::MainGame::GS_GameOverMenu:
		mFunction_UpdateLogic_GameOverMenu();
		break;
	}
}

void IMainGame::Render()
{
	gRenderer.Clear();

	switch (mMainGameState)
	{
	case GameState::MainGame::GS_Playing:
		mFunction_Render_Playing();
		break;

	case GameState::MainGame::GS_PauseMenu:
		mFunction_Render_PauseMenu();
		break;

	case GameState::MainGame::GS_GameOverMenu:
		mFunction_Render_GameOverMenu();
		break;
	}

	gRenderer.Present();
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

	//state transition-  switch to Game Over Menu
	if (mPlayer.IsDead() == TRUE)
	{
		mMainGameState = GameState::MainGame::GS_GameOverMenu;
	}
}

void IMainGame::mFunction_UpdateLogic_GameOverMenu()
{
	static int menuState = GameState::MainGame::GameOverMenu::GS_Choose_Restart;
	switch (menuState)
	{
	case GameState::MainGame::GameOverMenu::GS_Choose_Restart:
		break;

	case GameState::MainGame::GameOverMenu::GS_Choose_Quit:
		break;

	default:
		DEBUG_MSG1("Game State Error!!");
		break;
	}
}

void IMainGame::mFunction_UpdateLogic_PauseMenu()
{
	static int menuState = GameState::MainGame::PauseMenu::GS_Choose_Continue;
	switch (menuState)
	{
	case GameState::MainGame::PauseMenu::GS_Choose_Continue:
		break;

	case GameState::MainGame::PauseMenu::GS_Choose_Quit:
		break;

	default:
		DEBUG_MSG1("Game State Error!!");
		break;
	}
}

void IMainGame::mFunction_Render_Playing()
{
	mSceneMgr.Render();
	mChickenBoss.Render();
	mPlayer.Render();
	mBulletMgr.Render();
}

void IMainGame::mFunction_Render_GameOverMenu()
{
}

void IMainGame::mFunction_Render_PauseMenu()
{
}

void IMainGame::mFunction_Render_HUD()
{
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

