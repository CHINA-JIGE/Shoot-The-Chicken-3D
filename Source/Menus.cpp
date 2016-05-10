#include "Game.h"

using namespace GamePlay;

void IMenus::Init()
{
	mMainMenuBgr_Start.LoadPPM("Media/GUI/MainMenu_start.ppm");
	mMainMenuBgr_Exit.LoadPPM("Media/GUI/MainMenu_Exit.ppm");
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
			gMainGame.Init();
			gRootGameState = GameState::GS_MainGame;
			Sleep(300);
		}
		if (IS_KEY_DOWN(VK_DOWN))
		{
			//move cursor to choose "QUIT"
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
