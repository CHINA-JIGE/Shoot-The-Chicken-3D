#pragma once

#include "MyConsoleEngine.h"

#include "Base_GameObject.h"
#include "Asteroid.h"
#include "SceneEnvironment.h"
#include "Chicken.h"
#include "Player.h"
#include "MainGameLogic.h"

namespace GamePlay
{
	/*enum GameState
	{
		GS_StartAnimation = 1,
		GS_StartMenu = 2,
		GS_ChooseSceneMenu = 3,
		GS_MainGame = 4,
	};*/
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
			const int GS_ChooseScene1 = 30;
		};

		namespace MainGame
		{
			const int GS_Playing = 40;
			const int GS_GameOverMenu = 41;
			const int GS_PauseMenu = 42;

			namespace  GameOverMenu 
			{
				const int GS_Choose_Restart = 410;
				const int GS_Choose_Quit = 411;
			};

			namespace PauseMenu 
			{ 
				const int GS_Choose_Continue=420;
				const int GS_Choose_Quit=421;
			};

		};
	};

	extern IRenderer			gRenderer;
	extern UINT					gFPS;
	extern float					gTimeElapsed;
	extern ICamera				gCamera;
	extern int						gRootGameState;
	extern IMainGame		gMainGame;


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