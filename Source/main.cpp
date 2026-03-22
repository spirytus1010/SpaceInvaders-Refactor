#include "Window.h"
#include "DrawScope.h"
#include "game.h"
#include <iostream>
#include <stdexcept>


int main()
{
	try
	{
		constexpr int screenWidth = 1920;
		constexpr int screenHeight = 1080;
		Window window(screenWidth, screenHeight, "SPACE INVADERS");

		Game game;

		while (!WindowShouldClose())
		{
			game.Update();

			{
				DrawScope frame;
				ClearBackground(BLACK);
				game.Render();
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}