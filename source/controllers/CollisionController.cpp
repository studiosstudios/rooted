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

    // Twice to swap
    for (int i = 0; i < 2; i++) {

        Wheat* wheat = dynamic_cast<Wheat*>(bd2);
        BabyCarrot* b2babycarrot = dynamic_cast<BabyCarrot*>(bd2);
        Farmer* b2farmer = dynamic_cast<Farmer*>(bd2);
        
        for (auto carrot : _map->getCarrots()) {
            if (bd1 == carrot.get()) {
                if (wheat) {
//                    printf("carrot collision with wheat \n");
                    wheat->setRustling(true);
                }
                
                if (b2babycarrot) {
//                    printf("carrot collision with baby carrot \n");
                    carrot->captureBabyCarrot();
                    b2babycarrot->gotCaptured();
                }
                
                if (b2farmer) {
                    // TODO: carrot gets rooted
                }
                
            }
        }
        
        for (auto babyCarrot : _map->getBabyCarrots()) {
            if (bd1 == babyCarrot.get()) {
                if (wheat) {
                    wheat->setRustling(true);
                }
            }
        }
        
        for (auto farmer : _map->getFarmers()) {
            if (bd1 == farmer.get()) {
                if (wheat) {
                    wheat->setRustling(true);
                }
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

    for (int i = 0; i < 2; i++) {
        
        Wheat* wheat = dynamic_cast<Wheat*>(bd2);
        BabyCarrot* b2babycarrot = dynamic_cast<BabyCarrot*>(bd2);
        
        for (auto carrot : _map->getCarrots()) {
            if (bd1 == carrot.get()) {
                if (wheat) {
                    wheat->setRustling(false);
                }
                
                if (b2babycarrot) {
                    
                }
            }
        }
        
        for (auto babyCarrot : _map->getBabyCarrots()) {
            if (bd1 == babyCarrot.get()) {
                if (wheat) {
                    wheat->setRustling(false);
                }
            }
        }
        
        for (auto farmer : _map->getFarmers()) {
            if (bd1 == farmer.get()) {
                if (wheat) {
                    wheat->setRustling(false);
                }
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

        BabyCarrot* b2babycarrot = dynamic_cast<BabyCarrot*>(bd2);
        
        for (auto carrot : _map->getCarrots()) {
            if (bd1 == carrot.get()) {
                
                if (b2babycarrot) {
                    
                }
            }
        }
        
        for (auto babyCarrot : _map->getBabyCarrots()) {
            if (bd1 == babyCarrot.get()) {
              
            }
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
