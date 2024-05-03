//
// Created by Jeffrey Zhou on 4/25/24.
//
// This is a lightweight mock network controller for the tutorial scene only.

#include "MockNetworkController.h"

bool MockNetworkController::init() {
    _isHost = true;
    return true;
}

void MockNetworkController::pushOutEvent(const std::shared_ptr<NetEvent> &e) {
    _eventQueue.push(e);
}

std::shared_ptr<NetEvent> MockNetworkController::popInEvent() {
    auto e = _eventQueue.front();
    _eventQueue.pop();
    return e;
}
