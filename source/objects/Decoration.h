//
//  Decoration.h
//  Rooted
//
//  Created by Nicole Thean on 4/29/24.
//

#ifndef Decoration_h
#define Decoration_h

#include <cugl/cugl.h>

#pragma mark -
#pragma mark Drawing Constants
/** The texture for the character avatar */
#define BARN_TEXTURE    "barn"
#define TRACTOR_TEXTURE   "tractor"
#define MILL_TEXTURE   "mill"

using namespace cugl;

class Decoration : public cugl::physics2::BoxObstacle {
private:
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    
    Size _size;
    
    bool _shouldAnimate = false;
    
    /** The time it takes for the currently active animation to complete 1 cycle (in seconds) */
    float curAnimDuration = 1.5f;
    
    /** The amount of time that has elapsed in the current animation cycle
        For example, if the player is in a walking animation cycle that is 1.5 seconds long, and this field is 0.7 seconds, then the animation is roughly at its middle frame */
    float curAnimTime = 0.0f;
        
protected:
    std::shared_ptr<cugl::scene2::SpriteNode> _node;
    std::shared_ptr<cugl::scene2::SpriteNode> _decorationSprite;
    
public:
    /**
     * Creates a degenerate obstacle object.
     */
    Decoration() : BoxObstacle() { }
    
    /**
     * Destroys this PlantingSpot, releasing all resources.
     */
    virtual ~Decoration(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this planting spot
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a planting spot may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Standard constructor
     */
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);
    
    /**
     * Creates a new planting spot at the given position.
     *
     * The planting spot is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     *
     * @return  A newly allocated planting spot at the given position, with the given radius
     */
    static std::shared_ptr<Decoration> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Decoration> result = std::make_shared<Decoration>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    void setShouldAnimate(bool anim) { _shouldAnimate = anim; }
    
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
     * Sets all of the sprite nodes associated with this decoration
     *
     */
    void setSpriteNodes(const std::shared_ptr<cugl::scene2::SpriteNode>& decSprite) {
        _decorationSprite = decSprite;
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
     * Steps the current sprite's animation by dt.
     *
     * This steps the current sprite node associated with this EntityModel by incrementing curAnimTime by dt and comparing it with curAnimDuration
     */
    void stepAnimation(float dt);
    
    /**
     * Returns the scene graph node representing this PlantingSpot.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this PlantingSpot.
     */
    const std::shared_ptr<cugl::scene2::SpriteNode>& getSceneNode() const { return _node; }
    
};


#endif /* Decoration_h */
