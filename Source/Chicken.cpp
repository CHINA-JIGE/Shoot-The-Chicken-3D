#include "Game.h"

using namespace GamePlay;

IChickenMonster::IChickenMonster(IBulletManager * pBulletMgr)
	: m_pBulletMgr(pBulletMgr),
	mAttackState(CHICKEN_ATTACK_STATE_CHASE_PLAYER),
	Base_GameObject(c_chickenInitialHealth)
{
	mMat_Common.ambient = { 0.3f,0.3f,0.3f };
	mMat_Common.diffuse = { 0.8f,0.8f,0.8f };
	mMat_Common.specular = { 0.5f,0.5f,0.5f };

	mMat_Red.ambient = { 0.7f,0,0 };
	mMat_Red.diffuse = { 0.3f,0,0 };
	mMat_Common.specular = { 0.5f,0.5f,0.5f };
}

void IChickenMonster::Init()
{
	mTexture_Common.LoadPPM("Media/chicken.ppm");
	mMesh.LoadFile_OBJ("Media/chicken.obj");
	mPos = VECTOR3(100.0f, -100.0f, -200.0f);
	mLookat=VECTOR3(100.0f, -100.0f, 0.0f);


	mMesh.SetMaterial(mMat_Common);

	mMatType = CHICKEN_MATERIAL_TYPE_COMMON;
	mMesh.SetTexture(&mTexture_Common);
	mMesh.SetPosition(mPos.x,mPos.y,mPos.z);

}

void IChickenMonster::Update(const VECTOR3& playerPos)
{
	mFunction_UpdateMovement(playerPos);
	mFunction_UpdateTexture();
}

void IChickenMonster::Render()
{
	gRenderer.RenderMesh(mMesh);
}

void IChickenMonster::GetBoundingBox(BOUNDINGBOX& outBox)
{
	mMesh.ComputeBoundingBox(outBox);
}

void IChickenMonster::BeHitAndChangeColor()
{
	mMatType = CHICKEN_MATERIAL_TYPE_HIT;
}

/***********************************************************
								P R I V A T E
***********************************************************/

void IChickenMonster::mFunction_UpdateMovement(const VECTOR3& playerPos)
{
	static float timeCounter = 0.0f;

	//local time counter updates
	timeCounter += gTimeElapsed;

	switch (mAttackState)
	{
	case CHICKEN_ATTACK_STATE_CHASE_PLAYER:
	{
		if (timeCounter <= 7000.0f)
		{
			//try to rotate to look to player
			VECTOR3 toPlayer = playerPos - mPos;
			float currentYawAngle = mMesh.GetRotationY_Yaw();
			float targetYawAngle = atan2(toPlayer.x, toPlayer.z);

			//I don't want the chicken to abruptly look to chicken,
			//so lerp is inevitable (instead of directly set rotation Y)
			mMesh.SetRotationY_Yaw(Lerp(currentYawAngle, targetYawAngle, 0.2f));
		}

		if (timeCounter > 5000.0f && timeCounter <= 5500.0f)
		{
			//move vertically to get to the same level with play
			mPos.y = Lerp(mPos.y, playerPos.y, 0.2f);
		}
		mFunction_Fire(playerPos - mPos);

		//this state can end
		if (timeCounter > 10000.0f)
		{
			timeCounter = 0.0f;
			mAttackState = CHICKEN_ATTACK_STATE_TYPE1;
		}

		break;
	}

	case CHICKEN_ATTACK_STATE_TYPE1:
	{
		if (timeCounter <= 10000.0f)
		{
			//horizontally rotate
			mMesh.RotateY_Yaw(0.001f*gTimeElapsed);
			float angleY = mMesh.GetRotationY_Yaw();
			VECTOR3 shootDir = { sinf(angleY),0,cosf(angleY) };
			mFunction_Fire(shootDir);
		}
		if (timeCounter > 10000.0f)
		{
			timeCounter = 0.0f;
			mAttackState = CHICKEN_ATTACK_STATE_TYPE2;
		}

		break;
	}

	case CHICKEN_ATTACK_STATE_TYPE2:
	{
		mAttackState = CHICKEN_ATTACK_STATE_ULTIMATE_EXPLODE;
		break;
	}

	case CHICKEN_ATTACK_STATE_ULTIMATE_EXPLODE:
	{
		if (timeCounter <= 5000.0f)
		{
			//actually, explode bullets don't need a shoot direction
			mFunction_Fire(VECTOR3(1.0f, 0, 0));
		}
		if (timeCounter > 5000.0f)
		{
			timeCounter = 0.0f;
			mAttackState = CHICKEN_ATTACK_STATE_CHASE_PLAYER;
		}
		break;
	}

	default:
		break;
	}

	mMesh.SetPosition(mPos);
}

void IChickenMonster::mFunction_Fire(VECTOR3 shootDir)
{
	static float timeCounter = 0.0f;

	//different types of bullets have various cool down time
	const float fireTimeThreshold_common = 500.0f;
	const float fireTimeThreshold_rotate1 = 100.0f;
	const float fireTimeThreshold_rotate2 = 1000.0f;
	const float fireTimeThreshold_explode = 2000.0f;
	static std::default_random_engine rndEngine;
	static std::uniform_real_distribution<float> dirDist1(-0.1f,0.1f);
	static std::uniform_real_distribution<float> dirDist2(-0.5f, 0.5f);
	static std::uniform_real_distribution<float> dirDist3(-1.0f, 1.0f);

	//update time counter
	timeCounter += gTimeElapsed;

	switch (mAttackState)
	{
	case CHICKEN_ATTACK_STATE_CHASE_PLAYER:
	{
		if (timeCounter > fireTimeThreshold_common)
		{
			shootDir.Normalize();

			for (int i = 0;i < 5;++i)
			{
				//shoot direction (add some random offset)
				VECTOR3 dir = shootDir + VECTOR3(dirDist1(rndEngine), dirDist1(rndEngine), dirDist1(rndEngine));
				m_pBulletMgr->SpawnBullet(mPos, dir*0.5f, VECTOR3(1.0f, 0, 0));
			}

			//time counter reset
			timeCounter = 0.0f;
		}
	}
	break;

	case CHICKEN_ATTACK_STATE_TYPE1:
	{
		if (timeCounter > fireTimeThreshold_rotate1)
		{
			shootDir.Normalize();

			for (int i = 0;i < 15;++i)
			{
				//shoot direction (add some random offset)
				VECTOR3 dir = { shootDir.x,0,shootDir.z };
				//Y direction offset ( a whole column of bullets)
				dir.y += ((-7 + i) * 0.08f);
				dir.Normalize();
				m_pBulletMgr->SpawnBullet(mPos, dir*0.5f, VECTOR3(1.0f, 0, 0));
			}

			//time counter reset
			timeCounter = 0.0f;
		}
		break;
	}

	case CHICKEN_ATTACK_STATE_TYPE2:
	{
		if (timeCounter > fireTimeThreshold_rotate2)
		{
			shootDir.Normalize();

			for (int i = 0;i < 20;++i)
			{
				VECTOR3 dir = shootDir + VECTOR3(dirDist2(rndEngine), dirDist2(rndEngine), dirDist2(rndEngine));
				m_pBulletMgr->SpawnBullet(mPos, dir*0.5f, VECTOR3(1.0f, 0, 0));
			}

			//time counter reset
			timeCounter = 0.0f;
		}
		break;
	}

	case CHICKEN_ATTACK_STATE_ULTIMATE_EXPLODE:
	{
		if (timeCounter > fireTimeThreshold_explode)
		{
			for (int i = 0;i < 500;++i)
			{
				//shoot direction (add some random offset)
				VECTOR3 dir = { dirDist3(rndEngine),dirDist3(rndEngine) ,dirDist3(rndEngine) };
				//Y direction offset ( a whole column of bullets)
				dir.Normalize();
				m_pBulletMgr->SpawnBullet(mPos, dir*0.5f, VECTOR3(0, 0, 1.0f));
			}
			timeCounter = 0.0f;
		}
		break;
	}

	default:
		break;
	}

}

void IChickenMonster::mFunction_UpdateTexture()
{
	const float c_colorChangeMaxTime = 50.0f;
	static float colorChangeTime = 0.0f;

	//if the chicken was hit,change its color for a while
	if (mMatType == CHICKEN_MATERIAL_TYPE_HIT)
	{
		mMesh.SetMaterial(mMat_Red);
		//update time duration
		colorChangeTime += gTimeElapsed;
		if (colorChangeTime > c_colorChangeMaxTime)
		{
			mMatType = CHICKEN_MATERIAL_TYPE_COMMON;
			colorChangeTime = 0.0f;
		}
	}
	else
	{
		mMesh.SetMaterial(mMat_Common);
	}
}
