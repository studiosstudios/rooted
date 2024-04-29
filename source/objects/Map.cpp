
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Map.h"
#include "../objects/EntityModel.h"
#include "../controllers/NetworkController.h"
#include "../RootedConstants.h"

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
# define PLANTING_SPOT_TEXTURE "planting spot"
# define GRASS_TEXTURE "grass"
/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::GREEN

const bool FULL_WHEAT_HEIGHT = true; //change this to turn off wheat height and make shaders more efficient (hopefully)

using namespace cugl;

/**
* Creates a new, empty Map.
*/
Map::Map() :
        _root(nullptr),
        _world(nullptr),
        _worldnode(nullptr),
        _debugnode(nullptr) {
    _worldbounds.size.set(1.0f, 1.0f);
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
    
    for (auto rock: _rocks) {
        rock->setDrawScale(value);
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
    _scale.set(_root->getContentSize().width / _worldbounds.size.width,
               _root->getContentSize().height / _worldbounds.size.height);
    
    // Create, but transfer ownership to root
    // needs to be an ordered node in order to reorder some elements
    _worldnode = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(Vec2::ZERO);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(Vec2::ZERO);
    
    _entitiesNode = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::PRE_ASCEND);
    _entitiesNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _entitiesNode->setPosition(Vec2::ZERO);
    _entitiesNode->setPriority(float(DrawOrder::ENTITIES));
//    _entitiesNode->allocNode();
//    _entitiesNode->setPriority(float(DrawOrder::ENTITIES));
    
    bool showGrid = true; //change this to show the grid in debug
    if (showGrid) {
        for (int x = 0; x < _mapbounds.size.width; x++) {
            std::shared_ptr<scene2::WireNode> rect = scene2::WireNode::allocWithPath(Rect(Vec2::ZERO, Vec2(1, _worldbounds.size.height)));
            rect->setColor(Color4::WHITE);
            rect->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
            rect->setPosition(Vec2(x, 0));
            _debugnode->addChild(rect);
        }
        
        for (int y = 0; y < _mapbounds.size.height; y++) {
            std::shared_ptr<scene2::WireNode> rect = scene2::WireNode::allocWithPath(Rect(Vec2::ZERO, Vec2(_worldbounds.size.width, 1)));
            rect->setColor(Color4::WHITE);
            rect->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
            rect->setPosition(Vec2(0, y));
            _debugnode->addChild(rect);
        }
    }

    _root->addChild(_worldnode);
//    _worldnode->addChild(_entitiesNode);
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
bool Map::init(const std::shared_ptr<AssetManager> &assets, bool tutorial) {
    setAssets(assets);
    auto json = _assets->get<JsonValue>("mapNames");
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load map names");
    }
    _mapNames = json->get("names")->asStringArray();
    
    _tutorial = tutorial;

    return true;
}

void Map::generate(int randSeed, int numFarmers, int numCarrots, int numBabyCarrots, int numPlantingSpots){
    
    if (_tutorial) {
        _worldbounds.size.set(Size(MAP_UNIT_WIDTH, MAP_UNIT_HEIGHT) * 3);
        _mapbounds.size.set(Size(MAP_UNIT_WIDTH, MAP_UNIT_HEIGHT * 3));
        _mapInfo.resize(1, std::vector<std::pair<std::string, float>>(3));
        
        //load in tutorial map
        std::shared_ptr<JsonValue> json = _assets->get<JsonValue>("tutorialBottom");
        loadTiledJson(json, 0, 0);
        json = _assets->get<JsonValue>("tutorialMid");
        loadTiledJson(json, 0, 1);
        json = _assets->get<JsonValue>("tutorialTop");
        loadTiledJson(json, 0, 2);
        
        
    } else {
        _rand32.seed(randSeed);
        _worldbounds.size.set(Size(MAP_UNIT_WIDTH, MAP_UNIT_HEIGHT) * 3);
        _mapbounds.set(_worldbounds);
        
        _mapInfo.resize(_mapbounds.size.width / MAP_UNIT_WIDTH, std::vector<std::pair<std::string, float>>(_mapbounds.size.height / MAP_UNIT_HEIGHT));
        
        //randomly select a map for each location and object info lists
        for (int i = 0; i < _mapbounds.size.width / MAP_UNIT_WIDTH; i++ ) {
            for (int j = 0; j < _mapbounds.size.height / MAP_UNIT_HEIGHT; j++) {
                std::string mapName = _mapNames[floor(float(_rand32()) / _rand32.max() * _mapNames.size())];
                std::shared_ptr<JsonValue> json = _assets->get<JsonValue>(mapName);
                loadTiledJson(json, i, j);
            }
        }
    }
    
    //randomly shuffle object positions
    std::shuffle(_carrotSpawns.begin(), _carrotSpawns.end(), _rand32);
    std::shuffle(_farmerSpawns.begin(), _farmerSpawns.end(), _rand32);
    std::shuffle(_babyCarrotSpawns.begin(), _babyCarrotSpawns.end(), _rand32);
    std::shuffle(_plantingSpawns.begin(), _plantingSpawns.end(), _rand32);
    
    //take just first num{object} elements of each vector
    _carrotSpawns = std::vector(_carrotSpawns.begin(), _carrotSpawns.begin() + std::min(numCarrots, int(_carrotSpawns.size())));
    _farmerSpawns = std::vector(_farmerSpawns.begin(), _farmerSpawns.begin() + std::min(numFarmers, int(_farmerSpawns.size())));
    _babyCarrotSpawns = std::vector(_babyCarrotSpawns.begin(), _babyCarrotSpawns.begin() + std::min(numBabyCarrots, int(_babyCarrotSpawns.size())));
    _plantingSpawns = std::vector(_plantingSpawns.begin(), _plantingSpawns.begin() + std::min(numPlantingSpots, int(_plantingSpawns.size())));
}
 
void Map::loadTiledJson(std::shared_ptr<JsonValue>& json, int i, int j) {
    
    CUAssertLog(json != nullptr, "Failed to load tiled json");
    
    std::shared_ptr<JsonValue> layers = json->get("layers");
    int height = json->getFloat("height");
    int tileSize = json->getInt("tilewidth");
    for (auto layer : layers->children()) {
        std::string name = layer->getString("name");
        
        //default empty wheat texture
        if (name == "wheat") {
            _mapInfo[i][j] = std::pair("", 0);
        }
        
        auto objects = layer->get("objects");
        for (auto object : objects->children()) {
            readProperties(object, tileSize, height);
            
            std::string type = std::any_cast<std::string>(_propertiesMap.at("type"));
            float x = std::any_cast<float>(_propertiesMap.at("x"));
            float y = std::any_cast<float>(_propertiesMap.at("y"));
            float width = std::any_cast<float>(_propertiesMap.at("width"));
            float height = std::any_cast<float>(_propertiesMap.at("height"));
            
            if (name == "wheat") {
                std::string name = std::any_cast<std::string>(_propertiesMap.at("name"));
                float bladeColorScale = std::any_cast<float>(_propertiesMap.at("blade_color_scale"));
                _mapInfo[i][j] = std::pair(name, bladeColorScale);
                break;
            } else if (name == "environment") {
                if (type == "PlantingSpot") {
                    _plantingSpawns.push_back(Rect(x + i * MAP_UNIT_WIDTH+ 0.5 * width, y + j * MAP_UNIT_HEIGHT + 0.5 * height, width, height));
                } else if (type == "Rock") {
                    _rockSpawns.push_back(std::pair(Rect(x + i * MAP_UNIT_WIDTH+ 0.5 * width, y + j * MAP_UNIT_HEIGHT + 0.5 * height, width, height), true));
                } else {
                    CUWarn("TILED JSON: Unrecognized environmental object: %s. Are you sure you have placed the object in the correct layer?", type.c_str());
                }
            } else if (name == "entities"){
                if (type == "Farmer") {
                    _farmerSpawns.push_back(Rect(x + i * MAP_UNIT_WIDTH+ 0.5 * width, y + j * MAP_UNIT_HEIGHT + 0.5 * height, width, height));
                } else if (type == "Baby") {
                    _babyCarrotSpawns.push_back(Rect(x + i * MAP_UNIT_WIDTH+ 0.5 * width, y + j * MAP_UNIT_HEIGHT + 0.5 * height, width, height));
                } else if (type == "Carrot") {
                    _carrotSpawns.push_back(Rect(x + i * MAP_UNIT_WIDTH+ 0.5 * width, y + j * MAP_UNIT_HEIGHT + 0.5 * height, width, height));
                } else {
                    CUWarn("TILED JSON: Unrecognized entity: %s. Are you sure you have placed the object in the correct layer?", type.c_str());
                }
            } else {
                CUWarn("TILED JSON: Unrecognized layer name: %s", name.c_str());
                break;
            }
        }
    }
        
}

void Map::populate() {

    /** Create the physics world */
    _world = physics2::net::NetWorld::alloc(getWorldBounds(), Vec2(0, 0));

    _wheatscene = WheatScene::alloc(_assets, _mapInfo, _scale, _worldbounds.size);
    _numRockSpawns = 0;
    _spawnCooldown = 0;

    _shaderrenderer = ShaderRenderer::alloc(_wheatscene->getTexture(), _assets, _worldbounds.size, FULL_WHEAT_HEIGHT);
    _shaderrenderer->setScale(_scale.x);
    _shaderrenderer->buildShaders();
    _groundnode = ShaderNode::alloc(_shaderrenderer, ShaderNode::ShaderType::GROUND);
    _wheatnode = ShaderNode::alloc(_shaderrenderer, ShaderNode::ShaderType::WHEAT);
    _cloudsnode = ShaderNode::alloc(_shaderrenderer, ShaderNode::ShaderType::CLOUDS);
    _groundnode->setPriority(float(Map::DrawOrder::SHADOWS));
    _wheatnode->setPriority(float(Map::DrawOrder::WHEAT));
    _cloudsnode->setPriority(float(Map::DrawOrder::CLOUDS));
    
    _shaderedEntitiesNode = EntitiesNode::alloc(_entitiesNode, _wheatscene->getTexture(), _assets, FULL_WHEAT_HEIGHT);
    _shaderedEntitiesNode->setPriority(float(Map::DrawOrder::ENTITIESSHADER));
    
    _worldnode->addChild(_shaderedEntitiesNode);
    _worldnode->addChild(_wheatnode);
    _worldnode->addChild(_groundnode);
    _worldnode->addChild(_cloudsnode);
    
    spawnPlantingSpots();
    spawnFarmers();
    spawnCarrots();
    spawnBabyCarrots();
    
    //place boundary walls
    loadBoundary(Vec2(-0.5, _mapbounds.size.height/2), Size(1, _mapbounds.size.height));
    loadBoundary(Vec2(_mapbounds.size.width+0.5, _mapbounds.size.height/2), Size(1, _mapbounds.size.height));
    loadBoundary(Vec2(_mapbounds.size.width/2, -0.5), Size(_mapbounds.size.width, 1));
    loadBoundary(Vec2(_mapbounds.size.width/2, _mapbounds.size.height+0.5), Size(_mapbounds.size.width, 1));
    
    //add grass background node
    float grassScale = 16.0 * DEFAULT_DRAWSCALE / _scale.x;
    std::shared_ptr<Texture> grassTex = _assets->get<Texture>(GRASS_TEXTURE);
    Size nodesize; //ensure node properly fills scene
    if (float(grassTex->getWidth())/grassTex->getHeight() > float(_root->getContentWidth())/_root->getContentHeight()) {
        nodesize = grassScale * Size(float(_root->getContentHeight()) * grassTex->getWidth() / grassTex->getHeight(), _root->getContentHeight());
    } else {
        nodesize = grassScale * Size(_root->getContentWidth(), float(_root->getContentWidth()) * grassTex->getHeight() / grassTex->getWidth());
    }
    auto grassnode = scene2::PolygonNode::allocWithPoly(Rect(Vec2::ZERO, nodesize));
    grassnode->setTexture(grassTex);
    grassnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    grassnode->setPriority(float(DrawOrder::GRASS));
    grassnode->setScale(1/grassScale);
    _worldnode->addChild(grassnode);
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
    _farmers.clear();
    for (auto it = _boundaries.begin(); it != _boundaries.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _boundaries.clear();
    for (auto it = _plantingSpot.begin(); it != _plantingSpot.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _plantingSpot.clear();
    for (auto it = _rocks.begin(); it != _rocks.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
        (*it) = nullptr;
    }
    _rocks.clear();
    if (_world != nullptr) {
        _world->clear();
        _world->dispose();
        _world = nullptr;
    }
    _character = nullptr;
    if (_shaderedEntitiesNode != nullptr) {
        _shaderedEntitiesNode->dispose();
        _shaderedEntitiesNode = nullptr;
    }
    if (_wheatscene != nullptr) {
        _wheatscene->dispose();
        _wheatscene = nullptr;
    }
    if (_shaderrenderer != nullptr) {
        _shaderrenderer->dispose();
        _shaderrenderer = nullptr;
    }
    
    _mapInfo.clear();
    _carrotSpawns.clear();
    _farmerSpawns.clear();
    _babyCarrotSpawns.clear();
    _plantingSpawns.clear();
    _rockSpawns.clear();
}

std::shared_ptr<EntityModel> Map::loadPlayerEntities(std::vector<std::string> players, std::string hostUUID, std::string thisUUID) {
    std::shared_ptr<EntityModel> ret;
    _playerUUIDs = players;
    _hostUUID = hostUUID;
    _thisUUID = thisUUID;
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
    //REMINDER TO MYSELF: UNDO NETWORLD CHANGE
    auto ownerMap = _world->getOwnedObstacles();
    std::cout << "owned obstacles size: " << ownerMap.size();
    for (auto it = _walls.begin(); it != _walls.end(); ++it) {
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
    for (auto it = _plantingSpot.begin(); it != _plantingSpot.end(); ++it) {
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

void Map::spawnPlantingSpots() {
    for (Rect rect : _plantingSpawns) {
        std::shared_ptr<PlantingSpot> plantingSpot = PlantingSpot::alloc(rect.origin, rect.size, _scale.x);
        plantingSpot->setDebugColor(DEBUG_COLOR);
        plantingSpot->setName("planting spot");
        plantingSpot->setPlantingID((unsigned)_plantingSpot.size());
        _plantingSpot.push_back(plantingSpot);

        plantingSpot->setSceneNode(_assets, float(Map::DrawOrder::PLANTINGSPOT));
        addObstacle(plantingSpot, plantingSpot->getSceneNode());
    }
}

void Map::spawnFarmers() {
    for (Rect rect : _farmerSpawns) {
        std::shared_ptr<Farmer> farmer = Farmer::alloc(rect.origin, rect.size, _scale.x);
        farmer->setDebugColor(DEBUG_COLOR);
        farmer->setName("farmer");
        
        auto farmerSouthWalkSprite = _assets->get<Texture>(FARMER_SOUTH_WALK_SPRITE);
        auto farmerNorthWalkSprite = _assets->get<Texture>(FARMER_NORTH_WALK_SPRITE);
        auto farmerEastWalkSprite  = _assets->get<Texture>(FARMER_EAST_WALK_SPRITE);
        auto farmerNorthEastWalkSprite  = _assets->get<Texture>(FARMER_NORTHEAST_WALK_SPRITE);
        auto farmerSouthEastWalkSprite  = _assets->get<Texture>(FARMER_SOUTHEAST_WALK_SPRITE);

        auto farmerSouthWalkNode = scene2::SpriteNode::allocWithSheet(farmerSouthWalkSprite, 3, 4);
        farmerSouthWalkNode->setScale(0.18f * _scale/DEFAULT_DRAWSCALE);
        farmerSouthWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        
        auto farmerNorthWalkNode = scene2::SpriteNode::allocWithSheet(farmerNorthWalkSprite, 3, 4);
        farmerNorthWalkNode->setScale(0.18f * _scale/DEFAULT_DRAWSCALE);
        farmerNorthWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        farmerNorthWalkNode->setVisible(false);
        
        auto farmerEastWalkNode = scene2::SpriteNode::allocWithSheet(farmerEastWalkSprite, 3, 4);
        farmerEastWalkNode->setScale(0.11f * _scale/DEFAULT_DRAWSCALE);
        farmerEastWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        farmerEastWalkNode->setVisible(false);
        
        auto farmerNorthEastWalkNode = scene2::SpriteNode::allocWithSheet(farmerNorthEastWalkSprite, 3, 4);
        farmerNorthEastWalkNode->setScale(0.12f * _scale/DEFAULT_DRAWSCALE);
        farmerNorthEastWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        farmerNorthEastWalkNode->setVisible(false);
        
        auto farmerSouthEastWalkNode = scene2::SpriteNode::allocWithSheet(farmerSouthEastWalkSprite, 3, 4, 9);
        farmerSouthEastWalkNode->setScale(0.14f * _scale/DEFAULT_DRAWSCALE);
        farmerSouthEastWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        farmerSouthEastWalkNode->setVisible(false);
        
        auto carrotfarmerNode = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(CARROTFARMER_TEXTURE), 1, 1);
        carrotfarmerNode->setVisible(false);
        carrotfarmerNode->setScale(0.23f * _scale/DEFAULT_DRAWSCALE);
        
        _entitiesNode->addChild(farmerSouthWalkNode);
        _entitiesNode->addChild(farmerNorthWalkNode);
        _entitiesNode->addChild(farmerEastWalkNode);
        _entitiesNode->addChild(farmerNorthEastWalkNode);
        _entitiesNode->addChild(farmerSouthEastWalkNode);
        _entitiesNode->addChild(carrotfarmerNode);
        
        farmer->setSpriteNodes(farmerNorthWalkNode,
                               farmerNorthEastWalkNode,
                               farmerEastWalkNode,
                               farmerSouthEastWalkNode,
                               farmerSouthWalkNode);
        
        farmer->setNormalNode(farmerSouthWalkNode);
        farmer->setCaptureNode(carrotfarmerNode);
        
        farmer->setSceneNode(farmerSouthWalkNode);
        farmer->setDrawScale(
                _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM

        farmer->setDebugScene(_debugnode);

        auto wheatnode = farmer->allocWheatHeightNode();
        _wheatscene->getRoot()->addChild(wheatnode);
        
        _farmers.push_back(farmer);

        _world->initObstacle(farmer);
    }
}

void Map::spawnBabyCarrots() {
    for (Rect rect : _babyCarrotSpawns) {
        std::shared_ptr<BabyCarrot> baby = BabyCarrot::alloc(rect.origin, rect.size, _scale.x);
        baby->setDebugColor(DEBUG_COLOR);
        baby->setName("baby");
        baby->setID((unsigned)_babies.size());
        _babies.push_back(baby);
        
        _assets->get<Texture>(BABY_TEXTURE)->setName("baby");
        auto babyNode = scene2::SpriteNode::allocWithSheet(
                _assets->get<Texture>(BABY_TEXTURE), 1, 1);
    //        babyNode->setColor(Color4::BLUE);
        baby->setSceneNode(babyNode);
        babyNode->setName("baby");
        babyNode->setPriority(float(DrawOrder::ENTITIES));
        //    babyNode->setColor(Color4::BLACK);
        baby->setDrawScale(
                           _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
        babyNode->setScale(_scale/DEFAULT_DRAWSCALE);
        // Create the polygon node (empty, as the model will initialize)
        babyNode->setHeight(32*_scale.y/DEFAULT_DRAWSCALE);
        _entitiesNode->addChild(babyNode);
        baby->setDebugScene(_debugnode);
        
        auto wheatnode = baby->allocWheatHeightNode();
        _wheatscene->getRoot()->addChild(wheatnode);
        
        _world->initObstacle(baby);
    }
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

void Map::spawnCarrots() {
    for (Rect rect : _carrotSpawns) {
        std::shared_ptr<Carrot> carrot = Carrot::alloc(rect.origin, rect.size, _scale.x);
        carrot->setDebugColor(DEBUG_COLOR);
        carrot->setName("carrot");
        _carrots.push_back(carrot);
        
        auto carrotSouthWalkSprite = _assets->get<Texture>(CARROT_SOUTH_WALK_SPRITE);
        auto carrotNorthWalkSprite = _assets->get<Texture>(CARROT_NORTH_WALK_SPRITE);
        auto carrotEastWalkSprite = _assets->get<Texture>(CARROT_EAST_WALK_SPRITE);
        auto carrotNorthEastWalkSprite = _assets->get<Texture>(CARROT_NORTHEAST_WALK_SPRITE);
        auto carrotSouthEastWalkSprite = _assets->get<Texture>(CARROT_SOUTHEAST_WALK_SPRITE);
        
        auto carrotSouthWalkNode = scene2::SpriteNode::allocWithSheet(
                                                                      carrotSouthWalkSprite, 3, 5);
        carrotSouthWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        carrotSouthWalkNode->setScale(0.1f * _scale/DEFAULT_DRAWSCALE);
        carrotSouthWalkNode->setAnchor(Vec2(0.5, 0.25));
        carrotSouthWalkNode->setFrame(0);
        
        auto carrotNorthWalkNode = scene2::SpriteNode::allocWithSheet(
                                                                      carrotNorthWalkSprite, 3, 5);
        carrotNorthWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        carrotNorthWalkNode->setScale(0.1f * _scale/DEFAULT_DRAWSCALE);
        carrotNorthWalkNode->setAnchor(Vec2(0.5, 0.25));
        carrotNorthWalkNode->setVisible(false);
        
        auto carrotEastWalkNode = scene2::SpriteNode::allocWithSheet(
                                                                     carrotEastWalkSprite, 3, 5);
        carrotEastWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        carrotEastWalkNode->setScale(0.1f * _scale/DEFAULT_DRAWSCALE);
        carrotEastWalkNode->setAnchor(Vec2(0.5, 0.25));
        carrotEastWalkNode->setVisible(false);
        
        auto carrotNorthEastWalkNode = scene2::SpriteNode::allocWithSheet(carrotNorthEastWalkSprite, 3, 5);
        carrotNorthEastWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        carrotNorthEastWalkNode->setScale(0.1f * _scale/DEFAULT_DRAWSCALE);
        carrotNorthEastWalkNode->setAnchor(Vec2(0.5, 0.25));
        carrotNorthEastWalkNode->setVisible(false);
        
        auto carrotSouthEastWalkNode = scene2::SpriteNode::allocWithSheet(carrotSouthEastWalkSprite, 3, 5);
        carrotSouthEastWalkNode->setPriority(float(Map::DrawOrder::ENTITIES));
        carrotSouthEastWalkNode->setScale(0.1f * _scale/DEFAULT_DRAWSCALE);
        carrotSouthEastWalkNode->setAnchor(Vec2(0.5, 0.25));
        carrotSouthEastWalkNode->setVisible(false);
        
        _entitiesNode->addChild(carrotNorthWalkNode);
        _entitiesNode->addChild(carrotEastWalkNode);
        _entitiesNode->addChild(carrotSouthWalkNode);
        _entitiesNode->addChild(carrotNorthEastWalkNode);
        _entitiesNode->addChild(carrotSouthEastWalkNode);
        
        carrot->setSceneNode(carrotSouthWalkNode);
        carrot->setDrawScale(
                             _scale.x);  //scale.x is used as opposed to scale since physics scaling MUST BE UNIFORM
        // Create the polygon node (empty, as the model will initialize)
        carrot->setSpriteNodes(carrotNorthWalkNode,
                               carrotNorthEastWalkNode,
                               carrotEastWalkNode,
                               carrotSouthEastWalkNode,
                               carrotSouthWalkNode);
        
        carrot->setDebugScene(_debugnode);
        
        auto wheatnode = carrot->allocWheatHeightNode();
        _wheatscene->getRoot()->addChild(wheatnode);
        
        _world->initObstacle(carrot);
    }
}

void Map::updateShaders(float step, Mat4 perspective) {
    int size = (unsigned)(_carrots.size() + _farmers.size() + _babies.size() + _rocks.size());
    float positions[2*size]; // must be 1d array
    float velocities[size];
    float ratio = _shaderrenderer->getAspectRatio();
    float scale = _root->getContentSize().width/_scale.x;
    for (int i = 0; i < _carrots.size(); i++) {
        positions[2 * i] = _carrots.at(i)->getX() / scale;
        positions[2 * i + 1] = 1 - (_carrots.at(i)->getY() - _carrots.at(i)->getHeight()/2) / scale * ratio;
        velocities[i] = _carrots.at(i)->getLinearVelocity().length();
    }
    for (int i = 0; i < _farmers.size(); i++) {
        positions[2 * i + 2* _carrots.size()] = _farmers.at(i)->getX() / scale;
        positions[2 * i + 1 + 2 * _carrots.size()] = 1 - (_farmers.at(i)->getY() - _farmers.at(i)->getHeight()/2) / scale * ratio;
        velocities[i + _carrots.size()] = _farmers.at(i)->getLinearVelocity().length();
    }
    for (int i = 0; i < _babies.size(); i++) {
        positions[2 * i + 2* (_carrots.size() + _farmers.size())] = _babies.at(i)->getX() / scale;
        positions[2 * i + 1 + 2 * (_carrots.size() + _farmers.size())] = 1 - (_babies.at(i)->getY() - _babies.at(i)->getHeight()/2) / scale * ratio;
        velocities[i + _carrots.size() + _farmers.size()] = _babies.at(i)->getLinearVelocity().length();
    }
    for (int i = 0; i < _rocks.size(); i++) {
        positions[2 * i + 2* (_carrots.size() + _farmers.size() + _babies.size())] = _rocks.at(i)->getX() / scale;
        positions[2 * i + 1 + 2 * (_carrots.size() + _farmers.size() + _babies.size())] = 1 - (_rocks.at(i)->getY() - _rocks.at(i)->getHeight()/2) / scale * ratio;
        velocities[i + _carrots.size() + _farmers.size() + _rocks.size()] = _rocks.at(i)->getLinearVelocity().length();
    }
    _shaderrenderer->update(step, perspective, size, positions, velocities, _character->getPosition() / scale * Vec2(1.0, ratio));
    _shaderedEntitiesNode->update(step);
}

void Map::resetPlantingSpots() {
    for(auto sp : _plantingSpot){
        sp->setCarrotPlanted(false);
        sp->setBelowAvatar(false);
    }
}

void Map::resetPlayers() {
    for(auto carrot : _carrots){
        carrot->resetCarrot();
    }
    _farmers.at(0)->resetFarmer();
}

std::shared_ptr<EntityModel> &Map::changeCharacter(std::string UUID) {
    _character->setLinearVelocity(Vec2::ZERO);
    _thisUUID = UUID;
    if (_thisUUID == _hostUUID) {
        _character = _farmers.at(0);
    } else {
        auto carrot = _carrots.begin();
        for (std::string uuid: _playerUUIDs) {
            if (uuid == _thisUUID) {
                _character = (*carrot);
                break;
            }
            carrot += uuid != _hostUUID;
        }
    }

    _character->getSceneNode()->setPriority(float(Map::DrawOrder::PLAYER));
    return _character;
}

void Map::destroyRock(std::shared_ptr<Collectible> rock) {
    // do not attempt to remove a rock that has already been removed
    if (rock->isRemoved()) {
        return;
    }
    if (!rock->isFired()) {
        _rockSpawns.at(rock->getSpawnIndex()).second = true;
        _numRockSpawns--;
    }
    rock->getDebugNode()->dispose();
    rock->getWheatHeightNode()->dispose();
    _entitiesNode->removeChild(rock->getSceneNode());
    rock->setDebugScene(nullptr);
    rock->markRemoved(true);
}

bool Map::shouldSpawnRock() {
    _spawnCooldown--;
    return _spawnCooldown < 0 && _numRockSpawns < MAX_NUM_COLLECTIBLES && _numRockSpawns < _rockSpawns.size() && _rand32() / _rand32.max() < SPAWN_RATE ;
}

std::pair<Vec2, int> Map::getRandomRockSpawn() {
    std::vector<int> valididxs;
    int i = 0;
    for (auto &spawn : _rockSpawns) {
        if (spawn.second) {
            valididxs.push_back(i);
        }
        i++;
    }
    int randidx = floor(float(_rand32()) / _rand32.max() * valididxs.size());
    auto &rand = _rockSpawns.at(valididxs.at(randidx));
    rand.second = false;
    _numRockSpawns++;
    _spawnCooldown = SPAWN_COOLDOWN;
    return std::pair(rand.first.origin, valididxs.at(randidx));
}

void Map::spawnRock(Vec2 pos, int idx, Vec2 vel) {
    auto rockTexture = _assets->get<Texture>("rock");

    auto rock = Collectible::alloc(pos, Vec2(0.5, 0.5), _scale.x, !vel.isZero());
    rock->setDebugColor(DEBUG_COLOR);
    rock->setName(vel.isZero() ? "rock_spawn" : "rock");
    rock->setSpawnIndex(idx);
    rock->setInitVelocity(vel);
    rock->setLinearVelocity(vel);
    
    auto rockNode = scene2::SpriteNode::allocWithSheet(rockTexture, 1, 1);
    rock->setSceneNode(rockNode);
    rock->setDrawScale(_scale.x);
    // set slightly below entities
    rockNode->setPriority(float(DrawOrder::ENTITIES) - 0.1);

    rockNode->setScale(0.05f * _scale/DEFAULT_DRAWSCALE);
    // Create the polygon node (empty, as the model will initialize)
    // 512 * scale
//    rockNode->setHeight(0.18f * _scale.y/DEFAULT_DRAWSCALE);
    rockNode->setName(vel.isZero() ? "rock_spawn" : "rock");
    _entitiesNode->addChild(rockNode);
    rock->setDebugScene(_debugnode);
    
    _rocks.push_back(rock);
    
    auto wheatnode = rock->allocWheatHeightNode(1);
    _wheatscene->getRoot()->addChild(wheatnode);
    
    _world->initObstacle(rock);
}
