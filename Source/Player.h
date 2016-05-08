#pragma once

namespace GamePlay
{
	const float c_playerInitalHealth = 100.0f;

	class IPlayer : public Base_GameObject
	{
	public:

		IPlayer(IBulletManager* BulletMgr);

		void	Init();

		void	Update();

		void	Render();

		void GetBoundingBox(BOUNDINGBOX& outBox);

		VECTOR3 GetPosition();

		VECTOR3	GetPrevPosition();

	private:

		void	mFunction_Fire(float timeElapsed);//shoot a bullet

		void	mFunction_UpdateMovement(float timeElapsed);

		//spawn Bullets
		IBulletManager*	const m_pBulletMgr;

		//to calculate player velocity
		VECTOR3 mLastPos;
		VECTOR3 mCurrentPos;

		const float	c_halfPlayerWidth	=5.0f;
		const float	c_halfPlayerHeight= 5.0f;
		const float	c_halfPlayerDepth	= 5.0f;

	};
};