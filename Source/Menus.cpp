#include "Game.h"

using namespace GamePlay;

void IMenus::Init()
{
	mMainMenuBgr_Start.LoadPPM("Media/GUI/MainMenu_start.ppm");
	mMainMenuBgr_Exit.LoadPPM("Media/GUI/MainMenu_Exit.ppm");

	mChooseSceneBgr_Cosmos.LoadPPM("Media/GUI/ChooseScene_Cosmos.ppm");
	mChooseSceneBgr_Checkerboard.LoadPPM("Media/GUI/ChooseScene_checkerBoard.ppm");
}

void IMenus::UpdateAndRender_StartMenu()
{
	static int startMenuState = GameState::StartMenu::GS_Choose_Start;

	//draw to the whole screen
	UINT width = gRenderer.GetBufferWidth();
	UINT height = gRenderer.GetBufferHeight();

	gRenderer.Clear();

	switch (startMenuState)
	{
	case GameState::StartMenu::GS_Choose_Start:
	{
		//in start menu,choose "Start Game"
		if (IS_KEY_DOWN(VK_RETURN))
		{
			gRootGameState = GameState::GS_ChooseSceneMenu;
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_DOWN))
		{
			//move cursor to choose "QUIT"
			startMenuState = GameState::StartMenu::GS_Choose_Quit;
			Sleep(300);
		}
		
		//render
		gRenderer.DrawPicture(mMainMenuBgr_Start, 0, 0, width, height);

		break;
	}
	case GameState::StartMenu::GS_Choose_Quit:
	{
		if (IS_KEY_DOWN(VK_RETURN))
		{
			//exit the program
			exit(0);
			break;
		}
		if (IS_KEY_DOWN(VK_UP))
		{
			startMenuState = GameState::StartMenu::GS_Choose_Start;
			Sleep(300);
		}

		//render
		gRenderer.DrawPicture(mMainMenuBgr_Exit, 0, 0, width, height);
		break;
	}

	default:
		DEBUG_MSG1("game state error!!");
		break;
	}


	gRenderer.Present();
}

void IMenus::UpdateAndRender_ChooseScene()
{
	static int chooseSceneMenuState = GameState::ChooseSceneMenu::GS_Cosmos;

	//draw to the whole screen
	UINT width = gRenderer.GetBufferWidth();
	UINT height = gRenderer.GetBufferHeight();

	gRenderer.Clear();

	switch (chooseSceneMenuState)
	{

	case GameState::ChooseSceneMenu::GS_Cosmos:
	{
		//SCENE1: COSMOS 迷之宇宙
		if (IS_KEY_DOWN(VK_RETURN))
		{
			gMainGame.Init(SCENE_TYPE::SCENE_COSMOS1);
			gRootGameState = GameState::GS_MainGame;
			Sleep(300);
		};

		//next scene
		if (IS_KEY_DOWN(VK_LEFT))
		{
			chooseSceneMenuState = GameState::ChooseSceneMenu::GS_CheckerBoard;
			Sleep(300);
		}

		if (IS_KEY_DOWN(VK_RIGHT))
		{
			chooseSceneMenuState = GameState::ChooseSceneMenu::GS_CheckerBoard;
			Sleep(300);
		}

		//render
		gRenderer.DrawPicture(mChooseSceneBgr_Cosmos, 0, 0, width, height);
		break;
	}

	case GameState::ChooseSceneMenu::GS_CheckerBoard:
	{		
		//SCENE2: CHECKERBOARD 格子世界
		if (IS_KEY_DOWN(VK_RETURN))
		{
			gMainGame.Init(SCENE_TYPE::SCENE_CHECKERBOARD);
			gRootGameState = GameState::GS_MainGame;
			Sleep(300);
		};

		//next scene
		if (IS_KEY_DOWN(VK_LEFT))
		{
			chooseSceneMenuState = GameState::ChooseSceneMenu::GS_Cosmos;
			Sleep(300);
		}

		if (IS_KEY_DOWN(VK_RIGHT))
		{
			chooseSceneMenuState = GameState::ChooseSceneMenu::GS_Cosmos;
			Sleep(300);
		}

		//render
		gRenderer.DrawPicture(mChooseSceneBgr_Checkerboard, 0, 0, width, height);
		break;
	}

	}

	gRenderer.Present();
}

