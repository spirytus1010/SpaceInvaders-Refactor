#pragma once
#include "TextureHandle.h"

struct Resources
{
	Resources() noexcept(false)
		: alienTexture("./Assets/Alien.png")
		, barrierTexture("./Assets/Barrier.png")
		, laserTexture("./Assets/Laser.png")
		, shipTexture1("./Assets/Ship1.png")
		, shipTexture2("./Assets/Ship2.png")
		, shipTexture3("./Assets/Ship3.png")
	{
	}

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) = delete;
	Resources& operator=(Resources&&) = delete;

	const Texture2D& getShipTexture(int index) const
	{
		switch (index)
		{
		case 0: return shipTexture1.get();
		case 1: return shipTexture2.get();
		case 2: return shipTexture3.get();
		default: throw std::out_of_range("Invalid ship texture index");
		}
	}

	const Texture2D& getAlienTexture() const noexcept { return alienTexture.get(); }
	const Texture2D& getBarrierTexture() const noexcept { return barrierTexture.get(); }
	const Texture2D& getLaserTexture() const noexcept { return laserTexture.get(); }

private:
	TextureHandle alienTexture;
	TextureHandle barrierTexture;
	TextureHandle laserTexture;
	TextureHandle shipTexture1;
	TextureHandle shipTexture2;
	TextureHandle shipTexture3;
};