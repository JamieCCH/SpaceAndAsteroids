#pragma once

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include "TextNode.hpp"
#include "Command.hpp"
#include "Projectile.hpp"

class Aircraft : public Entity
{
	public:
		enum Type
		{
			Eagle,
			Raptor,
            Avenger, //enemy
            TypeCount,
		};


	public:
		Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts);
		virtual unsigned int	getCategory() const;
        float                   getMaxSpeed() const;
    
        void                    fire();
        void                    launchMissile();
        bool                    isAllied() const;
    
    private:
        void                    checkProjectileLaunch(sf::Time dt, CommandQueue& commands);
        void                    createBullets(SceneNode& node, const TextureHolder& textures) const;
        void                    createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;


		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void            updateCurrent(sf::Time dt, CommandQueue& commands);

		Type					mType;
		sf::Sprite				mSprite;
        TextNode*               mHealthDisplay;
        void                    updateTexts();
    
        //The distance already travelled for each direction
        float                   mTravelledDistance;
    
        //which direction the plane is currently taking
        std::size_t             mDirectionIndex;
    
        //check if there are movement patterns for current type, otherwise the plane flies straight down
        void                    updateMovementPattern(sf::Time dt);
    
        Command                 mFireCommand;
        Command                 mMissileCommand;
        sf::Time                mFireCountdown;
        bool                    mIsFiring; //to avoid firing a projectile for every frame
        bool                    mIsLaunchingMissile;
        bool                    mIsMarkedForRemoval;

        int                     mFireRateLevel;
        int                     mSpreadLevel;
        int                     mMissileAmmo;

        TextNode*               mMissileDisplay;
};


