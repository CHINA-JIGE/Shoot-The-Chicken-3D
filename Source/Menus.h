#pragma once

namespace GamePlay
{
	class IMenus
	{
	public:

		void	Init();

		void	UpdateAndRender_StartMenu();

	private:
		IPicture mMainMenuBgr_Start;
		IPicture mMainMenuBgr_Exit;

	};
}