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
    
    auto babycarrot = _map->getBabyCarrots().at(0);
    
    // TODO: generalize for all players other stuff
    
    // Twice to swap
    for (int i = 0; i < 2; i++) {
        
//        BabyCarrot* babycarrot = static_cast<BabyCarrot*>(bd1);

        // Player collisions:
        if (bd1 == avatar.get()) {
            
            Wheat* wheat = static_cast<Wheat*>(bd2);
            
            // TODO: this ain't it, it is very jank sorry I will fix later
            if (fix2->IsSensor() && wheat) {
                // Initialize random number generator
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 5); // Range: [0, 5]
                // Generate and print random number
                int randomNumber = dis(gen);
                wheat->animateWheat();
                wheat->rustle(randomNumber);
            }
            
//            if (typeid(bd2) == typeid(BabyCarrot)) {
            if (bd2 == _map->getBabyCarrots().at(0).get()) {
                // TODO: baby carrot stuff
                printf("player collided with baby carrot");
                avatar->captureBabyCarrot();
                babycarrot->gotCaptured();
//                _map->getBabyCarrots().erase(_map->getBabyCarrots().begin() + 0);
            }
        }
        
        // Baby Carrot collisions:
        if (bd1 == babycarrot.get()) {
//            printf("baby carrot collision");
            Wheat* wheat = static_cast<Wheat*>(bd2);
            
            if (fix2->IsSensor() && wheat) {
                // Initialize random number generator
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 5); // Range: [0, 5]
                // Generate and print random number
                int randomNumber = dis(gen);
                wheat->animateWheat();
                wheat->rustle(randomNumber);
            }
        }
   
        // Swap everything
        b2Fixture* fixTemp = fix1;
        fix1 = fix2;
        fix2 = fixTemp;
        
        b2Body* bodyTemp = body1;
        body1 = body2;
        body2 = bodyTemp;
        
        std::string* fdTemp = fd1;
        fd1 = fd2;
        fd2 = fdTemp;
        
        physics2::Obstacle* bdTemp = bd1;
        bd1 = bd2;
        bd2 = bdTemp;
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
    
    
    for (int i = 0; i < 2; i++) {

        // Player collisions:
        if (bd1 == avatar.get()) {
            if (typeid(bd2) == typeid(Wheat::BoxObstacle)) {
                // TODO: wheat stuff
            }
            
            if (typeid(bd2) == typeid(BabyCarrot)) {
                // TODO: baby carrot stuff
            }
        }
   
        // Swap everything
        b2Fixture* fixTemp = fix1;
        fix1 = fix2;
        fix2 = fixTemp;
        
        b2Body* bodyTemp = body1;
        body1 = body2;
        body2 = bodyTemp;
        
        std::string* fdTemp = fd1;
        fd1 = fd2;
        fd2 = fdTemp;
        
        physics2::Obstacle* bdTemp = bd1;
        bd1 = bd2;
        bd2 = bdTemp;
    }

}

/** for now everything collides with everything*/
bool CollisionController::shouldCollide(b2Fixture* f1, b2Fixture* f2) {
    b2Body* body1 = f1->GetBody();
    b2Body* body2 = f2->GetBody();
    
    std::string* fd1 = reinterpret_cast<std::string*>(f1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(f2->GetUserData().pointer);
    
    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);
    
    for (int i = 0; i < 2; i++) {
        
        // TODO: this actually is not doing anything rn oop
        if (typeid(bd1) == typeid(Carrot) && typeid(bd2) == typeid(Wheat::BoxObstacle)) {
            return false;
        }
        
        if (typeid(bd1) == typeid(Carrot) && typeid(bd2) == typeid(BabyCarrot)) {
            return false;
        }
        
        // Swap everything
        b2Fixture* fixTemp = f1;
        f1 = f2;
        f2 = fixTemp;
        
        b2Body* bodyTemp = body1;
        body1 = body2;
        body2 = bodyTemp;
        
        std::string* fdTemp = fd1;
        fd1 = fd2;
        fd2 = fdTemp;
        
        physics2::Obstacle* bdTemp = bd1;
        bd1 = bd2;
        bd2 = bdTemp;
        
    }
    return true;
}
