//
//  Collectible.cpp
//  Rooted
//
//  Created by Miranda Luo on 4/19/24.
//

#include "Collectible.h"
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new dude at the given position.
 *
 * The dude is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Collectible::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, bool fired, string ownerUUID) {
    // Obstacle dimensions and drawing initialization
    Size nsize = size;
    _drawScale = scale;
    _fired = fired;
    _ownerUUID = ownerUUID;
    _age = 0;
    
    if (BoxObstacle::init(pos, nsize)) {
        setSensor(!fired);
        setBullet(true);
        
        updateCurAnimDurationForState();
        return true;
    }
    return false;
}

#pragma mark -
#pragma mark Animation

/**
    Whether this EntityModel should step in its animation for this frame.
 
    For now, we step only when there is a directional input OR the state is DASHING/PLANTING.
 */
bool Collectible::animationShouldStep() {
    return false;
}

void Collectible::stepAnimation(float dt) {
//    if (_node != nullptr) {
//        if (animationShouldStep()) {
//                curAnimTime += dt;
//                if (curAnimTime > (curAnimDuration)) { curAnimTime = 0;}
//                 _node->setFrame(std::floor(_node->getSpan() * curAnimTime / curAnimDuration));
//        }
//        else if (_node->getFrame() != 0) {
//            _node->setFrame(0);
//            curAnimTime = 0;
//        }
//    }
}

#pragma mark -
#pragma mark Attribute Properties


#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void Collectible::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    BoxObstacle::createFixtures();
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void Collectible::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }
    BoxObstacle::releaseFixtures();
}

/**
 * Disposes all resources and assets of this DudeModel
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a DudeModel may not be used until it is initialized again.
 */
void Collectible::dispose() {
    _node = nullptr;
    _wheatHeightNode = nullptr;
    _geometry = nullptr;
}

/**
 * Applies the force to the body of this dude
 *
 * This method should be called after the force attribute is set.
 */
void Collectible::applyForce() {
    if (!isEnabled()) {
        return;
    }
    
    // using a quadradic ease out like function
    if (_fired) {
        setLinearVelocity(_initVelocity * (2 * (MAX_COLLECTIBLE_AGE - _age)));
    }
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void Collectible::update(float dt) {
    BoxObstacle::update(dt);
    
    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
        
        if (!_fired) {
            _node->setPositionY(_node->getPositionY() + (std::sin(_age*1.5) + 1.0) * _drawScale/4.0);
            _node->setColor(Color4(255,255,255,std::min(255.0, _age * 255.0 / 1.5)));
        }
    }
    
    if (_wheatHeightNode != nullptr) {
        updateWheatHeightNode();
    }
    
    _age += PROGRESS;
}


#pragma mark -
#pragma mark Scene Graph Methods
/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 */
void Collectible::resetDebug() {
    BoxObstacle::resetDebug();
}

std::shared_ptr<cugl::scene2::SceneNode> Collectible::allocWheatHeightNode(float initCharHeight) {
    _initCharacterHeight = initCharHeight;
    pf = PolyFactory(0.01);
    _wheatHeightTarget = 0.0;
    _wheatSizeTarget = 0.75;
    _currWheatHeight = _wheatHeightTarget;
    _currWheatSize = _wheatSizeTarget;
    _wheatHeightNode = scene2::PolygonNode::allocWithPoly(pf.makeEllipse(Vec2(0,0), _wheatSizeTarget * Size(1.6, 0.9)));
    _wheatHeightNode->setColor(Color4(0, 0, 0, 255));
    _wheatHeightNode->setBlendFunc(GL_DST_ALPHA, GL_ZERO, GL_ONE, GL_ONE);
    _wheatHeightNode->setAnchor(Vec2::ANCHOR_CENTER);
    _wheatHeightNode->setPosition(getX(), getY()-_initCharacterHeight);
    return _wheatHeightNode;
}

void Collectible::updateWheatHeightNode() {
    // TODO: slowly decrease height of the collectible
    _wheatHeightNode->setPosition(getX(), getY()-_initCharacterHeight);
    _wheatSizeTarget = 0.75;
    _wheatHeightTarget = round(getLinearVelocity().length());
    _currWheatHeight += (_wheatHeightTarget - _currWheatHeight) * 0.2;
    _currWheatSize += (_wheatSizeTarget - _currWheatSize) * 0.2;
    _wheatHeightNode->setPolygon(pf.makeEllipse(Vec2(0,0), _currWheatSize * Size(1.6, 0.9)));
    _wheatHeightNode->setColor(Color4(0,_currWheatHeight > 0 ? int(_currWheatHeight) : 0,
                                      _currWheatHeight < 0 ? -int(_currWheatHeight) : 0,255));
}
