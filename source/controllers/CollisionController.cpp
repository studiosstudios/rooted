//
//  CollisionController.cpp
//  Rooted
//
//  Created by Jeffrey Zhou on 2/22/24.
//

#include "CollisionController.h"

using namespace cugl;

bool CollisionController::init(std::shared_ptr<Map> &map) {
    _map = map;
    _isComplete = false;
    return true;
}

#pragma mark -
#pragma mark Collision Handling
/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door.
 *
 * @param  contact  The two bodies that collided
 */
void CollisionController::beginContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    auto avatar = _map->getCarrots().at(0);

    // See if we have landed on the ground.
    if ((avatar->getSensorName() == fd2 && avatar.get() != bd1) ||
        (avatar->getSensorName() == fd1 && avatar.get() != bd2)) {
        avatar->setGrounded(true);
        // Could have more than one ground
        _sensorFixtures.emplace(avatar.get() == bd1 ? fix2 : fix1);
    }

    // If we hit the "win" door, we are done
    if((bd1 == avatar.get()   && bd2 == _map->getGoalDoor().get()) ||
       (bd1 == _map->getGoalDoor().get() && bd2 == avatar.get())) {
        _isComplete = true;
    }
}

/**
 * Callback method for the start of a collision
 *
 * This method is called when two objects cease to touch.  The main use of this method
 * is to determine when the characer is NOT on the ground.  This is how we prevent
 * double jumping.
 */
void CollisionController::endContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    auto avatar = _map->getCarrots().at(0);

    if ((avatar->getSensorName() == fd2 && avatar.get() != bd1) ||
        (avatar->getSensorName() == fd1 && avatar.get() != bd2)) {
        _sensorFixtures.erase(avatar.get() == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty()) {
            avatar->setGrounded(false);
        }
    }
}

/** for now everything collides with everything*/
bool CollisionController::shouldCollide(b2Fixture* f1, b2Fixture* f2) {
    return true;
}
