#pragma once
#include "raylib.h"
#include "vector"

// TODO: No destructor - memory leak! Add ~Resources() and Rule of 5
// TODO: Make members private
struct Resources 
{
	~Resources();
	void Load();

	std::vector<Texture2D> shipTextures;
	Texture2D alienTexture;
	Texture2D barrierTexture;
	Texture2D laserTexture;
};