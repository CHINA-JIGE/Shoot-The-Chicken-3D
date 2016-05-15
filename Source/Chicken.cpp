#include "Game.h"

using namespace GamePlay;

static float movePatternTimeCounter = 0.0f;

static float attackPatternTimeCounter = 0.0f;

IChickenMonster::IChickenMonster(IBulletManager * pBulletMgr)
	: m_pBulletMgr(pBulletMgr),
	mAttackState(CHICKEN_ATTACK_STATE_CHASE_PLAYER),
	Base_GameObject(c_chickenInitialHealth)
{
	mMat_Common.ambient = { 0.3f,0.3f,0.3f };
	mMat_Common.diffuse = { 0.8f,0.8f,0.8f };
	mMat_Common.specular = { 0.5f,0.5f,0.5f };

	mMat_Red.ambient = { 0.9f,0,0 };
	mMat_Red.diffuse = { 0.3f,0,0 };
	mMat_Common.specular = { 0.5f,0.5f,0.5f };
}

void IChickenMonster::Init(SCENE_TYPE type)
{
	//different type, different texture
	switch (type)
	{
	case SCENE_TYPE::SCENE_COSMOS1:
		mTexture_Common.LoadPPM("Media/chicken.ppm");
		break;

	case SCENE_TYPE::SCENE_CHECKERBOARD:
		mTexture_Common.LoadPPM("Media/checkerBoard.ppm");
		break;
	}

	//chicken mesh
	mMesh.LoadFile_OBJ("Media/chicken.obj");
	mPos = VECTOR3(0,0,0);
	mLookat=VECTOR3(100.0f, -100.0f, 0.0f);

	mMesh.SetMaterial(mMat_Common);

	//chicken will change its material to relative red when hit.
	mMatType = CHICKEN_MATERIAL_TYPE_COMMON;
	mMesh.SetTexture(&mTexture_Common);
	mMesh.SetPosition(mPos.x,mPos.y,mPos.z);

	Base_GameObject::SetHP(c_chickenInitialHealth);
	movePatternTimeCounter = 0.0f;
	attackPatternTimeCounter = 0.0f;
	mAttackState = CHICKEN_ATTACK_STATE_CHASE_PLAYER;
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

VECTOR3 IChickenMonster::GetPosition()
{
	return mPos;
}

float IChickenMonster::GetInitialHealth() const
{
	return c_chickenInitialHealth;
}

/***********************************************************
								P R I V A T E
***********************************************************/

void IChickenMonster::mFunction_UpdateMovement(const VECTOR3& playerPos)
{
	//STATE TRANSITION
	//CHASE PLAYER -->> TYPE1 -->> TYPE2-->>EXPLODE-->>CHASE PLAYER


	//local time counter updates
	movePatternTimeCounter += gTimeElapsed;

	switch (mAttackState)
	{
	case CHICKEN_ATTACK_STATE_CHASE_PLAYER:
	{
		if (movePatternTimeCounter <= 7000.0f)
		{
			//try to rotate to look to player
			VECTOR3 toPlayer = playerPos - mPos;
			float currentYawAngle = mMesh.GetRotationY_Yaw();
			float targetYawAngle = atan2(toPlayer.x, toPlayer.z);

			//I don't want the chicken to abruptly look to chicken,
			//so lerp is inevitable (instead of directly set rotation Y)
			mMesh.SetRotationY_Yaw(Lerp(currentYawAngle, targetYawAngle, 0.2f));
		}

		if (movePatternTimeCounter > 5000.0f && movePatternTimeCounter <= 5500.0f)
		{
			//move vertically to get to the same level with play
			mPos.y = Lerp(mPos.y, playerPos.y, 0.2f);
		}
		mFunction_Fire(playerPos - mPos);

		//this state can end
		if (movePatternTimeCounter > 10000.0f)
		{
			movePatternTimeCounter = 0.0f;
			mAttackState = CHICKEN_ATTACK_STATE_TYPE1;
		}

		break;
	}

	case CHICKEN_ATTACK_STATE_TYPE1:
	{
		if (movePatternTimeCounter <= 10000.0f)
		{
			//move vertically to get to the same level with play
			mPos.y = Lerp(mPos.y, playerPos.y, 0.02f);

			//horizontally rotate
			mMesh.RotateY_Yaw(0.001f*gTimeElapsed);
			float angleY = mMesh.GetRotationY_Yaw();
			VECTOR3 shootDir = { sinf(angleY),0,cosf(angleY) };
			mFunction_Fire(shootDir);
		}
		if (movePatternTimeCounter > 10000.0f)
		{
			movePatternTimeCounter = 0.0f;
			mAttackState = CHICKEN_ATTACK_STATE_TYPE2;
		}

		break;
	}

	case CHICKEN_ATTACK_STATE_TYPE2:
	{
		if (movePatternTimeCounter <= 8000.0f)
		{
			//move vertically to get to the same level with play
			mPos.y = Lerp(mPos.y, playerPos.y, 0.02f);

			//horizontally rotate
			mMesh.RotateY_Yaw(-0.002f*gTimeElapsed);
			float angleY = mMesh.GetRotationY_Yaw();
			VECTOR3 shootDir = { sinf(angleY),0,cosf(angleY) };
			mFunction_Fire(shootDir);
		}
		if (movePatternTimeCounter > 8000.0f)
		{
			movePatternTimeCounter = 0.0f;
			mAttackState = CHICKEN_ATTACK_STATE_ULTIMATE_EXPLODE;
		}
		break;
	}

	case CHICKEN_ATTACK_STATE_ULTIMATE_EXPLODE:
	{
		if (movePatternTimeCounter <= 5000.0f)
		{
			//actually, explode bullets don't need a shoot direction
			mFunction_Fire(VECTOR3(1.0f, 0, 0));
		}
		if (movePatternTimeCounter > 5000.0f)
		{
			movePatternTimeCounter = 0.0f;
			//another cycle
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

	//different types of bullets have various cool down time
	const float fireTimeThreshold_common = 500.0f;
	const float fireTimeThreshold_rotate1 = 200.0f;
	const float fireTimeThreshold_rotate2 = 300.0f;
	const float fireTimeThreshold_explode = 2000.0f;
	static std::default_random_engine rndEngine;
	static std::uniform_real_distribution<float> dirDist1(-0.1f,0.1f);
	static std::uniform_real_distribution<float> dirDist2(-0.5f, 0.5f);
	static std::uniform_real_distribution<float> dirDist3(-1.0f, 1.0f);

	//update time counter
	attackPatternTimeCounter += gTimeElapsed;

	switch (mAttackState)
	{
	case CHICKEN_ATTACK_STATE_CHASE_PLAYER:
	{
		if (attackPatternTimeCounter > fireTimeThreshold_common)
		{
			shootDir.Normalize();

			for (int i = 0;i < 5;++i)
			{
				//shoot direction (add some random offset)
				VECTOR3 dir = shootDir + VECTOR3(dirDist1(rndEngine), dirDist1(rndEngine), dirDist1(rndEngine));
				m_pBulletMgr->SpawnBullet(mPos, dir*0.5f, VECTOR3(1.0f, 0, 0));
			}

			//time counter reset
			attackPatternTimeCounter = 0.0f;
		}
	}
	break;

	case CHICKEN_ATTACK_STATE_TYPE1:
	{
		if (attackPatternTimeCounter > fireTimeThreshold_rotate1)
		{
			shootDir.Normalize();

			for (int i = 0;i < 20;++i)
			{
				//shoot direction (add some random offset)
				VECTOR3 dir = { shootDir.x,0,shootDir.z };
				//Y direction offset ( a whole column of bullets)
				dir.y += ((-10 + i) * 0.1f);
				dir.Normalize();
				m_pBulletMgr->SpawnBullet(mPos, dir*0.5f, VECTOR3(1.0f, 0, 0));
			}

			//time counter reset
			attackPatternTimeCounter = 0.0f;
		}
		break;
	}

	case CHICKEN_ATTACK_STATE_TYPE2:
	{
		if (attackPatternTimeCounter > fireTimeThreshold_rotate2)
		{
			shootDir.Normalize();

			for (int i = 0;i < 20;++i)
			{
				VECTOR3 dir = shootDir + VECTOR3(dirDist2(rndEngine), dirDist2(rndEngine), dirDist2(rndEngine));
				m_pBulletMgr->SpawnBullet(mPos, dir, VECTOR3(0,1.0f, 0));
			}

			//time counter reset
			attackPatternTimeCounter = 0.0f;
		}
		break;
	}

	case CHICKEN_ATTACK_STATE_ULTIMATE_EXPLODE:
	{
		if (attackPatternTimeCounter > fireTimeThreshold_explode)
		{
			for (int i = 0;i < 1000;++i)
			{
				//shoot direction (add some random offset)
				VECTOR3 dir = { dirDist3(rndEngine),dirDist3(rndEngine) ,dirDist3(rndEngine) };
				//Y direction offset ( a whole column of bullets)
				dir.Normalize();
				m_pBulletMgr->SpawnBullet(mPos, dir, VECTOR3(0, 0, 1.0f));
			}
			attackPatternTimeCounter = 0.0f;
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
