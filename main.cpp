
#include "MyConsoleEngine.h"
#include "Game.h"

Renderer myRenderer;

int main() 
{
	myRenderer.SetWindowTitle("Soft Renderer - By Jige");
	myRenderer.Init(250,100);

	while (1)
	{
		myRenderer.Clear(COLOR3(128,128,0),TRUE);
		myRenderer.Render();
		myRenderer.Present();
	}
}
