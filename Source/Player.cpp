#include "Game.h"

using namespace GamePlay;

const float IPlayer::c_halfPlayerWidth = 25.0f;
const float IPlayer::c_halfPlayerHeight = 35.0f;
const float IPlayer::c_halfPlayerDepth = 25.0f;

IPlayer::IPlayer(IBulletManager* BulletMgr)
	:m_pBulletMgr(BulletMgr),
	Base_GameObject(c_playerInitalHealth)
{
	
};

void IPlayer::Init()
{
	::SetCursorPos(0, 0);
	//------------------PLAYER EYE--------------------
	gCamera.SetPosition(200.0f, 150.0f, 200.0f);
	gCamera.SetLookAt(0, 0, 0);
	gCamera.SetViewAngle(Math::CONST_PI / 2.0f, 2.5f);
	gCamera.SetViewFrustumPlane(1.0f, 2500.0f);
	Base_GameObject::SetHP(c_playerInitalHealth);
	
}

void IPlayer::Update()
{
	mFunction_UpdateMovement(gTimeElapsed);
	mFunction_Fire(gTimeElapsed);
}

void IPlayer::Render()
{
	
}

void IPlayer::GetBoundingBox(BOUNDINGBOX & outBox)
{
	outBox.min =
	{
		mCurrentPos.x - c_halfPlayerWidth,
		mCurrentPos.y - c_halfPlayerHeight,
		mCurrentPos.z - c_halfPlayerDepth
	};
	outBox.max =
	{
		mCurrentPos.x + c_halfPlayerWidth,
		mCurrentPos.y + c_halfPlayerHeight,
		mCurrentPos.z + c_halfPlayerDepth
	};
}

VECTOR3 IPlayer::GetPosition()
{
	return mCurrentPos;
}

VECTOR3 IPlayer::GetPrevPosition()
{
	return mLastPos;
}

float IPlayer::GetInitialHealth() const
{
	return c_playerInitalHealth;
}

/*************************************************************
								P R I V A T E
***************************************************************/
void IPlayer::mFunction_Fire(float timeElapsed)
{
	static float fireCoolingDownTime = 0.0f;

	//100ms cool down time to shoot 1 bullets
	static const float fireTimeCoolDownThresold = 100.0f;

	fireCoolingDownTime += timeElapsed;

	//if time accumulates to surpass cool down threshold
	if (fireCoolingDownTime >= fireTimeCoolDownThresold)
	{
		//create a new bullet (consider the camera movement)
		//VECTOR3 vel_Camera = mCurrentPos - mLastPos;
		VECTOR3 unitCameraDir = gCamera.GetDirection().Normalize();
		VECTOR3 vel_Bullet = unitCameraDir;

		//add some offset to fire pos(in strafe direction,left and right relative to player)

		//in case that the bullets collide with self bounding box
		float gunDistanceFromPlayer = 0.0f;

		//OR!!! the bullet could hit SELF bounding box
		float gunDistanceFromMainAxis = min(c_halfPlayerWidth,c_halfPlayerDepth)-1.0f;

		//right gun
		/*VECTOR3 pos_Bullet1 = mCurrentPos+
			VECTOR3(gunDistanceFromMainAxis*cos(gCamera.GetRotationY_Yaw()),0, -gunDistanceFromMainAxis*sin(gCamera.GetRotationY_Yaw()))
		+ unitCameraDir*gunDistanceFromPlayer;*/
		VECTOR3 pos_Bullet1 = mCurrentPos +
			VECTOR3(gunDistanceFromMainAxis*cos(gCamera.GetRotationY_Yaw()), 0, -gunDistanceFromMainAxis*sin(gCamera.GetRotationY_Yaw()))
			+ unitCameraDir*gunDistanceFromPlayer;

		//left gun
		VECTOR3 pos_Bullet2 = mCurrentPos +
			VECTOR3(-gunDistanceFromMainAxis*cos(gCamera.GetRotationY_Yaw()), 0, gunDistanceFromMainAxis*sin(gCamera.GetRotationY_Yaw()))
			+ unitCameraDir*gunDistanceFromPlayer;

		//-----------------Spawn Bullet------------------

		//right bullet
		m_pBulletMgr->SpawnBullet(pos_Bullet1, vel_Bullet*0.8f, { (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f});
		//left bullet
		m_pBulletMgr->SpawnBullet(pos_Bullet2, vel_Bullet*0.8f, { (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f});

		//reset cool down time
		fireCoolingDownTime = 0.0f;
	}
}

void IPlayer::mFunction_UpdateMovement(float timeElapsed)
{
	//--------------------------keyboard------------------------------
	VECTOR3 moveVector = { 0,0,0 };
	if (IS_KEY_DOWN('A'))
	{
		moveVector.x -= 1.0f;
	}
	if (IS_KEY_DOWN('D'))
	{
		moveVector.x += 1.0f;
	}
	if (IS_KEY_DOWN('W'))
	{
		moveVector.z += 1.0f;
	}
	if (IS_KEY_DOWN('S'))
	{
		moveVector.z -= 1.0f;
	}
	if (IS_KEY_DOWN(VK_LCONTROL))
	{
		moveVector.y -= 1.0f;
	}
	if (IS_KEY_DOWN(VK_SPACE))
	{
		moveVector.y += 1.0f;
	}


	//in case that camera moves faster if 3 directions has projection of speed
	moveVector.Normalize();
	moveVector *= (0.2f*timeElapsed);
	gCamera.fps_MoveRight(moveVector.x);
	gCamera.fps_MoveForward(moveVector.z);
	gCamera.fps_MoveUp(moveVector.y);

	//restrict player movement to a Box
	VECTOR3 camPos = gCamera.GetPosition();
	/*camPos = { Clamp(camPos.x,-c_halfMovementRestrictBoxWidth,c_halfMovementRestrictBoxWidth),
		Clamp(camPos.y,-c_halfMovementRestrictBoxWidth,c_halfMovementRestrictBoxWidth),
		Clamp(camPos.z,-c_halfMovementRestrictBoxWidth,c_halfMovementRestrictBoxWidth) };
	gCamera.SetPosition(camPos);*/

	//update Position
	mLastPos = mCurrentPos;
	mCurrentPos = camPos;


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
	gCamera.RotateY_Yaw(0.0002f * cursorDeltaX*timeElapsed);
	gCamera.RotateX_Pitch(0.0002f* cursorDeltaY*timeElapsed);

}
