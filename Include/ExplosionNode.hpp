//
//  ExplosionNode.hpp
//  FinalAS_SpacesShoot_Jamie
//
//  Created by Jamie on 2018/4/14.
//  Copyright © 2018 Jamie. All rights reserved.
//

#ifndef ExplosionNode_h
#define ExplosionNode_h

#include "SceneNode.hpp"
#include "ResourceIdentifiers.hpp"
#include "Explosion.hpp"

#include <SFML/Graphics/VertexArray.hpp>

#include <deque>

class ExplosionNode : public SceneNode
{
public:
    ExplosionNode(Explosion::Type type, const TextureHolder& textures);
    
    void                    addExplosion(sf::Vector2f position);
    Explosion::Type         getExplosionType() const;
    virtual unsigned int    getCategory() const;

private:
    virtual void            updateCurrent(sf::Time dt, CommandQueue& commands);
    virtual void            drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
    void                    addVertex(float worldX, float worldY, float texCoordX, float texCoordY, const sf::Color& color) const;
    void                    computeVertices() const;
    
private:
    std::deque<Explosion>      mExplosion;
    const sf::Texture&         mTexture;
    Explosion::Type            mType;
    mutable sf::VertexArray    mVertexArray;
    mutable bool               mNeedsVertexUpdate;
    
};





#endif /* ExplosionNode_h */
