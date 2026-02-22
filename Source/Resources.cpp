#include "Resources.h"
#include <iostream>


// TODO: Implement destructor to unload all textures
// TODO: Implement move constructor and move assignment
void Resources::Load()
{
	alienTexture = LoadTexture("./Assets/Alien.png");
	barrierTexture = LoadTexture("./Assets/Barrier.png");
	shipTextures.push_back(LoadTexture("./Assets/Ship1.png"));
	shipTextures.push_back(LoadTexture("./Assets/Ship2.png"));
	shipTextures.push_back(LoadTexture("./Assets/Ship3.png"));
	laserTexture = LoadTexture("./Assets/Laser.png");

    isLoaded = true;
}

Resources::~Resources() 
{
    if (isLoaded) 
    {
        for (auto& texture : shipTextures) 
        {
            UnloadTexture(texture);
        }
        
        UnloadTexture(alienTexture);
        UnloadTexture(barrierTexture);
        UnloadTexture(laserTexture);
    }
}

// Move constructor£¬transfer ownership from 'other' to this object
Resources::Resources(Resources&& other) noexcept
    : shipTextures(std::move(other.shipTextures))
    , alienTexture(other.alienTexture)
    , barrierTexture(other.barrierTexture)
    , laserTexture(other.laserTexture)
    , isLoaded(other.isLoaded)
{
    other.isLoaded = false;
}

// Move assignment, transfer ownership from 'other' to this object
Resources& Resources::operator=(Resources&& other) noexcept
{
    if (this != &other)
    {
        this->~Resources();

        shipTextures = std::move(other.shipTextures);
        alienTexture = other.alienTexture;
        barrierTexture = other.barrierTexture;
        laserTexture = other.laserTexture;
        isLoaded = other.isLoaded;

        other.isLoaded = false;
    }
    return *this;
}

/*
void Resources::Unload()
{
	UnloadTexture(barrierTexture);
	UnloadTexture(alienTexture);
}
*/