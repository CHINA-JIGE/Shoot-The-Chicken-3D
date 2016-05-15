
#include "Game.h"

using namespace GamePlay;

static ITimer				localTimer(TIMER_TIMEUNIT_MILLISECOND);

namespace GamePlay
{
	IRenderer				gRenderer;

	ICamera				gCamera;
	IMainGame			gMainGame;
	int						gRootGameState;
	float						gTimeElapsed = 0.0f;
	UINT						gFPS=0;
	IMenus					gMenus;

};

void GamePlay::InitGlobal()
{
	gRootGameState = GameState::GS_StartAnimation;
	gRenderer.Init(250, 85);
	gRenderer.SetWindowTitle("Shoot The Chicken 3D - By Jige");
	gRenderer.SetCamera(gCamera);
	gMenus.Init();
}

void GamePlay::UpdateWindowTitle()
{
	std::string titleStr;
	titleStr = "Shoot The Chicken 3D - By Jige  FPS:" + std::to_string(gFPS);
	gRenderer.SetWindowTitle(titleStr.c_str());
}

void GamePlay::GameStateSelector()
{
	switch (UINT(gRootGameState))
	{
	case GameState::GS_StartAnimation:
		StartAnimation();
		break;

	case GameState::GS_StartMenu:
		StartMenu();
		break;

	case GameState::GS_ChooseSceneMenu:
		ChooseSceneMenu();
		break;

	case GameState::GS_MainGame:
		MainGame();
		break;

	default:
		DEBUG_MSG1("game state error!!");
		exit(0);
		break;
	}
}


void GamePlay::StartAnimation()
{
	//currently no start animation
	gRootGameState = GameState::GS_StartMenu;
}

void GamePlay::StartMenu()
{
	gMenus.UpdateAndRender_StartMenu();
}

void GamePlay::MainGame()
{
	//use unified global time line
	localTimer.NextTick();
	gTimeElapsed = Clamp(localTimer.GetInterval(), 0.0f, 100.0f);
	gFPS = localTimer.GetFPS();

	gMainGame.UpdateAndRenderMainGame();
}

void GamePlay::ChooseSceneMenu()
{
	gMenus.UpdateAndRender_ChooseScene();
}
