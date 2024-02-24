//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Map.h"
#include "../objects/EntityModel.h"
#include "../objects/Spinner.h"
#include "../objects/RopeBridge.h"
#include "../objects/Bullet.h"

// TODO: put all constants into JSON

#pragma mark -
#pragma mark Level Geography

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 9.0/16.0

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
#define WALL_VERTS 12
#define WALL_COUNT  2

float M_WALL[WALL_COUNT][WALL_VERTS] = {
        {16.0f, 18.0f, 0.0f,  18.0f, 0.0f,  0.0f,
                1.0f,  0.0f,  1.0f,  17.0f, 16.0f, 17.0f},
        {32.0f, 18.0f, 16.0f, 18.0f, 16.0f, 17.0f,
                31.0f, 17.0f, 31.0f, 0.0f,  32.0f, 0.0f}
};

/** The number of platforms */
#define PLATFORM_VERTS  8
#define PLATFORM_COUNT  10

/** The outlines of all of the platforms */
float M_PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
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

/** The goal door position */
float M_GOAL_POS[] = {4.0f, 14.0f};
/** The position of the spinning barrier */
float M_SPIN_POS[] = {13.0f, 12.5f};
/** The initial position of the dude */
float M_DUDE_POS[] = {2.5f, 5.0f};
/** The position of the rope bridge */
float M_BRIDGE_POS[] = {9.0f, 3.8f};

#pragma mark -
#pragma mark Physics Constants
/** The new heavier gravity for this world (so it is not so floaty) */
#define DEFAULT_GRAVITY -28.9f
/** The density for most physics objects */
#define BASIC_DENSITY   0.0f
/** The density for a bullet */
#define HEAVY_DENSITY   10.0f
/** Friction of most platforms */
#define BASIC_FRICTION  0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION   0.1f
/** The width of the rope bridge */
#define BRIDGE_WIDTH    14.0f
/** Offset for bullet when firing */
#define BULLET_OFFSET   0.5f
/** The speed of the bullet after firing */
#define BULLET_SPEED   20.0f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240


#pragma mark -
#pragma mark Asset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE    "goal"
/** The key for the win door texture in the asset manager */
#define BULLET_TEXTURE  "bullet"
/** The name of a bullet (for object identification) */
#define BULLET_NAME     "bullet"
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"
/** The font for victory/failure messages */
#define MESSAGE_FONT    "retro"
/** The message for winning the game */
#define WIN_MESSAGE     "VICTORY!"
/** The color of the win message */
#define WIN_COLOR       Color4::YELLOW
/** The message for losing the game */
#define LOSE_MESSAGE    "FAILURE!"
/** The color of the lose message */
#define LOSE_COLOR      Color4::RED
/** The key the basic game music */
#define GAME_MUSIC      "game"
/** The key the victory game music */
#define WIN_MUSIC       "win"
/** The key the failure game music */
#define LOSE_MUSIC      "lose"
/** The sound effect for firing a bullet */
#define PEW_EFFECT      "pew"
/** The sound effect for a bullet collision */
#define POP_EFFECT      "pop"
/** The sound effect for jumping */
#define JUMP_EFFECT     "jump"
/** The volume for the music */
#define MUSIC_VOLUME    0.7f
/** The volume for sound effects */
#define EFFECT_VOLUME   0.8f
/** The image for the left dpad/joystick */
#define LEFT_IMAGE      "dpad_left"
/** The image for the right dpad/joystick */
#define RIGHT_IMAGE     "dpad_right"

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

        Poly2 wall(reinterpret_cast<Vec2 *>(M_WALL[ii]), WALL_VERTS / 2);
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
        Poly2 platform(reinterpret_cast<Vec2 *>(M_PLATFORMS[ii]), 4);

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
        addObstacle(platobj, sprite, worldnode, debugnode,1);
    }

#pragma mark : Dude
    Vec2 dudePos = M_DUDE_POS;
    image = assets->get<Texture>(DUDE_TEXTURE);
    auto avatar = Carrot::alloc(dudePos, image->getSize() / scale, scale);
    sprite = scene2::PolygonNode::allocWithTexture(image);
    sprite->setColor(Color4::ORANGE);
    avatar->setSceneNode(sprite);
    avatar->setDebugColor(DEBUG_COLOR);
    addObstacle(avatar, sprite, worldnode, debugnode); // Put this at the very front

    _carrots.push_back(avatar);

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

