#pragma once

#include "RaylibWrapper.h"
#include <stdexcept>
#include <string>

class TextureHandle
{
public:
	explicit TextureHandle(const std::string& filepath)
		: texture(LoadTexture(filepath.c_str()))
	{
		if (texture.id == 0)
		{
			throw std::runtime_error("Failed to load texture: " + filepath);
		}
	}

	~TextureHandle()
	{
		UnloadTexture(texture);
	}

	TextureHandle(const TextureHandle&) = delete;
	TextureHandle& operator=(const TextureHandle&) = delete;
	TextureHandle(TextureHandle&&) = delete;
	TextureHandle& operator=(TextureHandle&&) = delete;

	const Texture2D& get() const noexcept { return texture; }

private:
	Texture2D texture;
};
