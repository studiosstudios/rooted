//
//  Created by Kimmy Lin on 2/23/24.
//

#include "GameScene.h"
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

/**
 * Converts a hexadecimal string to a decimal string
 *
 * This function assumes that the string is 4 hexadecimal characters
 * or less, and therefore it converts to a decimal string of five
 * characters or less (as is the case with the lobby server). We
 * pad the decimal string with leading 0s to bring it to 5 characters
 * exactly.
 *
 * @param hex the hexadecimal string to convert
 *
 * @return the decimal equivalent to hex
 */
static int hex2dec(const std::string hex) {
    Uint32 value = strtool::stou32(hex,0,16);
    std::string result = strtool::to_string(value);
    return stoi(result);
}

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
    
    _genTimes.push_back(0.1);
    _genTimes.push_back(0.2);
    _genTimes.push_back(0.3);
    _genTimes.push_back(0.4);
    _genTimes.push_back(0.5);
    _genTimes.push_back(0.6);
    _genTimes.push_back(0.7);
    _genTimes.push_back(0.8);
    _genTimes.push_back(1.0);
    _genTimes.push_back(1.4);
    _genTimes.push_back(1.8);
    _genTimes.push_back(2.2);
    _genTimes.push_back(2.8);
    
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

    _rootnode->setContentSize(Size(SCENE_WIDTH, SCENE_HEIGHT));
        
    _seed = hex2dec(_network->getRoomID());
    _map = Map::alloc(_assets, false); // Obtains ownership of root.
//    if (!_map->populate()) {
//        CULog("Failed to populate map");
//        return false;
//    }

    _numFarmers = 1;
    _numCarrots = _network->getNumPlayers() - 1;
    _numBabies = (_network->getNumPlayers() - 1) * 15 + 5;
    _numPlanting = 8;

    _map->generate(_seed, _numFarmers, _numCarrots, _numBabies, _numPlanting);
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
    float zoom = DEFAULT_CAMERA_ZOOM * DEFAULT_DRAWSCALE / _scale;
    addChild(_rootnode);
    addChild(_uinode);

    _input = InputController::alloc(getBounds(), _assets->get<cugl::JsonValue>("line-gesture"), _assets->get<cugl::JsonValue>("circle-gesture"));
    _collision.init(_map, _network);
    _action.init(_map, _input, _network, _assets, _haptics);
    _active = true;
    _complete = false;
    _soundScale = 1.0f;
    setDebug(false);
    
    // Network world synchronization
    // Won't compile unless I make this variable with type NetWorld :/
    if (_isHost) {
        _map->acquireMapOwnership();
        _babies = _map->loadBabyEntities();
    }
    _farmerUUID = _network->getOrderedPlayers().at(_seed % _network->getNumPlayers());
    _character = _map->loadPlayerEntities(_network->getOrderedPlayers(), _farmerUUID, _network->getNetcode()->getUUID());
    
    // TODO: Putting this set here for now, little weird that's it's separate from the rest of ui init -CJ
    _ui.setCharacter(_character);
    
    std::shared_ptr<NetWorld> w = _map->getWorld();
    _network->enablePhysics(w);
    if (!_network->isHost()) {
        _network->getPhysController()->acquireObs(_character, 0);
    } else {
        for (auto baby : _babies) {
            _network->getPhysController()->acquireObs(baby, 0);
        }
        for (auto obs : _map->getPlantingSpots()) {
            _network->getPhysController()->acquireObs(obs, 0);
        }
        for (auto obs : _map->getBoundaries()) {
            _network->getPhysController()->acquireObs(obs, 0);
        }
        _network->getPhysController()->acquireObs(_character, 0);
    }
    
    _network->attachEventType<ResetEvent>();
    _network->attachEventType<ReadyEvent>();
    
    // set the camera after all of the network is loaded
    _ui.init(_assets, _input, _uinode, _offset, zoom, _scale);
    setComplete(false);
    setFailure(false);
    _isGameOverScreen = false;
    
    _cam.init(_rootnode, CAMERA_GLIDE_RATE, _camera, _uinode, 32.0f, _scale, 
              Rect(_map->getMapBounds().origin/_map->getWorldBounds().size, _map->getMapBounds().size/_map->getWorldBounds().size), false);
    
    float mapX = _map->getMapBounds().size.width*_scale;
    float mapY = _map->getMapBounds().size.height*_scale;
    
    float beginning_zoom = std::max(dimen.width/mapX, dimen.height/mapY);
    _cam.setZoom(beginning_zoom);
    _mapGen = true;
    _cam.setPosition(_map->getCharacter()->getPosition() * _scale);
    
    _round = 1;
    _startTime = Timestamp();
    
    // using a vector because the positions should be the same each time
    // i.e. bunny, then each of the carrots
    for (int i = 0; i < _map->getPlayers().size(); i++) {
        _points.push_back(0);
    }
        
    _ready = 0;
    
    // XNA nostalgia
//    Application::get()->setClearColor(Color4(142,114,78,255));
    Application::get()->setClearColor(Color4::CLEAR);
        
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
        Haptics::stop();
        _rootnode = nullptr;
        _uinode = nullptr;
        _map = nullptr;
        _action.dispose();
        _collision.dispose();
        _ui.dispose();
        _complete = false;
        _debug = false;
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
//    if (!_mapGen) { _mapGen = true; }
    // Load a new level
    _seed++;
    _map->clearRootNode();
    _map->clearWorld();
    _map->generate(_seed, _numFarmers, _numCarrots, _numBabies, _numPlanting);
    _map->setRootNode(_rootnode);
    _map->populate();

    _ui.dispose();
    _collision.dispose();
    _action.dispose();

    std::shared_ptr<physics2::ObstacleWorld> world = _map->getWorld();
    activateWorldCollisions(world);

    _map->resetPlantingSpots();
    _map->resetPlayers();
    
    _collision.init(_map, _network);
    _action.init(_map, _input, _network, _assets, _haptics);

    if (_isHost) {
        _map->acquireMapOwnership();
        _babies = _map->loadBabyEntities();
    }
    _farmerUUID = _network->getOrderedPlayers().at(_seed % _network->getNumPlayers());
    _character = _map->loadPlayerEntities(_network->getOrderedPlayers(), _farmerUUID, _network->getNetcode()->getUUID());
    
    std::shared_ptr<NetWorld> w = _map->getWorld();
    _network->enablePhysics(w);
    if (!_network->isHost()) {
        _network->getPhysController()->acquireObs(_character, 0);
    } else {
        for (auto baby : _babies) {
            _network->getPhysController()->acquireObs(baby, 0);
        }
        for (auto obs : _map->getPlantingSpots()) {
            _network->getPhysController()->acquireObs(obs, 0);
        }
        for (auto obs : _map->getBoundaries()) {
            _network->getPhysController()->acquireObs(obs, 0);
        }
        _network->getPhysController()->acquireObs(_character, 0);
    }
    
    Size dimen = computeActiveSize();
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / world->getBounds().getMaxX() :
             dimen.height / world->getBounds().getMaxY();
    _offset = Vec2((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);
    _cam.init(_rootnode, CAMERA_GLIDE_RATE, _camera, _uinode, 32.0f, _scale,
              Rect(_map->getMapBounds().origin/_map->getWorldBounds().size, _map->getMapBounds().size/_map->getWorldBounds().size), false);
    
    float zoom = DEFAULT_CAMERA_ZOOM * DEFAULT_DRAWSCALE / _scale;
    float mapX = _map->getMapBounds().size.width*_scale;
    float mapY = _map->getMapBounds().size.height*_scale;
    
    float beginning_zoom = std::max(dimen.width/mapX, dimen.height/mapY);
    _cam.setZoom(beginning_zoom);
    _cam.setPosition(_map->getCharacter()->getPosition() * _scale);

    _ui.init(_assets, _input, _uinode, _offset, zoom, _scale);
//
    //need to reset game state, otherwise gonna loop forever because gamestate is always in a position where a team has already won
    setDebug(false);
    setComplete(false);
    setFailure(false);
    
    _isGameOverScreen = false;
    
    _round += 1;
    _startTime = Timestamp();
    
    _ready = 0;
    
    _ui.setEndVisible(false);
}

void GameScene::gameReset() {
    reset();
    // reset round and points
    _round = 1;
    // reset points
    std::fill(_points.begin(), _points.end(), 0);
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
    _time += dt;
    if (_mapGen) {
        CULog("time: %f", _time);
        if (_genTimes.size() > 0) {
            if (_time > _genTimes.front() && !_justGenerated) {
                _justGenerated = true;
                _genTimes.pop_front();
                reset();
            } else if (_time > _genTimes.front()) {
                _justGenerated = false;
            }
        }
    }
  
    if (_map == nullptr || (_countdown >= 0 && _network->getNumPlayers() > 1)) {
        return;
    }

    _input->update(dt);

    // Process the toggled key commands
    if (_input->didDebug()) { setDebug(!isDebug()); }
    if (_input->didReset()) {
        _network->pushOutEvent(ResetEvent::allocResetEvent());
        return;
    }
    if (_input->didExit()) {
        CULog("Shutting down");
        Application::get()->quit();
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
    if (_mapGen) {
        _map->getWorld()->update(step);
        _map->updateShaders(step, _cam.getCamera()->getCombined());
        _cam.setShake(_character->getStunTime() * STUN_SCREEN_SHAKE);
        _cam.setTarget(_character->getPosition()*_scale);
        _cam.update(step);
    }
    else {
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
            if(auto captureBarrotEvent = std::dynamic_pointer_cast<CaptureBarrotEvent>(e)){
                _action.processBarrotEvent(captureBarrotEvent);
            }
            if(auto resetEvent = std::dynamic_pointer_cast<ResetEvent>(e)){
                processResetEvent(resetEvent);
            }
            if(auto moveEvent = std::dynamic_pointer_cast<MoveEvent>(e)){
                _action.processMoveEvent(moveEvent);
            }
            if(auto freeEvent = std::dynamic_pointer_cast<FreeEvent>(e)){
                _action.processFreeEvent(freeEvent);
            }
            if(auto spawnRockEvent = std::dynamic_pointer_cast<SpawnRockEvent>(e)){
                _action.processSpawnRockEvent(spawnRockEvent);
            }
            if(auto collectedRockEvent = std::dynamic_pointer_cast<CollectedRockEvent>(e)){
                _action.processCollectedRockEvent(collectedRockEvent);
            }
            if(auto readyEvent = std::dynamic_pointer_cast<ReadyEvent>(e)) {
                _ready += 1;
            }
        }
        if (_countdown >= 0 && _network->getNumPlayers() > 1){
            return;
        }
        
        _map->getWorld()->update(step);
        _cam.setShake(_character->getStunTime() * STUN_SCREEN_SHAKE);
        _cam.setTarget(_character->getPosition()*_scale);
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
    if (_mapGen) {
        if (_genTimes.size() == 0 && _time > 3.0f) {
            _justGenerated = false;
            _mapGen = false;
            _time = 0.0;
        }
        _map->getWorld()->garbageCollect();
    }
    else {
        // Reset the game if we win or lose.
        
        _ui.update(remain, _cam.getCamera(), getCarrotsLeft(), _map->getBabyCarrots().size(), _debug, _character->canDash());
            
        if (_countdown > 0) {
            _countdown--;
        } else if (_countdown == 0 && _network->getNumPlayers() > 1) {
            // are we displaying the end game screen
            if (!_isGameOverScreen) {
                _isGameOverScreen = true;
                
                // get the vector of carrots that have been rooted
                vector<int> carrots;
                for (auto it = _map->getCarrots().begin(); it != _map->getCarrots().end(); it++) {
                    if ((*it)->isRooted()) {
                        carrots.push_back(_map->getCarrotTypeForUUID((*it)->getUUID()));
                    }
                }
                
                // set how the end screen should display
                _ui.setEndVariables(_round, (Timestamp()).ellapsedMillis(_startTime), _map->getBabyCarrots().size(), carrots, _points, _map->getCarrotTypeForUUID(_character->getUUID()));
                
                // display end scene
                _ui.setEndVisible(true);
            }

            if (_ready == _network->getNumPlayers() && _network->isHost()) {
                _network->pushOutEvent(ResetEvent::allocResetEvent());
                _ready = 0; // need this otherwise it will send out two of these events
            }
            
            if (_ui.getNextRound() == 1){
                CULog("hello pushing out event");
                _ui.setNextRound(2);
                _network->pushOutEvent(ReadyEvent::alloc());
            }
            else{
                //do nothing and wait for host to reset
            }
        }
        else{
            _action.postUpdate(remain);

            // Since items may be deleted, garbage collect
            _map->getWorld()->garbageCollect();
            
            bool farmerWin = _map->getCarrots().size() > 0;
            for(auto carrot : _map->getCarrots()){
                if(!carrot->isRooted()){
                    farmerWin = false;
                }
            }
            if(farmerWin){
                // add points for farmer
                _points[_map->getCarrotTypeForUUID(_farmerUUID)] += 3;

                if(_character->getUUID() == _farmerUUID){
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
                // add points for carrot
                for (int ii = 0; ii < _points.size(); ii++) {
                    if (ii != _map->getCarrotTypeForUUID(_farmerUUID)) {
                        _points[ii] += 1;
                    }
                }
                if(_character->getUUID() == _farmerUUID){
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
        _map->getCharacter()->getSceneNode()->TexturedNode::setIsPlayer(true);
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
    world->beforeSolve = [this](b2Contact* contact, const b2Manifold* manifold) {
        _collision.beforeSolve(contact, manifold);
    };
    world->afterSolve = [this](b2Contact* contact, const b2ContactImpulse* impulse) {
        _collision.afterSolve(contact, impulse);
    };
}

void GameScene::pauseNonEssentialAudio(){
    AudioEngine::get()->clear("root-carrot");
    AudioEngine::get()->clear("root-bunny");
    AudioEngine::get()->clear(_map->getFarmers().at(0)->getUUID());
    for(auto carrot:_map->getCarrots()){
        AudioEngine::get()->clear(carrot->getUUID());
    }
    AudioEngine::get()->clear("barrot");
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
        pauseNonEssentialAudio();
        std::shared_ptr<Sound> source;
        if(_map->isFarmer()){
            source = _assets->get<Sound>(F_WIN_MUSIC);
        }
        else{
            source = _assets->get<Sound>(C_WIN_MUSIC);
        }
        AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME*_soundScale);
        _ui.setWinVisible(true);
        _countdown = EXIT_COUNT;
    } else if (!value) {
        _ui.setWinVisible(false);
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
        pauseNonEssentialAudio();
        std::shared_ptr<Sound> source;
        if(_map->isFarmer()){
            source = _assets->get<Sound>(F_LOSE_MUSIC);
        }
        else{
            source = _assets->get<Sound>(C_LOSE_MUSIC);
        }
        AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME*_soundScale);
        _ui.setLoseVisible(true);
        _countdown = EXIT_COUNT;
    } else {
        _ui.setLoseVisible(false);
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
    _map->getWheatScene()->render(batch);
    Scene2::render(batch);
//    _map->getWheatScene()->renderToScreen(1.0); //for debugging the wheat texture
}

void GameScene::processResetEvent(const std::shared_ptr<ResetEvent>& event) {
    _network->disablePhysics();
    while(_network->isInAvailable()){
        _network->popInEvent();
    }
    reset();
}

int GameScene::getCarrotsLeft() {
    int i = _network->getNumPlayers() - 1;
    for (auto it = _map->getPlantingSpots().begin(); it != _map->getPlantingSpots().end(); it++) {
        if ((*it)->getCarrotPlanted()) {
            i--;
        }
    }
    return i;
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
void GameScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            // just reset the scene
            gameReset();
        } else {
            // stop everything that we don't want in this scene
            _map->clearRootNode();
            _map->dispose();
            _ui.dispose();
            _collision.dispose();
            _action.dispose();
        }
    }
}
