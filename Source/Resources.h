#pragma once
#include "TextureHandle.h"
#include <vector>
#include <memory>

struct Resources
{
	Resources() noexcept(false)
		: alienTexture("./Assets/Alien.png")
		, barrierTexture("./Assets/Barrier.png")
		, laserTexture("./Assets/Laser.png")
	{
		shipTextures.push_back(std::make_unique<TextureHandle>("./Assets/Ship1.png"));
		shipTextures.push_back(std::make_unique<TextureHandle>("./Assets/Ship2.png"));
		shipTextures.push_back(std::make_unique<TextureHandle>("./Assets/Ship3.png"));
	}

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) = delete;
	Resources& operator=(Resources&&) = delete;

	const Texture2D& getShipTexture(int index) const
	{
		return shipTextures.at(index)->get();
	}

	const Texture2D& getAlienTexture() const noexcept { return alienTexture.get(); }
	const Texture2D& getBarrierTexture() const noexcept { return barrierTexture.get(); }
	const Texture2D& getLaserTexture() const noexcept { return laserTexture.get(); }

private:
	TextureHandle alienTexture;
	TextureHandle barrierTexture;
	TextureHandle laserTexture;
	std::vector<std::unique_ptr<TextureHandle>> shipTextures;
};