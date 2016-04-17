
/***********************************************************************

                           .cpp£ºGame Play

			Desc : Implementation of GamePlays

************************************************************************/


#include "Game.h"

using namespace GamePlay;

namespace GamePlay
{
	IRenderer myRenderer;
	ITimer		myTimer(TIMER_TIMEUNIT_MILLISECOND);

	namespace Res
	{
		IPicture  myTestPic;
	}
}


void GamePlay::Init()
{
	myRenderer.Init(250,100);
	myRenderer.SetWindowTitle("Console Soft Renderer - By Jige");

	//------------------Init Art Resources------------------
	Res::myTestPic.LoadPPM("Media/beautiful_scene.ppm");
	
}

void GamePlay::UpdateTimer()
{
	myTimer.NextTick();
}

void GamePlay::UpdateWindowTitle()
{
	std::string titleStr;
	titleStr = "Soft Renderer - By Jige  FPS:" + std::to_string(myTimer.GetFPS());
	myRenderer.SetWindowTitle(titleStr.c_str());
}

void GamePlay::InputProcess()
{
}

void GamePlay::Render()
{
	myRenderer.Clear(COLOR3(0, 0, 0), TRUE);
	//myRenderer.DrawPicture(Res::myTestPic, 0, 0, 250,100);
	myRenderer.DrawLine(COLOR3(255, 0, 0), 0, 0, 250, 100);
	myRenderer.DrawLine(COLOR3(0, 255, 0),20, 80, 200, 20);
	myRenderer.DrawLine(COLOR3(0, 0, 255), 30, 0, 300, 70);
	myRenderer.Present();
}
