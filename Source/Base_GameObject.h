#pragma once

namespace GamePlay
{
	class Base_GameObject
	{
	public:

		Base_GameObject(float initialHealth);

		void	SetHP(float hp);

		float	GetHP();

		void	ReduceHP(float deltaHP);

		BOOL IsDead();

		virtual void GetBoundingBox(BOUNDINGBOX& outBox)=0;

	private:


		//Game attribute
		float				mHP;

	};
}