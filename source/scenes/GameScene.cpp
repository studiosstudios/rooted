//
//  Created by Kimmy Lin on 2/23/24.
//

#include "GameScene.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "../objects/EntityModel.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;

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
/** Zoom of camera relative to scene */
#define CAMERA_ZOOM 1.5
/** Camera gliding rate */
#define CAMERA_GLIDE_RATE 0.06f

#pragma mark -
#pragma mark Physics Constants
/** No gravity because we are top down */
#define DEFAULT_GRAVITY 0.0f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240


#pragma mark -
#pragma mark Asset Constants
/** The font for victory/failure messages */
#define MESSAGE_FONT    "gyparody"
/** The message for winning the game */
#define WIN_MESSAGE     "YOU WIN!"
/** The color of the win message */
#define WIN_COLOR       Color4::BLUE
/** The message for losing the game */
#define LOSE_MESSAGE    "YOU LOSE!"
/** The color of the lose message */
#define LOSE_COLOR      Color4::RED
/** The message for resetting the game */
#define RESET_MESSAGE    "RESETTING"
/** The color of the reset message */
#define RESET_COLOR      Color4::YELLOW
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

#define JOY_MAIN        "joystick-main"
#define JOY_BACK        "joystick-back"

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::GREEN
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192


#pragma mark -
#pragma mark Constructors

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : Scene2(),
                         _complete(false),
                         _debug(false) {
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager> &assets) {
    // Initialize the scene to a locked width
    Size dimen = computeActiveSize();
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    _assets = assets;
    
    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _rootnode->setPosition(_offset);

    // Create the scene graph
    _uinode = scene2::SceneNode::alloc();
    _uinode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    
    // To be changed -CJ
//    _debugjoynode = scene2::PolygonNode::allocWithPoly(PolyFactory().makeRect(Vec2(0,0), Vec2(0.35f * 1024 / 1.5, 0.5f * 576 / 1.5)));
//    _debugjoynode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
//    _debugjoynode->setColor(Color4(Vec4(0, 0, 0, 0.25)));
//    _uinode->addChild(_debugjoynode);

    _winnode = scene2::Label::allocWithText(WIN_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _winnode->setAnchor(Vec2::ANCHOR_CENTER);
    _winnode->setPosition(dimen / 1.8f/ CAMERA_ZOOM);
    _winnode->setScale(1/CAMERA_ZOOM);
    _winnode->setForeground(WIN_COLOR);
    setComplete(false);

    _losenode = scene2::Label::allocWithText(LOSE_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _losenode->setAnchor(Vec2::ANCHOR_CENTER);
    _losenode->setPosition(dimen.width / 1.8f / CAMERA_ZOOM, dimen.height / 1.8f / CAMERA_ZOOM);
    _losenode->setScale(1/CAMERA_ZOOM);
    _losenode->setForeground(LOSE_COLOR);
    setFailure(false);

    _rootnode->setContentSize(Size(SCENE_WIDTH, SCENE_HEIGHT));
    
    _wheatrenderer = _wheatrenderer->alloc(_assets);
    
    _map = Map::alloc(_assets, _rootnode, assets->get<JsonValue>("map"), _wheatrenderer); // Obtains ownership of root.

    if (!_map->populate()) {
        CULog("Failed to populate map");
        return false;
    }
    
    _map->populateWithCarrots(_network->getNumPlayers() - 1);

    addChild(_rootnode);
    addChild(_uinode);
    _uinode->addChild(_winnode);
    _uinode->addChild(_losenode);

    // Create the world and attach the listeners.
    std::shared_ptr<physics2::ObstacleWorld> world = _map->getWorld();
    activateWorldCollisions(world);

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / world->getBounds().getMaxX() :
             dimen.height / world->getBounds().getMaxY();
    _offset = Vec2((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);
    
    _input = InputController::alloc(getBounds());
    _collision.init(_map, _network);
    _action.init(_map, _input, _network);
    _active = true;
    _complete = false;
    setDebug(false);
    
    // Network world synchronization
    // Won't compile unless I make this variable with type NetWorld :/
    if (_isHost) {
        _map->acquireMapOwnership();
    }
    _character = _map->loadPlayerEntities(_network->getOrderedPlayers(), _network->getNetcode()->getHost(), _network->getNetcode()->getUUID());
    _babies = _map->loadBabyEntities();
    
    std::shared_ptr<NetWorld> w = _map->getWorld();
    _network->enablePhysics(w);
    if (!_network->isHost()) {
        _network->getPhysController()->acquireObs(_character, 0);
    } else {
        for (auto baby : _babies) {
            _network->getPhysController()->acquireObs(baby, 0);
        }
    }
    
    // set the camera after all of the network is loaded
    _ui.init(_uinode, _offset, CAMERA_ZOOM);
    
    _cam.init(_map->getCharacter(), _rootnode, CAMERA_GLIDE_RATE, _camera, _uinode, 2.0f, _scale);
    _cam.setZoom(CAMERA_ZOOM);
    _initCamera = _cam.getCamera()->getPosition();

    // XNA nostalgia
//    Application::get()->setClearColor(Color4(142,114,78,255));
    Application::get()->setClearColor(Color4(118,118,118,255));
    
    _wheatrenderer->setScale(_scale);
    _wheatrenderer->setCamera(_cam.getCamera());
    _wheatrenderer->buildShaders();

    return true;
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<NetworkController> network, bool isHost) {
    _network = network;
    _isHost = isHost;
    
    return init(assets);
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        _input = nullptr;
        _rootnode = nullptr;
        _winnode = nullptr;
        _uinode = nullptr;
        _winnode = nullptr;
        _losenode = nullptr;
        _collision.dispose();
        _action.dispose();
        _ui.dispose();
        _complete = false;
        _debug = false;
        _map = nullptr;
        _wheatrenderer->dispose();
        _character = nullptr;
        unload();
        Scene2::dispose();
    }
}

void GameScene::unload() {
    for (auto it = _babies.begin(); it != _babies.end(); ++it) {
        (*it) = nullptr;
    }
    _babies.clear();
}


#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset() {
    // Load a new level
    _map->clearRootNode();
    _map->setRootNode(_rootnode);
    _map->dispose();
    _map->populate();
    _map->populateWithCarrots(_network->getNumPlayers() - 1);

    _collision.dispose();
    _action.dispose();

    std::shared_ptr<physics2::ObstacleWorld> world = _map->getWorld();
    activateWorldCollisions(world);

    _collision.init(_map, _network);
    _action.init(_map, _input, _network);

    if (_isHost) {
        _map->acquireMapOwnership();
    }
    _character = _map->loadPlayerEntities(_network->getOrderedPlayers(), _network->getNetcode()->getHost(), _network->getNetcode()->getUUID());
    _babies = _map->loadBabyEntities();
    
    std::shared_ptr<NetWorld> w = _map->getWorld();
    _network->enablePhysics(w);
    if (!_network->isHost()) {
        _network->getPhysController()->acquireObs(_character, 0);
    } else {
        for (auto baby : _babies) {
            _network->getPhysController()->acquireObs(baby, 0);
        }
    }
    
    Size dimen = computeActiveSize();
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / world->getBounds().getMaxX() :
             dimen.height / world->getBounds().getMaxY();
    _offset = Vec2((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);
    _cam.init(_map->getCharacter(), _rootnode, CAMERA_GLIDE_RATE, _camera, _uinode, 2.0f, _scale);
    
//
    //need to reset game state, otherwise gonna loop forever because gamestate is always in a position where a team has already won
    setDebug(false);
    setComplete(false);
    setFailure(false);
}

void GameScene::switchPlayer() {
    _map->togglePlayer();
    _map->clearRustling();
    if(_map->isFarmerPlaying()){
        _cam.setTarget(_map->getFarmers().at(0));
    }
    else{
        _cam.setTarget(_map->getCarrots().at(0));
    }
}

#pragma mark -
#pragma mark Physics Handling

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the beginning of the core application loop,
 * which is concluded with a call to {@link #postUpdate}.
 *
 * This method should be used to process any events that happen early in
 * the application loop, such as user input or events created by the
 * {@link schedule} method. In particular, no new user input will be
 * recorded between the time this method is called and {@link #postUpdate}
 * is invoked.
 *
 * Note that the time passed as a parameter is the time measured from the
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #postUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void GameScene::preUpdate(float dt) {
    if (_map == nullptr || _countdown >= 0) {
        return;
    }

    _input->update(dt);

    // Process the toggled key commands
    if (_input->didDebug()) { setDebug(!isDebug()); }
    if (_input->didReset()) {
        reset();
        return;
    }
    if (_input->didExit()) {
        CULog("Shutting down");
        Application::get()->quit();
    }
    
    if (_input->didShowPlayer()) { 
        _map->toggleShowPlayer();
    }
        
    // Test out wheat rustling via a key
    if (_input->didRustle()) {
        for (auto w : _map->getWheat()) {
            // Random number generator for testing
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, DASH_INTENSITY);

            // Generate and print random number
            int randomNumber = dis(gen);
            w->rustle(randomNumber);
        }
    }
    
    if(_input->didSwitch()) {
        switchPlayer();
    }

    // Process the movement
    if (_input->withJoystick()) {
        // do something here
    }

    _action.preUpdate(dt);
}

/**
 * The method called to provide a deterministic application loop.
 *
 * This method provides an application loop that runs at a guaranteed fixed
 * timestep. This method is (logically) invoked every {@link getFixedStep}
 * microseconds. By that we mean if the method {@link draw} is called at
 * time T, then this method is guaranteed to have been called exactly
 * floor(T/s) times this session, where s is the fixed time step.
 *
 * This method is always invoked in-between a call to {@link #preUpdate}
 * and {@link #postUpdate}. However, to guarantee determinism, it is
 * possible that this method is called multiple times between those two
 * calls. Depending on the value of {@link #getFixedStep}, it can also
 * (periodically) be called zero times, particularly if {@link #getFPS}
 * is much faster.
 *
 * As such, this method should only be used for portions of the application
 * that must be deterministic, such as the physics simulation. It should
 * not be used to process user input (as no user input is recorded between
 * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
 *
 * The time passed to this method is NOT the same as the one passed to
 * {@link #preUpdate}. It will always be exactly the same value.
 *
 * @param step  The number of fixed seconds for this step
 */
void GameScene::fixedUpdate(float step) {
    // Turn the physics engine crank.
    if (_countdown >= 0){
        return;
    }
    if (_network->isInAvailable()) {
//        CULog("NetEvent in queue, discarding for now");
    }
    _map->getWorld()->update(step);
    _ui.update(step, _cam.getCamera(), _input->withJoystick(), _input->getJoystick());
    _cam.update(step);
    
    auto carrots = _map->getCarrots();
    auto farmers = _map->getFarmers();
    auto babies = _map->getBabyCarrots();
    int size = carrots.size() + farmers.size() + babies.size();
    float positions[2*size]; // must be 1d array
    float velocities[size];
    float ratio = _wheatrenderer->getAspectRatio();
    for (int i = 0; i < carrots.size(); i++) {
        positions[2 * i] = carrots.at(i)->getX() / _scale;
        positions[2 * i + 1] = 1 - (carrots.at(i)->getY() - carrots.at(i)->getHeight()/2) / _scale * ratio;
        velocities[i] = carrots.at(i)->getLinearVelocity().length();
    }
    for (int i = 0; i < farmers.size(); i++) {
        positions[2 * i + 2* carrots.size()] = farmers.at(i)->getX() / _scale;
        positions[2 * i + 1 + 2 * carrots.size()] = 1 - (farmers.at(i)->getY() - farmers.at(i)->getHeight()/2) / _scale * ratio;
        velocities[i + carrots.size()] = farmers.at(i)->getLinearVelocity().length();
    }
    for (int i = 0; i < babies.size(); i++) {
        positions[2 * i + 2* (carrots.size() + farmers.size())] = babies.at(i)->getX() / _scale;
        positions[2 * i + 1 + 2 * (carrots.size() + farmers.size())] = 1 - (babies.at(i)->getY() - babies.at(i)->getHeight()/2) / _scale * ratio;
        velocities[i + carrots.size() + farmers.size()] = babies.at(i)->getLinearVelocity().length();
    }
    _wheatrenderer->update(step, size, positions, velocities);
    _action.fixedUpdate();
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the end of the core application loop, which was
 * begun with a call to {@link #preUpdate}.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.dpa
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * This method should not be used to process user input, as no new input
 * will have been recorded since {@link #preUpdate} was called.
 *
 * Note that the time passed as a parameter is the time measured from the
 * last call to {@link #fixedUpdate}. That is because this method is used
 * to interpolate object position for animation.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void GameScene::postUpdate(float remain) {
    // Since items may be deleted, garbage collect
    // Reset the game if we win or lose.
    if (_countdown > 0) {
        _countdown--;
    } else if (_countdown == 0) {
        reset();
    }
    else{
        _action.postUpdate(remain);
        
        _map->getWorld()->garbageCollect();
        
        bool farmerWin = true;
        for(auto carrot : _map->getCarrots()){
            if(!carrot->isRooted()){
                farmerWin = false;
            }
        }
        if(farmerWin){
            if(_isHost){
                setComplete(true);
            }
            else{
                setFailure(true);
            }
        }
        bool carrotWin = true;
        for(auto babyCarrot : _map->getBabyCarrots()){
            if(!babyCarrot->isCaptured()){
                carrotWin = false;
            }
        }
        if(carrotWin){
            if(_isHost){
                setFailure(true);
            }
            else{
                setComplete(true);
            }
        }
//        if(farmerWin || carrotWin){
//            _network->disconnect();
//        }
    }
}

/**
* Activates world collision callbacks on the given physics world and sets the onBeginContact and beforeSolve callbacks
*
* @param world the physics world to activate world collision callbacks on
*/
void GameScene::activateWorldCollisions(const std::shared_ptr<physics2::ObstacleWorld> &world) {
    world->activateCollisionCallbacks(true);
    world->activateFilterCallbacks(true);
    world->onBeginContact = [this](b2Contact *contact) {
        _collision.beginContact(contact);
    };
    world->onEndContact = [this](b2Contact *contact) {
        _collision.endContact(contact);
    };
    world->shouldCollide = [this](b2Fixture *f1, b2Fixture *f2) {
        return _collision.shouldCollide(f1, f2);
    };
}

/**
* Sets whether the level is completed.
*
* If true, the level will advance after a countdown
*
* @param value whether the level is completed.
*/
void GameScene::setComplete(bool value) {
    bool change = _complete != value;
    _complete = value;
    if (value && change) {
        std::shared_ptr<Sound> source = _assets->get<Sound>(WIN_MUSIC);
        AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
        _winnode->setVisible(true);
        _countdown = EXIT_COUNT;
    } else if (!value) {
        _winnode->setVisible(false);
        _countdown = -1;
    }
}

/**
 * Sets whether the level is failed.
 *
 * If true, the level will reset after a countdown
 
 * @param value whether the level is failed.
 */
void GameScene::setFailure(bool value) {
    _failed = value;
    if (value) {
        std::shared_ptr<Sound> source = _assets->get<Sound>(LOSE_MUSIC);
        AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
        _losenode->setVisible(true);
        _countdown = EXIT_COUNT;
    } else {
        _losenode->setVisible(false);
        _countdown = -1;
    }
}

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float) SCENE_WIDTH) / ((float) SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH / dimen.width;
    } else {
        dimen *= SCENE_HEIGHT / dimen.height;
    }
    return dimen;
}

void GameScene::render(const std::shared_ptr<SpriteBatch> &batch) {
//    _wheatrenderer->renderGround();
    Scene2::render(batch);
//    _wheatrenderer->renderWheat();
}

