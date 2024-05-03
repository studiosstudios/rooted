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
    
    bool _shouldAnimate;
        
protected:
    /** The scene graph node for the planting spot. */
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    
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
    
    /**
     * Sets the scene graph node representing this planting spot.
     *
     * @param node  The scene graph node representing this planting spot, which has
     *              been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::AssetManager>& assets, float priority, std::string texkey);
    
    /**
     * Returns the scene graph node representing this PlantingSpot.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this PlantingSpot.
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const { return _node; }
    
private:
    void addTileNode(Vec2 position, float texscale, const std::shared_ptr<cugl::AssetManager> &assets, std::string texkey);

};


#endif /* Decoration_h */
