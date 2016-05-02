#include "Game.h"

using namespace GamePlay;

IChickenMonster::IChickenMonster(IBulletManager * pBulletMgr)
	: m_pBulletMgr(pBulletMgr)
{

}

void IChickenMonster::Init()
{
	mTexture.LoadPPM("Media/chicken.ppm");
	mMesh.LoadFile_OBJ("Media/chicken.obj");
	mMesh.SetTexture(&mTexture);
	mMesh.SetPosition(0, 0, 0);
	mBoundingBox = mMesh.ComputeBoundingBox();
}

void IChickenMonster::UpdateMovement()
{
}

void IChickenMonster::Fire()
{
}

void IChickenMonster::Render()
{
	gRenderer.RenderMesh(mMesh);
}

BOUNDINGBOX IChickenMonster::GetBoundingBox()
{
	return mBoundingBox;
}
