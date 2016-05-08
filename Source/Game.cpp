
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

};

void GamePlay::InitGlobal()
{
	gRootGameState = GameState::GS_StartAnimation;
	gRenderer.Init(250, 100);
	gRenderer.SetWindowTitle("Console Soft Renderer - By Jige");
	gRenderer.SetCamera(gCamera);

}

void GamePlay::UpdateWindowTitle()
{
	std::string titleStr;
	titleStr = "Soft Renderer - By Jige  FPS:" + std::to_string(gFPS);
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
		std::terminate();
		break;
	}
}


void GamePlay::StartAnimation()
{
	gRootGameState = GameState::GS_StartMenu;
}

void GamePlay::StartMenu()
{
	gMainGame.Init();
	gRootGameState = GameState::GS_MainGame;

}

void GamePlay::MainGame()
{
	//use unified global time line
	localTimer.NextTick();
	gTimeElapsed = Clamp(localTimer.GetInterval(), 0.0f, 100.0f);
	gFPS = localTimer.GetFPS();

	gMainGame.UpdateLogic();
	gMainGame.Render();
}

void GamePlay::ChooseSceneMenu()
{
}
