//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_MAP_H
#define ROOTED_MAP_H

#include <cugl/cugl.h>
#include <any>

#include "BabyCarrot.h"
#include "Carrot.h"
#include "Farmer.h"
#include "Wheat.h"
#include "PlantingSpot.h"
#include "../shaders/EntitiesNode.h"
#include "../shaders/ShaderNode.h"
#include "../shaders/WheatRenderer.h"

class Map {
private:
    /** reference to this game's player's active EntityModel */
    std::shared_ptr<EntityModel> _character;
    /** references to the baby carrots */
    std::vector<std::shared_ptr<BabyCarrot>> _babies;
    /** references to the carrots */
    std::vector<std::shared_ptr<Carrot>> _carrots;
    /** references to the farmer */
    std::vector<std::shared_ptr<Farmer>> _farmers;
    /** references to the boundaries around the world **/
    std::vector<std::shared_ptr<cugl::physics2::BoxObstacle>> _boundaries;
    /** references to the planting spots */
    std::vector<std::shared_ptr<PlantingSpot>> _plantingSpot;
    /** references to the walls */
    std::vector<std::shared_ptr<physics2::PolygonObstacle>> _walls;
    /** reference to the box2d world */
    std::shared_ptr<cugl::physics2::net::NetWorld> _world;
    /** The root node of this level */
    std::shared_ptr<scene2::SceneNode> _root;
    /** The bounds of this level in physics coordinates */
    Rect _bounds;
    /** The global gravity for this level */
    Vec2 _gravity;
    /** The scale between the physics world and the screen */
    Vec2 _scale;
    /** Reference to map json */
    std::shared_ptr<JsonValue> _json;
    /** The AssetManager for the game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _debugnode;
    /** Reference to the wheat node of the scene graph */
    std::shared_ptr<ShaderNode> _wheatnode;
    /** Reference to the ground node of the scene graph */
    std::shared_ptr<ShaderNode> _groundnode;
    
    std::shared_ptr<EntitiesNode> _entitiesNode;

    std::unordered_map<std::string, std::any> _propertiesMap;

    std::shared_ptr<WheatRenderer> _wheatrenderer;

    /** Possible init positions of carrots */
    std::vector<float> _carrotPosList;
    
public:
    /**
     * Enum representing the draw order of scene nodes. Nodes will be drawn in the order they are listed in the enum.
     */
    enum class DrawOrder : int {
        GROUND,
        PLANTINGSPOT,
        WHEAT,
        PLAYER,
        ENTITIES,
        WALLS
    };

#pragma mark -
#pragma mark Constructors and Destructors

    Map(void);

    ~Map(void);

    /**
     * Creates a new Map with the given source file.
     *
     * This method does NOT load the level. You must call the load() method to do that.
     * This method returns false if file does not exist.
     *
     * @return  an autoreleased level file
     */
    static std::shared_ptr<Map> alloc(const std::shared_ptr<AssetManager> &assets,
                                      const std::shared_ptr<scene2::SceneNode> &root,
                                      const std::shared_ptr<cugl::JsonValue> &json) {
        std::shared_ptr<Map> result = std::make_shared<Map>();
        return (result->init(assets, root, json) ? result : nullptr);
    }

    bool init(const std::shared_ptr<AssetManager> &assets,
              const std::shared_ptr<scene2::SceneNode> &root,
              const std::shared_ptr<cugl::JsonValue> &json);

    void populate();
    
    /**
     * populate the map with Carrots
     */
    void populateWithCarrots(int numCarrots);
    
    /**
     * Adds a carrot to the game (adds to the carrot vector)
     */
    void spawnCarrot(const Vec2 position, float width, float height);

#pragma mark Physics Attributes

    /**
     * Returns the bounds of this level in physics coordinates
     *
     * @return the bounds of this level in physics coordinates
     */
    const Rect &getBounds() const { return _bounds; }

#pragma mark Drawing Methods

    /**
     * Clears the root scene graph node for this level
     */
    void clearRootNode();
    
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
     * Unloads this game level, releasing all sources
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded and
     * unloaded in parallel, not in sequence.  If an asset (like a game level) has
     * references to other assets, then these should be disconnected earlier.
     */
    void dispose();

    /**
     * Loads the players as a bunny farmer or carrot depending on whether they are the host or not.
     */
    std::shared_ptr<EntityModel>
    loadPlayerEntities(std::vector<std::string> players, std::string hostUUID,
                       std::string thisUUID);

    std::vector<std::shared_ptr<EntityModel>> loadBabyEntities();

    void acquireMapOwnership();

#pragma mark -
#pragma mark Getters and Setters

    std::vector<std::shared_ptr<BabyCarrot>> &getBabyCarrots() { return _babies; }

    std::vector<std::shared_ptr<Carrot>> &getCarrots() { return _carrots; }

    std::vector<std::shared_ptr<Farmer>> &getFarmers() { return _farmers; }

    std::shared_ptr<EntityModel> &getCharacter() { return _character; }

    std::vector<std::shared_ptr<PlantingSpot>> &getPlantingSpots() { return _plantingSpot; }

    std::shared_ptr<cugl::physics2::net::NetWorld> getWorld() { return _world; }

#pragma mark -
#pragma mark Drawing
    void updateShaders(float step, Mat4 perspective, Vec2 camPos, float camZoom);

private:
#pragma mark -
#pragma mark Internal Helper Methods
    
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
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle> &obj, const std::shared_ptr<cugl::scene2::SceneNode> &node);
    
    /**
     * Reads the JSON of an object in a Tiled JSON, and puts it into _propertiesMap.
     *
     * @param json  JSON of the object
     * @param tileSize   (Pixel) size of the Tiled tiles in the JSON
     * @param levelHeight   Height of the map in physics coordinates
     */
    bool readProperties(const std::shared_ptr<cugl::JsonValue> &json, int tileSize, int levelHeight);

    /**
     * Loads a single planting spot of variable size into the world.
     */
    void loadPlantingSpot(float x, float y, float width, float height);

    /**
     * Loads and builds the shaders for a specific map texture, and adds the shader nodes to the world node. This method should only be called once per initialization, any subsequent calls will override previous calls.
     */
    void loadWheat();
    
    /**
     * Loads a single farmer into the world.
     */
    void loadFarmer(float x, float y, float width, float height);
    
    /**
     * Loads a single baby carrot into the world.
     */
    void loadBabyCarrot(float x, float y, float width, float height);

    /**
     * Loads a single carrot into the world.
     */
    void loadCarrot(float x, float y, float width, float height);
    
    /**
     * Adds a boundary box obstacle to the world.
     */
    void loadBoundary(Vec2 pos, Size size);

};

#endif //ROOTED_MAP_H
