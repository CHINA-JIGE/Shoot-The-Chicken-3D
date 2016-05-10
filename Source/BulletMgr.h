#pragma once

namespace GamePlay
{
	struct Bullet
	{
		VECTOR3 pos;
		VECTOR3 velocity;
		VECTOR3 color;
		float travelledDistance;
	};


	class IBulletManager
	{
	public:

		IBulletManager();

		~IBulletManager();

		//generate(spawn) a new bullets
		void	SpawnBullet(const VECTOR3& pos, const VECTOR3& velocity, const VECTOR3& color);

		void	KillBullet(UINT index);

		void	KillAllBullet();

		void	CollisionDetection(BOUNDINGBOX testBox,BOOL killCollidedBullets,std::vector<VECTOR3>& outCollidePointsList);//vector(pos,pos-velocity)

		void	UpdateBullets();

		void	Render();

	private:

		std::vector<Bullet>* m_pBulletList;

		IPointCollection		mRenderPointList;

		//std::vector<IMesh*>*	m_pRenderMeshList;
	};
}