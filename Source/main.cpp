
#include "MyConsoleEngine.h"
#include "Game.h"

int main() 
{
	GamePlay::InitGlobal();

	while (1)
	{
		GamePlay::UpdateWindowTitle();
		GamePlay::UpdateGlobalTimer();
		//a state machine to drive different scene
		//std::terminate() will be called when quit.
		GamePlay::GameStateSelector();
	}
	return 0;
}
