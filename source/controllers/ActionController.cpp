//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"

using namespace cugl;

bool ActionController::init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input) {
    _map = map;
    _input = input;
    _world = _map->getWorld();
    _ai.init(map);
    return true;
}

void ActionController::preUpdate(float dt) {
    for (auto carrot : _map->getCarrots()) {
        carrot->setMovement(_input->getMovement() * carrot ->getForce());
        carrot->applyForce();
    }

    for (auto farmer : _map->getFarmers()) {

    }

    for (auto babyCarrot : _map->getBabyCarrots()) {
//        babyCarrot->setMovement(Vec2(1,0));
        
        _ai.updateBabyCarrot(babyCarrot);
    }
}

void ActionController::postUpdate(float dt) {

}
