#pragma once

#include "MyConsoleEngine.h"
#include "SceneEnvironment.h"
#include "Chicken.h"
#include "Player.h"
#include "MainGameLogic.h"

namespace GamePlay
{
	enum GameState
	{
		GameState_StartAnimation = 1,
		GameState_StartMenu = 2,
		GameState_ChooseSceneMenu = 3,
		GameState_MainGame = 4,
		GameState_PauseMenu = 5,
	};

	extern IRenderer			gRenderer;
	extern ITimer				gTimer;
	extern ICamera				gCamera;
	extern GameState			gGameState;
	extern IMainGame		gMainGame;
	
	//global initialization
	void	InitGlobal();

	//driven program of game state machine
	void	GameStateSelector();

	void	UpdateGlobalTimer();

	void UpdateWindowTitle();


	//loop body for each game state
	void	StartAnimation();

	void	StartMenu();

	void	ChooseSceneMenu();

	void	MainGame();

	void	PauseMenu();


};