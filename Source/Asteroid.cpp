#include "Game.h"

using namespace GamePlay;

static std::default_random_engine rndEngine;
static std::uniform_real_distribution<float> posDist(-c_halfMovementRestrictBoxWidth/2.0f, c_halfMovementRestrictBoxWidth/2.0f);
static std::uniform_real_distribution<float> speedDist(-0.2f, 0.2f);
static std::uniform_real_distribution<float> rotSpeedDist(-0.001f, 0.001f);

void IAsteroid::Init(UINT asteroidType)
{
	//floating rocks(asteroids), random engine is used to generator random coordinate
	switch(UINT(asteroidType))
	{
		case AsteroidType_Rock1:
			mMesh.LoadFile_OBJ("Media/rock1.obj");
			break;

		case AsteroidType_Rock2:
			mMesh.LoadFile_OBJ("Media/rock2.obj");
			break;

		case AsteroidType_Rock3:
			mMesh.LoadFile_OBJ("Media/rock3.obj");
			break;

		case AsteroidType_Box:
		default:
			float width = rand() % 70 + 70;
			mMesh.CreateBox(width, width, width, 3, 3, 3);
			break;

	}

	//texture
	mTexture.LoadPPM("Media/rock.ppm");
	//mMesh.SetTexture(&mTexture);

	Material mat;
	mat.ambient = { 0.3f,0.3f,0.3f };
	mat.diffuse = { 0.7f,0.7f,0.7f };
	mMesh.SetMaterial(mat);

	//movement param init
	mMesh.SetPosition(posDist(rndEngine), posDist(rndEngine), posDist(rndEngine));
	mRotateSpeed = { rotSpeedDist(rndEngine),rotSpeedDist(rndEngine), rotSpeedDist(rndEngine)};
	mMoveSpeed = { speedDist(rndEngine),speedDist(rndEngine), speedDist(rndEngine) };
}

void IAsteroid::Update()
{
	//move
	VECTOR3 pos = mMesh.GetPosition();
	pos += mMoveSpeed;

	//if rock go out of the restricted boundary,re-init pos
	if (abs(pos.x) > c_halfMovementRestrictBoxWidth ||
		abs(pos.y) > c_halfMovementRestrictBoxWidth ||
		abs(pos.z) > c_halfMovementRestrictBoxWidth)
	{
		pos = { posDist(rndEngine), posDist(rndEngine), posDist(rndEngine) };
	}

	mMesh.SetPosition(pos);

	//rotate
	mMesh.RotateX_Pitch(mRotateSpeed.x*gTimeElapsed);
	mMesh.RotateY_Yaw(mRotateSpeed.y*gTimeElapsed);
	mMesh.RotateZ_Roll(mRotateSpeed.z*gTimeElapsed);

}

void IAsteroid::Render()
{
	gRenderer.RenderMesh(mMesh);
}

void IAsteroid::GetBoundingBox(BOUNDINGBOX & outBox)
{
	mMesh.ComputeBoundingBox(outBox);
}
