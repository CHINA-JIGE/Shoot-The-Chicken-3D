#pragma once

#include "MyConsoleEngine.h"

#include "Menus.h"
#include "Base_GameObject.h"
#include "Asteroid.h"
#include "SceneEnvironment.h"
#include "Chicken.h"
#include "Player.h"
#include "MainGameLogic.h"

namespace GamePlay
{
	namespace GameState
	{
		const int GS_StartAnimation = 1;
		const int GS_StartMenu = 2;
		const int GS_ChooseSceneMenu=3;
		const int GS_MainGame = 4;

		namespace  StartMenu
		{
			const int GS_Choose_Start =20;
			const int GS_Choose_Quit = 21;
		}

		namespace  ChooseSceneMenu
		{
			const int GS_Cosmos = 30;
			const int GS_CheckerBoard = 31;
		};

		namespace MainGame
		{
			const int GS_MainGameStartAnimation = 45;
			const int GS_Playing = 40;
			const int GS_GameOverMenu = 41;
			const int GS_PauseMenu = 42;
			const int GS_DeathExplode = 43;

			namespace  GameOverMenu
			{
				const int GS_Win_Choose_Back = 410;
				const int GS_Lose_Choose_Back = 411;
			};

			namespace PauseMenu
			{
				const int GS_Choose_Continue = 420;
				const int GS_Choose_Quit = 421;
			};
		};
	};

	extern IRenderer			gRenderer;
	extern UINT					gFPS;
	extern float					gTimeElapsed;
	extern ICamera				gCamera;
	extern int						gRootGameState;

	extern IMainGame		gMainGame;
	extern IMenus				gMenus;

	//global initialization
	void	InitGlobal();

	//driven program of game state machine
	void	GameStateSelector();

	void UpdateWindowTitle();


	//loop body for each game state
	void	StartAnimation();

	void	StartMenu();

	void	ChooseSceneMenu();

	void	MainGame();

};