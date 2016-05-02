
/***********************************************************************

                           .cpp£ºGame Play

			Desc : Implementation of GamePlays

************************************************************************/


#include "Game.h"

using namespace GamePlay;

namespace GamePlay
{
	IRenderer				myRenderer;
	ITimer					myTimer(TIMER_TIMEUNIT_MILLISECOND);
	ICamera				myCamera;

	namespace Res
	{
		Material myDefaultMaterial;
		IPicture  myTestPic;
		IMesh	  myModel1;
		DirectionalLight myLight1;
	}
}

void GamePlay::Init()
{
	myRenderer.Init(250,100);
	myRenderer.SetWindowTitle("Console Soft Renderer - By Jige");
	myRenderer.SetCamera(myCamera);

	//------------------CAMERA--------------------
	myCamera.SetPosition(0, 100.0f, 200.0f);
	myCamera.SetLookAt(0, 0, 0);
	myCamera.SetViewAngle(Math::CONST_PI / 2.0f, 2.5f);
	myCamera.SetViewFrustumPlane(1.0f, 500.0f);

	//------------------Init Art Resources------------------
	Res::myDefaultMaterial.ambient = { 0.5f,0.5f,0.5f };
	Res::myDefaultMaterial.diffuse = { 1.0f,1.0f,1.0f };
	Res::myDefaultMaterial.specular = { 1.0f,1.0f,1.0f };
	Res::myDefaultMaterial.specularSmoothLevel = 20;
	Res::myTestPic.LoadPPM("Media/test3.ppm");
	Res::myModel1.LoadFile_OBJ("Media/chicken.obj");
	Res::myModel1.SetPosition(0, 0, 0);
	Res::myModel1.SetMaterial(Res::myDefaultMaterial);
	Res::myModel1.SetTexture(&Res::myTestPic);

	//-----------------Lights----------------------
	Res::myLight1.mAmbientColor = { 1.0f,1.0f,1.0f };
	Res::myLight1.mDiffuseColor = { 1.0f,1.0f,1.0f };
	Res::myLight1.mDiffuseIntensity =1.0f;
	Res::myLight1.mDirection = { -1.0f,-1.0f,-1.0f };
	Res::myLight1.mIsEnabled = TRUE;
	Res::myLight1.mSpecularColor = { 1.0f,1.0f,1.0f };
	Res::myLight1.mSpecularIntensity = 1.2f;
	myRenderer.SetLight(0, Res::myLight1);

	//-----------------Init Cursor--------------------
	::SetCursorPos(0, 0);
}

void GamePlay::UpdateGlobalTimer()
{
	myTimer.NextTick();
}

void GamePlay::UpdateWindowTitle()
{
	std::string titleStr;
	titleStr = "Soft Renderer - By Jige  FPS:" + std::to_string(myTimer.GetFPS()) +"angleY"+std::to_string(myCamera.GetRotationY_Yaw());
	myRenderer.SetWindowTitle(titleStr.c_str());
}

void GamePlay::MouseAndKeyBoradProcess()
{
	//--------------------------keyboard------------------------------
	float timeElapsed = float(myTimer.GetInterval());
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
	if (IS_KEY_DOWN(VK_SPACE))
	{
		myCamera.fps_MoveUp(0.05f*timeElapsed);
	}
	if (IS_KEY_DOWN(VK_LCONTROL))
	{
		myCamera.fps_MoveUp(-0.05f*timeElapsed);
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

	if (currentCursorPos.y == scrHeight-1)
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

void GamePlay::Render()
{
	myRenderer.Clear(COLOR3(0, 0, 0), TRUE);
	myRenderer.DrawPicture(Res::myTestPic, 0, 0, 250,100);
	//myRenderer.DrawLine(COLOR3(1.0f, 0, 0), 0, 0, 250, 100);
	//myRenderer.DrawLine(COLOR3(0, 1.0f, 0),20, 80, 200, 20);
	//myRenderer.DrawLine(COLOR3(0, 0, 1.0f), 30, 0, 300, 70);
	//myRenderer.RenderMesh(Res::myModel1);
	/*static float tmpF = 0.0f;
	tmpF += 0.05f;
	myRenderer.DrawTriangle(COLOR3(255, 0, 255), VECTOR2(0,0), VECTOR2(200.0f,0.0f), VECTOR2(255.0f,100.0f));*/
	myRenderer.Present();
}
