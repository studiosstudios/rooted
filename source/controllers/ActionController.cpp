//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"
#include "NetworkController.h"

using namespace cugl;

/** Time after dashing when carrot can be captured */
#define CAPTURE_TIME    10 //TEMPORARY DASH TO ROOT SOLUTION
#define DASH_TIME       2
/** The sound effect for a bunny rooting a carrot */
#define ROOTING_BUNNY_EFFECT      "bunny-root"
#define ROOTING_CARROT_EFFECT     "carrot-root"
#define UNROOTING_EFFECT          "unroot"
#define DASH_EFFECT               "dash"

/**
 * Initializes an ActionController
 */
bool ActionController::init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input,
    std::shared_ptr<NetworkController> &network, const std::shared_ptr<cugl::AssetManager> &assets) {
    _map = map;
    _input = input;
    _world = _map->getWorld();
    _network = network;
    _assets = assets;
    if (_network->isHost()) {
        _ai.init(map);
    }
    _network->attachEventType<CaptureEvent>();
    _network->attachEventType<RootEvent>();
    _network->attachEventType<UnrootEvent>();
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
    playerEntity->setDashInput(didDash);
    if(didDash){
        std::shared_ptr<Sound> source = _assets->get<Sound>(DASH_EFFECT);
        AudioEngine::get()->play("dash", source);
    }
    playerEntity->setRootInput(_input->didRoot());
    playerEntity->setUnrootInput(_input->didUnroot());
    
    playerEntity->updateState();
    playerEntity->applyForce();
    playerEntity->stepAnimation(dt);
    
    // Find current character's planting spot
    // TODO: Can the current planting spot be stored with the EntityModel instead? -CJ
    std::shared_ptr<PlantingSpot> plantingSpot = nullptr;
    for(auto ps : _map->getPlantingSpots()){
        if(ps->getBelowAvatar()){
            plantingSpot = ps;
            break;
        }
    }
    
    if (_network->isHost()) { // Farmer (host) specific actions
        auto farmerEntity = std::dynamic_pointer_cast<Farmer>(playerEntity);
        
        // Step baby carrot AI
        for (auto babyCarrot : _map->getBabyCarrots()) {
            _ai.updateBabyCarrot(babyCarrot);
        }
        
        if(_input->didRoot() && _map->getFarmers().at(0)->canPlant() && plantingSpot != nullptr && !plantingSpot->getCarrotPlanted()){
            //        std::cout<<"farmer did the rooting\n";
            Haptics::get()->playContinuous(1.0, 0.3, 0.2);
            std::shared_ptr<Sound> source = _assets->get<Sound>(ROOTING_BUNNY_EFFECT);
            AudioEngine::get()->play("root-bunny", source);
            
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
}

void ActionController::processCaptureEvent(const std::shared_ptr<CaptureEvent>& event){
    _map->getFarmers().at(0)->grabCarrot();
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
                Haptics::get()->playContinuous(1.0, 0.3, 0.1);
                std::shared_ptr<Sound> source = _assets->get<Sound>(ROOTING_CARROT_EFFECT);
                AudioEngine::get()->play("root-carrot", source);
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
                Haptics::get()->playContinuous(1.0, 0.3, 0.1);
                std::shared_ptr<Sound> source = _assets->get<Sound>(UNROOTING_EFFECT);
                AudioEngine::get()->play("unroot", source);
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
