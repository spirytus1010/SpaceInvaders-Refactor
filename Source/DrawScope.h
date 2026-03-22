#pragma once

#include "RaylibWrapper.h"

class DrawScope
{
public:
	DrawScope() noexcept
	{
		BeginDrawing();
	}

	~DrawScope()
	{
		EndDrawing();
	}

	DrawScope(const DrawScope&) = delete;
	DrawScope& operator=(const DrawScope&) = delete;
	DrawScope(DrawScope&&) = delete;
	DrawScope& operator=(DrawScope&&) = delete;
};