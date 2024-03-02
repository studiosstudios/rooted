//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "AIController.h"
#include "../objects/BabyCarrot.h"

#define RADIUS 0.2f
#define MOVEMENT_SPEED 0.75f

using namespace cugl;

bool AIController::init(std::shared_ptr<Map> &map) {
    _map = map;
    return true;
}

void AIController::updateBabyCarrot(const std::shared_ptr<BabyCarrot> &babyCarrot) {
    if (nearTarget(babyCarrot->getPosition(), babyCarrot->getTarget())) {
        babyCarrot->setTarget(Vec2::ZERO);
    }
    if (babyCarrot->hasTarget()) {
        Vec2 movement = Vec2(babyCarrot->getPosition(), babyCarrot->getTarget());
        movement.normalize();
        movement * MOVEMENT_SPEED;
        babyCarrot->setMovement(Vec2(0,0));
        babyCarrot->setMovement(movement);
        babyCarrot->applyForce();
    } else {
        Vec2 newTarget = Vec2(((float) std::rand()/ RAND_MAX)*10+10, ((float) std::rand()/ RAND_MAX)*5+10);
        babyCarrot->setTarget(newTarget);
        
    }
//    if (babyCarrot->getPosition().x >= 20) {
//        babyCarrot->setMovement(Vec2(0,0));
//        babyCarrot->setMovement(Vec2(-1,0));
//    }
//    else if (babyCarrot->getX() <= 10) {
//        babyCarrot->setMovement(Vec2(0,0));
//        babyCarrot->setMovement(Vec2(1,0));
//    }
}

bool AIController::nearTarget(const Vec2 vec1, const Vec2 vec2) {
    return abs(vec1.x - vec2.x) < RADIUS && abs(vec1.y - vec2.y) < RADIUS;
}
