//
//  Collectible.cpp
//  Rooted
//
//  Created by Miranda Luo on 4/19/24.
//

#include "Collectible.h"
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include <cugl/math/CUEasingFunction.h>

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
bool Collectible::init(const cugl::Vec2& pos, const cugl::Size& size, const cugl::Size& collidersize, float scale, bool fired, string ownerUUID) {
    // Obstacle dimensions and drawing initialization
    Size nsize = size;
    _drawScale = scale;
    _fired = fired;
    _ownerUUID = ownerUUID;
    _disappearing = false;
    _collected = false;
    _age = 0;
    _collidersize = collidersize;
    
    if (BoxObstacle::init(pos, nsize)) {
        setSensor(!fired);
        setBullet(true);
        setDensity(2.0);
        setRestitution(0.9);
        setFixedRotation(true);
        setMass(2.0);
        return true;
    }
    return false;
}

#pragma mark -
#pragma mark Attribute Properties
void Collectible::setScale(Vec2 scale) {
    _nodeScale = scale;
    if (_fired) _node->setScale(_nodeScale);
}

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
    _collidershape.SetAsBox(_collidersize.width/2, _collidersize.height/2, b2Vec2(0, -(getHeight() - _collidersize.height)/2), 0);
    
    b2FixtureDef fixturedef;
    fixturedef.shape = &_collidershape;
    fixturedef.isSensor = !_fired;
    fixturedef.density = 2.0;
    _collidername = "collider";
    fixturedef.userData.pointer = reinterpret_cast<uintptr_t>(&_collidername);
    _colliderfixture = _body->CreateFixture(&fixturedef);
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
    if (_colliderfixture != nullptr) {
        _body->DestroyFixture(_colliderfixture);
        _colliderfixture = nullptr;
    }
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

    if (_fired) {
        setLinearVelocity(getLinearVelocity() * 0.97);
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
            if (_collected) {
                _node->setScale(std::max(0.0f, (1.0f-4.0f*EasingFunction::backIn(std::min(_age*4.0f, 1.0f)))) * _nodeScale);
                markRemoved(_node->getScaleX() * _node->getScaleY() == 0.0f);
            } else {
                _node->setScale(EasingFunction::elasticOut(std::min(_age, 1.0f), 0.2) * _nodeScale);
                _node->setPositionY(_node->getPositionY() + _drawScale / 2.0 - (_age > 0.5) *
                        EasingFunction::bounceOut(std::min(_age - 0.5f, 1.0f)) * _drawScale / 2.0);
            }
        } else{
            if (_disappearing) {
//                float alpha = std::clamp(3.0f - _age, 0.0f, 1.0f);
//                _node->setColor(Color4(255,255,255,255 * alpha));
//                markRemoved(alpha == 0.0f);
                _node->setScale(std::max(0.0f, (1.0f-4.0f*EasingFunction::backIn(std::min(_age*4.0f, 1.0f)))) * _nodeScale);
                markRemoved(_node->getScaleX() * _node->getScaleY() == 0.0f);
            } else {
                _node->setScale(EasingFunction::backOut(std::min(_age*4.0f, 1.0f)) * _nodeScale);
                if (_age > MAX_COLLECTIBLE_AGE || getLinearVelocity().lengthSquared() < 0.005f) {
                    _disappearing = true;
                    _age = 0;
                }
            }
        }
    }
    
    if (_wheatHeightNode != nullptr) {
        updateWheatHeightNode();
    }
    
    _age += dt;
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
    if (_colliderdebug == nullptr) {
        _colliderdebug = scene2::WireNode::allocWithPath(Rect(Vec2::ANCHOR_CENTER,_collidersize));
        _colliderdebug->setRelativeColor(false);
        _colliderdebug->setColor(Color4::RED);
        if (_scene != nullptr) {
            _debug->addChild(_colliderdebug);
        }
    } else {
        _colliderdebug->setPath(Rect(Vec2::ZERO,_collidersize));
    }
    _colliderdebug->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
    _colliderdebug->setPosition(Vec2(getWidth()/2, 0));
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
