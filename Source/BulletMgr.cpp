#include "Game.h"

using namespace GamePlay;

IBulletManager::IBulletManager()
	: m_pBulletList(new std::vector<Bullet>)
{
}

void IBulletManager::SpawnBullet(const VECTOR3 & pos, const VECTOR3 & velocity, const VECTOR3 & color)
{
	//spawn(create) new bullet
	Bullet bullet;
	bullet.pos = pos;
	bullet.color = color;
	bullet.velocity = velocity;
	bullet.travelledDistance = 0.0f;
	m_pBulletList->push_back(bullet);

	Vertex v;
	v.pos = pos;
	v.color = VECTOR4(color.x,color.y,color.z,1.0f);
	mRenderPointList.AddPoint(v);
}

void IBulletManager::KillBullet(UINT index)
{
	if (index < m_pBulletList->size())
	{
		m_pBulletList->erase(m_pBulletList->begin() + index);
		mRenderPointList.DeletePoint(index);
	}
}

void IBulletManager::CollisionDetection(BOUNDINGBOX testBox,BOOL killCollidedBullets)
{

}

void IBulletManager::UpdateBullets()
{
	std::vector<UINT> bulletDeleteList;
	float timeElapsed = gTimer.GetInterval();

	//traverse every existed bullets
	for (UINT i = 0;i < m_pBulletList->size();++i)
	{
		//translation of bullets
		auto& currBullet = m_pBulletList->at(i);
		currBullet.pos += m_pBulletList->at(i).velocity * timeElapsed;//ds=v*dt
		currBullet.travelledDistance += (currBullet.velocity.Length() * timeElapsed);

		//add to render list
		mRenderPointList.SetPointPos(i, currBullet.pos);

		//if bullets are out of boundary, kill them 
		const float maxDistance = 1000.0f;
		if (currBullet.travelledDistance >= maxDistance)
		{
			bulletDeleteList.push_back(i);
		}
	}

	//then kill bullets
	for (auto idx : bulletDeleteList)
	{
		//deleting elements will cause List size to change
		static int offset = 0;
		KillBullet(offset +idx);
		--offset;
	}
}

void IBulletManager::Render()
{
	gRenderer.RenderPointCollection(mRenderPointList);
}
