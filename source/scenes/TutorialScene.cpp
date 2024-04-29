//
// Created by Jeffrey Zhou on 4/25/24.
//

#include "TutorialScene.h"

#include "TutorialScene.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "../objects/EntityModel.h"
#include "../RootedConstants.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;


#pragma mark -
#pragma mark Constructors

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
TutorialScene::TutorialScene() : Scene2(),
_debug(false),
_returnToMenu(false) {
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
bool TutorialScene::init(const std::shared_ptr<AssetManager> &assets) {
    // Initialize the scene to a locked width
    Size dimen = computeActiveSize();
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    _network = MockNetworkController::alloc();
    _assets = assets;

    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _rootnode->setPosition(_offset);

    _rootnode->setContentSize(Size(SCENE_WIDTH, SCENE_HEIGHT));

    _map = Map::alloc(_assets, true); // Obtains ownership of root.

    _map->generate(0, 1, 2, 10, 2);
    _map->setRootNode(_rootnode);
    _map->populate();

    // Create the scene graph
    _uinode = scene2::SceneNode::alloc();
    _uinode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

    // Create the world and attach the listeners.
    std::shared_ptr<physics2::ObstacleWorld> world = _map->getWorld();
    activateWorldCollisions(world);

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / world->getBounds().getMaxX() :
             dimen.height / world->getBounds().getMaxY();
    _offset = Vec2((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);
    float zoom = DEFAULT_CAMERA_ZOOM * DEFAULT_DRAWSCALE / _scale * std::max(dimen.width/SCENE_WIDTH, dimen.height/SCENE_HEIGHT);
    addChild(_rootnode);
    addChild(_uinode);

    _input = InputController::alloc(getBounds()); 
    _collision.init(_map, _network);
    _action.init(_map, _input, _network, _assets);
    setDebug(false);

    _map->acquireMapOwnership();
    _babies = _map->loadBabyEntities();
    
    _farmerUUID = "farmer";
    _carrotUUID = "carrot";
    _carrot2UUID = "carrot2";
    _character = _map->loadPlayerEntities(std::vector<std::string>{_farmerUUID, _carrotUUID, _carrot2UUID}, _farmerUUID, _carrotUUID);

    _network->attachEventType<ResetEvent>();

    // set the camera after all of the network is loaded
    _ui.init(_assets, _input, _uinode, _offset, zoom, _scale);

    _cam.init(_rootnode, CAMERA_GLIDE_RATE, _camera, _uinode, 32.0f, _scale, _map->getMapBounds().size/_map->getWorldBounds().size);
    _cam.setZoom(zoom);
    _cam.setPosition(Vec2(_map->getMapBounds().size/2.0) * _scale);
    _cam.setTarget(Vec2(_map->getMapBounds().size/2.0) * _scale);
    _initCamera = _cam.getCamera()->getPosition();

    // XNA nostalgia
//    Application::get()->setClearColor(Color4(142,114,78,255));
    Application::get()->setClearColor(Color4::CLEAR);
    _cam.setFrac(Vec2(1.0/3.0, 2.0/3.0));
    _action.getAIController()->setBabyBounds(Rect(0,0,MAP_UNIT_WIDTH, MAP_UNIT_HEIGHT));
    
    _input->pause();
    _state = TutorialState::JAILBREAK;
    _time = 0;
    
    _black = scene2::PolygonNode::allocWithPoly(Rect(0,0,SCENE_WIDTH,SCENE_HEIGHT));
    _black->setColor(Color4::CLEAR);
    addChild(_black);
    _map->getBabyCarrots().at(9)->setBodyType(b2BodyType::b2_staticBody);
    
    _pausePhysics = false;

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void TutorialScene::dispose() {
    if (_active) {
        _input = nullptr;
        _rootnode = nullptr;
        _uinode = nullptr;
        _collision.dispose();
        _action.dispose();
        _ui.dispose();
        _debug = false;
        _map = nullptr;
        _character = nullptr;
        unload();
        Scene2::dispose();
    }
}

void TutorialScene::unload() {
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
void TutorialScene::reset() {
    // Load a new level
    _map->clearRootNode();
    _map->dispose();
    _map->generate(0, 1, 2, 10, 2);
    _map->setRootNode(_rootnode);
    _map->populate();

    _ui.dispose();
    _collision.dispose();
    _action.dispose();

    std::shared_ptr<physics2::ObstacleWorld> world = _map->getWorld();
    activateWorldCollisions(world);

    _map->resetPlantingSpots();
    _map->resetPlayers();

    _map->acquireMapOwnership();
    _babies = _map->loadBabyEntities();
    
    _farmerUUID = "farmer";
    _carrotUUID = "carrot";
    _character = _map->loadPlayerEntities(std::vector<std::string>{_farmerUUID, _carrotUUID, _carrot2UUID}, _farmerUUID, _carrotUUID);
    
    _collision.init(_map, _network);
    _action.init(_map, _input, _network, _assets);

    Size dimen = computeActiveSize();
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / world->getBounds().getMaxX() :
             dimen.height / world->getBounds().getMaxY();
    _offset = Vec2((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);

    float zoom = DEFAULT_CAMERA_ZOOM * DEFAULT_DRAWSCALE / _scale;
    _cam.setZoom(zoom);

    _ui.init(_assets, _input, _uinode, _offset, zoom, _scale);
    _cam.init(_rootnode, CAMERA_GLIDE_RATE, _camera, _uinode, 32.0f, _scale, _map->getMapBounds().size/_map->getWorldBounds().size);
    _cam.setZoom(zoom);
    _cam.setPosition(Vec2(_map->getMapBounds().size/2.0) * _scale);
    _cam.setTarget(Vec2(_map->getMapBounds().size/2.0) * _scale);
    _initCamera = _cam.getCamera()->getPosition();

    // XNA nostalgia
//    Application::get()->setClearColor(Color4(142,114,78,255));
    Application::get()->setClearColor(Color4::CLEAR);
    _cam.setFrac(Vec2(1.0/3.0, 2.0/3.0));
    _action.getAIController()->setBabyBounds(Rect(0,0,MAP_UNIT_WIDTH, MAP_UNIT_HEIGHT));
//
    //need to reset game state, otherwise gonna loop forever because gamestate is always in a position where a team has already won
    setDebug(false);
    
    _network->setHost(true);
    _input->pause();
    _state = TutorialState::JAILBREAK;
    _time = 0;
    _black = scene2::PolygonNode::allocWithPoly(Rect(0,0,SCENE_WIDTH,SCENE_HEIGHT));
    _black->setColor(Color4::CLEAR);
    addChild(_black);
    _map->getBabyCarrots().at(9)->setBodyType(b2BodyType::b2_staticBody);
    
    _pausePhysics = false;
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
void TutorialScene::preUpdate(float dt) {
    _time += dt;
    
    _input->update(dt);
    
    // Process the toggled key commands
    if (_input->didDebug()) { setDebug(!isDebug()); }
    if (_input->didReset()) {
        _network->pushOutEvent(ResetEvent::allocResetEvent());
        return;
    }
    // to exit to menu/quit out of tutorial
    if (_input->didExit()) {
        // switch screen back to the menu
        _returnToMenu = true;
        return;
    }
    
    switch (_state) {
        case JAILBREAK:
            //wait a bit until baby carrots move in
            if (_time > 1.0) {
                _action.updateBabyCarrots();
                
                //move camera down
                if (_time > 3.0) {
                    float f = (_time - 3.0) / 3.0;
                    _cam.setTarget(_map->getMapBounds().size.width/2.0 * _scale,
                                   (1-f) * _map->getMapBounds().size.height/2.0 * _scale +
                                   f * MAP_UNIT_HEIGHT / 2.0 * _scale);
                }
                
                //move camera back up
                if (_time > 7.0) {
                    _cam.setTarget(_map->getMapBounds().size.width/2.0 * _scale,
                                   _map->getMapBounds().size.height/2.0 * _scale);
                    _character->setX(_map->getMapBounds().size.width/2.0);
                }
                
                //move player down
                if (_time > 7.5 && _time < 9.75) {
                    _character->setMovement(Vec2(0,-1.0));
                } else {
                    _character->setMovement(Vec2::ZERO);
                }
                _character->updateState();
                _character->applyForce();
                _character->stepAnimation(dt);
                
            }
            break;
        case MOVEMENT:
            _action.preUpdate(dt);
            _cam.setTarget(_character->getPosition()*_scale);
            break;
        case CATCHBABIES:
            _action.preUpdate(dt);
            _cam.setTarget(_character->getPosition()*_scale);
            break;
        case SHOWFARMER:
            {
            if (_character->getName() == "carrot") { //wait until carrot animation finishes
                _action.preUpdate(dt);
                if (_character->getEntityState() == EntityModel::EntityState::STANDING) {
                    _map->getFarmers().at(0)->setX(_character->getX());
                    changeCharacter(_farmerUUID);
                    _time = 0;
                }
            } else {
                _action.updateBabyCarrots();
                
                //move farmer
                if (_time > 0.5 && _time < 3.0) {
                    _character->setMovement(Vec2(0, -1));
                    _character->setDashInput(fmod((_time - dt), 0.7) > fmod(_time, 0.7)); //dash every 0.7s
                } else {
                    _character->setMovement(Vec2::ZERO);
                }
                _character->updateState();
                _character->applyForce();
                _character->stepAnimation(dt);
                
                //fade to black
                if (_time > 2.5) {
                    _black->setColor(Color4(0,0,0,std::min((_time-2.5) * 2 * 255,  255.0)));
                }
                
            }
                _cam.setTarget(_character->getPosition()*_scale);
            }
            break;
        case ESCAPEFARMER:
        {
            _black->setColor(Color4(0,0,0, std::max(255.0 - _time * 2 * 255,  0.0)));
            changeCharacter(_carrotUUID);
            _cam.setTarget(_character->getPosition()*_scale);
            _action.preUpdate(dt);
            _action.updateBabyCarrots();
            
            //move farmer
            auto farmer = _map->getFarmers().at(0);
            float x = 0;
            float mod = fmod(_time + 1, 6);
            if (mod > 3 && mod < 5) {
                x = 0.2;
            } else if (mod < 2) {
                x = -0.2;
            }
            farmer->setMovement(Vec2(x, 0));
            farmer->updateState();
            farmer->applyForce();
            farmer->stepAnimation(dt);

            break;
        }
        case ROCK:
            break;
        case UNROOT: {
            if (_time > 0.25 && _time < 2.5) {
                _character = _map->changeCharacter(_carrot2UUID);
                _cam.setTarget(_character->getPosition()*_scale);
                _character = _map->changeCharacter(_carrotUUID);
                _pausePhysics = true;
            } else {
                _character = _map->changeCharacter(_carrotUUID);
                _cam.setTarget(_character->getPosition()*_scale);
            }
            
            if (_time > 3.0) {
                _input->unpause();
                _pausePhysics = false;
                
                _action.preUpdate(dt);
                _action.updateBabyCarrots();
                
                //move farmer
                auto farmer = _map->getFarmers().at(0);
                farmer->setMovement((_character->getPosition() - farmer->getPosition()).getNormalization() * 0.2);
                farmer->updateState();
                farmer->applyForce();
                farmer->stepAnimation(dt);
            }
        }
            break;
        case CARROTLEAVES: {
            _action.preUpdate(dt);
            _cam.setTarget(_character->getPosition()*_scale);
            
            //move farmer
            auto farmer = _map->getFarmers().at(0);
            farmer->setMovement((_character->getPosition() - farmer->getPosition()).getNormalization() * 0.2);
            farmer->updateState();
            farmer->applyForce();
            farmer->stepAnimation(dt);
            
            //move unrooted carrot
            auto carrot = _map->getCarrots().at(1);
            carrot->setMovement(Vec2(-1, 0));
            carrot->updateState();
            carrot->setSensor(true);
            carrot->applyForce();
            carrot->stepAnimation(dt);
        }
            break;
        case BABYENTERS: {
            if (_map->getBabyCarrots().at(0)->getY() < 17) {
                _cam.setTarget(_map->getBabyCarrots().at(0)->getPosition()*_scale);
                if (!_pausePhysics) {
                    _pausePhysics = true;
                    _input->pause();
                    _time = 0;
                }
            } else {
                _action.preUpdate(dt);
                _action.updateBabyCarrots();
                _cam.setTarget(_character->getPosition()*_scale);
                
                //move farmer
                auto farmer = _map->getFarmers().at(0);
                farmer->setMovement((_character->getPosition() - farmer->getPosition()).getNormalization() * 0.2);
                farmer->updateState();
                farmer->applyForce();
                farmer->stepAnimation(dt);
            }
            
        }
            break;
        case LASTBABY:
            {
                _action.preUpdate(dt);
                _action.updateBabyCarrots();
                _cam.setTarget(_character->getPosition()*_scale);
                
                //move farmer
                auto farmer = _map->getFarmers().at(0);
                farmer->setMovement((_character->getPosition() - farmer->getPosition()).getNormalization() * 0.2);
                farmer->updateState();
                farmer->applyForce();
                farmer->stepAnimation(dt);
            }
            break;
        case CARROTWIN:
            break;
        case SWITCH:
            _black->setColor(Color4(0,0,0, std::min(_time * 2.0 * 255,  255.0)));
            break;
        case CATCHCARROT:
            break;
        case ROOT:
            break;
        case FARMERWIN:
            break;
        case FREEPLAY:
            break;
    }
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
void TutorialScene::fixedUpdate(float step) {
    // Turn the physics engine crank.
    while(_network->isInAvailable()){
        auto e = _network->popInEvent();
        if(auto captureEvent = std::dynamic_pointer_cast<CaptureEvent>(e)){
            //            CULog("Received dash event");
            _action.processCaptureEvent(captureEvent);
        }
        if(auto rootEvent = std::dynamic_pointer_cast<RootEvent>(e)){
            //            std::cout<<"got a root event\n";
            _action.processRootEvent(rootEvent);
        }
        if(auto unrootEvent = std::dynamic_pointer_cast<UnrootEvent>(e)){
            _action.processUnrootEvent(unrootEvent);
        }
        if (_state == CATCHBABIES || _state == LASTBABY) {
            if(auto captureBarrotEvent = std::dynamic_pointer_cast<CaptureBarrotEvent>(e)){
                _action.processBarrotEvent(captureBarrotEvent);
            }
        }
        if(auto resetEvent = std::dynamic_pointer_cast<ResetEvent>(e)){
            processResetEvent(resetEvent);
        }
        if(auto freeEvent = std::dynamic_pointer_cast<FreeEvent>(e)){
            _action.processFreeEvent(freeEvent);
        }
    }
     
    if (!_pausePhysics) _map->getWorld()->update(step);
    _cam.update(step);

    _map->updateShaders(step, _cam.getCamera()->getCombined());


    //check if entities are in wheat
    //TODO: make entities vector in map for convenience?
    //not entirely sure if it is ok to put this here because of opengl stuff but so far it seems fine

    //create queries
    for (auto baby : _map->getBabyCarrots()) {
        baby->setWheatQueryId(_map->getWheatScene()->addWheatQuery(baby->getPosition() - Vec2(0, baby->getHeight()/2)));
    }
    for (auto farmer : _map->getFarmers()) {
        farmer->setWheatQueryId(_map->getWheatScene()->addWheatQuery(farmer->getPosition() - Vec2(0, farmer->getHeight()/2)));
        //farmer->getSceneNode()->setColor(farmer->isInWheat() ? Color4::RED : Color4::WHITE);
    }
    for (auto carrot : _map->getCarrots()) {
        carrot->setWheatQueryId(_map->getWheatScene()->addWheatQuery(carrot->getPosition() - Vec2(0, carrot->getHeight()/2)));
    }

    //resolve queries
    _map->getWheatScene()->doQueries();

    //fetch results
    for (auto farmer : _map->getFarmers()) {
        farmer->setInWheat(_map->getWheatScene()->getWheatQueryResult(farmer->getWheatQueryId()));
    }
    for (auto carrot : _map->getCarrots()) {
        carrot->setInWheat(_map->getWheatScene()->getWheatQueryResult(carrot->getWheatQueryId()));
    }
    for (auto baby : _map->getBabyCarrots()) {
        baby->setInWheat(_map->getWheatScene()->getWheatQueryResult(baby->getWheatQueryId()));
//        baby->getSceneNode()->setColor(baby->isInWheat() ? Color4::RED : Color4::WHITE);
    }
    _map->getWheatScene()->clearQueries();
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
void TutorialScene::postUpdate(float remain) {
    
    // do state transitions
    switch (_state) {
        case JAILBREAK:
            if (_time > 10.0) {
                CULog("MOVE WITH JOYSTICK");
                _state = MOVEMENT;
                _input->unpause();
                _time = 0;
            }
            break;
        case MOVEMENT:
            if (_character->getY() < 10) {
                CULog("CATCH BABY CARROTS! SWIPE TO DASH");
                _state = CATCHBABIES;
                _time = 0;
            }
            break;
        case CATCHBABIES:
            if (_map->getBabyCarrots().size() == 1) {
                _input->pause();
                _state = SHOWFARMER;
                _time = 0;
            }
            break;
        case SHOWFARMER:
            if (_time > 4.5) {
                CULog("ESCAPE FROM THE FARMER BY SHAKING DEVICE");
                _input->unpause();
                
                //root the npc carrot
                auto plantingspot = _map->getPlantingSpots().at(1);
                changeCharacter(_carrot2UUID);
                _character->setPosition(plantingspot->getPosition());
                changeCharacter(_farmerUUID);
                _character->setPosition(plantingspot->getPosition());
                auto e = RootEvent::allocRootEvent(_carrot2UUID, plantingspot->getPlantingID());
                _action.processRootEvent(std::dynamic_pointer_cast<RootEvent>(e));
                
                //capture the player carrot
                float x = _map->getMapBounds().size.width/2.0;
                float y = _map->getMapBounds().size.height/2.0;
                _character->setPosition(x, y);
                changeCharacter(_carrotUUID);
                _character->setPosition(x, y);
                e = CaptureEvent::allocCaptureEvent(_character->getUUID());
                _action.processCaptureEvent(std::dynamic_pointer_cast<CaptureEvent>(e));
                
                _state = ESCAPEFARMER;
                _cam.setPosition(_character->getPosition() * _scale);
                _time = 0;
            }
            break;
        case ESCAPEFARMER:
            if (_time > 1.0 && !(std::dynamic_pointer_cast<Carrot>(_character))->isCaptured()) {
                _input->pause();
                _time = 0;
                _state = UNROOT;
                CULog("unroot your friend");
            }
            break;
        case UNROOT:
            if (!_map->getCarrots().at(1)->isRooted()) {
                _time = 0;
                _map->getCarrots().at(1)->setSensor(true);
                _state = CARROTLEAVES;
            }
            break;
        case CARROTLEAVES:
            if (_map->getCarrots().at(1)->getX() < -2) {
                _time = 0;
                _map->getBabyCarrots().at(0)->setBodyType(b2BodyType::b2_dynamicBody);
                _state = BABYENTERS;
            }
            break;
        case BABYENTERS:
            if (_pausePhysics && _time > 2.0) {
                _time = 0;
                _pausePhysics = false;
                _input->unpause();
                _cam.setTarget(_character->getPosition()*_scale);
                _state = LASTBABY;
            }
            break;
        case LASTBABY:
            if (_map->getBabyCarrots().size() == 0) {
                _input->pause();
                _state = CARROTWIN;
                _pausePhysics = true;
                _time = 0;
                
                pauseNonEssentialAudio();
                std::shared_ptr<Sound> source = _assets->get<Sound>(WIN_MUSIC);
                AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
                _ui.setWinVisible(true);
            }
            break;
        case ROCK:
            break;
        case CARROTWIN:
            if (_time > 1.5){
                _state = SWITCH;
                _time = 0;
            }
            break;
        case SWITCH:
            break;
        case CATCHCARROT:
            break;
        case ROOT:
            break;
        case FARMERWIN:
            break;
        case FREEPLAY:
            break;
    }

    // TEMP CODE FOR OPEN BETA - CJ
    int i = _network->getNumPlayers() - 1;
    for (auto it = _map->getPlantingSpots().begin(); it != _map->getPlantingSpots().end(); it++) {
        if ((*it)->getCarrotPlanted()) {
            i--;
        }
    }
    // TEMP CODE FOR OPEN BETA

    _ui.update(remain, _cam.getCamera(), i, _map->getBabyCarrots().size(), _debug);
    _action.postUpdate(remain);

    // Since items may be deleted, garbage collect
    _map->getWorld()->garbageCollect();

    _map->getCharacter()->getSceneNode()->TexturedNode::setIsPlayer(true);

}

/**
* Activates world collision callbacks on the given physics world and sets the onBeginContact and beforeSolve callbacks
*
* @param world the physics world to activate world collision callbacks on
*/
void TutorialScene::activateWorldCollisions(const std::shared_ptr<physics2::ObstacleWorld> &world) {
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

void TutorialScene::pauseNonEssentialAudio(){
    AudioEngine::get()->clear("root-carrot");
    AudioEngine::get()->clear("root-bunny");
    AudioEngine::get()->clear(_map->getFarmers().at(0)->getUUID());
    for(auto carrot:_map->getCarrots()){
        AudioEngine::get()->clear(carrot->getUUID());
    }
}

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size TutorialScene::computeActiveSize() const {
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

void TutorialScene::render(const std::shared_ptr<SpriteBatch> &batch) {
    _map->getWheatScene()->render(batch);
    Scene2::render(batch);
//    _map->getWheatScene()-> renderToScreen(1.0); //for debugging the wheat texture
}

void TutorialScene::processResetEvent(const std::shared_ptr<ResetEvent>& event){
    _network->disablePhysics();
    while(_network->isInAvailable()){
        _network->popInEvent();
    }
    reset();
}

/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void TutorialScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            // TODO: currently does not reset the scene
            _returnToMenu = false;
        }
    }
}

void TutorialScene::changeCharacter(std::string UUID) {
    _network->setHost(UUID == _farmerUUID);
    _character = _map->changeCharacter(UUID);
}
