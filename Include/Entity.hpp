#ifndef BOOK_ENTITY_HPP
#define BOOK_ENTITY_HPP

#include "SceneNode.hpp"


class Entity : public SceneNode
{
	public:
		void				setVelocity(sf::Vector2f velocity);
		void				setVelocity(float vx, float vy);
		void				accelerate(sf::Vector2f velocity);
		void				accelerate(float vx, float vy);
		sf::Vector2f		getVelocity() const;
    
        explicit            Entity(int hitpoints);
        int                 getHitpoints() const;

        //hitpoints increased when the plane is repaired
        void                repair(int points);
    
        //hitpoints decreased when the plane is damaged
        void                damage(int points);
    
        //instantly destroys the entity and set the hitpoints to zero
        void                destroy();
    
        virtual bool        isDestroyed() const;

	protected:
		virtual void		updateCurrent(sf::Time dt, CommandQueue& commands);


	private:
		sf::Vector2f		mVelocity;
    
    //Hitpoints (HP) are a measure for the integrity of an entity
    //The entity is destroyed as soon as the hitpoint each or fall below zero
        int                    mHitpoints;
};

#endif // BOOK_ENTITY_HPP
