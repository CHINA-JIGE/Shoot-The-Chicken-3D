
#include "MyConsoleEngine.h"
#include "Game.h"

ITimer myTimer(TIMER_TIMEUNIT_MILLISECOND);


int main() 
{
	GamePlay::Init();

	while (1)
	{
		GamePlay::UpdateWindowTitle();
		GamePlay::UpdateTimer();
		GamePlay::MouseAndKeyBoradProcess();
		GamePlay::Render();
	}
}
