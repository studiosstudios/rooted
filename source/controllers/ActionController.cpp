//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"
#include "NetworkController.h"

using namespace cugl;

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
        if (!_map->isFarmerPlaying()) {
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
        if (_map->isFarmerPlaying()){
            if (_input->didDash()) {
                farmer->setMovement(_input->getMovement() * farmer->getForce() * 100);
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

}

void ActionController::networkQueuePositions() {
    
}
