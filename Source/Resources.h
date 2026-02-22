#pragma once
#include "raylib.h"
#include "vector"

// TODO: Make members private
struct Resources 
{
	Resources() = default;

	~Resources();

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;

	Resources(Resources&& other) noexcept;
	Resources& operator=(Resources&& other) noexcept;

	void Load();

	std::vector<Texture2D> shipTextures;
	Texture2D alienTexture;
	Texture2D barrierTexture;
	Texture2D laserTexture;
	bool isLoaded = false;
};