//
//  NetworkController.cpp
//  Rooted
//
//  Created by Joshua Guo on 3/6/24.
//

#include "NetworkController.h"

NetworkController::NetworkController() :
_network(nullptr) {}

void NetworkController::dispose() {
    
}

bool NetworkController::init(std::shared_ptr<NetEventController> network) {
    _network = network;
    return true;
}
