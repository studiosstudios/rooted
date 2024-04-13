//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Farmer.h"

bool Farmer::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return EntityModel::init(pos, size, scale);
}

void Farmer::dispose() {
    EntityModel::dispose();
    _normalNode = nullptr;
    _captureNode = nullptr;
}

void Farmer::grabCarrot(){
    _isHoldingCarrot = true;
//    _normalNode->setVisible(false);
//    _captureNode->setVisible(true);
    setSceneNode(_captureNode);
}

void Farmer::rootCarrot(){
    _isHoldingCarrot = false;
//    _captureNode->setVisible(false);
//    _normalNode->setVisible(true);
    setSceneNode(_normalNode);
}

void Farmer::setMovement(Vec2 movement) {
    _movement = movement;
    if (_isHoldingCarrot) {
        return;
    }
    EntityModel::setMovement(movement);
}

void Farmer::updateCurAnimDurationForState() {
    switch (_state) {
        case MOVING: {
            curAnimDuration = walkAnimDuration;
            break;
        }
        default: {
            // This state duration is not implemented yet
            return;
        }
    }
}
