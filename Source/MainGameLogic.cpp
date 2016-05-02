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
}

void IMainGame::UpdateLogic()
{
	mSceneMgr.Update();
	mPlayer.Update();
	mChickenBoss.UpdateMovement();
	mBulletMgr.UpdateBullets();
}

void IMainGame::Render()
{
	gRenderer.Clear();

	//--------START RENDER----------
	mSceneMgr.Render();
	mChickenBoss.Render();
	mPlayer.Render();
	mBulletMgr.Render();
	//---------RENDER END--------------

	gRenderer.Present();
}

