//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"
#include "NetworkController.h"

using namespace cugl;

/** Time after dashing when carrot can be captured */
#define CAPTURE_TIME    10 //TEMPORARY DASH TO ROOT SOLUTION
#define DASH_TIME       2
#define EVADE_DIST      2
/** The sound effect for a bunny rooting a carrot */
#define ROOTING_BUNNY_EFFECT      "bunny-root"
/** The sound effect for a carrot being rooted*/
#define ROOTING_CARROT_EFFECT     "carrot-root"
/** The sound effect for a carrot being unrooted*/
#define UNROOTING_EFFECT          "unroot"
/** The sound effect for a character dashing */
#define DASH_EFFECT               "dash"
/** The key the rustle sounds */
#define RUSTLE_MUSIC              "rustle"
/** The sound effect for capture event */
#define CAPTURE_EFFECT            "capture"
/** Limits volume to be in between 0-1 */
#define VOLUME_FACTOR    1

/**
 * Initializes an ActionController
 */
bool ActionController::init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input,
    const std::shared_ptr<NetworkController> &network, const std::shared_ptr<cugl::AssetManager> &assets, bool haptics) {
    _map = map;
    _input = input;
    _world = _map->getWorld();
    _network = network;
    _assets = assets;
    _haptics = haptics;
    _soundScale = 1.0f;
    _freeMeter = 0;
    if (_network->isHost()) {
        _ai = AIController::alloc(map);
    }
    _network->attachEventType<CaptureEvent>();
    _network->attachEventType<RootEvent>();
    _network->attachEventType<UnrootEvent>();
    _network->attachEventType<MoveEvent>();
    _network->attachEventType<FreeEvent>();
    _network->attachEventType<SpawnRockEvent>();
    return true;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * Processes user input and updates objects in the map accordingly.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void ActionController::preUpdate(float dt) {
    auto playerEntity = _map->getCharacter();
    playerEntity->setMovement(_input->getMovement());
    bool didDash = _input->didDash();
    playerEntity->setDashInput(didDash, _input->getDashVector());
    playerEntity->setRootInput(_input->didRoot());
    playerEntity->setUnrootInput(_input->didUnroot());
    EntityModel::EntityState oldState = playerEntity->getEntityState();
    playerEntity->updateState(dt);
    if(didDash && playerEntity->getEntityState() == EntityModel::EntityState::DASHING){
        std::cout<<"getting player swipe status: "<<playerEntity->getSwipe()<<"\n";
        std::shared_ptr<Sound> source = _assets->get<Sound>(DASH_EFFECT);
        AudioEngine::get()->play("dash", source, false, _soundScale);
    }
    if(playerEntity->getEntityState() != oldState){
        _network->pushOutEvent(MoveEvent::allocMoveEvent(playerEntity->getUUID(), playerEntity->getEntityState()));
    }
    playerEntity->applyForce();
    
    updateRustlingNoise();
    float mapheight = _map->getMapBounds().size.height;
    for (auto it = _map->getRocks().begin(); it != _map->getRocks().end(); it++) {
        (*it)->getSceneNode()->setPriority((float) Map::DrawOrder::ENTITIES + 0.5 - ((*it)->getY()-(*it)->getHeight()/2)/mapheight/2.0);
        (*it)->applyForce();
    }
    auto players = _map->getPlayers();
    for (auto it = players.begin(); it != players.end(); ++it) {
        (*it)->getSceneNode()->setPriority((float) Map::DrawOrder::ENTITIES + 0.5 - ((*it)->getY()-(*it)->getHeight()/2)/mapheight/2.0);
        if ((*it)->getUUID() != playerEntity->getUUID()) {
            (*it)->updateSprite(dt, false);
        }
    }
    auto barrots = _map->getBabyCarrots();
    for (auto it = barrots.begin(); it != barrots.end(); ++it) {
        (*it)->getSceneNode()->setPriority((float) Map::DrawOrder::ENTITIES + 0.5 - ((*it)->getY()-(*it)->getHeight()/2)/mapheight/2.0);
        (*it)->updateSprite(dt, false);
    }
    
    // decoration animations
    auto decs = _map->getDecorations();
    for (auto it = decs.begin(); it != decs.end(); ++it) {
        (*it)->stepAnimation(dt);
    }
    
    // Find current character's planting spot
    // TODO: Can the current planting spot be stored with the EntityModel instead? -CJ
    std::shared_ptr<PlantingSpot> plantingSpot = nullptr;
    for(auto ps : _map->getPlantingSpots()){
        if(ps->getBelowAvatar()){
            plantingSpot = ps;
            break;
        }
    }

    if (_input->didThrowRock() && playerEntity->hasRock()) {
        _network->pushOutEvent(SpawnRockEvent::allocSpawnRockEvent(playerEntity->getPosition()-Vec2(0, playerEntity->getHeight()/2), 0, playerEntity->getFacing().normalize() * THROW_SPEED + playerEntity->getLinearVelocity(), playerEntity->getUUID()));
        playerEntity->setHasRock(false);
    }
    
    if (_network->isHost()) {
        updateBabyCarrots();
        if (_map->shouldSpawnRock()) {
            //optional spawn rock
            auto spawn = _map->getRandomRockSpawn();
            _network->pushOutEvent(SpawnRockEvent::allocSpawnRockEvent(spawn.first, spawn.second, Vec2::ZERO, ""));
        }
    }
    
    if (_map->isFarmer()) { // Farmer (host) specific actions
        auto farmerEntity = std::dynamic_pointer_cast<Farmer>(playerEntity);
        if(_input->didRoot() && _map->getFarmers().at(0)->canPlant() && plantingSpot != nullptr && !plantingSpot->getCarrotPlanted() && _map->getFarmers().at(0)->isHoldingCarrot()){
            //        std::cout<<"farmer did the rooting\n";
            if(_haptics)
                Haptics::get()->playContinuous(1.0, 0.3, 0.2);
            std::shared_ptr<Sound> source = _assets->get<Sound>(ROOTING_BUNNY_EFFECT);
            AudioEngine::get()->play("root-bunny", source, false, _soundScale);
            
            // look through ever carrot to see if it's rooted (invariant is only one carrot has rooted to be true)
            for (auto carrot : _map->getCarrots()) {
                if (carrot->isCaptured()) {
                    _network->pushOutEvent(RootEvent::allocRootEvent(carrot->getUUID(), plantingSpot->getPlantingID()));
                }
            }
        }
        
    }
    else { // Carrot specific actions
        auto carrotEntity = std::dynamic_pointer_cast<Carrot>(playerEntity);
        
        if(_input->didUnroot() && plantingSpot != nullptr && plantingSpot->getCarrotPlanted()){
            auto currPos = carrotEntity->getPosition();
            std::shared_ptr<Carrot> closestCarrot = nullptr;
            for (auto carrot : _map->getCarrots()){
                if(carrot->getUUID() != carrotEntity->getUUID() && (closestCarrot == nullptr || currPos.distance(carrot->getPosition()) < currPos.distance(closestCarrot->getPosition()))){
                    closestCarrot = carrot;
                }
            }
            if(closestCarrot != nullptr && currPos.distance(closestCarrot->getPosition()) < 1.0){
                _network->pushOutEvent(UnrootEvent::allocUnrootEvent(closestCarrot->getUUID(), plantingSpot->getPlantingID()));
            }
        }
    }

    if(!_map->isFarmer()){
        auto carrotEntity = std::dynamic_pointer_cast<Carrot>(_map->getCharacter());
        if(_input->didShakeDevice() && carrotEntity->isCaptured()){
            _freeMeter+=2;
            std::cout<<"free meter" << _freeMeter << "\n";
            if(_freeMeter >= 50){
                _freeMeter = 0;
                _network->pushOutEvent(FreeEvent::allocFreeEvent(carrotEntity->getUUID()));
                if(_haptics)
                    Haptics::get()->playContinuous(1.0, 0.3, 0.1);
            }
        }
        else{
            _freeMeter = max(0, _freeMeter-1);
        }
    }
}

void ActionController::updateBabyCarrots() {
    // Step baby carrot AI
    for (auto babyCarrot : _map->getBabyCarrots()) {
        // I'm slightly worried that this could get expensive but when I think about it
        // it's really no different than just checking for collisions so idk
        for (auto farmer : _map->getFarmers()) {
            if (farmer->getPosition().distance(babyCarrot->getPosition()) <= EVADE_DIST) {
                babyCarrot->setState(State::EVADE);
                babyCarrot->setTarget(babyCarrot->getPosition().add(farmer->getPosition().subtract(babyCarrot->getPosition()).normalize().scale(-3)).clamp(Vec2(1, 1), Vec2(13, 13)));
            }
        }
        _ai->updateBabyCarrot(babyCarrot);
    }
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void ActionController::postUpdate(float dt) {
    //remove objects from vectors when necessary
    auto it = _map->getBabyCarrots().begin();
    while (it != _map->getBabyCarrots().end()) {
        if ((*it)->isRemoved()) {
            (*it)->getSceneNode()->dispose();
            (*it)->getDebugNode()->dispose();
            (*it)->getWheatHeightNode()->dispose();
            _map->getBabyCarrots().erase(it);
        }
        else ++it;
    }
    for(std::shared_ptr<Carrot> c : _map->getCarrots()){
        if(c->isCaptured()){
            c->setSensor(true);
            c->setX(_map->getFarmers().at(0)->getX());
            c->setY(_map->getFarmers().at(0)->getY());
        }
        else if(!c->isRooted()){
            c->setSensor(false);
        }
    }
    auto iit = _map->getRocks().begin();
    while(iit != _map->getRocks().end()){
        if ((*iit)->isRemoved()) {
            _map->destroyRock(*iit);
            _map->getRocks().erase(iit);
        }
        else ++iit;
    }
}

/**
 * Local method to calculate the volume of wheat rustling based on character movement and distance from one's own character
 */
float calculateVolume(EntityModel::EntityState state, float distance){
    float stateToNum;
    switch(state){
        case EntityModel::EntityState::STUNNED:
        case EntityModel::EntityState::STANDING:
        case EntityModel::EntityState::ROOTING:
        case EntityModel::EntityState::UNROOTING:
        case EntityModel::EntityState::CAUGHT:
        case EntityModel::EntityState::ROOTED:
            stateToNum = 0;
            break;
        case EntityModel::EntityState::SNEAKING:
            stateToNum = 1;
            break;
        case EntityModel::EntityState::WALKING:
        case EntityModel::EntityState::CARRYING:
            stateToNum = 2;
            break;
        case EntityModel::EntityState::RUNNING:
            stateToNum = 3;
            break;
        case EntityModel::EntityState::DASHING:
            stateToNum = 4;
            break;
    }
    if(distance == 0){
        return stateToNum/4.0;
    }
    else if(distance > 0 && distance < 2){
        return stateToNum/4.0;
    }
    else if(distance >= 2 && distance < 4){
        return stateToNum/5.0;
    }
    else if(distance >= 4 && distance < 6){
        return stateToNum/7.0;
    }
    else{
        return stateToNum/10.0;
    }
//    return distance == 0 ? stateToNum/4.0 : (stateToNum/(distance*distance))/VOLUME_FACTOR; //needs to be approx a function between 0 and 1
}

void ActionController::playRustling(std::shared_ptr<EntityModel> player, float distance, bool isBarrot){
    std::shared_ptr<Sound> source = _assets->get<Sound>(RUSTLE_MUSIC);
//    std::cout<<"position: "<<_map->getFarmers().at(0)->getPosition().x << "\n";
    float newVolume;
    if(distance > 12 || (player != nullptr && !player->isInWheat())){
        newVolume = 0;
    }
    else {
        newVolume = isBarrot? calculateVolume(EntityModel::EntityState::WALKING, distance) : calculateVolume(player->getEntityState(), distance);
    }
    if(isBarrot){
        if(AudioEngine::get()->getState("barrot") != AudioEngine::State::PLAYING && newVolume != 0){
            AudioEngine::get()->play("barrot", source, true, newVolume*_soundScale, false);
        }
        else{
            if(newVolume == 0){
                AudioEngine::get()->clear("barrot", 0.5);
            }
            else if (AudioEngine::get()->getState("barrot") == AudioEngine::State::PLAYING){
                AudioEngine::get()->setVolume("barrot", newVolume > 1 ? 1 : newVolume*_soundScale);
            }
        }
    }
    else{
        if(AudioEngine::get()->getState(player->getUUID()) != AudioEngine::State::PLAYING && newVolume != 0){
            AudioEngine::get()->play(player->getUUID(), source, true, newVolume*_soundScale, false);
        }
        else{
            if(newVolume == 0){
                AudioEngine::get()->clear(player->getUUID(),0.5);
            }
            else if (AudioEngine::get()->getState(player->getUUID()) == AudioEngine::State::PLAYING){
                AudioEngine::get()->setVolume(player->getUUID(), (newVolume > 1 ? 1 : newVolume)*_soundScale);
            }
        }
    }
}

void ActionController::updateRustlingNoise(){
    auto playerEntity = _map->getCharacter();
    for(auto carrot : _map->getCarrots()){
        if(carrot->getUUID() == playerEntity->getUUID()){
            playRustling(playerEntity, 0, false);
        }
        else{
            float distanceFromCharacter = playerEntity->getPosition().distance(carrot->getPosition());
            playRustling(carrot, distanceFromCharacter, false);
        }
    }
    auto farmerEntity = _map->getFarmers().at(0);
    if(farmerEntity->getUUID() == playerEntity->getUUID()){
        playRustling(playerEntity, 0, false);
    }
    else{
        float distanceFromCharacter = playerEntity->getPosition().distance(farmerEntity->getPosition());
        playRustling(farmerEntity, distanceFromCharacter, false);
//        std::cout << "bunny state: " << farmerEntity->getEntityState() << "\n";
    }
    float closestBarrot = 13;
    float numBarrotsClose = 0;
    for(auto barrot : _map->getBabyCarrots()){
        float tempBarrotDist = barrot->getPosition().distance(playerEntity->getPosition());
        closestBarrot = closestBarrot > tempBarrotDist ? tempBarrotDist : closestBarrot;
        if(tempBarrotDist < 12 && barrot->isInWheat()){
            numBarrotsClose+=1;
        }
    }
    if(numBarrotsClose > 1){
        numBarrotsClose *= 0.75;
    }
    
//    std::cout<<"closestBarrot: " << closestBarrot << "\n";
//    std::cout<<"num nearby barrots: " << numBarrotsClose << "\n";
    
    playRustling(nullptr, numBarrotsClose > 0? closestBarrot/(numBarrotsClose):13, true);
}

void ActionController::processCaptureEvent(const std::shared_ptr<CaptureEvent>& event){
    _map->getFarmers().at(0)->grabCarrot(_map->getCarrotTypeForUUID(event->getUUID()));
    if(_map->isFarmer()){
        std::shared_ptr<Sound> source = _assets->get<Sound>(CAPTURE_EFFECT);
        AudioEngine::get()->play("capture", source, false, _soundScale);
    }
    if(_map->getCharacter()->getUUID() == event->getUUID()){
        if(_haptics)
            Haptics::get()->playContinuous(1.0, 0.8, 0.3);
    }
    for(auto carrot : _map->getCarrots()){
        if(carrot->getUUID() == event->getUUID()){
            carrot->setSensor(true);
            carrot->gotCaptured();
            if (_map->getFarmers().at(0)->getUUID() == _map->getCharacter()->getUUID()) {
                carrot->getSceneNode()->setPriority(float(Map::DrawOrder::PLAYER));
            }
            if (carrot->getUUID() == _map->getCharacter()->getUUID()) {
                _map->getFarmers().at(0)->getSceneNode()->setPriority(float(Map::DrawOrder::PLAYER));
            }
        }
    }
//    auto farmerNode = std::dynamic_pointer_cast<scene2::PolygonNode>(_map->getFarmers().at(0)->getSceneNode());
//    farmerNode->setTexture(CARROTFARMER_TEXTURE);
}

void ActionController::processRootEvent(const std::shared_ptr<RootEvent>& event){
    _map->getFarmers().at(0)->rootCarrot();
    for(auto carrot : _map->getCarrots()){
        if(carrot->getUUID() == event->getUUID()){
            if (_map->getFarmers().at(0)->getUUID() == _map->getCharacter()->getUUID()) {
                carrot->getSceneNode()->setPriority(float(Map::DrawOrder::ENTITIES));
            }
            if (_map->getCharacter()->getUUID() != _map->getFarmers().at(0)->getUUID()) {
                _map->getFarmers().at(0)->getSceneNode()->setPriority(float(Map::DrawOrder::ENTITIES));
            }
            carrot->gotRooted();
            if(carrot->getUUID() == _map->getCharacter()->getUUID()){
                if(_haptics)
                    Haptics::get()->playContinuous(1.0, 0.3, 0.1);
                std::shared_ptr<Sound> source = _assets->get<Sound>(ROOTING_CARROT_EFFECT);
                AudioEngine::get()->play("root-carrot", source, false, _soundScale);
            }
        }
    }
    for(auto ps : _map->getPlantingSpots()){
        if(ps->getPlantingID() == event->getPlantingSpotID()){
            ps->setCarrotPlanted(true);
        }
    }
//    auto farmerNode = std::dynamic_pointer_cast<scene2::PolygonNode>(_map->getFarmers().at(0)->getSceneNode());
//    farmerNode->setTexture(FARMER_TEXTURE);
}

void ActionController::processUnrootEvent(const std::shared_ptr<UnrootEvent>& event){
    for(auto carrot : _map->getCarrots()){
        if(carrot->getUUID() == event->getUUID()){
            carrot->gotUnrooted();
            if(carrot->getUUID() == _map->getCharacter()->getUUID()){
                if(_haptics)
                    Haptics::get()->playContinuous(1.0, 0.3, 0.1);
                std::shared_ptr<Sound> source = _assets->get<Sound>(UNROOTING_EFFECT);
                AudioEngine::get()->play("unroot", source, false, _soundScale);
            }
        }
    }
    for(auto ps : _map->getPlantingSpots()){
        if(ps->getPlantingID() == event->getPlantingSpotID()){
            ps->setCarrotPlanted(false);
        }
    }
}

void ActionController::processBarrotEvent(const std::shared_ptr<CaptureBarrotEvent>& event){
    for(auto carrot : _map->getCarrots()){
        if(carrot->getUUID() == event->getCarrotUUID()){
            carrot->captureBabyCarrot();
            for(auto barrot : _map->getBabyCarrots()){
                if(barrot->getID() == event->getBarrotID()){
                    barrot->gotCaptured();
                }
            }
        }
    }
}

void ActionController::processMoveEvent(const std::shared_ptr<MoveEvent>& event){
    //don't let network events update own state
    if(event->getUUID() == _map->getCharacter()->getUUID()){
        return;
    }
    else if(event->getUUID() == _map->getFarmers().at(0)->getUUID()){
        _map->getFarmers().at(0)->setEntityState(event->getState());
    }
    else{
        for(auto carrot : _map->getCarrots()){
            if(event->getUUID() == carrot->getUUID()){
                carrot->setEntityState(event->getState());
                return;
            }
        }
    }
}

void ActionController::processFreeEvent(const std::shared_ptr<FreeEvent>& event){
    _map->getFarmers().at(0)->carrotEscaped();
    if(_map->isFarmer()){
        if(_haptics)
            Haptics::get()->playContinuous(1.0, 0.8, 0.3);
    }
    else if(_map->getCharacter()->getUUID() == event->getUUID()){
        if(_haptics)
            Haptics::get()->playContinuous(1.0, 0.5, 0.2);
    }
    for(auto carrot : _map->getCarrots()){
        if(event->getUUID() == carrot->getUUID()){
            carrot->escaped();
            return;
        }
    }
}

void ActionController::processSpawnRockEvent(const std::shared_ptr<SpawnRockEvent>& event){
    if (event->getUUID() != "") {
        _map->getCharacter(event->getUUID())->setHasRock(false);
    }
    _map->spawnRock(event->getPosition(), event->getIndex(), event->getVelocity(), event->getUUID());
}

void ActionController::processCollectedRockEvent(const std::shared_ptr<CollectedRockEvent>& event){
    auto entity = _map->getCharacter(event->getUUID());
    entity->setHasRock(true);
    for (auto rock : _map->getRocks()) {
        if (!rock->isFired() && rock->getSpawnIndex() == event->getRockID()) {
            rock->collected();
        }
    }
}
