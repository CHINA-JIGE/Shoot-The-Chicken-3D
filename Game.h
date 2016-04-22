#pragma once

#include "MyConsoleEngine.h"



namespace GamePlay
{

	//Only Expose functions, resources are defined in .cpp
	void	Init();

	void	UpdateTimer();

	void UpdateWindowTitle();

	void MouseAndKeyBoradProcess();

	void	Render();

};