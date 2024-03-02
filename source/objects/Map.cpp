//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Map.h"
#include "../objects/EntityModel.h"

// TODO: put all constants into JSON

#pragma mark -
#pragma mark Level Geography
// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units

/** The initial position of the dude */
float DUDE_POS[2] = {2.5f, 2.5f};

float BABY_CARROT_POS[2] = {2.5f, 10.0f};

#pragma mark -
#pragma mark Physics Constants
/** The density for most physics objects */
#define BASIC_DENSITY   0.0f
/** Friction of most platforms */
#define BASIC_FRICTION  0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION   0.1f

#pragma mark -
#pragma mark Asset Constants
# define WHEAT_TEXTURE  "wheat"
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"
float CARROT_SIZE[2] = {1.0f, 1.0f}; //TODO: make json constants fil

float WHEAT_SIZE[2] = {1.0f, 1.0f};

float FARMER_SIZE[2] = {1.0f, 1.0f};

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::YELLOW

using namespace cugl;

/**
* Creates a new, empty Map.
*/
Map::Map(void) : Asset(),
                 _root(nullptr),
                 _world(nullptr),
                 _worldnode(nullptr),
                 _debugnode(nullptr) {
    _bounds.size.set(1.0f, 1.0f);
}

/**
* Destroys this Map, releasing all resources.
*/
Map::~Map(void) {
    unload();
    clearRootNode();
}

#pragma mark -
#pragma mark Drawing Methods

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
void Map::setDrawScale(float value) {
    for (auto carrot: _carrots) {
        carrot->setDrawScale(value);
    }

    for (auto baby: _babies) {
        baby->setDrawScale(value);
    }

    for (auto farmer: _farmers) {
        farmer->setDrawScale(value);
    }

    for (auto wheat: _wheat) {
        wheat->setDrawScale(value);
    }
}

/**
* Clears the root scene graph node for this level
*/
void Map::clearRootNode() {
    if (_root == nullptr) {
        return;
    }
    _worldnode->removeFromParent();
    _worldnode->removeAllChildren();
    _worldnode = nullptr;

    _debugnode->removeFromParent();
    _debugnode->removeAllChildren();
    _debugnode = nullptr;

    _root = nullptr;
}

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
void Map::setRootNode(const std::shared_ptr<scene2::SceneNode> &node) {
    if (_root != nullptr) {
        clearRootNode();
    }

    _root = node;
    _scale.set(_root->getContentSize().width / _bounds.size.width,
               _root->getContentSize().height / _bounds.size.height);

    // Create, but transfer ownership to root
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(Vec2::ZERO);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(Vec2::ZERO);

    _root->addChild(_worldnode);
    _root->addChild(_debugnode);

    for (auto it = _walls.begin(); it != _walls.end(); ++it) {
        std::shared_ptr<physics2::PolygonObstacle> wall = *it;
        auto sprite = scene2::PolygonNode::allocWithTexture(
                _assets->get<Texture>(EARTH_TEXTURE),
                wall->getPolygon() * _scale);
        addObstacle(wall, sprite);  // All walls share the same texture
    }

    for (auto it = _carrots.begin(); it != _carrots.end(); ++it) {
        std::shared_ptr<Carrot> carrot = *it;
        auto carrotNode = scene2::PolygonNode::allocWithTexture(
                _assets->get<Texture>(CARROT_TEXTURE));
//        carrotNode->setColor(Color4::ORANGE);
        carrot->setSceneNode(carrotNode);
        carrot->setDrawScale(
                _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
        // Create the polygon node (empty, as the model will initialize)
        _worldnode->addChild(carrotNode);
        carrot->setDebugScene(_debugnode);
    }

    for (auto it = _babies.begin(); it != _babies.end(); ++it) {
        std::shared_ptr<BabyCarrot> baby = *it;
        auto babyNode = scene2::PolygonNode::allocWithTexture(
                _assets->get<Texture>(BABY_TEXTURE));
//        babyNode->setColor(Color4::BLUE);
        baby->setSceneNode(babyNode);
        baby->setDrawScale(_scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
        // Create the polygon node (empty, as the model will initialize)
        _worldnode->addChild(babyNode);
        baby->setDebugScene(_debugnode);
    }
    
    for (auto it = _farmers.begin(); it != _farmers.end(); ++it) {
        std::shared_ptr<Farmer> farmer = *it;
        auto farmerNode = scene2::PolygonNode::allocWithTexture(
                _assets->get<Texture>(FARMER_TEXTURE));
        farmer->setSceneNode(farmerNode);
        farmer->setDrawScale(
                _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
        // Create the polygon node (empty, as the model will initialize)
        _worldnode->addChild(farmerNode);
        farmer->setDebugScene(_debugnode);
    }

    for (auto it = _wheat.begin(); it != _wheat.end(); ++it) {
        std::shared_ptr<Wheat> wheat = *it;
        auto spriteImage = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(WHEAT_TEXTURE),
                                                              1, WHEAT_FRAMES, WHEAT_FRAMES);
        wheat->setSceneNode(spriteImage);
        addObstacle(wheat, spriteImage);  // All walls share the same texture
        
    }

}

/**
* Toggles whether to show the debug layer of this game world.
*
* The debug layer displays wireframe outlines of the physics fixtures.
*
* @param  flag whether to show the debug layer of this game world
*/
void Map::showDebug(bool flag) {
    if (_debugnode != nullptr) {
        _debugnode->setVisible(flag);
    }
}

#pragma mark -
#pragma mark Asset Loading

/**
 * Loads this game level from the source file
 *
 * This load method should NEVER access the AssetManager.  Assets are loaded in
 * parallel, not in sequence.  If an asset (like a game level) has references to
 * other assets, then these should be connected later, during scene initialization.
 *
 * @return true if successfully loaded the asset from a file
 */
bool Map::preload(const std::string file) {
    std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
    return preload(reader->readJson());
}

/**
 * Loads this game level from the source file
 *
 * This load method should NEVER access the AssetManager.  Assets are loaded in
 * parallel, not in sequence.  If an asset (like a game level) has references to
 * other assets, then these should be connected later, during scene initialization.
 *
 * @return true if successfully loaded the asset from a file
 */
bool Map::preload(const std::shared_ptr<cugl::JsonValue> &json) {
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
    }
    // Initial geometry
    float w = json->get("width")->asFloat();
    float h = json->get("height")->asFloat();
    _bounds.size.set(w, h);

    /** Create the physics world */
    _world = physics2::ObstacleWorld::alloc(getBounds(), Vec2(0, 0));

    auto walls = json->get("walls");
    if (walls != nullptr) {
        // Convert the object to an array so we can see keys and values
        int wsize = (int) walls->size();
        for (int ii = 0; ii < wsize; ii++) {
            loadWall(walls->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load walls");
    }

    auto carrots = json->get("carrots");
    if (carrots != nullptr) {
        // Convert the object to an array so we can see keys and values
        int csize = (int) carrots->size();
        for (int ii = 0; ii < csize; ii++) {
            loadCarrot(carrots->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load carrots");
    }

    auto babies = json->get("babies");
    if (babies != nullptr) {
        // Convert the object to an array so we can see keys and values
        int bsize = (int) babies->size();
        for (int ii = 0; ii < bsize; ii++) {
            loadBabyCarrot(babies->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load babies");
    }

    auto farmers = json->get("farmers");
    if (farmers != nullptr) {
        // Convert the object to an array so we can see keys and values
        int fsize = (int) farmers->size();
        for (int ii = 0; ii < fsize; ii++) {
            loadFarmer(farmers->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load farmers");
    }

    auto wheat = json->get("wheat");
    if (wheat != nullptr) {
        // Convert the object to an array so we can see keys and values
        int wsize = (int) wheat->size();
        for (int ii = 0; ii < wsize; ii++) {
            loadWheat(wheat->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load wheat");
    }
    return true;
}

/**
* Unloads this game level, releasing all sources
*
* This load method should NEVER access the AssetManager.  Assets are loaded and
* unloaded in parallel, not in sequence.  If an asset (like a game level) has
* references to other assets, then these should be disconnected earlier.
*/
void Map::unload() {
    for (auto it = _walls.begin(); it != _walls.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _walls.clear();
    for (auto it = _carrots.begin(); it != _carrots.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _carrots.clear();
    for (auto it = _babies.begin(); it != _babies.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _babies.clear();
    for (auto it = _farmers.begin(); it != _farmers.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _wheat.clear();
    for (auto it = _wheat.begin(); it != _wheat.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _farmers.clear();
    if (_world != nullptr) {
        _world->clear();
        _world = nullptr;
    }
}


#pragma mark -
#pragma mark Individual Loaders

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
bool Map::loadWall(const std::shared_ptr<JsonValue> &json) {
    bool success = true;

    int polysize = json->getInt("vertices");
    success = polysize > 0;

    std::vector<float> vertices = json->get("boundary")->asFloatArray();
    success = success && 2 * polysize == vertices.size();

    Vec2 *verts = reinterpret_cast<Vec2 *>(&vertices[0]);
    Poly2 wall(verts, (int) vertices.size() / 2);
    EarclipTriangulator triangulator;
    triangulator.set(wall.vertices);
    triangulator.calculate();
    wall.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    // Get the object, which is automatically retained
    std::shared_ptr<physics2::PolygonObstacle> wallobj = physics2::PolygonObstacle::allocWithAnchor(
            wall, Vec2::ANCHOR_CENTER);
    wallobj->setName("wall");

    wallobj->setBodyType(b2_staticBody);
    wallobj->setDensity(BASIC_DENSITY);
    wallobj->setFriction(BASIC_FRICTION);
    wallobj->setRestitution(BASIC_RESTITUTION);
    wallobj->setDebugColor(DEBUG_COLOR);

    if (success) {
        _walls.push_back(wallobj);
    } else {
        wallobj = nullptr;
    }

    vertices.clear();
    return success;

}

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
bool Map::loadCarrot(const std::shared_ptr<JsonValue> &json) {
    bool success = true;

    auto posArray = json->get("position");
    success = posArray->isArray();
    Vec2 carrotPos = Vec2(posArray->get(0)->asFloat(), posArray->get(1)->asFloat());
    std::shared_ptr<Carrot> carrot = Carrot::alloc(carrotPos, CARROT_SIZE, _scale.x);
    carrot->setDebugColor(DEBUG_COLOR);
    carrot->setName("carrot");
    _carrots.push_back(carrot);

    if (success) {
        _world->addObstacle(carrot);
    }

    return success;

}

/**
 * Loads a single wheat
 *
 * The wheat will be retained and stored in the vector _wheat.  If the
 * wheat fails to load, then it will not be added to _wheat.
 *
 * @param  reader   a JSON reader with cursor ready to read the wheat
 *
 * @retain the baby carrot
 * @return true if the baby carrot was successfully loaded
 */
bool Map::loadWheat(const std::shared_ptr<JsonValue> &json) {

    bool success = true;
    success = json->isArray();
    Vec2 wheatPos = Vec2(json->get(0)->asFloat(), json->get(1)->asFloat()) + Vec2::ANCHOR_CENTER;
    std::shared_ptr<Wheat> wheat = Wheat::alloc(wheatPos, WHEAT_SIZE, _scale.x);
    wheat->setDebugColor(DEBUG_COLOR);
    wheat->setName("wheat");
    _wheat.push_back(wheat);

    return success;
}

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
bool Map::loadBabyCarrot(const std::shared_ptr<JsonValue> &json) {

    bool success = true;

    auto posArray = json->get("position");
    success = posArray->isArray();
    Vec2 carrotPos = Vec2(posArray->get(0)->asFloat(), posArray->get(1)->asFloat());
    std::shared_ptr<BabyCarrot> baby = BabyCarrot::alloc(carrotPos, CARROT_SIZE, _scale.x);
    baby->setDebugColor(DEBUG_COLOR);
    baby->setName("baby");
    _babies.push_back(baby);

    if (success) {
        _world->addObstacle(baby);
    }

    return success;

}

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
bool Map::loadFarmer(const std::shared_ptr<JsonValue> &json) {
    bool success = true;
    
    auto posArray = json->get("position");
    success = posArray->isArray();
    Vec2 farmerPos = Vec2(posArray->get(0)->asFloat(), posArray->get(1)->asFloat());
    std::shared_ptr<Farmer> farmer = Farmer::alloc(farmerPos, FARMER_SIZE, _scale.x);
    farmer->setDebugColor(DEBUG_COLOR);
    farmer->setName("farmer");
    _farmers.push_back(farmer);
    
    if (success) {
        _world->addObstacle(farmer);
    }
    
    return success;

}

/**
 * Adds the physics object to the physics world and loosely couples it to the scene graph
 *
 * There are two ways to link a physics object to a scene graph node on the
 * screen.  One way is to make a subclass of a physics object, like we did
 * with dude.  The other is to use callback functions to loosely couple
 * the two.  This function is an example of the latter.
 *
 * @param obj             The physics object to add
 * @param node            The scene graph node to attach it to
 */
void Map::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle> &obj,
                      const std::shared_ptr<cugl::scene2::SceneNode> &node) {
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);

    // Position the scene graph node (enough for static objects)
    node->setPosition(obj->getPosition() * _scale);
    _worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode *weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle *obs) {
            weak->setPosition(obs->getPosition() * _scale);
            weak->setAngle(obs->getAngle());
        });
    }
}

/**
 * Rustle wheats if in contact with an object
 */
void Map::rustleWheats(float amount) {
    for (auto w: _wheat) {
        if (w->getRustling()) {
            w->rustle(amount);
        }
    }
}

/**
 * Stops all wheat from rustling and clear any signs of occupancy
 */
void Map::clearRustling(){
    for (auto w: _wheat) {
        w->setRustling(false);
        w->setOccupied(false);
    }
}

/**
 * TEMP: Switches current player between carrot and farmer
 */
void Map::togglePlayer(){
    _farmerPlaying = !_farmerPlaying;
}

/**
 * TEMP: Switches whether we want to see player or not
 */
void Map::toggleShowPlayer(){
    _showPlayer = !_showPlayer;
}
