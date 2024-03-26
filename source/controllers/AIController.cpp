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

void AIController::updateBabyCarrotState(const std::shared_ptr<BabyCarrot> &babyCarrot) {
    if (babyCarrot->getState() == State::HOLD) {
        if (((float) std::rand()/ RAND_MAX) < 0.5) {
            Vec2 newTarget = Vec2(babyCarrot->getX()+(((float) std::rand()/ RAND_MAX)*2-1), babyCarrot->getY()+(((float) std::rand()/ RAND_MAX)*2-1));
            babyCarrot->setTarget(newTarget);
        } else {
            babyCarrot->setState(State::ROAM);
            Vec2 newTarget = Vec2(((float) std::rand()/ RAND_MAX)*27+2.5, ((float) std::rand()/ RAND_MAX)*13+2.5);
            babyCarrot->setTarget(newTarget);
        }
    } else {
        if (((float) std::rand()/ RAND_MAX) < 0.2) {
            babyCarrot->setState(State::HOLD);
            Vec2 newTarget = Vec2(babyCarrot->getX()+(((float) std::rand()/ RAND_MAX)*2-1), babyCarrot->getY()+(((float) std::rand()/ RAND_MAX)*2-1));
            babyCarrot->setTarget(newTarget);
        } else {
            babyCarrot->setState(State::ROAM);
            Vec2 newTarget = Vec2(((float) std::rand()/ RAND_MAX)*27+2.5, ((float) std::rand()/ RAND_MAX)*13+2.5);
            babyCarrot->setTarget(newTarget);
        }
    }
}

void AIController::updateBabyCarrot(const std::shared_ptr<BabyCarrot> &babyCarrot) {
    if (nearTarget(babyCarrot->getPosition(), babyCarrot->getTarget())) {
        babyCarrot->setTarget(Vec2::ZERO);
        babyCarrot->setLinearVelocity(Vec2::ZERO);
        updateBabyCarrotState(babyCarrot);
    }
    switch (babyCarrot->getState()) {
        case State::EVADE:
            // SHOULD DISCUSS IF WE WANT THIS
            break;
        case State::ROAM:
//            std::cout << "ROAMING ROAMING" << '\n';
            if (babyCarrot->hasTarget()) {
                Vec2 movement = Vec2(babyCarrot->getPosition(), babyCarrot->getTarget()).normalize();
                movement *= 2.5;
                babyCarrot->setLinearVelocity(movement);
            }
            break;
        case State::SIT:
            babyCarrot->setLinearVelocity(Vec2::ZERO);
            break;
        case State::HOLD:
//            std::cout << "HOLDING HOLDING" << '\n';
            if (babyCarrot->hasTarget()) {
                Vec2 movement = Vec2(babyCarrot->getPosition(), babyCarrot->getTarget()).normalize();
                movement *= 0.5;
                babyCarrot->setLinearVelocity(movement);
            }
            break;
    }
}

bool AIController::nearTarget(const Vec2 vec1, const Vec2 vec2) {
    return abs(vec1.x - vec2.x) < RADIUS && abs(vec1.y - vec2.y) < RADIUS;
}
