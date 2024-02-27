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
/** The wall vertices */
#define WALL_VERTS 12
#define WALL_COUNT  2

float WALL[WALL_COUNT][WALL_VERTS] = {
        {16.0f, 18.0f, 0.0f,  18.0f, 0.0f,  0.0f,
                1.0f,  0.0f,  1.0f,  17.0f, 16.0f, 17.0f},
        {32.0f, 18.0f, 16.0f, 18.0f, 16.0f, 17.0f,
                31.0f, 17.0f, 31.0f, 0.0f,  32.0f, 0.0f}
};

/** The number of platforms */
#define PLATFORM_VERTS  8
#define PLATFORM_COUNT  10

/** The outlines of all of the platforms */
float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
        {1.0f,  3.0f,  1.0f,  2.5f,  6.0f,  2.5f,  6.0f,  3.0f},
        {6.0f,  4.0f,  6.0f,  2.5f,  9.0f,  2.5f,  9.0f,  4.0f},
        {23.0f, 4.0f,  23.0f, 2.5f,  31.0f, 2.5f,  31.0f, 4.0f},
        {26.0f, 5.5f,  26.0f, 5.0f,  28.0f, 5.0f,  28.0f, 5.5f},
        {29.0f, 7.0f,  29.0f, 6.5f,  31.0f, 6.5f,  31.0f, 7.0f},
        {24.0f, 8.5f,  24.0f, 8.0f,  27.0f, 8.0f,  27.0f, 8.5f},
        {29.0f, 10.0f, 29.0f, 9.5f,  31.0f, 9.5f,  31.0f, 10.0f},
        {23.0f, 11.5f, 23.0f, 11.0f, 27.0f, 11.0f, 27.0f, 11.5f},
        {19.0f, 12.5f, 19.0f, 12.0f, 23.0f, 12.0f, 23.0f, 12.5f},
        {1.0f,  12.5f, 1.0f,  12.0f, 7.0f,  12.0f, 7.0f,  12.5f}
};

/** The initial position of the dude */
float DUDE_POS[] = {2.5f, 5.0f};

#define WHEAT_COUNT     6
/** Positions of all of the wheat, is a single one for now */
float WHEAT_POS[WHEAT_COUNT][2] = {
    {7.0f, 7.0f},
    {8.0f, 7.0f},
    {9.0f, 7.0f},
    {7.0f, 8.0f},
    {8.0f, 8.0f},
    {9.0f, 8.0f},
};

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
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192

bool Map::init(const std::shared_ptr<cugl::AssetManager> &assets,
               std::shared_ptr<cugl::physics2::ObstacleWorld> &world,
               const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
               const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
               float scale) {

    _world = world;
    _scale = scale;

    _carrots.clear();
    _farmers.clear();
    _babies.clear();
    _wheat.clear();

#pragma mark : Walls
    // All walls and platforms share the same texture
    std::shared_ptr<Texture> image = assets->get<Texture>(EARTH_TEXTURE);
    std::shared_ptr<scene2::PolygonNode> sprite;
    std::shared_ptr<scene2::WireNode> draw;
    std::string wname = "wall";
    for (int ii = 0; ii < WALL_COUNT; ii++) {
        std::shared_ptr<physics2::PolygonObstacle> wallobj;

        Poly2 wall(reinterpret_cast<Vec2 *>(WALL[ii]), WALL_VERTS / 2);
        // Call this on a polygon to get a solid shape
        EarclipTriangulator triangulator;
        triangulator.set(wall.vertices);
        triangulator.calculate();
        wall.setIndices(triangulator.getTriangulation());
        triangulator.clear();

        wallobj = physics2::PolygonObstacle::allocWithAnchor(wall, Vec2::ANCHOR_CENTER);
        // You cannot add constant "".  Must stringify
        wallobj->setName(std::string(WALL_NAME) + cugl::strtool::to_string(ii));
        wallobj->setName(wname);

        // Set the physics attributes
        wallobj->setBodyType(b2_staticBody);
        wallobj->setDensity(BASIC_DENSITY);
        wallobj->setFriction(BASIC_FRICTION);
        wallobj->setRestitution(BASIC_RESTITUTION);
        wallobj->setDebugColor(DEBUG_COLOR);

        wall *= scale;
        sprite = scene2::PolygonNode::allocWithTexture(image, wall);
        addObstacle(wallobj, sprite, worldnode, debugnode, 1);  // All walls share the same texture
    }

#pragma mark : Platforms
    for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
        std::shared_ptr<physics2::PolygonObstacle> platobj;
        Poly2 platform(reinterpret_cast<Vec2 *>(PLATFORMS[ii]), 4);

        EarclipTriangulator triangulator;
        triangulator.set(platform.vertices);
        triangulator.calculate();
        platform.setIndices(triangulator.getTriangulation());
        triangulator.clear();

        platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_CENTER);
        // You cannot add constant "".  Must stringify
        platobj->setName(std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));

        // Set the physics attributes
        platobj->setBodyType(b2_staticBody);
        platobj->setDensity(BASIC_DENSITY);
        platobj->setFriction(BASIC_FRICTION);
        platobj->setRestitution(BASIC_RESTITUTION);
        platobj->setDebugColor(DEBUG_COLOR);

        platform *= scale;
        sprite = scene2::PolygonNode::allocWithTexture(image, platform);
        addObstacle(platobj, sprite, worldnode, debugnode, 1);
    }

#pragma mark : Dude
    Vec2 dudePos = DUDE_POS;
    image = assets->get<Texture>(DUDE_TEXTURE);
    auto avatar = Carrot::alloc(dudePos, image->getSize() / scale, scale);
    sprite = scene2::PolygonNode::allocWithTexture(image);
    sprite->setColor(Color4::ORANGE);
    avatar->setSceneNode(sprite);
    avatar->setDebugColor(DEBUG_COLOR);
    addObstacle(avatar, sprite, worldnode, debugnode); // Put this at the very front

    _carrots.push_back(avatar);

#pragma mark : Wheat
//    image = assets->get<Texture>(WHEAT_TEXTURE);
    for (int ii = 0; ii < WHEAT_COUNT; ii++) {
        Vec2 wheatPos = WHEAT_POS[ii];
        auto spriteImage = scene2::SpriteNode::allocWithSheet(assets->get<Texture>(WHEAT_TEXTURE),
                                                      1, WHEAT_FRAMES, WHEAT_FRAMES);
        auto singleWheat = Wheat::alloc(wheatPos, spriteImage->getSize() / scale, scale);
        
        singleWheat->setSceneNode(spriteImage);
        addObstacle(singleWheat, spriteImage, worldnode, debugnode);
        
        _wheat.push_back(singleWheat);
    }
    
    return true;
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
 * @param scale           The scale between the physics world and the screen (MUST BE UNIFORM)
 * @param useObjPosition  Whether to update the node's position to be at the object's position
 */
void Map::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle> &obj,
                      const std::shared_ptr<cugl::scene2::SceneNode> &node,
                      const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
                      const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
                      bool useObjPosition) {
    _world->addObstacle(obj);
    obj->setDebugScene(debugnode);

    // Position the scene graph node (enough for static objects)
    if (useObjPosition) {
        node->setPosition(obj->getPosition() * _scale);
    }
    worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode *weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle *obs) {
            weak->setPosition(obs->getPosition() * _scale);
            weak->setAngle(obs->getAngle());
        });
    }
}

