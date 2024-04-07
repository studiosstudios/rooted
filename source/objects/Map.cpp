
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Map.h"
#include "../objects/EntityModel.h"
#include "../controllers/NetworkController.h"

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
# define PLANTING_SPOT_TEXTURE "planting spot"
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"
/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::GREEN

using namespace cugl;

/**
* Creates a new, empty Map.
*/
Map::Map() :
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
    dispose();
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
    // needs to be an ordered node in order to reorder some elements
    _worldnode = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::PRE_ASCEND);
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(Vec2::ZERO);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(Vec2::ZERO);
    
    
    bool showGrid = true; //change this to show the grid in debug
    if (showGrid) {
        for (int x = 0; x < _bounds.size.width; x++) {
            std::shared_ptr<scene2::WireNode> rect = scene2::WireNode::allocWithPath(Rect(Vec2::ZERO, Vec2(1, _bounds.size.height)));
            rect->setColor(Color4::WHITE);
            rect->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
            rect->setPosition(Vec2(x, 0));
            _debugnode->addChild(rect);
        }
        
        for (int y = 0; y < _bounds.size.height; y++) {
            std::shared_ptr<scene2::WireNode> rect = scene2::WireNode::allocWithPath(Rect(Vec2::ZERO, Vec2(_bounds.size.width, 1)));
            rect->setColor(Color4::WHITE);
            rect->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
            rect->setPosition(Vec2(0, y));
            _debugnode->addChild(rect);
        }
    }

    _root->addChild(_worldnode);
    _root->addChild(_debugnode);

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
bool Map::init(const std::shared_ptr<AssetManager> &assets,
               const std::shared_ptr<scene2::SceneNode> &root,
               const std::shared_ptr<cugl::JsonValue> &json) {
    setAssets(assets);

    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
    }

    _json = json;

    // Initial geometry
    _bounds.size.set(_json->getFloat("width"), _json->getFloat("height"));

    setRootNode(root);

    return true;
}

void Map::populate() {
    
    /** Create the physics world */
    _world = physics2::net::NetWorld::alloc(getBounds(), Vec2(0, 0));
    
    int height = _json->getFloat("height");
    int tileSize = _json->getInt("tilewidth");
    
    std::shared_ptr<JsonValue> layers = _json->get("layers");
    for (auto layer : layers->children()) {
        std::string name = layer->getString("name");
        auto objects = layer->get("objects");
        for (auto object : objects->children()) {
            readProperties(object, tileSize, height);
            
            std::string type = std::any_cast<std::string>(_propertiesMap.at("type"));
            float x = std::any_cast<float>(_propertiesMap.at("x"));
            float y = std::any_cast<float>(_propertiesMap.at("y"));
            float width = std::any_cast<float>(_propertiesMap.at("width"));
            float height = std::any_cast<float>(_propertiesMap.at("height"));
            
            if (name == "wheat") {
                loadWheat();
                break;
            } else if (name == "environment") {
                if (type == "PlantingSpot") {
                    loadPlantingSpot(x, y, width, height);
                } else {
                    CUWarn("TILED JSON: Unrecognized environmental object: %s. Are you sure you have placed the object in the correct layer?", type.c_str());
                }
            } else if (name == "entities"){
                if (type == "Farmer") {
                    loadFarmer(x, y, width, height);
                } else if (type == "Baby") {
                    loadBabyCarrot(x, y, width, height);
                } else if (type == "Carrot") {
                    loadCarrot(x, y, width, height);
                } else {
                    CUWarn("TILED JSON: Unrecognized entity: %s. Are you sure you have placed the objected in the correct layer?", type.c_str());
                }
            } else {
                CUWarn("TILED JSON: Unrecognized layer name: %s", name.c_str());
                break;
            }
        }
        
    }
    
    //place boundary walls
    loadBoundary(Vec2(-0.5, _bounds.size.height/2), Size(1, _bounds.size.height));
    loadBoundary(Vec2(_bounds.size.width+0.5, _bounds.size.height/2), Size(1, _bounds.size.height));
    loadBoundary(Vec2(_bounds.size.width/2, -0.5), Size(_bounds.size.width, 1));
    loadBoundary(Vec2(_bounds.size.width/2, _bounds.size.height+0.5), Size(_bounds.size.width, 1));
    
}

void Map::populateWithCarrots(int numCarrots) {
    for (int ii = 0; ii < numCarrots; ii++) {
        Vec2 position = Vec2(_carrotPosList.at(ii * 2), _carrotPosList.at(ii * 2 + 1));
        spawnCarrot(position, 1, 1);
    }
}

/**
* Unloads this game level, releasing all sources
*
* This load method should NEVER access the AssetManager.  Assets are loaded and
* unloaded in parallel, not in sequence.  If an asset (like a game level) has
* references to other assets, then these should be disconnected earlier.
*/
void Map::dispose() {
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
    _boundaries.clear();
    for (auto it = _boundaries.begin(); it != _boundaries.end(); ++it) {
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
    _wheatrenderer->dispose();
}

std::shared_ptr<EntityModel> Map::loadPlayerEntities(std::vector<std::string> players, std::string hostUUID, std::string thisUUID) {
    std::shared_ptr<EntityModel> ret;
    bool isHost = hostUUID == thisUUID;

    auto carrot = _carrots.begin();
    for (std::string uuid : players) {
        if (uuid != hostUUID) {
            (*carrot)->setUUID(uuid);
            if (uuid == thisUUID) {
                ret = (*carrot);
                getWorld()->getOwnedObstacles().insert({*carrot, 0});
            }
            carrot++;
        }
    }
    
    _farmers.at(0)->setUUID(hostUUID);
    ret = ret == nullptr ? _farmers.at(0) : ret;
    if (isHost) {
        getWorld()->getOwnedObstacles().insert({_farmers.at(0), 0});
    }
    
    _character = ret;

    _character->getSceneNode()->setPriority(float(Map::DrawOrder::PLAYER));
    
    return ret;
}

std::vector<std::shared_ptr<EntityModel>> Map::loadBabyEntities() {
    std::vector<std::shared_ptr<EntityModel>> ret;
    for ( auto baby : _babies) {
        ret.push_back(baby);
        getWorld()->getOwnedObstacles().insert({baby, 0});
    }
    return ret;
}

void Map::acquireMapOwnership() {
    auto ownerMap = _world->getOwnedObstacles();
    std::cout << "owned obstacles size: " << ownerMap.size();
    for (auto it = _walls.begin(); it != _walls.end(); ++it) {
        ownerMap.insert({*it, 0});
    }
    for (auto it = _babies.begin(); it != _babies.end(); ++it) {
        ownerMap.insert({*it, 0});
    }
    for (auto it = _carrots.begin(); it != _carrots.end(); ++it) {
        ownerMap.insert({*it, 0});
    }
    for (auto it = _farmers.begin(); it != _farmers.end(); ++it) {
        ownerMap.insert({*it, 0});
    }
    for (auto it = _boundaries.begin(); it != _boundaries.end(); ++it) {
        ownerMap.insert({*it, 0});
    }
    std::cout << "owned obstacles size: " << _world->getOwnedObstacles().size();
}

#pragma mark -
#pragma mark Individual Loaders

/**
 * Adds a boundary box obstacle to the world.
 */
void Map::loadBoundary(Vec2 pos, Size size){
    std::shared_ptr<physics2::BoxObstacle> boundaryobj = physics2::BoxObstacle::alloc(Vec2::ZERO, size);
    boundaryobj->setName("boundary");

    boundaryobj->setBodyType(b2_staticBody);
    boundaryobj->setDensity(BASIC_DENSITY);
    boundaryobj->setFriction(BASIC_FRICTION);
    boundaryobj->setRestitution(BASIC_RESTITUTION);
    boundaryobj->setDebugColor(DEBUG_COLOR);

    _boundaries.push_back(boundaryobj);
    
    auto sprite = scene2::PolygonNode::alloc(); //empty texture
    sprite->setColor(Color4(0,0,0,0));
    addObstacle(boundaryobj, sprite);
    boundaryobj->setPosition(pos); //set position after adding to world since it is out of boundss
}

/**
 * Loads and builds the shaders for a specific map texture, and adds the shader nodes to the world
 * node. This method should only be called once per initialization, any subsequent calls will
 * override previous calls.
 */
void Map::loadWheat(){
    std::string name = std::any_cast<std::string>(_propertiesMap.at("name"));
    float bladeColorScale = std::any_cast<float>(_propertiesMap.at("blade_color_scale"));
    _wheatrenderer = WheatRenderer::alloc(_assets, name, bladeColorScale);
    _wheatrenderer->setScale(_scale.x);
    _wheatrenderer->buildShaders();
    _groundnode = ShaderNode::alloc(_wheatrenderer, ShaderNode::ShaderType::GROUND);
    _wheatnode = ShaderNode::alloc(_wheatrenderer, ShaderNode::ShaderType::WHEAT);
    _groundnode->setPriority(float(Map::DrawOrder::GROUND));
    _wheatnode->setPriority(float(Map::DrawOrder::WHEAT));
    
    _worldnode->addChild(_wheatnode);
    _worldnode->addChild(_groundnode);
}

/**
 * Loads a single planting spot of variable size into the world.
 */
void Map::loadPlantingSpot(float x, float y, float width, float height) {
    Vec2 spotPos = Vec2(x, y) + Vec2::ANCHOR_CENTER * Vec2(width, height);
    
    std::shared_ptr<PlantingSpot> plantingSpot = PlantingSpot::alloc(spotPos, {width, height}, _scale.x);
    plantingSpot->setDebugColor(DEBUG_COLOR);
    plantingSpot->setName("planting spot");
    plantingSpot->setPlantingID((unsigned)_plantingSpot.size());
    _plantingSpot.push_back(plantingSpot);

    auto spotNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(PLANTING_SPOT_TEXTURE));
    plantingSpot->setSceneNode(spotNode);
    spotNode->setColor(Color4(255, 255, 255, 255 * 0.4));
    spotNode->setPriority(float(Map::DrawOrder::PLANTINGSPOT));
    addObstacle(plantingSpot, spotNode);
}

/**
 * Loads a single farmer into the world.
 */
void Map::loadFarmer(float x, float y, float width, float height) {
    Vec2 farmerPos = Vec2(x, y) + Vec2::ANCHOR_CENTER * Vec2(width, height);
    std::shared_ptr<Farmer> farmer = Farmer::alloc(farmerPos, {width, height}, _scale.x);
    farmer->setDebugColor(DEBUG_COLOR);
    farmer->setName("farmer");
    
    auto farmerTexture = _assets->get<Texture>(FARMER_FRONT_WALK_SPRITE);

    auto farmerNode = scene2::SpriteNode::allocWithSheet(farmerTexture, 3, 4);
    farmerNode->setScale(0.25f, 0.25f);
    auto carrotfarmerNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(CARROTFARMER_TEXTURE));
    farmerNode->setFrame(0);
    carrotfarmerNode->setVisible(false);
    carrotfarmerNode->setScale(0.25f, 0.25f);
    
    farmer->setNormalNode(farmerNode);
    farmer->setCaptureNode(carrotfarmerNode);
    farmer->setSceneNode(farmerNode);
    farmer->setDrawScale(
            _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
    // Create the polygon node (empty, as the model will initialize)
    farmerNode->setPriority(float(Map::DrawOrder::ENTITIES));
    _worldnode->addChild(farmerNode);
    _worldnode->addChild(carrotfarmerNode);
    farmer->setDebugScene(_debugnode);

    _farmers.push_back(farmer);

    _world->initObstacle(farmer);
}

/**
 * Loads a single baby carrot into the world.
 */
void Map::loadBabyCarrot(float x, float y, float width, float height) {
    Vec2 carrotPos = Vec2(x, y)  + Vec2::ANCHOR_CENTER * Vec2(width, height);;
    std::shared_ptr<BabyCarrot> baby = BabyCarrot::alloc(carrotPos, {width, height}, _scale.x);
    baby->setDebugColor(DEBUG_COLOR);
    baby->setName("baby");
    baby->setID((unsigned)_babies.size());
    _babies.push_back(baby);

    auto babyNode = scene2::PolygonNode::allocWithTexture(
            _assets->get<Texture>(BABY_TEXTURE));
//        babyNode->setColor(Color4::BLUE);
    baby->setSceneNode(babyNode);
    baby->setDrawScale(
            _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
    // Create the polygon node (empty, as the model will initialize)
    babyNode->setPriority(float(Map::DrawOrder::ENTITIES));
    _worldnode->addChild(babyNode);
    baby->setDebugScene(_debugnode);

    _world->initObstacle(baby);
}

/**
 * Adds a possible carrot spawn location to the world.
 */
void Map::loadCarrot(float x, float y, float width, float height) {
    _carrotPosList.push_back(x + 0.5 * width);
    _carrotPosList.push_back(y + 0.5 * height);
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
    _world->initObstacle(obj);
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


bool Map::readProperties(const std::shared_ptr<cugl::JsonValue> &json, int tileSize, int levelHeight) {
    // this is copied from nine lives but might be a bit unnecessary

    _propertiesMap.clear();

    _propertiesMap.emplace("x", json->getFloat("x")/tileSize);
    _propertiesMap.emplace("y", levelHeight - json->getFloat("y")/tileSize);
    _propertiesMap.emplace("width", json->getFloat("width")/tileSize);
    _propertiesMap.emplace("height", json->getFloat("height")/tileSize);
    _propertiesMap.emplace("type", json->getString("type"));

    auto properties = json->get("properties");
    if (properties == nullptr) { return true; }

    for (auto property : properties->children()){
        std::string name = property->getString("name");
        std::string type = property->getString("type");

        if (type == "string") {
            _propertiesMap.emplace(name, property->getString("value"));
        } else if (type == "int") {
            _propertiesMap.emplace(name, property->getInt("value"));
        } else if (type == "bool") {
            _propertiesMap.emplace(name, property->getBool("value"));
        } else if (type == "float") {
            _propertiesMap.emplace(name, property->getFloat("value"));
        }

    }
    return true;
}

void Map::spawnCarrot(Vec2 position, float width, float height) {
    std::shared_ptr<Carrot> carrot = Carrot::alloc(position, {width, height}, _scale.x);
    carrot->setDebugColor(DEBUG_COLOR);
    carrot->setName("carrot");
    _carrots.push_back(carrot);

    auto carrotNode = scene2::PolygonNode::allocWithTexture(
            _assets->get<Texture>(CARROT_TEXTURE));
    carrot->setSceneNode(carrotNode);
    carrotNode->setPriority(float(Map::DrawOrder::ENTITIES));
    carrot->setDrawScale(
            _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
    // Create the polygon node (empty, as the model will initialize)
    _worldnode->addChild(carrotNode);
    carrot->setDebugScene(_debugnode);

    _world->initObstacle(carrot);
}



void Map::updateShader(float step, const Mat4 &perspective) {
    int size = (unsigned)(_carrots.size() + _farmers.size() + _babies.size());
    float positions[2*size]; // must be 1d array
    float velocities[size];
    float ratio = _wheatrenderer->getAspectRatio();
    for (int i = 0; i < _carrots.size(); i++) {
        positions[2 * i] = _carrots.at(i)->getX() / _scale.x;
        positions[2 * i + 1] = 1 - (_carrots.at(i)->getY() - _carrots.at(i)->getHeight()/2) / _scale.x * ratio;
        velocities[i] = _carrots.at(i)->getLinearVelocity().length();
    }
    for (int i = 0; i < _farmers.size(); i++) {
        positions[2 * i + 2* _carrots.size()] = _farmers.at(i)->getX() / _scale.x;
        positions[2 * i + 1 + 2 * _carrots.size()] = 1 - (_farmers.at(i)->getY() - _farmers.at(i)->getHeight()/2) / _scale.x * ratio;
        velocities[i + _carrots.size()] = _farmers.at(i)->getLinearVelocity().length();
    }
    for (int i = 0; i < _babies.size(); i++) {
        positions[2 * i + 2* (_carrots.size() + _farmers.size())] = _babies.at(i)->getX() / _scale.x;
        positions[2 * i + 1 + 2 * (_carrots.size() + _farmers.size())] = 1 - (_babies.at(i)->getY() - _babies.at(i)->getHeight()/2) / _scale.x * ratio;
        velocities[i + _carrots.size() + _farmers.size()] = _babies.at(i)->getLinearVelocity().length();
    }
    _wheatrenderer->update(step, perspective, size, positions, velocities);
}
