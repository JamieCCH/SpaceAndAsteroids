#include "Include/Aircraft.hpp"
#include "Include/DataTables.hpp"
#include "Include/Utility.hpp"
#include "Include/Pickup.hpp"
#include "Include/CommandQueue.hpp"
#include "Include/ResourceHolder.hpp"
#include "Include/ExplosionNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>
#include <iostream>

namespace
{
	const std::vector<AircraftData> Table = initializeAircraftData();
}

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts)
: Entity(Table[type].hitpoints)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mFireCommand()
, mMissileCommand()
, mFireCountdown(sf::Time::Zero)
, mIsFiring(false)
, mIsLaunchingMissile(false)
, mIsMarkedForRemoval(false)
, mFireRateLevel(1)
, mSpreadLevel(1)
, mMissileAmmo(2)
, mDropPickupCommand()
, mDisplayExplosion()
, mTravelledDistance(0.f)
, mDirectionIndex(0)
, mHealthDisplay(nullptr)
, mMissileDisplay(nullptr)
{
	centerOrigin(mSprite);

	mFireCommand.category = Category::SceneAirLayer;
	mFireCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createBullets(node, textures);
	};

	mMissileCommand.category = Category::SceneAirLayer;
	mMissileCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = Category::SceneAirLayer;
	mDropPickupCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};
    
    mDisplayExplosion.category = Category::SceneAirLayer;
    mDisplayExplosion.action = [this, &textures](SceneNode& node, sf::Time)
    {
        createExplosion(node,Explosion::ExplosionTypeA, textures);
    };

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == Category::PlayerAircraft)
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
        mMissileDisplay->setPosition(50.f, 0.f);
	}

	updateTexts();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
    //update spaceship texture
    
    
	// Entity has been destroyed: Possibly drop pickup, mark for removal ---------------------------------------
	if (isDestroyed())
	{
        checkExplosion(dt, commands);
		checkPickupDrop(commands);
		mIsMarkedForRemoval = true;
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);

	// Update texts
	updateTexts();
}

unsigned int Aircraft::getCategory() const
{
	if (isAllied())
		return Category::PlayerAircraft;
	else
		return Category::EnemyAircraft;
}

sf::FloatRect Aircraft::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const
{
	return mIsMarkedForRemoval;
}

bool Aircraft::isAllied() const
{
//    return mType == Eagle;
    return mType == Spaceship;
}

float Aircraft::getMaxSpeed() const
{
	return Table[mType].speed;
}

void Aircraft::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Aircraft::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Aircraft::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Aircraft::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Aircraft::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Aircraft::updateMovementPattern(sf::Time dt)
{
	// Enemy airplane: Movement pattern
	const std::vector<Direction>& directions = Table[mType].directions;
	if (!directions.empty())
	{
		// Moved long enough in current direction: Change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance)
		{
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}

		// Compute velocity from direction, Add 90 degrees to fly downwards, 180 to fly toward left
		float radians = toRadian(directions[mDirectionIndex].angle + 180.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);

		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

void Aircraft::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0)
        commands.push(mDropPickupCommand);
}

void Aircraft::checkExplosion(sf::Time dt, CommandQueue &commands)
{
    commands.push(mDisplayExplosion);
    std::cout<< "Check Explosion " <<std::endl<<std::endl;
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	if (!isAllied())
		fire();

	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for missile launch
	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		mIsLaunchingMissile = false;
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;

	switch (mSpreadLevel)
	{
		case 1:
			createProjectile(node, type, 0.0f, 0.5f, textures);
			break;

		case 2:
			createProjectile(node, type, -0.33f, 0.33f, textures);
			createProjectile(node, type, +0.33f, 0.33f, textures);
			break;

		case 3:
			createProjectile(node, type, -0.5f, 0.33f, textures);
			createProjectile(node, type,  0.0f, 0.5f, textures);
			createProjectile(node, type, +0.5f, 0.33f, textures);
			break;
	}
}

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

//    sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
//    sf::Vector2f velocity(0, projectile->getMaxSpeed());
    sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width +20.f, yOffset * mSprite.getGlobalBounds().height -20.f);
    sf::Vector2f velocity(projectile->getMaxSpeed(),0);

	float sign = isAllied() ? +1.f : -1.f;
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
    projectile->setRotation(getRotation() * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::createExplosion(SceneNode& node, Explosion::Type type, const TextureHolder& textures) const
{
    std::unique_ptr<ExplosionNode> ExplosionNode(new class ExplosionNode(Explosion::ExplosionTypeA, textures));
    ExplosionNode->setPosition(getWorldPosition());
    ExplosionNode->addExplosion(getPosition());
    node.attachChild(std::move(ExplosionNode));
    
    std::cout<< "create Explosion " <<std::endl<<std::endl;
}

void Aircraft::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Aircraft::updateTexts()
{
    if(isAllied()){
        mHealthDisplay->setString("HP: " + toString(getHitpoints()));
        mHealthDisplay->setPosition(-50.f, 0.f);
        mHealthDisplay->setRotation(-getRotation());
    }

	if (mMissileDisplay)
	{
        mMissileDisplay->setRotation(-getRotation());
        
		if (mMissileAmmo == 0)
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}
