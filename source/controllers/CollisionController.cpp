//
//  CollisionController.cpp
//  Rooted
//
//  Created by Jeffrey Zhou on 2/22/24.
//

#include "CollisionController.h"

using namespace cugl;

bool CollisionController::init(std::shared_ptr<Map> &map,  const std::shared_ptr<NetworkController> &network) {
    _map = map;
    _network = network;
    _network->attachEventType<CaptureBarrotEvent>();
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
            if(_map->getCharacter()->getUUID() == carrot->getUUID() && (name2 == "farmer" || name2 == "baby")){
                Haptics::get()->playTransient(0.8, 0.1);
            }
            if (name2 == "baby") {
                BabyCarrot* b2babycarrot = dynamic_cast<BabyCarrot*>(bd2);
                if(_map->getCharacter()->getUUID() == carrot->getUUID() && !(carrot->isCaptured() || carrot->isRooted())){
                    _network->pushOutEvent(CaptureBarrotEvent::allocCaptureBarrotEvent(carrot->getUUID(), b2babycarrot->getID()));
                }
            }
            if(name2 == "planting spot" && _map->getCharacter()->getUUID() == carrot->getUUID()) {
                PlantingSpot* plantingSpot = dynamic_cast<PlantingSpot*>(bd2);
                plantingSpot->setBelowAvatar(true);
            }
        }
        
        if (name1 == "baby") {
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                BabyCarrot* b1babycarrot = dynamic_cast<BabyCarrot*>(bd1);
                wheat->rustle(bd1->getLinearVelocity().length());
                b1babycarrot->changeWheatContacts(1);
            }
        }
        
        if (name1 == "farmer") {
            Farmer* farmer = dynamic_cast<Farmer*>(bd1);
            if(_map->getCharacter()->getUUID() == farmer->getUUID() && (name2 == "baby" || name2 == "carrot")){
                Haptics::get()->playTransient(0.8, 0.1);
            }
            if(name2 == "carrot") {
                Carrot* carrot = dynamic_cast<Carrot*>(bd2);
                if(farmer->isDashing() && !carrot->isCaptured() && !carrot->isRooted()){
                    _network->pushOutEvent(CaptureEvent::allocCaptureEvent(carrot->getUUID()));
                }
            }
            if(name2 == "planting spot" && _map->getCharacter()->getUUID() == farmer->getUUID()) {
                PlantingSpot* plantingSpot = dynamic_cast<PlantingSpot*>(bd2);
                farmer->setCanPlant(true);
                plantingSpot->setBelowAvatar(true);
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
            Carrot* carrot = dynamic_cast<Carrot*>(bd1);
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                wheat->setOccupied(false);
                carrot->changeWheatContacts(-1);
            }
            
            if (name2 == "baby") {
            }
            if(name2 == "planting spot" && _map->getCharacter()->getUUID() == carrot->getUUID()){
                PlantingSpot* plantingSpot = dynamic_cast<PlantingSpot*>(bd2);
                plantingSpot->setBelowAvatar(false);
            }
            
        }
        
        if (name1 == "baby") {
            if (name2 == "wheat") {
                BabyCarrot* baby = dynamic_cast<BabyCarrot*>(bd1);
                baby->changeWheatContacts(-1);
            }
        }
        
        if (name1 == "farmer") {
            Farmer* farmer = dynamic_cast<Farmer*>(bd1);
            if (name2 == "wheat") {
                Wheat* wheat = dynamic_cast<Wheat*>(bd2);
                wheat->setOccupied(false);
                farmer->changeWheatContacts(-1);
            }
            if(name2 == "planting spot" && _map->getCharacter()->getUUID() == farmer->getUUID()){
                PlantingSpot* plantingSpot = dynamic_cast<PlantingSpot*>(bd2);
                farmer->setCanPlant(false);
                plantingSpot->setBelowAvatar(false);
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
