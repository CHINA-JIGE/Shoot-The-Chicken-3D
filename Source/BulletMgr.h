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

		//generate(spawn) a new bullets
		void	SpawnBullet(const VECTOR3& pos, const VECTOR3& velocity, const VECTOR3& color);

		void	KillBullet(UINT index);

		void	CollisionDetection(BOUNDINGBOX testBox,BOOL killCollidedBullets);//vector(pos,pos-velocity)

		void	UpdateBullets();

		void	Render();

	private:

		std::vector<Bullet>* m_pBulletList;

		IPointCollection		mRenderPointList;

	};
}