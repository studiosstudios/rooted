//
//  NetworkController.cpp
//  Rooted
//
//  Created by Joshua Guo on 3/6/24.
//
#include <cugl/cugl.h>

#include "NetworkController.h"

using namespace cugl;


void NetworkController::dispose()  {
    NetEventController::dispose();
}

bool NetworkController::init(const std::shared_ptr<AssetManager>& assets) {
    return this->NetEventController::init(assets);
}

std::shared_ptr<cugl::net::NetcodeConnection> NetworkController::getNetcode() {
    return this->_network;
}
