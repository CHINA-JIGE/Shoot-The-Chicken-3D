#include "Game.h"

using namespace GamePlay;

IBulletManager::IBulletManager()
	: m_pBulletList(new std::vector<Bullet>)
	//m_pRenderMeshList(new std::vector<IMesh*>)
{
}

IBulletManager::~IBulletManager()
{
	delete m_pBulletList;
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
	v.color = { color.x,color.y,color.z,1.0f };
	mRenderPointList.AddPoint(v);

	//spawn new mesh
	/*Material mat;
	mat.ambient = { 0.3f,0,0 };
	mat.diffuse = { (rand() % 255) / 255.0f,(rand() % 255) / 255.0f,(rand() % 255) / 255.0f };
	IMesh* pM = new IMesh;
	m_pRenderMeshList->push_back(pM);
	pM->CreateSphere(5.0f, 4, 4, FALSE);
	pM->SetMaterial(mat);*/

}

void IBulletManager::KillBullet(UINT index)
{
	if (index < m_pBulletList->size())
	{
		m_pBulletList->erase(m_pBulletList->begin() + index);
		mRenderPointList.DeletePoint(index);
		//delete(m_pRenderMeshList->at(index));
		//m_pRenderMeshList->erase(m_pRenderMeshList->begin() + index);
	}
}

void IBulletManager::KillAllBullet()
{
	m_pBulletList->clear();
	mRenderPointList.ClearAll();
}

void IBulletManager::CollisionDetection(BOUNDINGBOX testBox,BOOL killCollidedBullets, std::vector<VECTOR3>& outCollidePointsList)
{
	outCollidePointsList.clear();

	for (UINT i = 0; i < m_pBulletList->size(); ++i)
	{
		//well, I should calculate the next position of this bullets,
		//then test the segment with given bounding box
		auto currBullet = m_pBulletList->at(i);
		VECTOR3 currentPos = currBullet.pos;
		VECTOR3 nextPos = currentPos + currBullet.velocity * gTimeElapsed;

		VECTOR3 outIntersectPoint;
		//Apply Ray(segment)-AABB intersection
		if (Intersect_Ray_AABB(currentPos, nextPos, testBox, outIntersectPoint,TRUE))
		{
			//user might like to know the intersect position to adequately react
			outCollidePointsList.push_back(outIntersectPoint);

			//kill collided bullet
			if (killCollidedBullets)KillBullet(i);
		}
	}
}

void IBulletManager::UpdateBullets()
{
	std::vector<UINT> bulletDeleteList;

	//traverse every existed bullets
	for (UINT i = 0;i < m_pBulletList->size();++i)
	{
		//movement of bullets : ds= v*dt
		auto& currBullet = m_pBulletList->at(i);
		currBullet.pos += currBullet.velocity * gTimeElapsed;//ds=v*dt
		currBullet.travelledDistance += (currBullet.velocity.Length() * gTimeElapsed);

		//add to render list
		mRenderPointList.SetPointPos(i, currBullet.pos);
		//m_pRenderMeshList->at(i)->SetPosition(currBullet.pos);

		//if bullets are out of boundary, kill them 
		const float maxDistance = 500.0f;
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
	/*for (auto mesh : *m_pRenderMeshList)
	{
		gRenderer.RenderMesh(*mesh);
	}*/
}
