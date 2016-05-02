#pragma once

namespace GamePlay
{
	class IChickenMonster
	{
	public:

		IChickenMonster(IBulletManager* pBulletMgr);

		void	Init();

		void	UpdateMovement();

		void	Fire();

		void	Render();

		BOUNDINGBOX GetBoundingBox();

	private:

		//spawn Bullets
		IBulletManager* const 	m_pBulletMgr;

		IMesh		mMesh;
		IPicture		mTexture;

		BOUNDINGBOX			mBoundingBox;
	};
};