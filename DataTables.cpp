#include "Include/DataTables.hpp"
#include "Include/Aircraft.hpp"
#include "Include/Projectile.hpp"
#include "Include/Pickup.hpp"
#include "Include/Particle.hpp"

#include <ctime>
#include <iostream>

//srand(time(NULL));

// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<AircraftData> initializeAircraftData()
{
	std::vector<AircraftData> data(Aircraft::TypeCount);
    
    data[Aircraft::Spaceship].hitpoints = 100;
    data[Aircraft::Spaceship].speed = 200.f;
    data[Aircraft::Spaceship].fireInterval = sf::seconds(1);
    data[Aircraft::Spaceship].texture = Textures::Spaceship;
//    data[Aircraft::Spaceship].textureRect = sf::IntRect(38, 0, 42, 37); //no particle
    data[Aircraft::Spaceship].textureRect = sf::IntRect(38, 39, 42, 43); //big particle
//    data[Aircraft::Spaceship].textureRect = sf::IntRect(38, 85, 42, 40); //small particle
    
    data[Aircraft::Asteroid].hitpoints = 20;
    data[Aircraft::Asteroid].speed = 60.f;
    data[Aircraft::Asteroid].texture = Textures::Rocks;
    data[Aircraft::Asteroid].textureRect = sf::IntRect(0,0,63,64);

    data[Aircraft::Asteroid].directions.push_back(Direction(+45.f, 150.f));
    data[Aircraft::Asteroid].directions.push_back(Direction(0.f, 80.f));
    data[Aircraft::Asteroid].directions.push_back(Direction(-45.f, 100.f));
    data[Aircraft::Asteroid].directions.push_back(Direction(0.f, 60.f));
    data[Aircraft::Asteroid].directions.push_back(Direction(+45.f, 120.f));
    
    data[Aircraft::SmallRock].hitpoints = 10;
    data[Aircraft::SmallRock].speed = 80.f;
    data[Aircraft::SmallRock].texture = Textures::smallRock;
    data[Aircraft::SmallRock].textureRect = sf::IntRect(0,0,60,60);
    data[Aircraft::SmallRock].directions.push_back(Direction(0.f, 80.f));
    data[Aircraft::SmallRock].directions.push_back(Direction(-55.f, 120.f));
    data[Aircraft::SmallRock].directions.push_back(Direction(+65.f, 150.f));
    data[Aircraft::SmallRock].directions.push_back(Direction(-45.f, 100.f));
    
    
    //    int ranAngle = sf::Randomizer::Random(-10,80);
    //    data[Aircraft::Asteroid].directions.push_back(Direction(rand()%90-10, 50.f));
    //    std::cout<<rand()%90-10<<std::endl;
    //    data[Aircraft::Asteroid].directions.push_back(Direction(rand()%90-10, 50.f));
    //    std::cout<<rand()%90-10<<std::endl;

//    data[Aircraft::Raptor].hitpoints = 20;
//    data[Aircraft::Raptor].speed = 80.f;
//    data[Aircraft::Raptor].texture = Textures::Entities;
//    data[Aircraft::Raptor].textureRect = sf::IntRect(144, 0, 84, 64);
//    data[Aircraft::Raptor].directions.push_back(Direction(+45.f, 80.f));
//    data[Aircraft::Raptor].directions.push_back(Direction(-45.f, 160.f));
//    data[Aircraft::Raptor].directions.push_back(Direction(+45.f, 80.f));
//
//    data[Aircraft::Raptor].fireInterval = sf::Time::Zero;
//
//    data[Aircraft::Avenger].hitpoints = 40;
//    data[Aircraft::Avenger].speed = 50.f;
//    data[Aircraft::Avenger].texture = Textures::Entities;
//    data[Aircraft::Avenger].textureRect = sf::IntRect(228, 0, 60, 59);
//    data[Aircraft::Avenger].directions.push_back(Direction(+45.f, 50.f));
//    data[Aircraft::Avenger].directions.push_back(Direction(0.f, 50.f));
//    data[Aircraft::Avenger].directions.push_back(Direction(-45.f, 100.f));
//    data[Aircraft::Avenger].directions.push_back(Direction(0.f, 50.f));
//    data[Aircraft::Avenger].directions.push_back(Direction(+45.f, 50.f));
//
//    data[Aircraft::Avenger].fireInterval = sf::seconds(2);

	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);

	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 300.f;
    data[Projectile::AlliedBullet].texture = Textures::FireBlue;
    data[Projectile::AlliedBullet].textureRect = sf::IntRect(2, 0, 27, 57);
//    data[Projectile::AlliedBullet].texture = Textures::Entities;
//    data[Projectile::AlliedBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 300.f;
	data[Projectile::EnemyBullet].texture = Textures::Entities;
	data[Projectile::EnemyBullet].textureRect = sf::IntRect(178, 64, 3, 14);

	data[Projectile::Missile].damage = 200;
	data[Projectile::Missile].speed = 150.f;
	data[Projectile::Missile].texture = Textures::Entities;
	data[Projectile::Missile].textureRect = sf::IntRect(160, 64, 15, 32);

	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);

	data[Pickup::HealthRefill].texture = Textures::Entities;
	data[Pickup::HealthRefill].textureRect = sf::IntRect(0, 64, 40, 40);
	data[Pickup::HealthRefill].action = [] (Aircraft& a) { a.repair(25); };
	data[Pickup::MissileRefill].texture = Textures::Entities;
	data[Pickup::MissileRefill].textureRect = sf::IntRect(40, 64, 40, 40);
	data[Pickup::MissileRefill].action = std::bind(&Aircraft::collectMissiles, _1, 3);
	data[Pickup::FireSpread].texture = Textures::Entities;
	data[Pickup::FireSpread].textureRect = sf::IntRect(80, 64, 40, 40);
	data[Pickup::FireSpread].action = std::bind(&Aircraft::increaseSpread, _1);
	data[Pickup::FireRate].texture = Textures::Entities;
	data[Pickup::FireRate].textureRect = sf::IntRect(120, 64, 40, 40);
	data[Pickup::FireRate].action = std::bind(&Aircraft::increaseFireRate, _1);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);

	data[Particle::Propellant].color = sf::Color(255, 255, 50);
	data[Particle::Propellant].lifetime = sf::seconds(0.6f);

	data[Particle::Smoke].color = sf::Color(50, 50, 50);
	data[Particle::Smoke].lifetime = sf::seconds(4.f);

	return data;
}



