#include "Game.h"

using namespace GamePlay;

void IMenus::Init()
{
	mMainMenuBgr_Start.LoadPPM("Media/GUI/MainMenu_start.ppm");
	mMainMenuBgr_Exit.LoadPPM("Media/GUI/MainMenu_Exit.ppm");

	//mPauseMenuBgr_Continue.LoadPPM("Media/GUI/PauseMenu_Continue.ppm");
	//mPauseMenuBgr_Back.LoadPPM("Media/GUI/PauseMenu_Back.ppm");
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
		if (IS_KEY_DOWN(VK_RETURN))
		{
			gMainGame.Init();
			
			gRootGameState = GameState::GS_MainGame;
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_DOWN))
		{
			startMenuState = GameState::StartMenu::GS_Choose_Quit;
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

/*void IMenus::UpdateAndRenderPauseMenu()
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
		if (IS_KEY_DOWN(VK_RETURN)==TRUE || IS_KEY_DOWN(VK_ESCAPE)==TRUE)
		{
			//go back to main game
			gRootGameState = GameState::GS_MainGame;
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
		}

		//ESCAPE can go back to main game
		if (IS_KEY_DOWN(VK_ESCAPE))
		{
			//go back to main game
			gRootGameState = GameState::GS_MainGame;
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
}*/

/*void IMenus::UpdateRenderGameOverMenu(BOOL hasWon)
{
}*/
