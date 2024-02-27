//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_MAP_H
#define ROOTED_MAP_H

#include "BabyCarrot.h"
#include "Carrot.h"
#include "Farmer.h"
#include "Wheat.h"

class Map : public Asset {
private:
    /** references to the baby carrots */
    std::vector<std::shared_ptr<BabyCarrot>> _babies;
    /** references to the carrots */
    std::vector<std::shared_ptr<Carrot>> _carrots;
    /** references to the farmer */
    std::vector<std::shared_ptr<Farmer>> _farmers;
    /** references to the wheat */
    std::vector<std::shared_ptr<Wheat>> _wheat;
    /** references to the walls */
    std::vector<std::shared_ptr<physics2::PolygonObstacle>> _walls;
    /** reference to the box2d world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** The root node of this level */
    std::shared_ptr<scene2::SceneNode> _root;
    /** The bounds of this level in physics coordinates */
    Rect _bounds;
    /** The global gravity for this level */
    Vec2 _gravity;
    /** The scale between the physics world and the screen */
    Vec2 _scale;
    /** The AssetManager for the game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _debugnode;


public:

#pragma mark -
#pragma mark Constructors and Destructors

    Map(void);

    virtual ~Map(void);

    /**
     * Creates a new Map with no source file.
     *
     * The source file can be set at any time via the setFile() method. This method
     * does NOT load the asset.  You must call the load() method to do that.
     *
     * @return  an autoreleased level file
     */
    static std::shared_ptr<Map> alloc() {
        std::shared_ptr<Map> result = std::make_shared<Map>();
        return (result->init("") ? result : nullptr);
    }

    /**
     * Creates a new Map with the given source file.
     *
     * This method does NOT load the level. You must call the load() method to do that.
     * This method returns false if file does not exist.
     *
     * @return  an autoreleased level file
     */
    static std::shared_ptr<Map> alloc(std::string file) {
        std::shared_ptr<Map> result = std::make_shared<Map>();
        return (result->init(file) ? result : nullptr);
    }


#pragma mark -
#pragma mark Internal Helper Methods

    /**
     * Clears the root scene graph node for this level
     */
    void clearRootNode();

    /**
     * Loads a single wall
     *
     * The crate will be retained and stored in the vector _walls.  If the
     * wall fails to load, then it will not be added to _walls.
     *
     * @param  reader   a JSON reader with cursor ready to read the wall
     *
     * @retain the wall
     * @return true if the crate was successfully loaded
     */
    bool loadWall(const std::shared_ptr<JsonValue>& json);

    /**
     * Loads a single farmer
     *
     * The farmer will be retained and stored in the vector _farmers.  If the
     * farmer fails to load, then it will not be added to _farmers.
     *
     * @param  reader   a JSON reader with cursor ready to read the farmer
     *
     * @retain the farmer
     * @return true if the crate was successfully loaded
     */
    bool loadFarmer(const std::shared_ptr<JsonValue>& json);

    /**
     * Loads a single carrot
     *
     * The carrots will be retained and stored in the vector _carrots.  If the
     * carrot fails to load, then it will not be added to _carrots.
     *
     * @param  reader   a JSON reader with cursor ready to read the carrot
     *
     * @retain the carrot
     * @return true if the carrot was successfully loaded
     */
    bool loadCarrot(const std::shared_ptr<JsonValue>& json);

    /**
     * Loads a single baby carrot
     *
     * The baby carrot will be retained and stored in the vector _babies.  If the
     * baby carrot fails to load, then it will not be added to _babies.
     *
     * @param  reader   a JSON reader with cursor ready to read the baby carrot
     *
     * @retain the baby carrot
     * @return true if the baby carrot was successfully loaded
     */
    bool loadBabyCarrot(const std::shared_ptr<JsonValue>& json);

    /**
     * Loads a single baby carrot
     *
     * The baby carrot will be retained and stored in the vector _babies.  If the
     * baby carrot fails to load, then it will not be added to _babies.
     *
     * @param  reader   a JSON reader with cursor ready to read the baby carrot
     *
     * @retain the baby carrot
     * @return true if the baby carrot was successfully loaded
     */
    bool loadWheat(const std::shared_ptr<JsonValue>& json);

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
     */
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle> &obj,
                     const std::shared_ptr<cugl::scene2::SceneNode> &node);

#pragma mark Physics Attributes

    /**
     * Returns the bounds of this level in physics coordinates
     *
     * @return the bounds of this level in physics coordinates
     */
    const Rect &getBounds() const { return _bounds; }

#pragma mark Drawing Methods

    /**
     * Returns the drawing scale for this game level
     *
     * The drawing scale is the number of pixels to draw before Box2D unit. Because
     * mass is a function of area in Box2D, we typically want the physics objects
     * to be small.  So we decouple that scale from the physics object.  However,
     * we must track the scale difference to communicate with the scene graph.
     *
     * We allow for the scaling factor to be non-uniform.
     *
     * @return the drawing scale for this game level
     */
    const Vec2 &getDrawScale() const { return _scale; }

    /**
     * Sets the drawing scale for this game level
     *
     * The drawing scale is the number of pixels to draw before Box2D unit. Because
     * mass is a function of area in Box2D, we typically want the physics objects
     * to be small.  So we decouple that scale from the physics object.  However,
     * we must track the scale difference to communicate with the scene graph.
     *
     * We allow for the scaling factor to be non-uniform.
     *
     * @param value  the drawing scale for this game level
     */
    void setDrawScale(float value);

    /**
     * Returns the scene graph node for drawing purposes.
     *
     * The scene graph is completely decoupled from the physics system.  The node
     * does not have to be the same size as the physics body. We only guarantee
     * that the node is positioned correctly according to the drawing scale.
     *
     * @return the scene graph node for drawing purposes.
     */
    const std::shared_ptr<scene2::SceneNode> &getRootNode() const { return _root; }

    /**
     * Sets the scene graph node for drawing purposes.
     *
     * The scene graph is completely decoupled from the physics system.  The node
     * does not have to be the same size as the physics body. We only guarantee
     * that the node is positioned correctly according to the drawing scale.
     *
     * @param value  the scene graph node for drawing purposes.
     *
     * @retain  a reference to this scene graph node
     * @release the previous scene graph node used by this object
     */
    void setRootNode(const std::shared_ptr<scene2::SceneNode> &root);

    /**
     * Sets the loaded assets for this game level
     *
     * @param assets the loaded assets for this game level
     */
    void setAssets(const std::shared_ptr<AssetManager> &assets) { _assets = assets; }

    /**
     * Toggles whether to show the debug layer of this game world.
     *
     * The debug layer displays wireframe outlines of the physics fixtures.
     *
     * @param  flag whether to show the debug layer of this game world
     */
    void showDebug(bool flag);

#pragma mark -
#pragma mark Asset Loading

    /**
     * Loads this game level from the source file
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded in
     * parallel, not in sequence.  If an asset (like a game level) has references to
     * other assets, then these should be connected later, during scene initialization.
     *
     * @param file the name of the source file to load from
     *
     * @return true if successfully loaded the asset from a file
     */
    virtual bool preload(const std::string file) override;


    /**
     * Loads this game level from a JsonValue containing all data from a source Json file.
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded in
     * parallel, not in sequence.  If an asset (like a game level) has references to
     * other assets, then these should be connected later, during scene initialization.
     *
     * @param json the json loaded from the source file to use when loading this game level
     *
     * @return true if successfully loaded the asset from the input JsonValue
     */
    virtual bool preload(const std::shared_ptr<cugl::JsonValue> &json) override;

    /**
     * Unloads this game level, releasing all sources
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded and
     * unloaded in parallel, not in sequence.  If an asset (like a game level) has
     * references to other assets, then these should be disconnected earlier.
     */
    void unload();


#pragma mark -
#pragma mark Getters and Setters

    std::vector<std::shared_ptr<BabyCarrot>> getBabyCarrots() { return _babies; }

    std::vector<std::shared_ptr<Carrot>> getCarrots() { return _carrots; }

    std::vector<std::shared_ptr<Farmer>> getFarmers() { return _farmers; }

    std::vector<std::shared_ptr<Wheat>> getWheat() { return _wheat; }

    std::shared_ptr<cugl::physics2::ObstacleWorld> getWorld() { return _world; }

    void rustleWheats(float amount);

};


#endif //ROOTED_MAP_H
