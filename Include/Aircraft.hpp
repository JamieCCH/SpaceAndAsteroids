#pragma once

#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "Projectile.hpp"
#include "TextNode.hpp"
#include "Animation.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Aircraft : public Entity
{
	public:
		enum Type
		{
            Spaceship,
            Asteroid,
            SmallRock,
//            Eagle,
			Raptor,
			Avenger,
			TypeCount
		};


	public:
								Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		virtual bool 			isMarkedForRemoval() const;
		bool					isAllied() const;
		float					getMaxSpeed() const;
        virtual void            remove();

		void					increaseFireRate();
		void					increaseSpread();
		void					collectMissiles(unsigned int count);

		void 					fire();
		void					launchMissile();


	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);
		void					updateMovementPattern(sf::Time dt);
		void					checkPickupDrop(CommandQueue& commands);
		void					checkProjectileLaunch(sf::Time dt, CommandQueue& commands);
//        void                    checkExplosion(sf::Time dt, CommandQueue& commands);

		void					createBullets(SceneNode& node, const TextureHolder& textures) const;
		void					createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
		void					createPickup(SceneNode& node, const TextureHolder& textures) const;
//        void                    createExplosion(SceneNode& node, Explosion::Type type,const TextureHolder& textures) const;

		void					updateTexts();
        void                    updateRollAnimation();


	private:
        Animation               mExplosion;
		Type					mType;
		sf::Sprite				mSprite;
		Command 				mFireCommand;
		Command					mMissileCommand;
		sf::Time				mFireCountdown;
		bool 					mIsFiring;
		bool					mIsLaunchingMissile;
		bool 					mIsMarkedForRemoval;
        bool                    mShowExplosion;

		int						mFireRateLevel;
		int						mSpreadLevel;
		int						mMissileAmmo;

		Command 				mDropPickupCommand;
        Command                 mDisplayExplosion;
		float					mTravelledDistance;
		std::size_t				mDirectionIndex;
		TextNode*				mHealthDisplay;
		TextNode*				mMissileDisplay;
};

