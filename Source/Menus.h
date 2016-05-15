#pragma once

namespace GamePlay
{
	/*template <typename func>
	struct MenuPage
	{
		int		prevStateID;
		int		currentStateID;
		int		nextStateID;
		int		VK_movePrev;//virtual key, VK_XXX
		int		VK_Confirm;
		int		VK_moveNext;
		IPicture pagePicture;
		func behaviour;//whatever, functors, lambdas. But operator() must be overloaded
	};*/

	class IMenus
	{
	public:

		void	Init();

		void	UpdateAndRender_StartMenu();

		void	UpdateAndRender_ChooseScene();

	private:
		IPicture mMainMenuBgr_Start;
		IPicture mMainMenuBgr_Exit;

		IPicture mChooseSceneBgr_Cosmos;
		IPicture	mChooseSceneBgr_Checkerboard;

	};
}