//
//  Collectible.h
//  Rooted
//
//  Created by Miranda Luo on 4/19/24.
//

#ifndef Collectible_h
#define Collectible_h

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

class Collectible : public cugl::physics2::BoxObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CU_DISALLOW_COPY_AND_ASSIGN(Collectible);
    
    /** How long the collectible should last */
    float _age;

protected:
    /** The scene graph node for the Collectible. */
    std::shared_ptr<cugl::scene2::SpriteNode> _node;
    
    /** The scale between the physics world and the screen */
    float _drawScale;
   
    /**
    * Redraws the outline of the physics fixtures to the debug node
    *
    * The debug node is use to outline the fixtures attached to this object.
    * This is very useful when the fixtures have a very different shape than
    * the texture (e.g. a circular shape attached to a square texture).
    */
    void resetDebug() override;
    
    /** The current movement (horizontal and vertical) of the object */
    cugl::Vec2 _movement;
    
    cugl::Vec2 _dashCache;
    /** Polygon factory for the wheat node. */
    cugl::PolyFactory pf;
    /** The wheat height adjustment node in the wheat scene from this entity's velocity */
    std::shared_ptr<cugl::scene2::PolygonNode> _wheatHeightNode;
    /** Target height for wheat node. This is very temporary */
    float _wheatHeightTarget;
    /** Current rendered height for wheat node. This is very temporary */
    float _currWheatHeight;
    /** Target size for wheat node. This is very temporary */
    float _wheatSizeTarget;
    /** Current rendered height for wheat node. This is very temporary */
    float _currWheatSize;
    /** Query id for in wheat query from wheat scene */
    unsigned int _wheatQueryId;
    /** If the middle bottom pixel of the hitbox of this entity model is in wheat */
    bool _inWheat;
    
    int _wheatContacts;
    
    float _initCharacterHeight;

public:
    
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a DudeModel, you must call init().
     */
    Collectible() : BoxObstacle() { }
    
    /**
     * Destroys this DudeModel, releasing all resources.
     */
    virtual ~Collectible(void) { dispose(); }
    
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
    static std::shared_ptr<Collectible> alloc() {
        std::shared_ptr<Collectible> result = std::make_shared<Collectible>();
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
    static std::shared_ptr<Collectible> alloc(const cugl::Vec2& pos) {
        std::shared_ptr<Collectible> result = std::make_shared<Collectible>();
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
    static std::shared_ptr<Collectible> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
        std::shared_ptr<Collectible> result = std::make_shared<Collectible>();
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
    static std::shared_ptr<Collectible> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Collectible> result = std::make_shared<Collectible>();
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
    const std::shared_ptr<cugl::scene2::SpriteNode>& getSceneNode() const { return _node; }

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
    void setSceneNode(const std::shared_ptr<cugl::scene2::SpriteNode>& node) {
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
    
    /**
     * Updates the current value in curAnimDuration with the current EntityModel's state value
     *
     * Virtual, should be implemented by all derived classes with respect to their specific animation durations.
     */
    void updateCurAnimDurationForState() {};
    
    /**
     * Returns whether the current EntityModel's state is one where the animation should be cycling
     *
     * Examples of states where this would return false is if the player is currently not moving or DASHING or ROOTED.
     */
    bool animationShouldStep();
    
    /**
     * Steps the current sprite's animation by dt.
     *
     * This steps the current sprite node associated with this EntityModel by incrementing curAnimTime by dt and comparing it with curAnimDuration
     */
    void stepAnimation(float dt);
    
#pragma mark -
#pragma mark Attribute Properties
//    /**
//     * Returns how much force to apply to get the dude moving
//     *
//     * Multiply this by the input to get the movement value.
//     *
//     * @return how much force to apply to get the dude moving
//     */
//    float getForce() const { return DUDE_FORCE; }
//    
//    /**
//     * Returns ow hard the brakes are applied to get a dude to stop moving
//     *
//     * @return ow hard the brakes are applied to get a dude to stop moving
//     */
//    float getDamping() const { return DUDE_DAMPING; }
//    
//    /**
//     * Returns the upper limit on dude left-right movement.
//     *
//     * This does NOT apply to vertical movement.
//     *
//     * @return the upper limit on dude left-right movement.
//     */
//    float getMaxSpeed() const { return DUDE_MAXSPEED; }
            
    bool isInWheat() const { return _wheatContacts > 0; }

    void changeWheatContacts(int dx) { _wheatContacts += dx; }
    
    float getAge() { return _age; }
    
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

    virtual std::shared_ptr<cugl::scene2::SceneNode> allocWheatHeightNode(float initCharHeight);

    virtual void updateWheatHeightNode();

    std::shared_ptr<cugl::scene2::SceneNode> getWheatHeightNode() { return _wheatHeightNode; };

    unsigned int getWheatQueryId() { return _wheatQueryId; };

    void setWheatQueryId(unsigned int id) { _wheatQueryId = id; };

    bool isInWheat() { return _inWheat; }

    void setInWheat(bool inWheat) { _inWheat = inWheat; }
};

#endif /* __PF_DUDE_MODEL_H__ */
