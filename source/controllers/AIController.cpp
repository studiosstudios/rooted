//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "AIController.h"
#include "BabyCarrot.h"

using namespace cugl;

bool AIController::init(std::shared_ptr<Map> &map) {
    _map = map;
    return true;
}

void AIController::updateBabyCarrot(const std::shared_ptr<BabyCarrot> &babyCarrot) {
    if (babyCarrot->getPosition().x >= 20) {
        babyCarrot->setMovement(Vec2(0,0));
        babyCarrot->setMovement(Vec2(-1,0));
    }
    else if (babyCarrot->getX() <= 10) {
        babyCarrot->setMovement(Vec2(0,0));
        babyCarrot->setMovement(Vec2(1,0));
    }
    babyCarrot->applyForce();
}
