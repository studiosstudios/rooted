//
//  PlantingSpot.hpp
//  Rooted
//
//  Created by Kimmy Lin on 3/11/24.
//

#ifndef PlantingSpot_h
#define PlantingSpot_h

#include <cugl/cugl.h>

using namespace cugl;

class PlantingSpot : public cugl::physics2::BoxObstacle {
private:
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    /** The scene graph node for the planting spot. */
    std::shared_ptr<cugl::scene2::PolygonNode> _node;
public:
    /**
     * Creates a degenerate planting spot object.
     *
     * This constructor does not initialize any of the planting spot  values beyond
     * the defaults.  To use a planting spot, you must call init().
     */
    PlantingSpot() : BoxObstacle() { }
    
    /**
     * Destroys this PlantingSpot, releasing all resources.
     */
    virtual ~PlantingSpot(void) { dispose(); }
    
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
    static std::shared_ptr<PlantingSpot> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<PlantingSpot> result = std::make_shared<PlantingSpot>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    /**
     * Sets the scene graph node representing this planting spot.
     *
     * @param node  The scene graph node representing this planting spot, which has
     *              been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::PolygonNode>& node) {
        _node = node;
    }
    
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

};
#endif /* PlantingSpot_h */
