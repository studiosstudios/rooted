//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef __ROOTED_WHEAT_H__
#define __ROOTED_WHEAT_H__
#include <cugl/cugl.h>

using namespace cugl;

#pragma mark -
#pragma mark Drawing Constants
/** The texture for the character avatar */
#define WHEAT_TEXTURE       "wheat"
#define WHEAT_FRAMES     7
#define WHEAT_ANIMATION_SPEED     0.5f

#define RECOVERY            0.01f
#define SNEAK_TRANSPARENCY  1
#define WALK_INTENSITY      3
#define WALK_TRANSPARENCY   0.8f
#define DASH_INTENSITY      5
#define DASH_TRANSPARENCY   0.55f


class Wheat : public cugl::physics2::BoxObstacle {

private:
    /** The current color of the Wheat */
    Color4 _color;
    /** Transparency when rustling */
    float _fadeout;
    /** The number of shakes that have happened */
    int _numShakes;
    /** Whether it is shaking */
    int _isShaking;
    /** The scene graph node for the Wheat. */
    std::shared_ptr<cugl::scene2::SpriteNode> _node;
    /** The current animation frame */
    float _animframe;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CU_DISALLOW_COPY_AND_ASSIGN(Wheat);
    
public:
#pragma mark Constructors
    /**
     * Creates a degenerate Wheat object.
     *
     * This constructor does not initialize any of the Wheat values beyond
     * the defaults.  To use a Wheat, you must call init().
     */
    Wheat() : BoxObstacle() { }
    
    /**
     * Destroys this Wheat, releasing all resources.
     */
    virtual ~Wheat(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this Wehat
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a Wheat may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Standard constructor
     */
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);
        
#pragma mark -
#pragma mark Static Constructors
    /**
     * Creates a new Wheat at the given position.
     *
     * The Wheat is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     *
     * @return  A newly allocated Wheat at the given position, with the given radius
     */
    static std::shared_ptr<Wheat> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Wheat> result = std::make_shared<Wheat>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
            
#pragma mark Animation
    /**
     * Returns the scene graph node representing this Wheat.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this Wheat.
     */
    const std::shared_ptr<cugl::scene2::SpriteNode>& getSceneNode() const { return _node; }
    
    /**
     * Sets the scene graph node representing this Wheat.
     *
     * @param node  The scene graph node representing this Wheat, which has
     *              been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SpriteNode>& node) {
        _node = node;
    }
    
    /**
     * Animates wheat.
     *
     * If the animation is not active, it will reset to the initial animation frame.
     *
     */
    void animateWheat();
    
    /**
     * Sets the ratio of the Wheat sprite to the physics body
     *
     * The Wheat needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @param scale The ratio of the Wheat sprite to the physics body
     */
    void setDrawScale(float scale) {
        _drawScale = scale;
    }
    
#pragma mark -
#pragma mark Physics Methods
    /**
    * Updates the object's physics state (NOT GAME LOGIC).
    *
    * @param dt Number of seconds since last animation frame
    */
    void update(float dt) override;
    
    
#pragma mark -
#pragma mark Attribute Properties

    /** Rustle this wheat, changing different values depending on the input value
     *
     *  @param amount The amount to rustle, usually the velocity of the entity passing through the wheat
     */
    void rustle(float amount);
    
};

#endif //ROOTED_WHEAT_H
