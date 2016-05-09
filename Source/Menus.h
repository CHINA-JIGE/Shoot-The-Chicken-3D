#pragma once

namespace GamePlay
{
	class IMenus
	{
	public:

		void	Init();

		void	UpdateAndRender_StartMenu();

		//void	UpdateAndRenderPauseMenu();

		//void	UpdateRenderGameOverMenu(BOOL hasWon);

	private:
		IPicture mMainMenuBgr_Start;
		IPicture mMainMenuBgr_Exit;

	};
}