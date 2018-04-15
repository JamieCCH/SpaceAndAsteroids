//
//  Explosion.hpp
//  FinalAS_SpacesShoot_Jamie
//
//  Created by Jamie on 2018/4/14.
//  Copyright © 2018 Jamie. All rights reserved.
//

#ifndef Explosion_h
#define Explosion_h


#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>


struct Explosion
{
    enum Type
    {
        ExplosionTypeA,
        ExplosionTypeB,
        ExplosionTypeC,
        ExplosionCount
    };
    
    sf::Color       color;
    sf::Vector2f    position;
    sf::Time        lifetime;
};



#endif /* Explosion_h */
