//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"

using namespace cugl;

/**
 * Initializes an ActionController
 */
bool ActionController::init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input) {
    _map = map;
    _input = input;
    _world = _map->getWorld();
    _ai.init(map);
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
        if (_input->didDash()) {
            carrot->setMovement(_input->getMovement() * carrot->getForce() * 100);
        } else {
            carrot->setMovement(_input->getMovement() * carrot ->getForce());
        }
        carrot->applyForce();
        
    }

    for (auto farmer : _map->getFarmers()) {

    }

    for (auto babyCarrot : _map->getBabyCarrots()) {        
        _ai.updateBabyCarrot(babyCarrot);
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

}
