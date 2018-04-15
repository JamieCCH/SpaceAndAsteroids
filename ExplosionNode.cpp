//
//  ExplosionNode.cpp
//  FinalAS_SpacesShoot_Jamie
//
//  Created by Jamie on 2018/4/14.
//  Copyright © 2018 Jamie. All rights reserved.
//

#include <stdio.h>

#include "Include/ExplosionNode.hpp"
#include "Include/DataTables.hpp"
#include "Include/ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <algorithm>
#include <iostream>

namespace
{
    const std::vector<ExplosionData> Table = initializeExplosionData();
}

ExplosionNode::ExplosionNode(Explosion::Type type, const TextureHolder& textures)
: SceneNode()
, mExplosion()
, mTexture(textures.get(Textures::Explosion))
, mType(type)
, mVertexArray(sf::Quads)
, mNeedsVertexUpdate(true)
{
    std::cout<< "ExplosionNode" <<std::endl<<std::endl;
}

void ExplosionNode::addExplosion(sf::Vector2f position)
{
    Explosion explosion;
    explosion.position = position;
    explosion.lifetime = Table[mType].lifetime;
    mExplosion.push_back(explosion);
    
    std::cout<< "add ExplosionNode" <<std::endl<<std::endl;
}

Explosion::Type ExplosionNode::getExplosionType() const
{
    return mType;
}

unsigned int ExplosionNode::getCategory() const
{
    return Category::ExplosionSystem;
}


void ExplosionNode::updateCurrent(sf::Time dt, CommandQueue&)
{
    // Remove expired particles at beginning
    while (!mExplosion.empty() && mExplosion.front().lifetime <= sf::Time::Zero)
        mExplosion.pop_front();
    
    // Decrease lifetime of existing particles
    for (Explosion& explosion : mExplosion)
        explosion.lifetime -= dt;
    
    mNeedsVertexUpdate = true;
}


void ExplosionNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (mNeedsVertexUpdate)
    {
        computeVertices();
        mNeedsVertexUpdate = false;
    }
    
    // Apply particle texture
    states.texture = &mTexture;
    
    // Draw vertices
    target.draw(mVertexArray, states);
}

void ExplosionNode::addVertex(float worldX, float worldY, float texCoordX, float texCoordY, const sf::Color& color) const
{
    sf::Vertex vertex;
    vertex.position = sf::Vector2f(worldX, worldY);
    vertex.texCoords = sf::Vector2f(texCoordX, texCoordY);
    vertex.color = color;
    
    mVertexArray.append(vertex);
}

void ExplosionNode::computeVertices() const
{
    //Determine the vertex positions more easily
    sf::Vector2f size(mTexture.getSize());
    sf::Vector2f half = size / 2.f;
    
    // Refill vertex array
    mVertexArray.clear();
    for (const Explosion& explosion : mExplosion)
    {
        sf::Vector2f pos = explosion.position;
        sf::Color color = explosion.color;
        
        //Compute the ratio between the remaining and total lifetime - [0,1] --> [0, 255] sets the alpha value
        float ratio = explosion.lifetime.asSeconds() / Table[mType].lifetime.asSeconds();
//        color.a = static_cast<sf::Uint8>(255 * std::max(ratio, 0.f));

        addVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, color);
        addVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, color);
        addVertex(pos.x + half.x, pos.y + half.y, size.x, size.y, color);
        addVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, color);
        
    }
}
