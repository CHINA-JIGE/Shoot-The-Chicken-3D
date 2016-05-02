/***********************************************************************

                           h£ºMain Game Logic 

************************************************************************/

#pragma once

namespace GamePlay
{
	class IMainGame
	{
	public:

		IMainGame();

		void	Init();

		void	UpdateLogic();

		void	Render();


	private:
		IChickenMonster			mChickenBoss;
		IPlayer							mPlayer;
		ISceneModelManager	mSceneMgr;
		IBulletManager				mBulletMgr;

	};
};