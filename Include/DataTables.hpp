#pragma once
#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <functional>


// A direction consists of an angle and a distance
struct Direction
{
    Direction(float angle, float distance)
    : angle(angle)
    , distance(distance)
    {
    }
    
    float angle;
    float distance;
};

struct AircraftData
{
	int								hitpoints;
	float							speed;
	Textures::ID					texture;
    
    //sequence of directions that enemy airplane heads.
    std::vector<Direction>          directions;
    sf::Time                        fireInterval;
    
};

struct ProjectileData
{
    int                             damage;
    float                           speed;
    Textures::ID                    texture;
};

std::vector<AircraftData>	    initializeAircraftData();
std::vector<ProjectileData>     initializeProjectileData();

