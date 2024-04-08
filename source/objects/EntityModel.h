//
//  PFDudeModel.h
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
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#ifndef __PF_DUDE_MODEL_H__
#define __PF_DUDE_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#pragma mark -
#pragma mark Drawing Constants
/** The texture for the character avatar */
#define DUDE_TEXTURE    "dude"
#define CARROT_TEXTURE   "carrot"
#define FARMER_TEXTURE   "farmer"
#define CARROTFARMER_TEXTURE "carrotfarmer"
#define FARMER_FRONT_WALK_SPRITE "farmer-front-walk"
#define FARMER_NORTH_WALK_SPRITE "farmer-north-walk"
#define FARMER_EAST_WALK_SPRITE "farmer-east-walk"
#define BABY_TEXTURE   "baby"

#pragma mark -
#pragma mark Physics Constants
/** The factor to multiply by the input */
#define DUDE_FORCE      20.0f
/** The amount to slow the character down */
#define DUDE_DAMPING    10.0f
/** The maximum character speed */
#define DUDE_MAXSPEED   5.0f


#pragma mark -
#pragma mark Dude Model
/**
* Player avatar for the plaform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/
class EntityModel : public cugl::physics2::BoxObstacle {
private:
	/** This macro disables the copy constructor (not allowed on physics objects) */
	CU_DISALLOW_COPY_AND_ASSIGN(EntityModel);

protected:
    enum EntityFacing {
        EAST,
        NORTHEAST,
        NORTH,
        NORTHWEST,
        WEST,
        SOUTHWEST,
        SOUTH,
        SOUTHEAST
    };
    
    const std::map<cugl::Vec2, EntityFacing> _facingMap = {
        { cugl::Vec2(15 * M_PI / 8, 17 * M_PI / 8), EAST }, // Adjustment for the overflow for the EAST direction TODO: See if this can be simplified?
        { cugl::Vec2(M_PI / 8, 3 * M_PI / 8), NORTHEAST },
        { cugl::Vec2(3 * M_PI / 8, 5 * M_PI / 8), NORTH },
        { cugl::Vec2(5 * M_PI / 8, 7 * M_PI / 8), NORTHWEST },
        { cugl::Vec2(7 * M_PI / 8, 9 * M_PI / 8), WEST },
        { cugl::Vec2(9 * M_PI / 8, 11 * M_PI / 8), SOUTHWEST },
        { cugl::Vec2(11 * M_PI / 8, 13 * M_PI / 8), SOUTH },
        { cugl::Vec2(13 * M_PI / 8, 15 * M_PI / 8), SOUTHEAST },
    };
    
	/** Which direction is the character facing */
    EntityFacing _facing;

	/** The scene graph node for the Dude. */
	std::shared_ptr<cugl::scene2::SceneNode> _node;
    
    std::shared_ptr<cugl::scene2::SpriteNode> _eastWalkSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _southWalkSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _northWalkSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _northEastWalkSprite;
    std::shared_ptr<cugl::scene2::SpriteNode> _southEastWalkSprite;
    
	/** The scale between the physics world and the screen */
	float _drawScale;

    std::string _uuid;
    
    int _wheatContacts;
    
    float animTime;
   
	/**
	* Redraws the outline of the physics fixtures to the debug node
	*
	* The debug node is use to outline the fixtures attached to this object.
	* This is very useful when the fixtures have a very different shape than
	* the texture (e.g. a circular shape attached to a square texture).
	*/
	virtual void resetDebug() override;
    
    /* VELOCITY-BASED, STATE-MACHINE MOVEMENT SYSTEM*/
    
    /** State that a rooted! player entity can be in. Some of these states are specific
        to only a certain type of character (ex. only a bunny can be PLANTING), so
        we need to enforce the corresponding invariants for which states an entity can
        be in. */
    enum EntityState {
        MOVING,
        DASHING,
        CARRYING,   // bunny only
        PLANTING,   // bunny only
        CAUGHT,     // carrot only
        ROOTED      // carrot only
    };
    
    /** Current EntityState that this entity is in. */
    EntityState _state;
    
    /** The current movement (horizontal and vertical) of the character */
    cugl::Vec2 _movement;
    
    bool _dashInput;
    
    bool _plantInput;
    
    bool _rootInput;
    
    bool _unrootInput;
    
    cugl::Vec2 _dashCache;
    

public:
    int dashTimer;

public:
    
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a DudeModel, you must call init().
     */
    EntityModel() : BoxObstacle() { }
    
    /**
     * Destroys this DudeModel, releasing all resources.
     */
    virtual ~EntityModel(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this DudeModel
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a DudeModel may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Initializes a new dude at the origin.
     *
     * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(cugl::Vec2::ZERO, cugl::Size(1,1), 1.0f); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude is unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos) override { return init(pos, cugl::Size(1,1), 1.0f); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos, const cugl::Size size) override {
        return init(pos, size, 1.0f);
    }
    
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
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Creates a new dude at the origin.
	 *
	 * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @return  A newly allocated DudeModel at the origin
	 */
	static std::shared_ptr<EntityModel> alloc() {
		std::shared_ptr<EntityModel> result = std::make_shared<EntityModel>();
		return (result->init() ? result : nullptr);
	}

	/**
	 * Creates a new dude at the given position.
	 *
	 * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
     * @param pos   Initial position in world coordinates
	 *
	 * @return  A newly allocated DudeModel at the given position
	 */
	static std::shared_ptr<EntityModel> alloc(const cugl::Vec2& pos) {
		std::shared_ptr<EntityModel> result = std::make_shared<EntityModel>();
		return (result->init(pos) ? result : nullptr);
	}

    /**
	 * Creates a new dude at the given position.
	 *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
	 *
 	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
	 *
	 * @return  A newly allocated DudeModel at the given position with the given scale
	 */
	static std::shared_ptr<EntityModel> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<EntityModel> result = std::make_shared<EntityModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

	/**
	 * Creates a new dude at the given position.
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
	 * @return  A newly allocated DudeModel at the given position with the given scale
	 */
	static std::shared_ptr<EntityModel> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
		std::shared_ptr<EntityModel> result = std::make_shared<EntityModel>();
		return (result->init(pos, size, scale) ? result : nullptr);
	}
    

#pragma mark -
#pragma mark Animation
    /**
     * Returns the scene graph node representing this DudeModel.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this DudeModel.
     */
	const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const { return _node; }

    /**
     * Sets the scene graph node representing this DudeModel.
     *
     * Note that this method also handles creating the nodes for the body parts
     * of this DudeModel. Since the obstacles are decoupled from the scene graph,
     * initialization (which creates the obstacles) occurs prior to the call to
     * this method. Therefore, to be drawn to the screen, the nodes of the attached
     * bodies must be added here.
     *
     * The bubbler also uses the world node when adding bubbles to the scene, so
     * the input node must be added to the world BEFORE this method is called.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this DudeModel, which has been added to the world node already.
     */
	void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node) {
        if (_node != nullptr) {
            _node->setVisible(false);
        }
        _node = node;
        _node->setVisible(true);
        _node->setPosition(getPosition() * _drawScale);
    }

    /**
     * Sets the ratio of the Dude sprite to the physics body
     *
     * The Dude needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @param scale The ratio of the Dude sprite to the physics body
     */
    void setDrawScale(float scale) { _drawScale = scale; };
    
    bool animationShouldStep();
    
    void setSpriteNodes(const std::shared_ptr<cugl::scene2::SpriteNode>& northNode,
                        const std::shared_ptr<cugl::scene2::SpriteNode>& northEastNode,
                        const std::shared_ptr<cugl::scene2::SpriteNode>& eastNode,
                        const std::shared_ptr<cugl::scene2::SpriteNode>& southEastNode,
                        const std::shared_ptr<cugl::scene2::SpriteNode>& southNode) {
        _northWalkSprite = northNode;
        _northEastWalkSprite = northEastNode;
        _eastWalkSprite = eastNode;
        _southEastWalkSprite = southEastNode;
        _southWalkSprite = southNode;
    }
    
    void stepAnimation(float dt);
    
    EntityFacing calculateFacing(cugl::Vec2 movement);

    
#pragma mark -
#pragma mark Attribute Properties
    /**
     * Returns movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @return movement of this character.
     */
    cugl::Vec2 getMovement() const { return _movement; }
    
    /**
     * Sets left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @param value left/right movement of this character.
     */
    void setMovement(cugl::Vec2 value);
    
    void setDashInput(bool dashInput);
    
    void setPlantInput(bool plantInput);
    
    void setRootInput(bool rootInput);
    
    void setUnrootInput(bool unrootInput);
    
    /**
     * Returns how much force to apply to get the dude moving
     *
     * Multiply this by the input to get the movement value.
     *
     * @return how much force to apply to get the dude moving
     */
    float getForce() const { return DUDE_FORCE; }
    
    /**
     * Returns ow hard the brakes are applied to get a dude to stop moving
     *
     * @return ow hard the brakes are applied to get a dude to stop moving
     */
    float getDamping() const { return DUDE_DAMPING; }
    
    /**
     * Returns the upper limit on dude left-right movement.
     *
     * This does NOT apply to vertical movement.
     *
     * @return the upper limit on dude left-right movement.
     */
    float getMaxSpeed() const { return DUDE_MAXSPEED; }
    
    /**
     * Returns true if this character is facing right
     *
     * @return true if this character is facing right
     */
    bool getFacing() const { return _facing; }
    
    std::string getUUID() const { return _uuid; }
    
    void setUUID(std::string uuid) { _uuid = uuid; }
            
    bool isInWheat() const { return _wheatContacts > 0; }

    void changeWheatContacts(int dx) { _wheatContacts += dx; }

    
#pragma mark -
#pragma mark Physics Methods
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() override;
    
    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    void update(float dt) override;
    
    
    /**
     *  Steps the state machine of this EntityModel.
     *
     *  This method should be called after all relevant input attributes are set.
     */
    virtual void updateState();
    
    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();

    bool isDashing();
};

#endif /* __PF_DUDE_MODEL_H__ */
