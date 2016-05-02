#pragma once

namespace GamePlay
{
	class IPlayer
	{
	public:

		IPlayer(IBulletManager* BulletMgr);

		void	Init();

		void	Update();

		void	Render();

	private:

		void	mFunction_Fire(float timeElapsed);//shoot a bullet

		void	mFunction_UpdateMovement(float timeElapsed);

		//spawn Bullets
		IBulletManager*	const m_pBulletMgr;

		VECTOR3 mLastPos;
		VECTOR3 mCurrentPos;

		const float	c_halfPlayerWidth	=20.0f;
		const float	c_halfPlayerHeight	= 40.0f;
		const float	c_halfPlayerDepth	= 20.0f;
		BOUNDINGBOX			mBoundingBox;

	};
};