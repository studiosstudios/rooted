//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_MAP_H
#define ROOTED_MAP_H

#include "BabyCarrot.h"
#include "Carrot.h"
#include "Farmer.h"
#include "Wheat.h"

class Map {
private:
    /** references to the baby carrots */
    std::vector<std::shared_ptr<BabyCarrot>> _babies;
    /** references to the carrots */
    std::vector<std::shared_ptr<Carrot>> _carrots;
    /** references to the farmer */
    std::vector<std::shared_ptr<Farmer>> _farmers;
    /** references to the wheat */
    std::vector<std::shared_ptr<Wheat>> _wheat;
    /** reference to the box2d world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;


public:

#pragma mark -
#pragma mark Constructors and Destructors
    Map() {};

    ~Map() { dispose(); };

    /**
     * Disposes this Map, emptying all resources.
     */
    void dispose() {
        _carrots.clear();
        _farmers.clear();
        _babies.clear();
        _wheat.clear();
        _world = nullptr;
    }

    /**
     * Initializes the Map. The map contains and owns all game objects, and also has a reference to
     * the Box2D world, but DOES NOT own the world. The world is owned by the GameScene.
     *
     * @param assets        AssetManager for loading textures for the game objects
     * @param world         Box2D world
     * @param worldnode     Box2D world scene graph node
     * @param debugnode     Debug scene graph node
     * @param scale         The scale between the physics world and the screen (MUST BE UNIFORM)
     *
     * @return true if initialization succeeded
     */
    bool init(const std::shared_ptr<cugl::AssetManager> &assets,
              std::shared_ptr<cugl::physics2::ObstacleWorld> &world,
              const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
              const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
              float scale);

    /**
     * Creates a Map. The map contains and owns all game objects, and also has a reference to
     * the Box2D world, but DOES NOT own the world. The world is owned by the GameScene.
     *
     * @param assets        AssetManager for loading textures for the game objects
     * @param world         Box2D world
     * @param worldnode     Box2D world scene graph node
     * @param debugnode     Debug scene graph node
     * @param scale         The scale between the physics world and the screen (MUST BE UNIFORM)
     *
     * @return newly allocated Map
     */
    static std::shared_ptr<Map> alloc(const std::shared_ptr<cugl::AssetManager> assets,
                                      std::shared_ptr<cugl::physics2::ObstacleWorld> world,
                                      const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
                                      const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
                                      float scale) {
        std::shared_ptr<Map> result = std::make_shared<Map>();
        return (result->init(assets, world, worldnode, debugnode, scale) ? result : nullptr);
    }


    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with dude.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * @param obj   The physics object to add
     * @param node  The scene graph node to attach it to
     * @param worldnode  The box2d world scene graph node
     * @param debugnode  The debug scene graph node
     * @param useObjPosition  Whether to update the node's position to be at the object's position
     */
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle> &obj,
                     const std::shared_ptr<cugl::scene2::SceneNode> &node,
                     const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
                     const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
                     bool useObjPosition = true);

#pragma mark -
#pragma mark Getters and Setters

    std::vector<std::shared_ptr<BabyCarrot>> getBabyCarrots() { return _babies; }

    std::vector<std::shared_ptr<Carrot>> getCarrots() { return _carrots; }

    std::vector<std::shared_ptr<Farmer>> getFarmers() { return _farmers; }

    std::vector<std::shared_ptr<Wheat>> getWheat() { return _wheat; }

    std::shared_ptr<cugl::physics2::ObstacleWorld> getWorld() { return _world; }

};


#endif //ROOTED_MAP_H
