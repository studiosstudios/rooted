//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "AIController.h"
#include "../objects/BabyCarrot.h"
#include "time.h"

#define RADIUS 0.2f
#define MOVEMENT_SPEED 0.75f

using namespace cugl;

bool AIController::init(std::shared_ptr<Map> &map) {
    _map = map;
    srand(static_cast<unsigned int>( time(NULL) )^2);
    return true;
}

void AIController::updateBabyCarrot(const std::shared_ptr<BabyCarrot> &babyCarrot) {
    if (nearTarget(babyCarrot->getPosition(), babyCarrot->getTarget())) {
        babyCarrot->setTarget(Vec2::ZERO);
        babyCarrot->setMovement(Vec2(0,0));
        babyCarrot->applyForce();
    }
    if (babyCarrot->hasTarget()) {
        Vec2 movement = Vec2(babyCarrot->getPosition(), babyCarrot->getTarget()).normalize();
//        movement * MOVEMENT_SPEED;
        movement *= 2.5;
//        babyCarrot->setMovement(Vec2(0,0));
//        babyCarrot->setMovement(movement);
        babyCarrot->setLinearVelocity(movement);
//        babyCarrot->applyForce();
    } else {
        Vec2 newTarget = Vec2(((float) std::rand()/ RAND_MAX)*25+2, ((float) std::rand()/ RAND_MAX)*5+5);
        babyCarrot->setTarget(newTarget);
        
    }
}

bool AIController::nearTarget(const Vec2 vec1, const Vec2 vec2) {
    return abs(vec1.x - vec2.x) < RADIUS && abs(vec1.y - vec2.y) < RADIUS;
}
