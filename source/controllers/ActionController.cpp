//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"

using namespace cugl;

bool ActionController::init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input) {
    _map = map;
    _input = input;
    _world = _map->getWorld();
    return true;
}

void ActionController::preUpdate(float dt) {
    auto avatar = _map->getCarrots().at(0);
    avatar->setMovement(_input->getHorizontal() * avatar->getForce());
    avatar->setJumping(_input->didJump());
    avatar->applyForce();
}

void ActionController::postUpdate(float dt) {

}