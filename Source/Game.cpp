
#include "Game.h"

using namespace GamePlay;

namespace GamePlay
{
	IRenderer				gRenderer;
	ITimer					gTimer(TIMER_TIMEUNIT_MILLISECOND);
	ICamera				gCamera;
	IMainGame			gMainGame;
	GameState			gGameState;

}

void GamePlay::InitGlobal()
{
	gGameState = GameState::GameState_StartAnimation;
	gRenderer.Init(250, 100);
	gRenderer.SetWindowTitle("Console Soft Renderer - By Jige");
	gRenderer.SetCamera(gCamera);

}

void GamePlay::GameStateSelector()
{
	switch (UINT(gGameState))
	{
	case GameState::GameState_StartAnimation:
		StartAnimation();
		break;

	case GameState::GameState_StartMenu:
		StartMenu();
		break;

	case GameState::GameState_ChooseSceneMenu:
		ChooseSceneMenu();
		break;

	case GameState::GameState_MainGame:
		MainGame();
		break;

	case GameState::GameState_PauseMenu:
		PauseMenu();
		break;

	default:
		std::terminate();
		break;
	}
}


void GamePlay::StartAnimation()
{
	gGameState = GameState::GameState_StartMenu;
}

void GamePlay::StartMenu()
{
	gMainGame.Init();
	gGameState = GameState::GameState_MainGame;

}

void GamePlay::MainGame()
{
	gMainGame.UpdateLogic();
	gMainGame.Render();
}

void	GamePlay::PauseMenu()
{

}

void GamePlay::ChooseSceneMenu()
{
}


void GamePlay::UpdateGlobalTimer()
{
	gTimer.NextTick();
}

void GamePlay::UpdateWindowTitle()
{
	std::string titleStr;
	titleStr = "Soft Renderer - By Jige  FPS:" + std::to_string(gTimer.GetFPS());
	gRenderer.SetWindowTitle(titleStr.c_str());
}

/*
void GamePlay::MouseAndKeyBoradProcess()
{
	//--------------------------keyboard------------------------------
	float timeElapsed = Clamp(float(myTimer.GetInterval()),0,100.0f);

	VECTOR2 moveVector = { 0,0 };
	if (IS_KEY_DOWN('A'))
	{
		myCamera.fps_MoveRight(-0.05f*timeElapsed);
	}
	if (IS_KEY_DOWN('D'))
	{
		myCamera.fps_MoveRight(0.05f*timeElapsed);
	}
	if (IS_KEY_DOWN('W'))
	{
		myCamera.fps_MoveForward(0.05f*timeElapsed);
	}
	if (IS_KEY_DOWN('S'))
	{
		myCamera.fps_MoveForward(-0.05f*timeElapsed);
	}



	//-------------------------------cursor movement----------------------------------
	static POINT lastCursorPos = { 0,0 };
	static POINT currentCursorPos = { 0,0 };
	static const int scrWidth = ::GetSystemMetrics(SM_CXSCREEN);
	static const int scrHeight = ::GetSystemMetrics(SM_CYSCREEN);
	lastCursorPos = currentCursorPos;
	::GetCursorPos(&currentCursorPos);

	//if cursor reach the boundary, go to another side
	if (currentCursorPos.x == scrWidth - 1)
	{
		::SetCursorPos(0, currentCursorPos.y);
		lastCursorPos = { 0,currentCursorPos.y };
		currentCursorPos = lastCursorPos;
	}
	else
	{
		if (currentCursorPos.x == 0)
		{
			::SetCursorPos(scrWidth - 1, currentCursorPos.y);
			lastCursorPos = { scrWidth - 1,currentCursorPos.y };
			currentCursorPos = lastCursorPos;
		}
	}

	if (currentCursorPos.y == scrHeight - 1)
	{
		::SetCursorPos(currentCursorPos.x, 0);
		lastCursorPos = { currentCursorPos.x,0 };
		currentCursorPos = lastCursorPos;
	}
	else
	{
		if (currentCursorPos.y == 0)
		{
			::SetCursorPos(currentCursorPos.x, scrHeight - 1);
			lastCursorPos = { currentCursorPos.x,scrHeight - 1 };
			currentCursorPos = lastCursorPos;
		}
	}

	//camera rotation
	int cursorDeltaX = currentCursorPos.x - lastCursorPos.x;
	int cursorDeltaY = (currentCursorPos.y - lastCursorPos.y);
	myCamera.RotateY_Yaw(0.0005f * cursorDeltaX*timeElapsed);
	myCamera.RotateX_Pitch(0.0005f* cursorDeltaY*timeElapsed);

}
*/
