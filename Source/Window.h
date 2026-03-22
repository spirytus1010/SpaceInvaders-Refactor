#pragma once

#include "RaylibWrapper.h"

#include <stdexcept>
#include <string>

class Window
{
public:
	Window(int width, int height, const std::string& title)
	{
		InitWindow(width, height, title.c_str());

		if (!IsWindowReady())
		{
			throw std::runtime_error("Failed to initialize window.");
		}
		SetTargetFPS(60);
	}

	~Window()
	{
		CloseWindow();
	}

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
};