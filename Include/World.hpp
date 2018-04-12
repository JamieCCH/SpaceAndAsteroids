#ifndef BOOK_WORLD_HPP
#define BOOK_WORLD_HPP

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Aircraft.hpp"
#include "CommandQueue.hpp"
#include "Command.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <queue>


// Forward declaration
namespace sf
{
	class RenderWindow;
}

class World : private sf::NonCopyable
{
	public:
		explicit							World(sf::RenderWindow& window, FontHolder& fonts);
		void								update(sf::Time dt);
		void								draw();
		
		CommandQueue&						getCommandQueue();


	private:
		void								loadTextures();
		void								buildScene();
		void								adaptPlayerPosition();
		void								adaptPlayerVelocity();


	private:
		enum Layer
		{
			Background,
			Air,
			LayerCount
		};
    
        struct SpawnPoint
        {
            SpawnPoint(Aircraft::Type type, float x, float y)
            : type(type)
            , x(x)
            , y(y)
            {
            }
            
            Aircraft::Type type;
            float x;
            float y;
        };


	private:
		sf::RenderWindow&					mWindow;
		sf::View							mWorldView;
		TextureHolder						mTextures;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;

		sf::FloatRect						mWorldBounds;
		sf::Vector2f						mSpawnPosition;
		float								mScrollSpeed;
		Aircraft*							mPlayerAircraft;
        FontHolder&                         mFonts;
    
    
        //Hold all future spawn points. As soon as enemy enters the battlefield,
        //enemy is created and inserted to the scene graph and the spwan point is removed.
        std::vector<SpawnPoint>             mEnemySpawnPoints;
    
        //Battlefield area extends the view area by a small rectangle at the top,
        //inside which the new enemies spawn before they enter the view
        sf::FloatRect                       getViewBounds() const;
        sf::FloatRect                       getBattlefieldBounds() const;
    
        void                                spawnEnemies();
    
        std::vector<Aircraft*>              mActiveEnemies;
        void                                addEnemies();
        void                                addEnemy(Aircraft::Type type, float relX, float relY);
};

#endif // BOOK_WORLD_HPP
