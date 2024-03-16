//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"
#include "NetworkController.h"

using namespace cugl;

/** Time after dashing when carrot can be captured */
#define CAPTURE_TIME    10 //TEMPORARY DASH TO ROOT SOLUTION

/**
 * Initializes an ActionController
 */
bool ActionController::init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input,
    std::shared_ptr<NetworkController> &network) {
    _map = map;
    _input = input;
    _world = _map->getWorld();
    _ai.init(map);
    _network = network;
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
    for (auto carrot : _map->getCarrots()) {
        carrot->setMovement(Vec2::ZERO);
        if (_map->getCharacter()->getUUID() == carrot->getUUID() && !carrot->isCaptured() && !carrot->isRooted()) {
            if (_input->didDash()) {
                carrot->setMovement(_input->getMovement() * carrot->getForce() * 100);
            } else {
                carrot->setMovement(_input->getMovement() * carrot ->getForce());
            }
        }
        carrot->applyForce();
    }
    
    for (auto farmer : _map->getFarmers()) {
        farmer->setMovement(Vec2::ZERO);
        if (_map->getCharacter()->getUUID() == farmer->getUUID()){
            if(dashWindow == 0){
                farmer->setDash(false);
            }
            else{
                dashWindow--;
            }
            if (_input->didDash() && !farmer->isHoldingCarrot()) {
                farmer->setMovement(_input->getMovement() * farmer->getForce() * 100);
                dashWindow=CAPTURE_TIME;
                farmer->setDash(true);
            } else {
                farmer->setMovement(_input->getMovement() * farmer ->getForce());
            }
        }
        farmer->applyForce();
    }

    if (_network->isHost()) {
        for (auto babyCarrot : _map->getBabyCarrots()) {
            _ai.updateBabyCarrot(babyCarrot);
        }
    }
    
    networkQueuePositions();

    if(_input->didRoot() && _map->getFarmers().at(0)->canPlant()){
        _map->getFarmers().at(0)->rootCarrot();
        // look through ever carrot to see if it's rooted (invariant is only one carrot has rooted to be true)
        for (auto carrot : _map->getCarrots()) {
            if (carrot->isCaptured()) {
                carrot->gotRooted();
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
            _map->getBabyCarrots().erase(it);
        }
        else ++it;
    }
    for(std::shared_ptr<Carrot> c : _map->getCarrots()){
        if(c->isCaptured()){
            c->setSensor(true);
            c->setX(_map->getFarmers().at(0)->getX()-1);
            c->setY(_map->getFarmers().at(0)->getY()-1);
        }
        else if(!c->isRooted()){
            c->setSensor(false);
        }
    }
}

void ActionController::networkQueuePositions() {
    
}
