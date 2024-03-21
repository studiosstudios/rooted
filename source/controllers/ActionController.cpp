//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "ActionController.h"
#include "NetworkController.h"

using namespace cugl;

/** Time after dashing when carrot can be captured */
#define CAPTURE_TIME    10 //TEMPORARY DASH TO ROOT SOLUTION
#define DASH_TIME       2

/**
 * Initializes an ActionController
 */
bool ActionController::init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input,
    std::shared_ptr<NetworkController> &network) {
    _map = map;
    _input = input;
    _world = _map->getWorld();
    _ai.init(map);
    _network = network;
    _network->attachEventType<DashEvent>();
    _network->attachEventType<RootEvent>();
    return true;
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * Processes user input and updates objects in the map accordingly.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void ActionController::preUpdate(float dt) {
    for (auto carrot : _map->getCarrots()) {
        carrot->setMovement(Vec2::ZERO);
        if (_map->getCharacter()->getUUID() == carrot->getUUID() && !carrot->isCaptured() && !carrot->isRooted()) {
            if(carrot->dashTimer > 0){
                carrot->dashTimer -= 1;
            }
            if (_input->didDash()) {
                carrot->setMovement(_input->getMovement() * carrot->getForce() * 100);
                carrot->dashTimer=DASH_TIME;
                
            } else {
                carrot->setMovement(_input->getMovement() * carrot ->getForce());
            }
        }
        carrot->applyForce();
    }
    
    for (auto farmer : _map->getFarmers()) {
        farmer->setMovement(Vec2::ZERO);
        if (_map->getCharacter()->getUUID() == farmer->getUUID()){
            if(farmer->dashTimer > 0){
                farmer->dashTimer -= 1;
            }
            if(farmer->captureTime == 0){
                farmer->setDash(false);
            }
            else{
                farmer->captureTime -= 1;
            }
            if (_input->didDash() && !farmer->isHoldingCarrot()) {
                farmer->setMovement(_input->getMovement() * farmer->getForce() * 100);
                farmer->dashTimer=DASH_TIME;
                farmer->captureTime=CAPTURE_TIME;
                farmer->setDash(true);
            } else {
                farmer->setMovement(_input->getMovement() * farmer ->getForce());
            }
        }
        farmer->applyForce();
    }

    if (_network->isHost()) {
        for (auto babyCarrot : _map->getBabyCarrots()) {
            _ai.updateBabyCarrot(babyCarrot);
        }
    }
    
    networkQueuePositions();
    
    if(_input->didRoot() && _map->getFarmers().at(0)->canPlant()){
//        std::cout<<"farmer did the rooting\n";
        _map->getFarmers().at(0)->rootCarrot();
        // look through ever carrot to see if it's rooted (invariant is only one carrot has rooted to be true)
        for (auto carrot : _map->getCarrots()) {
            if (carrot->isCaptured()) {
                carrot->gotRooted();
                _network->pushOutEvent(RootEvent::allocRootEvent(carrot->getUUID()));
            }
        }
    }
}

void ActionController::fixedUpdate(){
    if(_network->isInAvailable()){
        auto e = _network->popInEvent();
        if(auto dashEvent = std::dynamic_pointer_cast<DashEvent>(e)){
//            CULog("Received dash event");
            processDashEvent(dashEvent);
        }
        if(auto rootEvent = std::dynamic_pointer_cast<RootEvent>(e)){
//            std::cout<<"got a root event\n";
            processRootEvent(rootEvent);
        }
    }
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void ActionController::postUpdate(float dt) {
    //remove objects from vectors when necessary
    auto it = _map->getBabyCarrots().begin();
    while (it != _map->getBabyCarrots().end()) {
        if ((*it)->isRemoved()) {
            (*it)->getSceneNode()->dispose();
            (*it)->getDebugNode()->dispose();
            _map->getBabyCarrots().erase(it);
        }
        else ++it;
    }
    for(std::shared_ptr<Carrot> c : _map->getCarrots()){
//        std::cout<<"capture status"<< c->isCaptured() << "\n";
        if(c->isCaptured()){
            c->setSensor(true);
            c->setX(_map->getFarmers().at(0)->getX()-0.5);
            c->setY(_map->getFarmers().at(0)->getY()-0.5);
        }
        else if(!c->isRooted()){
            c->setSensor(false);
        }
    }
}

void ActionController::networkQueuePositions() {
    
}

void ActionController::processDashEvent(const std::shared_ptr<DashEvent>& event){
    std::cout<<event->getUUID();
    for(auto carrot : _map->getCarrots()){
        if(carrot->getUUID() == event->getUUID()){
            carrot->setSensor(true);
            carrot->gotCaptured();
        }
    }
//    _map->getCarrots().at(0)->setSensor(true);
//    _map->getCarrots().at(0)->gotCaptured();
}

void ActionController::processRootEvent(const std::shared_ptr<RootEvent>& event){
    _map->getFarmers().at(0)->rootCarrot();
    for(auto carrot : _map->getCarrots()){
        if(carrot->getUUID() == event->getUUID()){
            std::cout<<"carrot rooted\n";
            std::cout<<_map->getFarmers().at(0)->isHoldingCarrot()<<"\n";
            carrot->gotRooted();
        }
    }
}
