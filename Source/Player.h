#pragma once

namespace GamePlay
{
	const float c_playerInitalHealth=500.0f;

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

		static const float	c_halfPlayerWidth;
		static const float	c_halfPlayerHeight;
		static const float	c_halfPlayerDepth;

	};
};