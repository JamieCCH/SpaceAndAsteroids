#include "Include/World.hpp"
#include "Include/Projectile.hpp"
#include "Include/Pickup.hpp"
#include "Include/TextNode.hpp"
#include "Include/ParticleNode.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

#include "ResourcePath.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>


World::World(sf::RenderWindow& window, FontHolder& fonts)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mFonts(fonts)
, mTextures() 
, mSceneGraph()
, mSceneLayers()
//, mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 2000.f)
, mWorldBounds(0.f, 0.f, 1600.f, mWorldView.getSize().y)
//, mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y)
, mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldView.getSize().y / 2.f)
, mScrollSpeed(50.f)
, mPlayerAircraft(nullptr)
, mEnemySpawnPoints()
, mActiveEnemies()
{
	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
//    std::cout<<"player position " <<mPlayerAircraft->getPosition().x <<" / " <<mPlayerAircraft->getPosition().y <<std::endl<<std::endl;
    
	// Scroll the world, reset player velocity
//    mWorldView.move(0.f, mScrollSpeed * dt.asSeconds());
    mWorldView.move(mScrollSpeed * dt.asSeconds(),0.f);
	mPlayerAircraft->setVelocity(0.f, 0.f);

	// Setup commands to destroy entities, and guide missiles
//    destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	// Collision detection and response (may destroy entities)
	handleCollisions();

	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();
	spawnEnemies();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
}

void World::draw()
{
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::hasAlivePlayer() const
{
	return !mPlayerAircraft->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const
{
	return !mWorldBounds.contains(mPlayerAircraft->getPosition());
    //mWorldBounds: 0,0,5000,768
}

void World::loadTextures()
{
	mTextures.load(Textures::Entities, resourcePath()+"Media/Textures/Entities.png");
	mTextures.load(Textures::Space, resourcePath()+"Media/Textures/Space.jpg");
    mTextures.load(Textures::Explosion, resourcePath()+"Media/Textures/Explosion.png");
	mTextures.load(Textures::Particle, resourcePath()+"Media/Textures/Particle.png");
	mTextures.load(Textures::FinishLine, resourcePath()+"Media/Textures/FinishLine.png");
    
    mTextures.load(Textures::Spaceship, resourcePath()+"Media/Textures/spaceship.png");
    mTextures.load(Textures::FireBlue, resourcePath()+"Media/Textures/fire_blue.png");
    mTextures.load(Textures::Rocks, resourcePath()+"Media/Textures/rock.png");
    mTextures.load(Textures::smallRock, resourcePath()+"Media/Textures/rock_small.png");
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;

	sf::Vector2f position = mPlayerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayerAircraft->setPosition(position);
    mPlayerAircraft->setRotation(90.f);
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mPlayerAircraft->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerAircraft->setVelocity(velocity / std::sqrt(2.f));

	// Add scrolling velocity
//    mPlayerAircraft->accelerate(0.f, mScrollSpeed);
    mPlayerAircraft->accelerate(mScrollSpeed, 0.f);
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	for(SceneNode::Pair pair: collisionPairs)
	{
		if (matchesCategories(pair, Category::PlayerAircraft, Category::EnemyAircraft))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& enemy = static_cast<Aircraft&>(*pair.second);

			// Collision: Player damage = enemy's remaining HP
			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}

		else if (matchesCategories(pair, Category::PlayerAircraft, Category::Pickup))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Apply pickup effect to player, destroy projectile
			pickup.apply(player);
			pickup.destroy();
		}

		else if (matchesCategories(pair, Category::EnemyAircraft, Category::AlliedProjectile)
			  || matchesCategories(pair, Category::PlayerAircraft, Category::EnemyProjectile))
		{
			auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			// Apply projectile damage to aircraft, destroy projectile
			aircraft.damage(projectile.getDamage());
			projectile.destroy();
		}
	}
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	sf::Texture& SpaceTexture = mTextures.get(Textures::Space);
	SpaceTexture.setRepeated(true);

//    float viewHeight = mWorldView.getSize().y;
    float viewWidth = mWorldView.getSize().x;
	sf::IntRect textureRect(mWorldBounds);
//    textureRect.height += static_cast<int>(viewHeight);
    textureRect.width += static_cast<int>(viewWidth);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> SpaceSprite(new SpriteNode(SpaceTexture, textureRect));
    SpaceSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Background]->attachChild(std::move(SpaceSprite));
    
	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Smoke, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));

	// Add propellant particle node to the scene
	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(Particle::Propellant, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(propellantNode));
    

    std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Spaceship, mTextures, mFonts));
    mPlayerAircraft = player.get();
    mPlayerAircraft->setPosition(mSpawnPosition);
    mSceneLayers[UpperAir]->attachChild(std::move(player));

	// Add enemy aircraft
	addEnemies();
}

void World::addEnemies()
{
	// Add enemies to the spawn point container 0<x<500
    
    addEnemy(Aircraft::Asteroid, 0.f, 0.f);
    addEnemy(Aircraft::Asteroid, 5.f, 320.f);
    addEnemy(Aircraft::Asteroid, 260.f, 50.f);
    addEnemy(Aircraft::Asteroid, 340.f, -150.f);
    addEnemy(Aircraft::Asteroid, 450.f, -250.f);
    addEnemy(Aircraft::Asteroid, 30.f, 200.f);
    addEnemy(Aircraft::Asteroid, 400.f, 250.f);
    addEnemy(Aircraft::Asteroid, 50.f, -10.f);
    addEnemy(Aircraft::Asteroid, 450.f, 10.f);
    addEnemy(Aircraft::Asteroid, 499.f, -150.f);
    
//    addEnemy(Aircraft::SmallRock, 20.f, -150.f);
//    addEnemy(Aircraft::SmallRock, 220.f, 150.f);
//    addEnemy(Aircraft::SmallRock, 390.f, -400.f);
//    addEnemy(Aircraft::SmallRock, 80.f, 50.f);
//    addEnemy(Aircraft::SmallRock, 0.f, 10.f);
//    addEnemy(Aircraft::SmallRock, 70.f, -30.f);
//    addEnemy(Aircraft::SmallRock, 0.f, 230.f);
//    addEnemy(Aircraft::SmallRock, 190.f, -100.f);
//    addEnemy(Aircraft::SmallRock, 460.f, 210.f);
//    addEnemy(Aircraft::SmallRock, 210.f, -210.f);
//    addEnemy(Aircraft::SmallRock, 160.f, -310.f);
//    addEnemy(Aircraft::SmallRock, 460.f, -50.f);
    

    srand(time(NULL));
    
    int rockNum = rand()%50 +50;
    
    float mX = 10.f;
    float mY = (rand()&350)-350;
    std::cout<< "rockNum "<<rockNum<<std::endl<<std::endl;
    std::cout<< "position y: "<<mY<<std::endl<<std::endl;
    
    for(int i = 0; i<= rockNum; i++)
    {
        mX = rand()&500;
        mY = (rand()&350)-350;
        addEnemy(Aircraft::SmallRock, mX, mY);
        std::cout<< "position x / y: "<<mX<<"/"<<mY<<std::endl<<std::endl;
    }
    
   
    
	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [] (SpawnPoint lhs, SpawnPoint rhs)
	{
		return lhs.x < rhs.x;
	});
}

void World::addEnemy(Aircraft::Type type, float relX, float relY)
{
    SpawnPoint spawn(type, getViewBounds().width + relX, mSpawnPosition.y - relY);
	mEnemySpawnPoints.push_back(spawn);
}

void World::spawnEnemies()
{
	// Spawn all enemies entering the view area (including distance) this frame
	while (!mEnemySpawnPoints.empty() && mEnemySpawnPoints.back().x < getBattlefieldBounds().width)
	{
		SpawnPoint spawn = mEnemySpawnPoints.back();
		
		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(270.f);

		mSceneLayers[UpperAir]->attachChild(std::move(enemy));

//        std::cout<<"Enemy x: " <<spawn.x << " y: "<< spawn.y <<std::endl<<std::endl;
//        std::cout<<"getBattlefieldBounds width "<<getBattlefieldBounds().width<<std::endl<<std::endl;
//        std::cout<<"getBattlefieldBounds top "<<getBattlefieldBounds().top<<std::endl<<std::endl;
        
		// Enemy is spawned, remove from the list to spawn
		mEnemySpawnPoints.pop_back();
	}
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>([this] (Entity& e, sf::Time)
	{
		if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
			e.destroy();
	});

	mCommandQueue.push(command);
}

void World::guideMissiles()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>([this] (Aircraft& enemy, sf::Time)
	{
		if (!enemy.isDestroyed())
			mActiveEnemies.push_back(&enemy);
	});

	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this] (Projectile& missile, sf::Time)
	{
		// Ignore unguided bullets
		if (!missile.isGuided())
			return;

		float minDistance = std::numeric_limits<float>::max();
		Aircraft* closestEnemy = nullptr;

		// Find closest enemy
		for(Aircraft* enemy: mActiveEnemies)
		{
			float enemyDistance = distance(missile, *enemy);

			if (enemyDistance < minDistance)
			{
				closestEnemy = enemy;
				minDistance = enemyDistance;
			}
		}

		if (closestEnemy)
			missile.guideTowards(closestEnemy->getWorldPosition());
	});

	// Push commands, reset active enemies
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
//    bounds.top -= 100.f;
//    bounds.height += 100.f;
    bounds.width += 500.f;

	return bounds;
}
