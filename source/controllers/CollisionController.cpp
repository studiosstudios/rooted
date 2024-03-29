//
//  CollisionController.cpp
//  Rooted
//
//  Created by Jeffrey Zhou on 2/22/24.
//

#include "CollisionController.h"

using namespace cugl;

bool CollisionController::init(std::shared_ptr<Map> &map,  std::shared_ptr<NetworkController> &network) {
    _map = map;
    _network = network;
    return true;
}

#pragma mark -
#pragma mark Collision Handling
/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.
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

        std::string name1 = bd1->getName();
        std::string name2 = bd2->getName();
        
        if (name1 == "carrot") {
            Carrot* carrot = dynamic_cast<Carrot*>(bd1);
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                wheat->rustle(bd1->getLinearVelocity().length());
                wheat->setOccupied(_map->isShowingPlayer());
                carrot->changeWheatContacts(1);
            }
            if (name2 == "baby") {
                BabyCarrot* b2babycarrot = dynamic_cast<BabyCarrot*>(bd2);
                if(!(carrot->isCaptured() || carrot->isRooted())){
                    carrot->captureBabyCarrot();
                    b2babycarrot->gotCaptured();
                }
            }
        }
        
        if (name1 == "baby") {
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                wheat->rustle(bd1->getLinearVelocity().length());
            }
        }
        
        if (name1 == "farmer") {
            Farmer* farmer = dynamic_cast<Farmer*>(bd1);
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                wheat->rustle(bd1->getLinearVelocity().length());
                wheat->setOccupied(_map->isShowingPlayer());
                farmer->changeWheatContacts(1);
            }
            if(name2 == "carrot") {
                Carrot* carrot = dynamic_cast<Carrot*>(bd2);
//                std::cout<<"carrot sensor status: "<< carrot->isSensor() << "\n";
                if(farmer->isDashing() && !carrot->isCaptured() && !carrot->isRooted()){
                    _network->pushOutEvent(DashEvent::allocDashEvent(carrot->getUUID()));
                    carrot->gotCaptured();
                    farmer->grabCarrot();
//                    std::shared_ptr<cugl::physics2::DistanceJoint> joint = std::make_shared<cugl::physics2::DistanceJoint>();
//                    std::shared_ptr<physics2::Obstacle> ptr1(bd1);
//                    std::shared_ptr<physics2::Obstacle> ptr2(bd2);
//                    joint->initWithObstacles(ptr1, ptr2, Vec2(0,0), Vec2(0,0));
//                    CULog("joint created");
                }
            }
            if(name2 == "planting spot") {
                farmer->setCanPlant(true);
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
 * This method is called when two objects cease to touch.
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
        std::string name1 = bd1->getName();
        std::string name2 = bd2->getName();
        
        if (name1 == "carrot") {
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                Carrot* carrot = dynamic_cast<Carrot*>(bd1);
                wheat->setOccupied(false);
                carrot->changeWheatContacts(-1);
            }
            
            if (name2 == "baby") {
            
            }
        }
        
        if (name1 == "baby") {
            if (name2 == "wheat") {
                
            }
        }
        
        if (name1 == "farmer") {
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                Farmer* farmer = dynamic_cast<Farmer*>(bd1);
                wheat->setOccupied(false);
                farmer->changeWheatContacts(-1);
            }
            if(name2 == "planting spot"){
                Farmer* farmer = dynamic_cast<Farmer*>(bd1);
                farmer->setCanPlant(false);
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

bool CollisionController::shouldCollide(b2Fixture* f1, b2Fixture* f2) {
    b2Body* body1 = f1->GetBody();
    b2Body* body2 = f2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(f1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(f2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    for (int i = 0; i < 2; i++) {
        
        std::string name1 = bd1->getName();
        std::string name2 = bd2->getName();
        
        // Baby carrots don't collide with each other
        if (name1 == "baby" && name2 == "baby") {
            return false;
        }
        
        if (name1 == "carrot" && name2 == "farmer") {
            Carrot* carrot = dynamic_cast<Carrot*>(bd1);
            return !carrot->isSensor();
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
