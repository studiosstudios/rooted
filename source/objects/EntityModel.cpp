//
//  PFDudeModel.cpp
//  PlatformDemo
//
//  This encapsulates all of the information for the character avatar.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  Pay close attention to how this class designed.  Subclasses of Cocos2d classes
//  (which are all subclasses of the class Ref) should never have normal public
//  constructors.  Instead, you should organize their constructors into three parts,
//  just like we have done in this class.
//
//  NORMAL CONSTRUCTOR:
//  The standard constructor should be protected (not private).  It should only
//  initialize pointers to nullptr and primitives to their defaults (pointers are
//  not always nullptr to begin with).  It should NOT take any arguments and should
//  not allocate any memory or call any methods.
//
//  STATIC CONSTRUCTOR
//  This is a static method that allocates the object and initializes it.  If
//  initialization fails, it immediately disposes of the object.  Otherwise, it
//  returns an autoreleased object, starting the garbage collection system.
//  These methods all look the same.  You can copy-and-paste them from sample code.
//  The only difference is the init method called.
//
//  INIT METHOD
//  This is a protected method that acts like what how would normally think a
//  constructor might work.  It allocates memory and initializes all values
//  according to provided arguments.  As memory allocation can fail, this method
//  needs to return a boolean indicating whether or not initialization was
//  successful.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/17
//
#include "EntityModel.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include "../RootedConstants.h"

#pragma mark -
#pragma mark Physics Constants
/** The amount to shrink the body fixture (vertically) relative to the image */
#define DUDE_VSHRINK  0.95f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define DUDE_HSHRINK  0.7f
/** The density of the character */
#define DUDE_DENSITY  1.0f
#define DUDE_DASH      12.0f


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
bool EntityModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    
    // Obstacle dimensions and drawing initialization
    Size nsize = size;
    nsize.width  *= DUDE_HSHRINK;
    nsize.height *= DUDE_VSHRINK;
    _drawScale = scale;
    _hasRock = false;
    dashTimer = 0;
    if (BoxObstacle::init(pos,nsize)) {
        setDensity(DUDE_DENSITY);
        setFriction(0.0f);
        setFixedRotation(true);
        
        // Gameplay attributes
        _facing = SOUTH;
        _state = STANDING;
        _prevState = STANDING;
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
    TODO: If we get idle animations, this will need to change
 */
bool EntityModel::animationShouldStep() {
    return isMoving() || _state == DASHING || _state == ROOTING;
}

void EntityModel::stepAnimation(float dt) {
    if (_node != nullptr) {
        if (animationShouldStep()) {
                curAnimTime += dt;
                if (curAnimTime > (curAnimDuration)) { curAnimTime = 0;}
                // PING-PING style animation
                // https://www.desmos.com/calculator/kszulthvhz
//                _node->setFrame(std::floor(_node->getSpan() * (-abs(curAnimTime - curAnimDuration) + curAnimDuration) / curAnimDuration));
                // LOOPING style animation
                 _node->setFrame(lround(_node->getSpan() * curAnimTime / curAnimDuration) % _node->getSpan());
        }
        else if (_node->getFrame() != 0) {
            _node->setFrame(0);
            curAnimTime = 0;
        }
    }
   
    
}

#pragma mark -
#pragma mark Attribute Properties

EntityModel::EntityFacing EntityModel::calculateFacing(cugl::Vec2 movement) {
    EntityFacing dir = _facing;
    float ang = atan(movement.y/movement.x);

    ang = abs(ang);
    if (movement.x < 0 && movement.y > 0) { // Top-left quadrant, excluding axes
        ang = M_PI_2 + (M_PI_2 - ang);
    }
    else if (movement.x <= 0 && movement.y <= 0) { // Bottom-left quadrant, including both axes
        ang += M_PI;
    }
    else if (movement.x > 0 && movement.y < 0) { // Bottom-right quadrant, excluding axes
        ang = 3 * M_PI_2 + (M_PI_2 - ang);
    }
    
    // Adjustment for the overflow for the EAST direction
    // TODO: See if this can be simplified?
    if (ang < M_PI / 8) {
        ang += 2 * M_PI;
    }
    for (auto i = _facingMap.begin(); i != _facingMap.end(); i++) {
        if (ang >= i->first.x && ang < i->first.y) {
            dir = i->second;
            break;
        }
    }
//    std::cout << "Ang: " << ang << " | Dir: " << dir << "\n";
    return dir;
}

/**
 * Sets left/right movement of this character.
 *
 * This is the result of input times dude force.
 *
 * @param value left/right movement of this character.
 */
void EntityModel::setMovement(Vec2 movement) {
    _movement = movement;
}

void EntityModel::updateSprite(float dt, bool useMovement) {
    EntityFacing face;
    if (_state != DASHING) {
        face = calculateFacing(useMovement ? _movement : getLinearVelocity());
    }
    else {
        face = calculateFacing(_dashVector);
    }
    if (!((_prevState == _state) && (_facing == face))) {
        
        auto sprite = _southWalkSprite;
        switch (_state) {
            case STUNNED:
                //TODO: stunned animation?
            case STANDING:
                // TODO: Idle animations here
            case SNEAKING:
            case WALKING:
                if (face == SOUTH) {
                    sprite = _southWalkSprite;
                }
                else if (face == NORTH) {
                    sprite = _northWalkSprite;
                }
                else if (face == EAST || face == WEST) {
                    sprite = _eastWalkSprite;
                }
                else if (face == NORTHEAST || face == NORTHWEST) {
                    sprite = _northEastWalkSprite;
                }
                else if (face == SOUTHEAST || face == SOUTHWEST) {
                    sprite = _southEastWalkSprite;
                }
                if (sprite->isFlipHorizontal() == (face == EAST || face == NORTHEAST || face == SOUTHEAST)) {
                    sprite->flipHorizontal(!sprite->isFlipHorizontal());
                }
                break;
            case RUNNING:
                if (face == SOUTH) {
                    sprite = _southRunSprite;
                }
                else if (face == NORTH) {
                    sprite = _northRunSprite;
                }
                else if (face == EAST || face == WEST) {
                    sprite = _eastRunSprite;
                }
                else if (face == NORTHEAST || face == NORTHWEST) {
                    sprite = _northEastRunSprite;
                }
                else if (face == SOUTHEAST || face == SOUTHWEST) {
                    sprite = _southEastRunSprite;
                }
                if (sprite->isFlipHorizontal() == (face == EAST || face == NORTHEAST || face == SOUTHEAST)) {
                    sprite->flipHorizontal(!sprite->isFlipHorizontal());
                }
                break;
            case DASHING:
                if (face == SOUTH) {
                    sprite = _southDashSprite;
                }
                else if (face == NORTH) {
                    sprite = _northDashSprite;
                }
                else if (face == EAST || face == WEST) {
                    sprite = _eastDashSprite;
                }
                else if (face == NORTHEAST || face == NORTHWEST) {
                    sprite = _northEastDashSprite;
                }
                else if (face == SOUTHEAST || face == SOUTHWEST) {
                    sprite = _southEastDashSprite;
                }
                if (sprite->isFlipHorizontal() == (face == EAST || face == NORTHEAST || face == SOUTHEAST)) {
                    sprite->flipHorizontal(!sprite->isFlipHorizontal());
                }
                break;
            case CARRYING:
            case CAUGHT:
            case ROOTING:
            case UNROOTING:
            case ROOTED:
                break;
        }
        setSceneNode(sprite);
        _facing = face;
    }
    stepAnimation(dt);
}

void EntityModel::setDashInput(bool dashInput, cugl::Vec2 dashVector) {
    _dashInput = dashInput;
    if (_state != DASHING) {
        // For now, only update dashVector when we're not already DASHING
        // So that if we are DASHING, we maintain the same dash vector for applying force -CJ
        _dashVector = dashVector;
    }
    
}

void EntityModel::setPlantInput(bool plantInput) {
    _plantInput = plantInput;
}

void EntityModel::setRootInput(bool rootInput) {
    _rootInput = rootInput;
}

void EntityModel::setUnrootInput(bool unrootInput) {
    _unrootInput = unrootInput;
}


#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void EntityModel::createFixtures() {
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
void EntityModel::releaseFixtures() {
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
void EntityModel::dispose() {
    _node = nullptr;
    _wheatHeightNode = nullptr;
    _geometry = nullptr;
    
    _northWalkSprite = nullptr;
    _northEastWalkSprite = nullptr;
    _eastWalkSprite = nullptr;
    _southEastWalkSprite = nullptr;
    _southWalkSprite = nullptr;
}

/**
 *  Steps the state machine of this EntityModel.
 *
 *  This method should be called after all relevant input attributes are set.
 */
void EntityModel::updateState(float dt) {
    if (!isEnabled()) {
        return;
    }
    
    if (_dashCooldown > 0) {
        _dashCooldown = std::max(0.0f, _dashCooldown - dt);
    }
    
    _prevState = _state;
    bool stateChanged = false;
    EntityState nextState = _state;
    
    switch (_state) {
        case STUNNED: {
            _stunTime += dt;
            // Stunned -> Moving
            if (_stunTime > STUN_SECS) {
                _state = getMovementState();
                stateChanged = true;
            }
            break;
        }
        case STANDING: {
            // Standing -> Moving
            nextState = getMovementState();
            stateChanged = (nextState != _state);
            _state = nextState;
            break;
        }
        case SNEAKING:
        case WALKING:
        case RUNNING: {
            // Moving -> Dashing
            if (_dashCooldown == 0 && dashTimer == 0 && _dashInput) {
                _state = DASHING;
                dashTimer = 8;
                _dashCooldown = DASH_COOLDOWN_SECS;
                stateChanged = true;
            }
            else {
                nextState = getMovementState();
                stateChanged = (nextState != _state);
                _state = nextState;
            }
            break;
        }
        case DASHING: {
            // Dashing -> Moving
            dashTimer--;
            if (dashTimer == 0) {
                _state = getMovementState();
                stateChanged = true;
            }
            break;
        }
        default: {
            CULog("updateState: Not implemented yet");
        }
    }
    
    if (stateChanged) {
        updateCurAnimDurationForState();
    }
    updateSprite(dt);
//    std::cout << _state << "\n";
}

void EntityModel::stun() {
    _state = STUNNED;
    _stunTime = 0;
}

/**
 * Applies the force to the body of this dude
 *
 * This method should be called after the force attribute is set.
 */
void EntityModel::applyForce() {
    if (!isEnabled()) {
        return;
    }
    
    Vec2 speed;
    Vec2 normMovement = getMovement().getNormalization();
    
    switch (_state) {
        case STUNNED:
        case STANDING: {
            setLinearVelocity(Vec2::ZERO);
            break;
        }
        case SNEAKING:
            speed.set(normMovement).scale(SNEAK_SPEED);
            setLinearVelocity(speed);
            break;
        case WALKING:
            speed.set(normMovement).scale(WALK_SPEED);
            setLinearVelocity(speed);
            break;
        case RUNNING: {
            speed.set(normMovement).scale(RUN_SPEED);
            setLinearVelocity(speed);
            break;
        }
        case DASHING: {
            setLinearVelocity(Vec2::normalize(_dashVector, &speed)->scale(DUDE_DASH));
            break;
        }
        default: {
            CULog("State not implemented yet");
        }
    }
    
    // Don't want to be moving. Damp out player motion
    
}

bool EntityModel::isDashing() {
    return _state == DASHING;
}

cugl::Vec2 EntityModel::getFacing() {
    switch (_facing) {
        case EAST:
            return Vec2(1, 0);
        case NORTHEAST:
            return Vec2(1, 1);
        case NORTH:
            return Vec2(0, 1);
        case NORTHWEST:
            return Vec2(-1, 1);
        case WEST:
            return Vec2(-1, 0);
        case SOUTHWEST:
            return Vec2(-1, -1);
        case SOUTH:
            return Vec2(0, -1);
        case SOUTHEAST:
            return Vec2(1, -1);
        default:
            return Vec2();
            break;
    }
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void EntityModel::update(float dt) {
    BoxObstacle::update(dt);
    
    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
//        if (isInWheat() && (int(_node->getPriority()) == 4)) {
//            _node->setColor(Color4(255, 255, 255, 255.0/2));
//        }
//        else {
//            _node->setColor(Color4(255, 255, 255, 255));
//        }
    }
    
    if (_wheatHeightNode != nullptr) {
        updateWheatHeightNode();
    }
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
void EntityModel::resetDebug() {
    BoxObstacle::resetDebug();
}

std::shared_ptr<cugl::scene2::SceneNode> EntityModel::allocWheatHeightNode() {
    pf = PolyFactory(0.01);
    _wheatHeightTarget = 0.0;
    _wheatSizeTarget = 0.75;
    _currWheatHeight = _wheatHeightTarget;
    _currWheatSize = _wheatSizeTarget;
    _wheatHeightNode = scene2::PolygonNode::allocWithPoly(pf.makeEllipse(Vec2(0,0), _wheatSizeTarget * Size(0.8, 0.8)));
    _wheatHeightNode->setColor(Color4(0, 0, 0, 255));
    _wheatHeightNode->setBlendFunc(GL_DST_ALPHA, GL_ZERO, GL_ONE, GL_ONE);
    _wheatHeightNode->setAnchor(Vec2::ANCHOR_CENTER);
    _wheatHeightNode->setPosition(getX(), getY()-getHeight());
    return _wheatHeightNode;
}

std::shared_ptr<cugl::scene2::SceneNode> EntityModel::allocWheatHeightNode(std::shared_ptr<cugl::Texture> &rustle) {
    pf = PolyFactory(0.01);
    _wheatHeightTarget = 0.0;
    _wheatSizeTarget = 0.75;
    _currWheatHeight = _wheatHeightTarget;
    _currWheatSize = _wheatSizeTarget;
    _wheatHeightNode = scene2::PolygonNode::allocWithTexture(rustle);
//    _wheatHeightNode->setColor(Color4(0, 0, 0, 255));
    _wheatHeightNode->setBlendFunc(GL_DST_ALPHA, GL_ZERO, GL_ONE, GL_ONE);
    _wheatHeightNode->setAnchor(Vec2::ANCHOR_CENTER);
    _wheatHeightNode->setPosition(getX(), getY()-getHeight());
    return _wheatHeightNode;
}


void EntityModel::updateWheatHeightNode() {
    _wheatHeightNode->setPosition(getX(), getY()-getHeight());
    
    Vec2 velocity = getLinearVelocity();
    float angle = atan2(velocity.y, velocity.x);
    
    if (_state == DASHING) {
        _wheatSizeTarget = 1.5;
        _wheatHeightTarget = -100;
    } else {
        _wheatSizeTarget = 0.75;
        _wheatHeightTarget = round(velocity.length());
    }
    _currWheatHeight += (_wheatHeightTarget - _currWheatHeight) * 0.1;
    _currWheatSize += (_wheatSizeTarget - _currWheatSize) * 0.1;
    
    _wheatHeightNode->setPolygon(pf.makeEllipse(Vec2(0,0), _currWheatSize * Size(1.6, 0.9)));
    _wheatHeightNode->setColor(Color4(0,_currWheatHeight > 0 ? int(_currWheatHeight) : 0,
                                      _currWheatHeight < 0 ? -int(_currWheatHeight) : 0,255));
}


