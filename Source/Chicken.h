#pragma once

namespace GamePlay
{


	enum CHICKEN_MATERIAL_TYPE
	{
		CHICKEN_MATERIAL_TYPE_COMMON = 0,
		CHICKEN_MATERIAL_TYPE_HIT = 1
	};

	enum CHICKEN_ATTACK_STATE
	{
		CHICKEN_ATTACK_STATE_CHASE_PLAYER =0,
		CHICKEN_ATTACK_STATE_TYPE1	=	1,
		CHICKEN_ATTACK_STATE_TYPE2 = 2,
		CHICKEN_ATTACK_STATE_ULTIMATE_EXPLODE =3,
	};


	class IChickenMonster: public Base_GameObject
	{
	public:

		IChickenMonster(IBulletManager* pBulletMgr);

		void	Init(SCENE_TYPE modelID);

		void	Update(const VECTOR3& playerPos);

		void	Render();

		void GetBoundingBox(BOUNDINGBOX& outBox);

		void	BeHitAndChangeColor();

		VECTOR3 GetPosition();

		float GetInitialHealth() const;

	private:

		const float c_chickenInitialHealth = 2500.0f;

		void		mFunction_UpdateMovement(const VECTOR3& shootDir);

		void		mFunction_Fire(VECTOR3 firePos);

		void		mFunction_UpdateTexture();

		//spawn Bullets
		IBulletManager* const 	m_pBulletMgr;

		VECTOR3 mPos;
		VECTOR3 mLookat;

		IMesh		mMesh;
		IPicture		mTexture_Common;
		CHICKEN_MATERIAL_TYPE	mMatType;
		Material								mMat_Common;
		Material								mMat_Red;
		CHICKEN_ATTACK_STATE		mAttackState;//which attack mode the chicken is using
	};
};